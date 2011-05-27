/*
 *  ACMultiMediaCycleOsgQt.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 16/11/10
 *  @copyright (c) 2010 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 */

#include "ACMultiMediaCycleOsgQt.h"
#include <fstream>
#include <iomanip> // for setw
#include <cstdlib> // for atoi

// ----------- class constants
// number of files above which to lauch thread.
const int ACMultiMediaCycleOsgQt::n_dir_for_threading = 10;
// -----------

static void mediacycle_callback(char* message, void *user_data) {

	ACMultiMediaCycleOsgQt *application = (ACMultiMediaCycleOsgQt*)user_data;

	application->mediacycleCallback(message);
}

void ACMultiMediaCycleOsgQt::mediacycleCallback(char* message) {
	if (message=="loaddirstart") {
		statusBar()->showMessage(tr("Loading Directory..."), 0);
	}

	if (message=="loaddirfinish") {
		this->updateLibrary();
		statusBar()->clearMessage();
	}
}

ACMultiMediaCycleOsgQt::ACMultiMediaCycleOsgQt(QWidget *parent) : QMainWindow(parent),
 features_known(false), plugins_scanned(false), detachedBrowser(0)
{
	ui.setupUi(this); // first thing to do
	this->media_type = MEDIA_TYPE_NONE;
	this->browser_mode = AC_MODE_NONE;
	this->media_cycle = 0;
	this->config_file_xml = "";
	this->project_directory = QApplication::applicationDirPath().append(QDir::separator()).toStdString();
	
	#if defined (SUPPORT_AUDIO)
		this->audio_engine = 0;
	#endif //defined (SUPPORT_AUDIO)

	this->use_segmentation = true;
	
	// Apple bundled *.app, just look for bundled osg plugins
	#ifndef USE_DEBUG
	#if defined __APPLE__ and not defined (XCODE)
		QDir dir(QApplication::applicationDirPath());
		dir.cdUp();
		dir.cd("PlugIns");
		osgDB::Registry::instance()->setLibraryFilePathList(dir.absolutePath().toStdString());
	#endif
	#endif

	// Dock Widgets
	dockWidgets.resize(0);
	dockWidgetFactory = new ACDockWidgetFactoryQt();
	lastDocksVisibilities.resize(0);
	connect(ui.actionShow_Hide_Controls, SIGNAL(triggered()), this, SLOT(syncControlToggleWithDocks()));
	wasControlsToggleChecked = ui.actionShow_Hide_Controls->isChecked();//true;

	aboutDialogFactory = new ACAboutDialogFactoryQt();

	// uses another window for settings = editing the config file
	settingsDialog = new SettingsDialog(this);

	// progress bar
	//pb = new QProgressBar(statusBar());
	//	pb->setTextVisible(false);
	//	pb->hide();
	//	statusBar()->addPermanentWidget(pb);

	aboutDialog = 0;

	// This is required to populate the available file extensions list at startup
	// until we clean mediacycle instead of deleting/creating it at every media type change.
	ACMediaFactory::getInstance();
	// Since it is time consuming, we might want to add a splash screen with progress bar at startup?

	// Docked osg browser
	ui.centralwidget->hide();
	compositeOsgView = new ACOsgCompositeViewQt();
	compositeOsgView->setSizePolicy ( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	osgViewDock = new QDockWidget(this);
	osgViewDock->setSizePolicy ( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	osgViewDock->setWidget(compositeOsgView);
	osgViewDock->setAllowedAreas(Qt::RightDockWidgetArea);
	osgViewDock->setFeatures(QDockWidget::DockWidgetFloatable);
	osgViewDock->setWindowTitle("Browser");
	this->addDockWidget(Qt::RightDockWidgetArea,osgViewDock);
	osgViewDockTitleBar = osgViewDock->titleBarWidget();
	osgViewDockNormalSize = QRect();

	// to make window appear on top of others.
	this->activateWindow();
	this->show();

	// Debugging accentuated media filenames
	#ifdef USE_DEBUG
	qDebug() << "System Locale name:"      << QLocale::system().name();
	qDebug() << "Qt codecForCStrings:" << QTextCodec::codecForCStrings();
	qDebug() << "Qt codecForLocale:"   << QTextCodec::codecForLocale()->name();
	if (!QTextCodec::codecForCStrings())
		QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));//CF hack for CF to load accents
	#endif

	// XS reminder: need to call configureSettings from the application main.

	// Keeps actions (mouse/keyboard shortcuts) active in fullscreen mode when bars are hidden
	this->addActions(ui.toolbar->actions());
	this->addActions(ui.menubar->actions());
}

ACMultiMediaCycleOsgQt::~ACMultiMediaCycleOsgQt(){
	this->destroyMediaCycle();
	delete settingsDialog;
	delete aboutDialogFactory;
	delete dockWidgetFactory;
    vector<ACAbstractDockWidgetQt*>::iterator dwiter;
    for (dwiter=dockWidgets.begin(); dwiter!=dockWidgets.end(); dwiter++)
        delete *dwiter;
    if (aboutDialog) delete aboutDialog;
    if (detachedBrowser) delete detachedBrowser;
}

