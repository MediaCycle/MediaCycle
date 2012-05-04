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

#ifdef OSG_LIBRARY_STATIC
#include <osgViewer/GraphicsWindow>

USE_GRAPHICSWINDOW()
#endif

ACQProgressBar::ACQProgressBar(QWidget *parent)
:   QProgressBar(parent){
	this->setMinimum(0);
	this->setFormat("Loading file %v/%m");// due to Apple standards the progress bar won't display this, it does under Ubuntu
}


void ACQProgressBar::loading_started()
{
	this->reset();
	this->show();
}
	
void ACQProgressBar::loading_finished()
{
	this->reset();
	this->hide();
}

void ACQProgressBar::loading_file(int media_id, int dir_size)
{
	this->setMaximum(dir_size);
	this->setValue(media_id);
}

void ACMultiMediaCycleOsgQt::mediaImported(int n,int nTot){
	std::string send = "";
	if (n==0) {
		send = "Loading Directory...";
		emit mediacycle_message_changed(QString(send.c_str()));
		emit loading_started();
	}
	if (n==nTot) {
		send = "";
		emit mediacycle_message_changed(QString(send.c_str()));
		emit loading_finished();
		this->updateLibrary();
	}
	
	if(n<nTot){

		if(media_cycle->getLibrary()->getParentIds().size()==1){
			dockWidgetsManager->updatePluginsSettings();
		}
		if (progressBar){
			emit loading_file(n,nTot);
			stringstream status_message;
			status_message << "Loading File " << n << "/" << nTot;
			send = status_message.str();
			emit mediacycle_message_changed(QString(send.c_str()));
		}	
	}
}

// ----------- 