// tries to read settings from previous run
// if it does not find any, use default (centered) geometry
void ACMultiMediaCycleOsgQt::configureSettings(){
	//CF temp disable
	/*if (this->readQSettings()){
		QMessageBox msgBox;
		msgBox.setText("Launching MediaCycle.");
		msgBox.setInformativeText("Do you want to load your media settings from previous session ?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::No);
		msgBox.setDetailedText ("GUI settings (window size, ...) are loaded automatically. Here you can load MediaCycle settings (media type, browser mode, ...).");
		int ret = msgBox.exec();
		switch (ret) {
			case QMessageBox::Yes:
				// config file normally in readSettings...
				if (this->readXMLConfig(this->config_file_xml))
					this->configureDockWidgets(this->media_type);
				else
					this->showError("Error occured reading XML. Please Load your media and save a new XML file.");
				break;
			case QMessageBox::No:
				// then dock widgets will be configured afterwards (in updateLibrary, after loading media)
				break;
			default:
				//should never be reached
				//throw runtime_error("problem reading settings");
				break;
		}
	}
	else
		this->setDefaultQSettings();*/
}

// creates a MediaCycle object (containing the whole application)
// with the appropriate type (audio/image/video/text/mixed/composite/...)
void ACMultiMediaCycleOsgQt::createMediaCycle(ACMediaType _media_type, ACBrowserMode _browser_mode){

	this->media_cycle = new MediaCycle(_media_type,"/tmp/","mediacycle.xml");
	media_cycle->setCallback(mediacycle_callback, (void*)this);

	compositeOsgView->setMediaCycle(this->media_cycle);
	// SD
	compositeOsgView->prepareFromBrowser();

	// keep track locally of the media and browser modes
	this->media_type = _media_type;
	this->browser_mode = _browser_mode;

	#if defined (SUPPORT_AUDIO)
	// XS TODO add checks on existing audio_engine ?
	// normally should not be any
	if (_media_type == MEDIA_TYPE_AUDIO){
		audio_engine = new ACAudioEngine();
		audio_engine->setMediaCycle(media_cycle);
		audio_engine->startAudioEngine();
		compositeOsgView->setAudioEngine(audio_engine);
	}
	#endif //defined (SUPPORT_AUDIO)

	for (int d=0;d<dockWidgets.size();d++){
		dockWidgets[d]->setMediaCycle(media_cycle);
		#if defined (SUPPORT_AUDIO)
			dockWidgets[d]->setAudioEngine(audio_engine);
		#endif //defined (SUPPORT_AUDIO)
		dockWidgets[d]->setOsgView(compositeOsgView);
	}
}

// destroys the MediaCycle object (containing the whole application)
// it should leave an empty blue frame, just as lauch time
// use with care; there should only be one instance of mediacycle per application
void ACMultiMediaCycleOsgQt::destroyMediaCycle(){
	#if defined (SUPPORT_AUDIO)
		if (audio_engine) {
			audio_engine->stopAudioEngine();
			delete audio_engine;
			audio_engine = 0;
		}
	#endif //defined (SUPPORT_AUDIO)
	delete media_cycle;
}


// XS in theory one could select multiple XML files and concatenate them (not tested yet)
void ACMultiMediaCycleOsgQt::on_actionLoad_XML_triggered(bool checked){
	this->readXMLConfig();
}

bool ACMultiMediaCycleOsgQt::readXMLConfig(string _filename){
	if (_filename==""){
		QString fileName;
		QFileDialog dialog(osgViewDock,"Open XML Library File(s)");
		dialog.setDefaultSuffix ("xml");
		dialog.setNameFilter("Library Files (*.xml)");
		dialog.setFileMode(QFileDialog::ExistingFile); 	// change to ExistingFiles for multiple file handling
		QStringList fileNames;
		if (dialog.exec())
			fileNames = dialog.selectedFiles();
		QStringList::Iterator file = fileNames.begin();
		// only one file -> if
		// many files -> while (not implemented yet)
		// while(file != fileNames.end()) {
		if(file != fileNames.end()) {
			fileName = *file;
			_filename=fileName.toStdString();
		}
		//++file;
	}

	std::cout << "Opening XML config file: '" << _filename << std::endl;

	//XS TODO: this is cut and paste from MediaCycle.
	// the problem is that we have no mediacycle instance
	// we need to read the XML file first.

	// 1) read header info
	//TiXmlHandle rootHandle = this->readXMLConfigHeader(_filename);
	try {
		if (_filename=="")
			return false;
			//throw runtime_error("bad XML file name");

		TiXmlDocument doc( _filename.c_str() );
		if (!doc.LoadFile( ))
			throw runtime_error("error reading XML file");
		TiXmlHandle docHandle(&doc);
		TiXmlHandle rootHandle = docHandle.FirstChildElement( "MediaCycle" );
// XS TODO make roothandle a pointer and check this
		//		if (!rootHandle)
//			throw runtime_error("corrupted XML file, missing <MediaCycle>");

		// XS TODO browser mode and media type as string instead of enum
		TiXmlText* browserModeText=rootHandle.FirstChild( "BrowserMode" ).FirstChild().Text();
		std::stringstream tmp;
		if (browserModeText){
			tmp << browserModeText->ValueStr();
			int bm; // ACBrowserMode
			tmp >> bm;
			this->setBrowserMode(ACBrowserMode (bm));
		}
		else{
			throw runtime_error("corrupted XML file, no browser mode");
		}
		TiXmlText* mediaTypeText=rootHandle.FirstChild( "MediaType" ).FirstChild().Text();
		if (mediaTypeText) {
			std::stringstream tmp2;
			tmp2 << mediaTypeText->ValueStr();
			int mt; //ACMediaType
			tmp2 >> mt;
			this->setMediaType(ACMediaType(mt));
		}
		else{
			throw runtime_error("corrupted XML file, no media type");
		}

		// features vector weights
		// XS TODO check that it equals the number of media
		int n_feat=-1;
		vector<float> fw;

		TiXmlElement* FeaturesWeightsNode=rootHandle.FirstChild( "FeaturesWeights" ).Element();
		if (FeaturesWeightsNode) {
			TiXmlText* FeaturesWeightsText=rootHandle.FirstChild( "FeaturesWeights" ).FirstChild().Text();
			FeaturesWeightsNode->QueryIntAttribute("NumberOfFeatures", &n_feat);
			if (n_feat < 0)
				throw runtime_error("corrupted XML file, wrong number of features weights");
			std::stringstream tmp3;
			tmp3 << FeaturesWeightsText->ValueStr();
			try {
				for (int j=0; j<n_feat; j++) {
					// XS TODO add tests !! on number of features
					float w;
					tmp3 >> w;
					fw.push_back(w);
				}
			}
			catch (...) {
				// attempt to catch potential problems and group them
				throw runtime_error("corrupted XML file, error reading feature weight");
			}
		}

		// 2) change mediacycle settings accordingly
		if (this->media_cycle){
			this->changeMediaType(this->media_type);
			this->media_cycle->changeBrowserMode(this->browser_mode);
		}
		else
			createMediaCycle(this->media_type, this->browser_mode);

		// 3) read the meat of media_cycle (features and plugins)
		media_cycle->readXMLConfigFileCore(rootHandle);
		media_cycle->readXMLConfigFilePlugins(rootHandle);

		// XS TODO check this.
		// should be overwritten if dimensions do not match
		media_cycle->setWeightVector(fw);

		// XML features are not normalized, so we force normalization here
		media_cycle->normalizeFeatures(1);

		// only after loading all XML files:
		this->updateLibrary();
		media_cycle->storeNavigationState();
	}
	catch (const exception& e) {
		this->showError(e);
		return false;
	}
	// no errors occured, no exception caught.
	return true;
}

void ACMultiMediaCycleOsgQt::on_actionSave_XML_triggered(bool checked){
	// XS TODO what is the use of "checked" (in argument above) ?
	this->writeXMLConfig();
}

// this saves the XML file "as is"
// ex: the user quits when all features have'nt been computed yet
void ACMultiMediaCycleOsgQt::writeXMLConfig(string _filename){
	if (! hasMediaCycle()) return;
	else {
		if (_filename=="") {
			// no file name supplied, ask for one
			QString fileName = QFileDialog::getSaveFileName(this, tr("Save Config as XML Library"),"",tr("MediaCycle XML Library (*.xml)"));
			if (fileName.isEmpty()) return;
			QFile file(fileName);

			if (!file.open(QIODevice::WriteOnly)) {
				QMessageBox::warning(this,
									 tr("File error"),
									 tr("Failed to open\n%1").arg(fileName));
			}
			else {
				_filename = fileName.toStdString();
			}
		}
		// filename supplied (either as argument or from QMessageBox above)
		cout << "saving config in XML file: " << _filename << endl;
		media_cycle->saveXMLConfigFile(_filename);
	}
}

// XS TODO: make sure it works if we add a new directory to the existing library ?
void ACMultiMediaCycleOsgQt::on_actionLoad_Media_Directory_triggered(bool checked){

	std::vector<string> directories;

	if (! hasMediaCycle()) return;

	QString select_dir = QFileDialog::getExistingDirectory
	(
	osgViewDock,
	 tr("Open Directory"),
	 "",
	 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	 );

	if (select_dir=="")
		return; // e.g. the user pressed "cancel"
	else
		directories.push_back((string)select_dir.toStdString());


	// check if the user wants segments
	bool do_segments = this->doSegments();
	bool forward_order = true; // only make it false for AudioGarden where media have been presegmented and segments have special names
	int recursive = 1;

// XS TODO to use progress bar, we need to import files one by one...
// so split the importdDirectory into scanDirectory + importFile
//	pb->show();
//	pb->setRange(0, 100);
//	pb->setValue(2);

	// XS TODO threaded version not working for images.
	// not necessary to thread if only few files.
//	if (directories.size() > n_dir_for_threading)
		media_cycle->importDirectoriesThreaded(directories, recursive, forward_order, do_segments);
//	else
//		media_cycle->importDirectories(directories, recursive, forward_order, do_segments);

	directories.empty();


	// SD not working with threaded version
	/*


	int warn_button = 0;
	if (res > 0){
		statusBar()->showMessage(tr("Directory Imported."), 2000);
		media_cycle->normalizeFeatures();
		statusBar()->showMessage(tr("Features Normalized."), 2000);
		this->updateLibrary();
	}
	else if (res==0) {
		warn_button = QMessageBox::warning(osgViewDock, "Warning",
											   "Empty Directory"); //, <#const QString button0Text#>, <#const QString button1Text#>, <#const QString button2Text#>, <#int defaultButtonNumber#>, <#int escapeButtonNumber#>);

	}
	else  {
		warn_button = QMessageBox::warning(osgViewDock, "Error",
											   "Problem Importing Directory"); //, <#const QString button0Text#>, <#const QString button1Text#>, <#const QString button2Text#>, <#int defaultButtonNumber#>, <#int escapeButtonNumber#>);
	}
	statusBar()->clearMessage();
	*/

}