ACMultiMediaCycleOsgQt::ACMultiMediaCycleOsgQt(QWidget *parent) : QMainWindow(parent),features_known(false),detachedBrowser(0),	aboutDialog(0),controlsDialog(0),compositeOsgView(0),osgViewDock(0),osgViewDockWidget(0),osgViewDockLayout(0),osgViewDockTitleBar(0),progressBar(0)
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

	this->use_segmentation_default = true;
	this->use_segmentation_current = true;
	this->use_feature_extraction = true;
	this->use_visualization_plugins = true;
	
	this->auto_connect_osc = false;
	
	// Apple bundled *.app, just look for bundled osg plugins
	#ifndef USE_DEBUG
	#if defined __APPLE__ and not defined (XCODE)
		QDir dir(QApplication::applicationDirPath());
		dir.cdUp();
		dir.cd("PlugIns");
		osgDB::Registry::instance()->setLibraryFilePathList(dir.absolutePath().toStdString());
	#endif
	#endif

    dockWidgetsManager = new ACDockWidgetsManagerQt(this);
    connect( dockWidgetsManager, SIGNAL( toggleControls(bool) ),
            ui.actionToggle_Controls, SLOT(setChecked(bool) ) );
	aboutDialogFactory = new ACAboutDialogFactoryQt();
    settingsDialog = new ACSettingsDialogQt(this);

	// This is required to populate the available file extensions list at startup
	// until we clean mediacycle instead of deleting/creating it at every media type change.
	ACMediaFactory::getInstance();
	//ACMediaFactory::getInstance().listSupportedMediaExtensions();
	/* #ifdef USE_DEBUG
	 ACMediaFactory::getInstance().listUncheckedMediaExtensions();
	 #endif //def USE_DEBUG*/
	// Since it is time consuming, we might want to add a splash screen with progress bar at startup?

	// Docked osg browser
	ui.centralwidget->hide();
	
	osgViewDockWidget = new QWidget; // it seems this intermediary widget is required to set a layout to a dock widget
	osgViewDockLayout = new QVBoxLayout;
	osgViewDockLayout->setSpacing(0); // no blank space between the progress bar and the osg view
	osgViewDockLayout->setContentsMargins(0,0,0,0);// no unnecessary corners in the osg view dock widget (this supersedes the OS theme defaults)
    osgViewDockWidget->setLayout(osgViewDockLayout);

	osgViewDock = new QDockWidget(this);
    osgViewDock->setSizePolicy ( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	osgViewDock->setWidget(osgViewDockWidget);
	osgViewDock->setAllowedAreas(Qt::RightDockWidgetArea);
	osgViewDock->setFeatures(QDockWidget::DockWidgetFloatable);
	osgViewDock->setWindowTitle("Browser");
	this->addDockWidget(Qt::RightDockWidgetArea,osgViewDock);
	osgViewDockTitleBar = osgViewDock->titleBarWidget();
	osgViewDockNormalSize = QRect();
	
	compositeOsgView = new ACOsgCompositeViewQt();
	compositeOsgView->setSizePolicy ( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	osgViewDockLayout->addWidget(compositeOsgView);

	progressBar = new ACQProgressBar(); //QProgressBar();		
	osgViewDockLayout->addWidget(progressBar);
	
	// to make window appear on top of others.
	this->activateWindow();
	this->showMaximized();//this->show();
	
	progressBar->hide();
	#ifdef __APPLE__ // due to Apple standards the progress bar won't display any overlaid message, while it is the case under Ubuntu
	connect( this, SIGNAL( mediacycle_message_changed(QString) ),
			this->statusBar(), SLOT(message(QString) ) );
	#endif
	connect( this, SIGNAL( loading_started() ),
			this->progressBar, SLOT(loading_started() ) );
	connect( this, SIGNAL( loading_finished() ),
			this->progressBar, SLOT(loading_finished() ) );
	connect( this, SIGNAL( loading_file(int,int) ),
			this->progressBar, SLOT(loading_file(int,int) ) );
	
	// Debugging accentuated media filenames
	/*#ifdef USE_DEBUG
	qDebug() << "System Locale name:"      << QLocale::system().name();
	qDebug() << "Qt codecForCStrings:" << QTextCodec::codecForCStrings();
	qDebug() << "Qt codecForLocale:"   << QTextCodec::codecForLocale()->name();
	#endif*/
	if (!QTextCodec::codecForCStrings())
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));//CF hack for CF to load accents

	// XS reminder: need to call configureSettings from the application main.

    // Adapt the help panel action to support standard keys
    ui.actionHelpAbout->setShortcuts(QList<QKeySequence>(ui.actionHelpAbout->shortcuts()) << QKeySequence(QKeySequence::HelpContents));

    // Keeps actions (mouse/keyboard shortcuts) active in fullscreen mode when bars are hidden
	this->addActions(ui.toolbar->actions());
	this->addActions(ui.menubar->actions());
	this->addActions(ui.menuFile->actions());
	this->addActions(ui.menuConfig->actions());
	this->addActions(ui.menuDisplay->actions());
	this->addActions(ui.menuHelp->actions());	
    //this->addActions(compositeOsgView->actions());

    this->setAcceptDrops(true); // for drag and drop
}

ACMultiMediaCycleOsgQt::~ACMultiMediaCycleOsgQt(){
	delete settingsDialog;
	delete aboutDialogFactory;
    delete dockWidgetsManager;
    if (aboutDialog) delete aboutDialog;
	if (controlsDialog) delete controlsDialog;
    if (detachedBrowser) delete detachedBrowser;
	if (compositeOsgView) delete compositeOsgView;
	if (progressBar) delete progressBar;
	if (osgViewDockLayout) delete osgViewDockLayout;
	if (osgViewDockWidget) delete osgViewDockWidget;
	if (osgViewDockTitleBar) delete osgViewDockTitleBar;
	if (osgViewDock) delete osgViewDock;
	this->destroyMediaCycle();
}

void ACMultiMediaCycleOsgQt::setMediaType(ACMediaType _mt)
{
    if(this->media_type != _mt)
        this->clean(); //CF this ensures that the browser is cleaned at every media type change, and kept when adding new media files of the same type
    this->media_type = _mt;
    dockWidgetsManager->setMediaType(_mt);
}

// tries to read settings from previous run
// if it does not find any, use default (centered) geometry
void ACMultiMediaCycleOsgQt::configureSettings(){
	if (this->readQSettings()){
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
                if (this->readXMLConfig(this->config_file_xml)){
                    dockWidgetsManager->changeMediaType(this->media_type);
                }
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
		this->setDefaultQSettings();
}

// creates a MediaCycle object (containing the whole application)
// with the appropriate type (audio/image/video/text/mixed/composite/...)
void ACMultiMediaCycleOsgQt::createMediaCycle(ACMediaType _media_type, ACBrowserMode _browser_mode){

	this->media_cycle = new MediaCycle(_media_type,"/tmp/","mediacycle.xml");
	//media_cycle->setCallback(mediacycle_callback, (void*)this);
	media_cycle->addListener(this);
	compositeOsgView->setMediaCycle(this->media_cycle);
	// SD
	compositeOsgView->prepareFromBrowser();
	compositeOsgView->prepareFromTimeline();
	
	// keep track locally of the media and browser modes
	this->media_type = _media_type;
	this->browser_mode = _browser_mode;
	
	#if defined (SUPPORT_AUDIO)
	// XS TODO add checks on existing audio_engine ?
	// normally should not be any
	if (_media_type == MEDIA_TYPE_AUDIO || _media_type == MEDIA_TYPE_MIXED){
		audio_engine = new ACAudioEngine();
		audio_engine->setMediaCycle(media_cycle);
		audio_engine->startAudioEngine();
		compositeOsgView->setAudioEngine(audio_engine);
		dockWidgetsManager->updateAudioEngine(audio_engine);
	}
	#endif //defined (SUPPORT_AUDIO)
	
	dockWidgetsManager->updateMediaCycle(media_cycle);
    dockWidgetsManager->updateOsgView(compositeOsgView);
	
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
            //CF todo propagate to docks
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
	
	ACPluginManager* acpl = media_cycle->getPluginManager();
	if (acpl->getFeaturesPluginsSize(media_type) &&/*acpl->getFeaturesPlugins()->getSize(media_type)>0 &&*/ !use_feature_extraction){ // if no feature extraction plugin was loaded before opening the XML and if the XML loaded one
		this->showError("Feature extraction plugin(s) now loaded again. Importing media files now enabled.");
		this->switchFeatureExtraction(true);
	}
    dockWidgetsManager->changeMediaType(this->media_type);
    dockWidgetsManager->updatePluginsSettings();
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

	if(!use_feature_extraction){
		this->showError("No feature extraction plugin loaded. Can't import media files.");
		return;
	}
		
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

	if (!(directories.empty())){
		this->importDirectoriesThreaded(directories);
		directories.empty();
	}
	

// XS TODO to use progress bar, we need to import files one by one...
// so split the importdDirectory into scanDirectory + importFile
//	pb->show();
//	pb->setRange(0, 100);
//	pb->setValue(2);

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
	
	if(!use_feature_extraction){
		this->showError("No feature extraction plugin loaded. Can't import media files.");
		return;
	}
	
	std::vector<string> directories;

	if (! hasMediaCycle()) return;

	QString fileName;
	QFileDialog dialog(osgViewDock,"Open MediaCycle Media File(s)");
	
	//CF generating supported file extensions from used media I/O libraries and current media type:
	std::vector<std::string> mediaExt = media_cycle->getExtensionsFromMediaType( media_cycle->getLibrary()->getMediaType() );
	
	if(mediaExt.size()==0){
		this->showError("No file extensions supported for this media type. Please check the media factory. Can't import media files.");
		return;
	}
	
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
		this->importDirectoriesThreaded(directories);
		directories.empty();
	}
}


// import (threaded or not) moved here instead of in MediaCycle
// because of problems with Qt and threads
// e.g., QCoreApplication::sendPostedEvents: Cannot send posted events for objects in another thread
void ACMultiMediaCycleOsgQt::importDirectoriesThreaded(vector<string> directories) {
	// check if the user wants segments
	bool do_segments = this->doSegments();
	bool forward_order = true; // only make it false for AudioGarden where media have been presegmented and segments have special names
	int recursive = 1;
	
	// XS TODO : for progress bar and threading, see:
	// http://hopf.chem.brandeis.edu/yanglingfa/Qt/threading/index.html
	
	// not necessary to thread if only few files.
    int n_dir_for_threading = 0;
    if (directories.size() > n_dir_for_threading)
        media_cycle->importDirectoriesThreaded(directories, recursive, forward_order, do_segments);
    else{
        media_cycle->importDirectories(directories, recursive, forward_order, do_segments);
        // XS TODO FIXME !!
        // does not belong here for threaded version
        this->updateLibrary();
    }
}