void ACMultiMediaCycleOsgQt::on_actionLoad_Media_Files_triggered(bool checked){

	std::vector<string> directories;

	if (! hasMediaCycle()) return;

	QString fileName;
	QFileDialog dialog(osgViewDock,"Open MediaCycle Media File(s)");
	//CF generating supported file extensions from used media I/O libraries and current media type:
	std::vector<std::string> mediaExt = media_cycle->getExtensionsFromMediaType( media_cycle->getLibrary()->getMediaType() );
	QString mediaExts = "Supported Extensions (";
	std::vector<std::string>::iterator mediaIter = mediaExt.begin();
	for(;mediaIter!=mediaExt.end();++mediaIter){
		if (mediaIter != mediaExt.begin())
			mediaExts.append(" ");
		mediaExts.append("*");
		mediaExts.append(QString((*mediaIter).c_str()));
	}
	mediaExts.append(")");
	dialog.setNameFilter(mediaExts);
	dialog.setFileMode(QFileDialog::ExistingFiles); // ExistingFile(s); "s" is for multiple file handling

	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();

	QStringList::Iterator file = fileNames.begin();
	while(file != fileNames.end()) {
		fileName = *file;
		++file;
		std::cout << "Opening: '" << fileName.toStdString() << "'" << std::endl;

		if (!(fileName.isEmpty())) {
			directories.push_back((string)fileName.toStdString());
		}
	}

	if (!(directories.empty())){

		// check if the user wants segments
		bool do_segments = this->doSegments();
		bool forward_order = true; // only make it false for AudioGarden where media have been presegmented and segments have special names
		int recursive = 1;

		// not necessary to thread if only few files.
//		if (directories.size() > n_dir_for_threading)
			media_cycle->importDirectoriesThreaded(directories, recursive, forward_order, do_segments);
//		else
//			media_cycle->importDirectories(directories, recursive, forward_order, do_segments);

		directories.empty();
	}
}

bool ACMultiMediaCycleOsgQt::doSegments(){
	bool do_segments = false;

	if(use_segmentation){
		int seg_button = QMessageBox::question(this,
			tr("Segmentation"),
			tr("Do you want to segment the media ?"),
			QMessageBox::Yes | QMessageBox::No);
		if (seg_button == QMessageBox::Yes) {
			// XS TODO: check that segmentation algorithms exist
			do_segments = true;
		}
	}	
	return do_segments;
}

void ACMultiMediaCycleOsgQt::on_actionEdit_Config_File_triggered(bool checked){
	cout << "Editing config file with Setting Dialog GUI..." << endl;
	settingsDialog->show();
	settingsDialog->setMediaCycleMainWindow(this);
	settingsDialog->setFocus();
}

bool ACMultiMediaCycleOsgQt::addControlDock(ACAbstractDockWidgetQt* dock)
{
	if (dock == 0){
		std::cerr << "Invalid dock widget" << std::endl;
		return false;
	}

	if (dock->getMediaType() == MEDIA_TYPE_NONE && dock->getClassName()==""){
		std::cerr << "Invalid dock widget type" << std::endl;
		return false;
	}

	for (int d=0;d<dockWidgets.size();d++){
		if (dock->getClassName() == dockWidgets[d]->getClassName()){
			std::cerr << "Dock widget type already added" << std::endl;
			return false;
		}

	}

	if (dock->getClassName() == "ACBrowserControlsClustersNeighborsDockWidgetQt" || dock->getClassName() == "ACBrowserControlsClustersDockWidgetQt"){
		for (int d=0;d<dockWidgets.size();d++){
			if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersNeighborsDockWidgetQt" || dockWidgets[d]->getClassName() == "ACBrowserControlsClustersDockWidgetQt"){
				std::cerr << "Only one browser control dock widget for clusters and/or neighbors can be added." << std::endl;
				return false;
			}
		}
	}

	//XS TODO check indices -- or use push_back
	dockWidgets.resize(dockWidgets.size()+1);
	lastDocksVisibilities.resize(lastDocksVisibilities.size()+1);

	dockWidgets[dockWidgets.size()-1]=dock;

	if( dock->getMediaType() == MEDIA_TYPE_ALL || dock->getMediaType() == media_type ){
		lastDocksVisibilities[lastDocksVisibilities.size()-1]=1;
		this->addDockWidget(Qt::LeftDockWidgetArea,dockWidgets[dockWidgets.size()-1]);
		dockWidgets[dockWidgets.size()-1]->setVisible(true);
		connect(dockWidgets[dockWidgets.size()-1], SIGNAL(visibilityChanged(bool)), this, SLOT(syncControlToggleWithDocks()));
	}
	else {
		lastDocksVisibilities[lastDocksVisibilities.size()-1]=0;
		dockWidgets[dockWidgets.size()-1]->setVisible(false);
	}

	if (dockWidgets[dockWidgets.size()-1]->getClassName()=="ACMediaConfigDockWidgetQt"){
		//CF ugly, use signals?
		connect(((ACMediaConfigDockWidgetQt*)dockWidgets[dockWidgets.size()-1])->getComboDefaultSettings(), SIGNAL(currentIndexChanged(const QString &)), this, SLOT(comboDefaultSettingsChanged()));
	}
	return true;
}

bool ACMultiMediaCycleOsgQt::addControlDock(std::string dock_type)
{
	this->addControlDock(dockWidgetFactory->createDockWidget(this,dock_type));
}

void ACMultiMediaCycleOsgQt::syncControlToggleWithDocks(){
	int docksVisibilitiesSum = 0;

	for (int d=0;d<dockWidgets.size();d++){
		if (dockWidgets[d]->getMediaType() == media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL)
			docksVisibilitiesSum += dockWidgets[d]->isVisible();
	}

	int lastDocksVisibilitiesSum = 0;
	for (int d=0; d<lastDocksVisibilities.size();d++)
		lastDocksVisibilitiesSum += lastDocksVisibilities[d];

	bool controlsToEnable = ui.actionShow_Hide_Controls->isChecked();

	if (controlsToEnable){
		if (docksVisibilitiesSum == 0 && lastDocksVisibilitiesSum == 1 && wasControlsToggleChecked){
			ui.actionShow_Hide_Controls->setChecked(false);
		}
		else if (lastDocksVisibilitiesSum == 0 && !wasControlsToggleChecked){
			for (int d=0;d<dockWidgets.size();d++){
				if (dockWidgets[d]->getMediaType() == media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL)
					dockWidgets[d]->setVisible(true);
			}

		}
		else {
			if (!wasControlsToggleChecked){
				for (int d=0;d<dockWidgets.size();d++){
					if (dockWidgets[d]->getMediaType() == media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL)
						dockWidgets[d]->setVisible((bool)(lastDocksVisibilities[d]));
				}
			}
		}
		for (int d=0;d<dockWidgets.size();d++){
			if (dockWidgets[d]->getMediaType() == media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL)
				lastDocksVisibilities[d]=dockWidgets[d]->isVisible();
		}
	}
	else {
		for (int d=0;d<dockWidgets.size();d++){
			if (dockWidgets[d]->getMediaType() == media_type || dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL)
				dockWidgets[d]->setVisible(false);
		}
	}
	wasControlsToggleChecked = ui.actionShow_Hide_Controls->isChecked();
}

bool ACMultiMediaCycleOsgQt::addAboutDialog(ACAbstractAboutDialogQt* dialog)
{
	if (dialog == 0){
		std::cerr << "Invalid about dialog" << std::endl;
		return false;
	}

	aboutDialog = dialog;

	return true;
}

bool ACMultiMediaCycleOsgQt::addAboutDialog(std::string about_type)
{
	this->addAboutDialog(aboutDialogFactory->createAboutDialog(this,about_type));
}

void ACMultiMediaCycleOsgQt::on_actionHelpAbout_triggered(bool checked) {
	if (aboutDialog==0)
		aboutDialog = new ACAboutDialogQt(this);
	aboutDialog->show();
}

void ACMultiMediaCycleOsgQt::on_actionDetachBrowser_triggered(bool checked) {
	if (osgViewDock->isFloating()){
		osgViewDock->setFloating(false);
	}
	else{
		osgViewDock->setFloating(true);
	}
	compositeOsgView->setFocus();
}

void ACMultiMediaCycleOsgQt::on_actionFullscreen_triggered(bool checked) {
	if (compositeOsgView->isFullScreen() || osgViewDock->isFullScreen() || this->isFullScreen()){
		std::cout << "Not fullscreen" << std::endl;
		//actionShow_Hide_Controls->setChecked
		osgViewDock->setTitleBarWidget(osgViewDockTitleBar);
		if (!osgViewDock->isFloating()){
			this->showNormal();
			compositeOsgView->showNormal();
			ui.actionShow_Hide_Controls->setChecked(true);
			this->syncControlToggleWithDocks();
			ui.menubar->show();
			ui.statusbar->show();
			ui.toolbar->show();
		}
		else{
			if (osgViewDockNormalSize != QRect())
				osgViewDock->setGeometry(osgViewDockNormalSize);
		}
		osgViewDock->showNormal();
	}
	else{

		if (!osgViewDock->isFloating()){
			ui.actionShow_Hide_Controls->setChecked(false);
			this->syncControlToggleWithDocks();
			ui.menubar->hide();
			ui.statusbar->hide();
			ui.toolbar->hide();

			QWidget* lTitleBar = osgViewDock->titleBarWidget();
			QWidget* lEmptyWidget = new QWidget();
			osgViewDock->setTitleBarWidget(lEmptyWidget);
			delete lTitleBar;


			compositeOsgView->showFullScreen();
			this->showFullScreen();
			//this->setFocusPolicy(Qt::StrongFocus);
			//compositeOsgView->showFullScreen();
		}
		else{
			osgViewDockNormalSize = osgViewDock->geometry();

			std::cout << QApplication::desktop()->screenCount() << " screen(s)"<< std::endl;

			std::cout << "Primary screen " << QApplication::desktop()->primaryScreen()<< std::endl;
			//QApplication::desktop()->screenGeometry ( int screen = -1 )<< std::endl;
			QRect geo = QApplication::desktop()->screenGeometry(osgViewDock);
			std::cout << "Browser in screen of size " << geo.width() <<" "<<geo.height()<< std::endl;
			std::cout << "Browser in screen number " << QApplication::desktop()->screenNumber(osgViewDock)<< std::endl;

			QWidget* lTitleBar = osgViewDock->titleBarWidget();
			QWidget* lEmptyWidget = new QWidget();
			osgViewDock->setTitleBarWidget(lEmptyWidget);
			delete lTitleBar;

			osgViewDock->setGeometry(geo);
			compositeOsgView->showFullScreen();
			osgViewDock->showFullScreen();
		}

		std::cout << "Fullscreen" << std::endl;
	}
	compositeOsgView->setFocus();
}

void ACMultiMediaCycleOsgQt::updateLibrary(){
	if (! hasMediaCycle()) return;
	// XS TODO updateLibrary()
	media_cycle->libraryContentChanged();

	media_cycle->setReferenceNode(0);
	// XS TODO this is sooo ugly:
	// XS TODO updateBrowser()
	media_cycle->getBrowser()->setState(AC_CHANGING);

	compositeOsgView->prepareFromBrowser();
	compositeOsgView->prepareFromTimeline();
	//browserOsgView->setPlaying(true);
	media_cycle->setNeedsDisplay(true);

	this->configurePluginDock();

	compositeOsgView->setFocus();
}