bool ACMultiMediaCycleOsgQt::doSegments(){
	bool do_segments = false;

	if(use_segmentation_current){
		int seg_button = QMessageBox::question(this,
			tr("Segmentation"),
			tr("Do you want to segment the media ?"),
			QMessageBox::Yes | QMessageBox::No);
		if (seg_button == QMessageBox::Yes) {
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

void ACMultiMediaCycleOsgQt::on_actionEdit_Input_Controls_triggered(bool checked){

    if (controlsDialog == 0){
        //this->addActions(compositeOsgView->actions());
        //std::cout << "this->actions().size() " << this->actions().size() << std::endl;
        ///QList<QAction *> _actions = QList<QAction *> (this->actions());
        //std::cout << "_actions.size() " << _actions.size() << std::endl;
        ///_actions.append(compositeOsgView->actions());
        //std::cout << "compositeOsgView->actions().size() " << compositeOsgView->actions().size() << std::endl;
        //std::cout << "_actions.size() " << _actions.size() << std::endl;
        controlsDialog = new ACInputControlsDialogQt(this);
        controlsDialog->addActions(this->actions());
        controlsDialog->addInputActions(compositeOsgView->getInputActions());
        //controlsDialog = new ACInputControlsDialogQt(this->actions(),this);
    }

    if(controlsDialog->isVisible()){
        controlsDialog->hide();
    }
    else{
        controlsDialog->show();
    }
}

bool ACMultiMediaCycleOsgQt::addControlDock(std::string dock_type)
{
    return dockWidgetsManager->addControlDock(dock_type);
}

void ACMultiMediaCycleOsgQt::on_actionToggle_Controls_triggered(bool controlsToEnable){
    dockWidgetsManager->updateDocksVisibility(controlsToEnable);
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
		osgViewDock->setTitleBarWidget(osgViewDockTitleBar);
		if (!osgViewDock->isFloating()){
			this->showNormal();
			compositeOsgView->showNormal();
           ui.actionToggle_Controls->setChecked(true);
           this->on_actionToggle_Controls_triggered(true);
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
           ui.actionToggle_Controls->setChecked(false);
           this->on_actionToggle_Controls_triggered(false);
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

void ACMultiMediaCycleOsgQt::dragEnterEvent(QDragEnterEvent *event)
{
     std::cout <<"<drop content>" << std::endl;
     event->acceptProposedAction();
}

void ACMultiMediaCycleOsgQt::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void ACMultiMediaCycleOsgQt::dropEvent(QDropEvent *event)
{
     const QMimeData *mimeData = event->mimeData();

     /*if (mimeData->hasImage()) {
              setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
          } else if (mimeData->hasHtml()) {
              setText(mimeData->html());
              setTextFormat(Qt::RichText);
          } else if (mimeData->hasText()) {
              setText(mimeData->text());
              setTextFormat(Qt::PlainText);
          } else*/ if (mimeData->hasUrls()) {
              QList<QUrl> urlList = mimeData->urls();
              QString text;
              for (int i = 0; i < urlList.size() && i < 32; ++i) {
                  QString url = urlList.at(i).path();
                  text += url + QString("\n");
                  std::cout << url.toStdString() << std::endl;
              }
              //setText(text);
          } /*else {
              setText(tr("Cannot display data"));
          }*/

     /*if (mimeData){
         foreach (QString format, mimeData->formats()) {
             std::cout << format.toStdString() << std::endl;
         }
    }*/
     event->acceptProposedAction();
}

void ACMultiMediaCycleOsgQt::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
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
    dockWidgetsManager->updatePluginsSettings();
	compositeOsgView->setFocus();
}

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

std::string ACMultiMediaCycleOsgQt::getPluginPathFromBaseName(std::string basename)
{
    std::string s_path = QApplication::applicationDirPath().toStdString();
    std::string plugins_path(""),plugin_subfolder(""),plugin_ext("");

    std::string build_type ("Release");
    #ifdef USE_DEBUG
        build_type = "Debug";
    #endif //USE_DEBUG

    #if defined(__APPLE__)
        plugin_ext = ".dylib";
        #if not defined (USE_DEBUG) and not defined (XCODE) // needs "make install" to be ran to work
            plugins_path = "@executable_path/../MacOS/";
            plugin_subfolder = "";
        #else
            #if defined(XCODE)
                plugins_path = s_path + "/../../../../../../plugins/";
                plugin_subfolder = basename + "/" + build_type + "/";
            #else
                plugins_path = s_path + "/../../../../../plugins/";
                plugin_subfolder = basename + "/";
            #endif
        #endif
    #elif defined (__WIN32__)
        plugin_ext = ".dll";
        plugins_path = s_path + "\\";
        plugin_subfolder = "";
    #else // Linux
        plugin_ext = ".so";
        #if not defined (USE_DEBUG) // needs "make package" to be ran to work
            plugins_path = "/usr/lib/"; // or a prefix path from CMake?
            plugin_subfolder = "";
        #else
            plugins_path = s_path + "/../../plugins/";
            plugin_subfolder = basename + "/";
        #endif
    #endif
    return plugins_path + plugin_subfolder + "mc_" + basename + plugin_ext;
}

int ACMultiMediaCycleOsgQt::loadPluginFromBaseName(std::string basename)
{
    if (media_cycle)
        return media_cycle->addPluginLibrary(this->getPluginPathFromBaseName(basename));
    else
        return -1;
}

int ACMultiMediaCycleOsgQt::tryLoadFeaturePluginFromBaseName(std::string basename)
{
    if (!media_cycle)
        return -1;

    //CF test if basename is a mediatype?

    std::string plugin = this->getPluginPathFromBaseName(basename);
    int n_elements = media_cycle->addPluginLibrary(plugin);
    if( n_elements == -1){
        this->showError("Couldn't load the " + basename + " feature extraction plugin. Importing media files might work only by loading XML library files.");
        this->switchFeatureExtraction(false);
        return n_elements;
    }
    else{
        if(!use_feature_extraction){ // if no feature extraction plugin could be loaded before this change of media type
            this->showError(basename + " feature extraction plugin now loaded again. Importing media files now enabled.");
        }
        this->switchFeatureExtraction(true);
        return n_elements;
    }
}

// load default configs for different media.
// 1) creates media_cycle (destroying any previous settings)
// 2) loads default features plugins
// XS assumes for the moment that the browsing mode is clusters
void ACMultiMediaCycleOsgQt::loadDefaultConfig(ACMediaType _media_type, ACBrowserMode _browser_mode){
	ACMediaType previous_media_type = this->media_type;

	string smedia = ACMediaFactory::getInstance().getLowCaseStringFromMediaType(_media_type);
	string sMedia = ACMediaFactory::getInstance().getNormalCaseStringFromMediaType(this->media_type);
	
	if (smedia=="none"){
		this->showError("need to define media type");
		return;
	}
    //if (_media_type == MEDIA_TYPE_NONE || _media_type == MEDIA_TYPE_MIXED || _media_type == MEDIA_TYPE_ALL){
    //	cerr <<"need to define media type"<< endl;
    //	return;
    //}
	
	// Testing the presence of FFmpeg plugin for OSG before loading the default config
	if(_media_type == MEDIA_TYPE_VIDEO){
		osg::ref_ptr<osgDB::ReaderWriter> movReaderWriter(0),ffmpegReaderWriter(0), pdfReaderWriter(0);
		std::string osg_plugin_error ="";
		
		if(_media_type == MEDIA_TYPE_VIDEO){
			ffmpegReaderWriter = osgDB::Registry::instance()->getReaderWriterForExtension("ffmpeg");
			movReaderWriter = osgDB::Registry::instance()->getReaderWriterForExtension("mov");
			osg_plugin_error = "Video plugins for OpenSceneGraph (FFmpeg or QuickTime or QTKit) are absent but necessary for interactive video navigation. Please install it or contact the MediaCycle team.";
		}
		if (_media_type == MEDIA_TYPE_VIDEO && !ffmpegReaderWriter && !movReaderWriter){
             dockWidgetsManager->resetMediaType(this->media_type);
			ffmpegReaderWriter = 0; movReaderWriter = 0; pdfReaderWriter = 0;
			this->showError(osg_plugin_error);
			return;
		}
		ffmpegReaderWriter = 0; movReaderWriter = 0; pdfReaderWriter = 0;
	}

	if (this->media_cycle){
		this->changeMediaType(_media_type);
		this->media_cycle->changeBrowserMode(_browser_mode);
	}
	else
		createMediaCycle(_media_type, _browser_mode);

	
	// Try to load feature extraction plugins:
	std::string f_plugin("");
	
#if defined (SUPPORT_MULTIMEDIA)
			
	if (_media_type==MEDIA_TYPE_MIXED){

        // Feature plugins
        #if defined (SUPPORT_AUDIO)
        this->tryLoadFeaturePluginFromBaseName("audio");
        #endif //defined (SUPPORT_AUDIO)

        #if defined (SUPPORT_TEXT)
        //media_cycle->setPreProcessPlugin("TextFeaturesSparse");
        this->tryLoadFeaturePluginFromBaseName("text");
        //this->tryLoadFeaturePluginFromBaseName("text_sparse");
        #endif //defined (SUPPORT_TEXT)

        #if defined (SUPPORT_VIDEO)
        this->tryLoadFeaturePluginFromBaseName("video");
        #endif //defined (SUPPORT_VIDEO)

        #if defined (SUPPORT_IMAGE)
        this->tryLoadFeaturePluginFromBaseName("image");
        #endif //defined (SUPPORT_IMAGE)

        #if defined (SUPPORT_3DMODEL)
        this->tryLoadFeaturePluginFromBaseName("3Dmodel");
        #endif //defined (SUPPORT_3DMODEL)

        #if defined (SUPPORT_ARCHIPEL)
		//Archipel Plugin
        string arch_plugin = this->getPluginPathFromBaseName("archipel");
		if(media_cycle->addPluginLibrary(arch_plugin) == -1){
			this->showError("Couldn't load the archipel data extraction plugin. ");
			this->switchFeatureExtraction(false);
		}
		else{
			media_cycle->setMediaReaderPlugin("ArchipelReader");
			media_cycle->setActiveMediaType("text");
		}
        #endif //defined (SUPPORT_ARCHIPEL)
	}
	else {
    #endif //defined (SUPPORT_MULTIMEDIA)
        if(smedia == "text"){
            this->tryLoadFeaturePluginFromBaseName("text");
        }
        else{
            this->tryLoadFeaturePluginFromBaseName(smedia);
        }
    #if defined (SUPPORT_MULTIMEDIA)
	}
    #endif //defined (SUPPORT_MULTIMEDIA)

    // Try to load segmentation plugins:
    std::string s_plugin("");
    s_plugin = this->getPluginPathFromBaseName("segmentation");
	
	if(this->use_segmentation_default && ACMediaFactory::getInstance().isMediaTypeSegmentable(_media_type)){
		if(media_cycle->addPluginLibrary(s_plugin) == -1){
			this->showError("Couldn't load the segmentation plugin. Segmentation disabled.");
			this->switchSegmentation(false);
		}
		else 
			this->switchSegmentation(true);
	}
	else
		this->switchSegmentation(false);

    // Try to load visualisation plugins:
    std::string v_plugin("");
    if(smedia == "text"){
        v_plugin = this->getPluginPathFromBaseName("visualisation");
    }
    else{
        v_plugin = this->getPluginPathFromBaseName("visualisation");
    }

	if(media_cycle->addPluginLibrary(v_plugin) == -1){
		this->showError("Couldn't load the visualization plugin. Default KMeans clustering and Propeller positioning will be used.");
		this->switchPluginVisualizations(false);
	}
	else{
		this->switchPluginVisualizations(true);
	}	
	
    // Define the preprocessing plugins:
	if(smedia == "text"){
		media_cycle->setPreProcessPlugin("TextFeatures");
		//media_cycle->setPreProcessPlugin("TextFeatures");
	}
	else{
		media_cycle->setPreProcessPlugin("");
	}	
	
    // update the plugin lists of the browser control dock through configureDockWidget
    dockWidgetsManager->changeMediaType(_media_type);
	this->setMediaType(_media_type);
    dockWidgetsManager->resetPluginsSettings();
}

void ACMultiMediaCycleOsgQt::comboDefaultSettingsChanged(QString media){
    string mt = media.toStdString();
        // custom settings = edit config file
        if (mt == "Custom"){
            cout << "editing configuration file" << endl;
            this->on_actionEdit_Config_File_triggered(true);
            return;
        }

        if (mt == "-- Config --"){
            return;
        }

        // default settings : find the right media
        stringToMediaTypeConverter::const_iterator iterm = stringToMediaType.find(mt);
        if( iterm == stringToMediaType.end() ) {
            cout << " media type not found : " << iterm->first << endl;
            return;
        }
        ACMediaType new_media_type = iterm->second;
        cout << iterm->first << " - corresponding media type code : " << new_media_type << endl;

        if (this->media_type != new_media_type)
            this->loadDefaultConfig(new_media_type);
        else
            return;
}

void ACMultiMediaCycleOsgQt::on_actionClean_triggered(bool checked) {
	this->clean();
}

void ACMultiMediaCycleOsgQt::clean(bool _updategl){
    if(!media_cycle) return;
    // need to turn all sounds off before leaving
	// do this before cleaning library !!
#if defined (SUPPORT_AUDIO)
	this->media_cycle->muteAllSources();
	//audio_engine->stopAudioEngine();
#endif //defined (SUPPORT_AUDIO)

	this->media_cycle->cleanLibrary();
	this->media_cycle->cleanBrowser();
    dockWidgetsManager->resetPluginsSettings();

	statusBar()->clearMessage();
	progressBar->reset();
	progressBar->hide();
	
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
        //this->showError ("No MediaCycle Instance - Define type of media first");

        // get a list of the available media types
        QStringList mediaTyped;
        std::vector< std::string > mediaTypes = ACMediaFactory::getInstance().listAvailableMediaTypes();
        for (std::vector< std::string >::iterator mediaType = mediaTypes.begin(); mediaType!=mediaTypes.end(); ++mediaType)
            mediaTyped << QString((*mediaType).c_str());

        // popup a dialog asking to set the media type
        bool ok;
        QString mediaType =  QInputDialog::getItem(this,tr("Choose the media type"),tr("Media type:"), mediaTyped, 0, false, &ok);
        if (ok && !mediaType.isEmpty()){
            dockWidgetsManager->setMediaType( ACMediaFactory::getInstance().guessMediaTypeFromString(mediaType.toStdString()) );
            this->comboDefaultSettingsChanged(mediaType);
            return true;
        }

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
	if (_media_type == MEDIA_TYPE_AUDIO || _media_type == MEDIA_TYPE_MIXED){
		if (!audio_engine){
			audio_engine = new ACAudioEngine();
			media_cycle->setAutoPlay(1);//Seneffe
		}
		audio_engine->setMediaCycle(media_cycle);
		audio_engine->startAudioEngine();
		compositeOsgView->setAudioEngine(audio_engine);
	}
#endif //defined (SUPPORT_AUDIO)
	this->media_cycle->changeMediaType(_media_type);
    dockWidgetsManager->setMediaType(_media_type);
}

void ACMultiMediaCycleOsgQt::useSegmentationByDefault(bool _status)
{
	use_segmentation_default = _status;
}

void ACMultiMediaCycleOsgQt::switchSegmentation(bool _status)
{
	if (use_segmentation_default && ACMediaFactory::getInstance().isMediaTypeSegmentable(media_type) )
		use_segmentation_current = _status;		
	else
		use_segmentation_current = false;
}

void ACMultiMediaCycleOsgQt::switchFeatureExtraction(bool _status)
{
	use_feature_extraction = _status;
}

void ACMultiMediaCycleOsgQt::switchPluginVisualizations(bool _status)
{
	use_visualization_plugins = _status;
}

void ACMultiMediaCycleOsgQt::autoConnectOSC(bool _status)
{
	auto_connect_osc = _status;
    dockWidgetsManager->autoConnectOSC(_status);
}