void ACMultiMediaCycleOsgQt::configurePluginDock() {
	// do not re-scan the directory for plugins once they have been loaded
	if (!plugins_scanned)
	{
		//CF ugly, use signals?
		// XS TODO change this
		for (int d=0;d<dockWidgets.size();d++){
			if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersNeighborsDockWidgetQt")
				((ACBrowserControlsClustersNeighborsDockWidgetQt*)dockWidgets[d])->configureCheckBoxes();
			if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersDockWidgetQt")
				((ACBrowserControlsClustersDockWidgetQt*)dockWidgets[d])->configureCheckBoxes();
		}
	}
	plugins_scanned = true;
}

// XS TODO when is this used again ??
//void ACMultiMediaCycleOsgQt::addPluginItem(QListWidgetItem *_item){
//	cout << "adding item : " << _item->text().toStdString() << endl;
//	QListWidgetItem * new_item = new QListWidgetItem(*_item);
//	//CF ugly, use signals?
//	// XS TODO change this
//	for (int d=0;d<dockWidgets.size();d++){
//		if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersNeighborsDockWidgetQt")
//			((ACBrowserControlsClustersNeighborsDockWidgetQt*)dockWidgets[d])->getFeaturesListWidget()->addItem(new_item);
//		if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersDockWidgetQt")
//			((ACBrowserControlsClustersDockWidgetQt*)dockWidgets[d])->getFeaturesListWidget()->addItem(new_item);
//	}
//	//////ui.featuresListWidget->addItem(new_item);
//}

// adds the plugins in _library pth via mediaCycle's pluginManager
// keeps track of the plugins added by the Settings Dialog
bool ACMultiMediaCycleOsgQt::addPluginLibrary(string _library){
	bool ok = false;
	if (! hasMediaCycle()) return false;
	if (this->media_cycle->addPluginLibrary(_library)){
		this->plugins_libraries.push_back(_library);
		cout << "added plugin " << _library << endl;
		ok = true;
	}
	if (!ok) throw runtime_error("failed adding plugin library " + _library);
	return ok;
}

bool ACMultiMediaCycleOsgQt::removePluginLibrary(string _library){
	bool ok = false;
	if (! hasMediaCycle()) return false;
	if (this->media_cycle->removePluginLibrary(_library)){
		vector<string>::iterator f = std::find(this->plugins_libraries.begin(), this->plugins_libraries.end(), _library);
		if( f != this->plugins_libraries.end() ) {
			this->plugins_libraries.erase(f);
			cout << "erased plugin " << _library << endl;
			ok = true;
		}
	}
	return ok;
}

ACPluginLibrary* ACMultiMediaCycleOsgQt::getPluginLibrary(string _library) {
	if (! hasMediaCycle()) return 0;
	return this->getMediaCycle()->getPluginLibrary(_library);
}

bool ACMultiMediaCycleOsgQt::removePluginFromLibrary(std::string _plugin_name, std::string _library_path){
	if (! hasMediaCycle()) return 0;
	return this->getMediaCycle()->removePluginFromLibrary(_plugin_name, _library_path);
}


//Return copies of a string with whitespace removed from the right
//http://www2.warwick.ac.uk/fac/sci/physics/research/epp/people/andrew_bennieston/computing/cpp/

std::string ACMultiMediaCycleOsgQt::rstrip(const std::string& s){
	std::string::size_type p = s.find_last_not_of(" \n\r\t");
	return std::string(s, 0, p+1);
}

// synchronize weights with what is loaded in mediacycle
// note: here weights are 1 or 0 (checkbox).
// conversion: 0 remains 0, and value > 0 becomes 1.
void ACMultiMediaCycleOsgQt::synchronizeFeaturesWeights(){
	//CF ugly, use signals?
	for (int d=0;d<dockWidgets.size();d++){
		if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersNeighborsDockWidgetQt")
			((ACBrowserControlsClustersNeighborsDockWidgetQt*)dockWidgets[d])->synchronizeFeaturesWeights();
		if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersDockWidgetQt")
			((ACBrowserControlsClustersDockWidgetQt*)dockWidgets[d])->synchronizeFeaturesWeights();
	}
}

// XS TODO this is a temporary solution -- very ugly
// XS TODO change dockwidgets class !!
void ACMultiMediaCycleOsgQt::configureDockWidgets(ACMediaType _media_type){
//	int index_media = (int)_media_type + 1; // XS change this -- will not work if items in different order!!!
	for (int d=0;d<dockWidgets.size();d++){
//		if (dockWidgets[d]->getClassName() == "ACMediaConfigDockWidgetQt"){
//			((ACMediaConfigDockWidgetQt*)dockWidgets[d])->getComboDefaultSettings()->setCurrentIndex(index_media);
//		}

		if (dockWidgets[d]->getMediaType() == MEDIA_TYPE_ALL || dockWidgets[d]->getMediaType() == _media_type){
			if (this->dockWidgetArea(dockWidgets[d]) == Qt::NoDockWidgetArea){
				this->addDockWidget(Qt::LeftDockWidgetArea,dockWidgets[d]);
				dockWidgets[d]->show();
				lastDocksVisibilities[d]=1;
				connect(dockWidgets[d], SIGNAL(visibilityChanged(bool)), this, SLOT(syncControlToggleWithDocks()));
			}
		}
		else {
			disconnect(dockWidgets[d], SIGNAL(visibilityChanged(bool)), this, SLOT(syncControlToggleWithDocks()));
			if (this->dockWidgetArea(dockWidgets[d]) == Qt::LeftDockWidgetArea){
				this->removeDockWidget(dockWidgets[d]);
			}
			lastDocksVisibilities[d]=0;
		}
	}
}

// load default ("vintage") config for different media.
// 1) creates media_cycle (destroying any previous settings)
// 2) loads default features plugins
// XS assumes for the moment that viewing mmode is clusters
void ACMultiMediaCycleOsgQt::loadDefaultConfig(ACMediaType _media_type, ACBrowserMode _browser_mode){
	this->configureDockWidgets(_media_type);

	string smedia = "none";
	switch (_media_type) {
		case MEDIA_TYPE_3DMODEL:
			#if defined (SUPPORT_3DMODEL)
			smedia="3Dmodel";
			#endif //defined (SUPPORT_3DMODEL)
			break;
		case MEDIA_TYPE_AUDIO:
			#if defined (SUPPORT_AUDIO)
			smedia="audio";
			#endif //defined (SUPPORT_AUDIO)
			break;
		case MEDIA_TYPE_IMAGE:
			#if defined (SUPPORT_IMAGE)
			smedia="image";
			#endif //defined (SUPPORT_IMAGE)
			break;
		case MEDIA_TYPE_VIDEO:
			#if defined (SUPPORT_VIDEO)
			smedia="video";
			#endif //defined (SUPPORT_VIDEO)
			break;
		case MEDIA_TYPE_TEXT:
			#if defined (SUPPORT_TEXT)
			smedia="text";
			#endif //defined (SUPPORT_TEXT)
			break;
		default:
			break;
	}

	if (smedia=="none"){
		this->showError("need to define media type");
		return;
	}
	/*if (_media_type == MEDIA_TYPE_NONE || _media_type == MEDIA_TYPE_MIXED || _media_type == MEDIA_TYPE_ALL){
		cerr <<"need to define media type"<< endl;
		return;
	}*/

	if (this->media_cycle){
		this->changeMediaType(_media_type);
		this->media_cycle->changeBrowserMode(_browser_mode);
	}
	else
		createMediaCycle(_media_type, _browser_mode);

	// -- media-specific features plugin + generic segmentation and visualisation plugins--
	std::string f_plugin, s_plugin, v_plugin;
	std::string s_path = QApplication::applicationDirPath().toStdString();
	
	std::string build_type ("Release");
#ifdef USE_DEBUG
	build_type = "Debug";
#endif //USE_DEBUG
#if defined(__APPLE__)
	#if not defined (USE_DEBUG) and not defined (XCODE) // needs "make install" to be ran to work
		f_plugin = "@executable_path/../MacOS/mc_" + smedia +".dylib";
		v_plugin = "@executable_path/../MacOS/mc_visualisation.dylib";
		s_plugin = "@executable_path/../MacOS/mc_segmentation.dylib";
	#else
		f_plugin = s_path + "/../../../../../../plugins/"+ smedia + "/" + build_type + "/mc_" + smedia +".dylib";
		v_plugin = s_path + "/../../../../../../plugins/visualisation/" + build_type + "/mc_visualisation.dylib";
		s_plugin = s_path + "/../../../../../../plugins/segmentation/" + build_type + "/mc_segmentation.dylib";
	#endif

	// common to all media, but only for mac...
#elif defined (__WIN32__)
	f_plugin = s_path + "\..\..\..\plugins\\" + smedia + "\mc_"+smedia+".dll";
	v_plugin = s_path + "/../../../plugins/visualisation/" + build_type + "/mc_visualisation.dll";
	s_plugin = s_path + "/../../../plugins/segmentation/" + build_type + "/mc_segmentation.dll";
#else
	#if not defined (USE_DEBUG) // needs "make package" to be ran to work
	    f_plugin = "/usr/lib/mc_"+smedia+".so";
	    v_plugin = "/usr/lib/mc_visualisation.so";
	    s_plugin = "/usr/lib/mc_segmentation.so";
    #else
	    f_plugin = s_path + "/../../plugins/"+smedia+"/mc_"+smedia+".so";
	    v_plugin = s_path + "/../../plugins/visualisation/mc_visualisation.so";
	    s_plugin = s_path + "/../../plugins/segmentation/mc_segmentation.so";
    #endif
#endif
	media_cycle->addPluginLibrary(f_plugin);
	media_cycle->addPluginLibrary(s_plugin);
	media_cycle->addPluginLibrary(v_plugin);
	
	if(smedia == "text"){
		media_cycle->setPreProcessPlugin("TextFeatures");
	}
	else{
		media_cycle->setPreProcessPlugin("");
	}	
	
    //CF sorry XD, I need position plugins to debug the segmentation!
    for (int d=0; d<dockWidgets.size(); d++)
    {
        if (dockWidgets[d]->getClassName()=="ACBrowserControlsClustersDockWidgetQt")
        {
            ((ACBrowserControlsClustersDockWidgetQt*)dockWidgets[d])->updatePluginLists();
        }
    }
}


void ACMultiMediaCycleOsgQt::comboDefaultSettingsChanged(){
	for (int d=0;d<dockWidgets.size();d++){
		if (dockWidgets[d]->getClassName()=="ACMediaConfigDockWidgetQt"){
			string mt = ((ACMediaConfigDockWidgetQt*)dockWidgets[d])->getComboDefaultSettings()->currentText().toStdString();

			// custom settings = edit config file
			if (mt == "Custom"){
				cout << "editing configuration file" << endl;
				this->on_actionEdit_Config_File_triggered(true);
				return;
			};

			// default settings : find the right media
			stringToMediaTypeConverter::const_iterator iterm = stringToMediaType.find(mt);
			if( iterm == stringToMediaType.end() ) {
				cout << " media type not found : " << iterm->first << endl;
				return;
			}
			ACMediaType new_media_type = iterm->second;
			cout << iterm->first << " - corresponding media type code : " << new_media_type << endl;
			this->loadDefaultConfig(new_media_type);
		}
	}
}

void ACMultiMediaCycleOsgQt::on_actionClean_triggered(bool checked) {
	this->clean();
}

void ACMultiMediaCycleOsgQt::clean(bool _updategl){
	if (! hasMediaCycle()) return;
	// need to turn all sounds off before leaving
	// do this before cleaning library !!
#if defined (SUPPORT_AUDIO)
	this->media_cycle->muteAllSources();
	//audio_engine->stopAudioEngine();
#endif //defined (SUPPORT_AUDIO)

	this->media_cycle->cleanLibrary();
	this->media_cycle->cleanBrowser();

	//was cleanCheckBoxes()
	for (int d=0;d<dockWidgets.size();d++){
		if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersNeighborsDockWidgetQt") {
			((ACBrowserControlsClustersNeighborsDockWidgetQt*)dockWidgets[d])->cleanCheckBoxes();
		}
		if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersDockWidgetQt") {
			((ACBrowserControlsClustersDockWidgetQt*)dockWidgets[d])->cleanCheckBoxes();
		}
	}

	// XS TODO : remove the boxes specific to the media that was loaded
	// e.g. ACAudioControlDockWidgets
	// modify the DockWidget's API to allow this
	plugins_scanned = false;

	compositeOsgView->clean(_updategl);
	compositeOsgView->setFocus();
}

void ACMultiMediaCycleOsgQt::showError(std::string s){
	int warn_button;
	const QString qs = QString::fromStdString(s);
	warn_button = QMessageBox::warning(osgViewDock, "Error", qs);
	cerr << s << endl;
}

void ACMultiMediaCycleOsgQt::showError(const exception& e) {
	this->showError(e.what());
}

bool ACMultiMediaCycleOsgQt::hasMediaCycle(){
	if (media_cycle == 0) {
		this->showError ("No MediaCycle Instance - Define type of media first");
		return false;
	}
	return true;
}

void ACMultiMediaCycleOsgQt::closeEvent(QCloseEvent *event) {
	// XS - SD TOOD : stop threads
	this->writeQSettings();
	QMessageBox msgBox;
	msgBox.setText("Quitting Application.");
	msgBox.setInformativeText("Do you want to save your media settings and features ?");
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);
	msgBox.setDetailedText ("GUI settings (window size, ...) are saved automatically. Here you can save MediaCycle settings (media type, browser mode, ...).");
	int ret = msgBox.exec();
	bool really_quit = true;
	switch (ret) {
		case QMessageBox::Save:
			this->writeXMLConfig(this->config_file_xml);
			break;
		case QMessageBox::Discard:
			break;
		case QMessageBox::Cancel:
			really_quit = false;
			break;
		default:
			// should never be reached
			break;
	}
	if (really_quit) {
		if(detachedBrowser) delete detachedBrowser;
		QMainWindow::closeEvent(event);
		qDebug("closed application window properly");
	}
	else {
		event->ignore();
		qDebug("Continuing Application");
	}
}

// platform-independent way of reading settings
// ("numediart", "MediaCycle") must be the same as in writeSettings
// The second argument to QSettings::value() is optional and specifies a default value for the setting if there exists none.
bool ACMultiMediaCycleOsgQt::readQSettings() {
	QSettings settings("numediart", "MediaCycle");
	QPoint pos = settings.value("pos").toPoint(); //, QPoint(200, 200)).toPoint();
	QSize size = settings.value("size").toSize(); //, QSize(400, 400)).toSize();
	QString scf = settings.value("configFile").toString();
	bool has_settings = true;
	if (size.isEmpty()) { // e.g. we did settings.clear() or run the app for the 1st time
		has_settings = false;
	}
	else{
		this->resize(size);
		this->move(pos);
		this->config_file_xml = scf.toStdString();
//		this->restoreState(settings.value("windowState").toByteArray());
		has_settings = true;
	}
	return has_settings;
}

// platform-independent way of saving settings
// ("numediart", "MediaCycle") must be the same as in readSettings
bool ACMultiMediaCycleOsgQt::writeQSettings() {
	//QSettings settings(QApplication::applicationDirPath().append(QDir::separator()).append("settings.ini"),
	//				   QSettings::NativeFormat);
	QSettings settings("numediart", "MediaCycle");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
	settings.setValue("configFile", QVariant(QString::fromStdString(this->config_file_xml)));
//	settings.setValue("windowState", saveState());
}

void ACMultiMediaCycleOsgQt::clearQSettings() {
	QSettings settings("numediart", "MediaCycle");
	settings.clear();
}
void ACMultiMediaCycleOsgQt::setDefaultQSettings() {
	this->setGeometry( QStyle::alignedRect(
										   Qt::LeftToRight,
										   Qt::AlignCenter,
										   this->size(),
										   qApp->desktop()->availableGeometry()
										   ));
}

void ACMultiMediaCycleOsgQt::changeMediaType(ACMediaType _media_type){
	// XS TODO turn off current audio engine if switch away from audio
#if defined (SUPPORT_AUDIO)
	if (_media_type == MEDIA_TYPE_AUDIO){
		if (!audio_engine){
			audio_engine = new ACAudioEngine();
		}
		audio_engine->setMediaCycle(media_cycle);
		audio_engine->startAudioEngine();
		compositeOsgView->setAudioEngine(audio_engine);

	}
#endif //defined (SUPPORT_AUDIO)
	this->media_cycle->changeMediaType(this->media_type);
}

void ACMultiMediaCycleOsgQt::useSegmentation(bool _status)
{
	use_segmentation = _status;
}
