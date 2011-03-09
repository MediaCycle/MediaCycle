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

static void mediacycle_callback(char* message, void *user_data) {
	
	ACMultiMediaCycleOsgQt *application = (ACMultiMediaCycleOsgQt*)user_data;
	
	application->mediacycleCallback(message);
}

void ACMultiMediaCycleOsgQt::mediacycleCallback(char* message) {
	if (message=="loaddirstart") {
		statusBar()->showMessage(tr("Loading Directory..."), 0);
	}
	
	if (message=="loaddirfinish") {
		statusBar()->clearMessage();
	}
}

ACMultiMediaCycleOsgQt::ACMultiMediaCycleOsgQt(QWidget *parent) : QMainWindow(parent), 
 features_known(false), plugins_scanned(false)
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
	
	this->activateWindow();
	this->show();
	
	// tries to read settings from previous run
	// if it does not find any, use default (centered) geometry
}

ACMultiMediaCycleOsgQt::~ACMultiMediaCycleOsgQt(){
	this->destroyMediaCycle();
	delete settingsDialog;
	delete aboutDialogFactory;
	delete dockWidgetFactory;
}

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
				this->readXMLConfig(this->config_file_xml);
				this->configureDockWidgets(this->media_type);
				break;
			case QMessageBox::No:
				// then dock widgets will be configured afterwards (in updateLibrary)
				break;
			default:
				// should never be reached
				break;
		}
	}
	else 
		this->setDefaultQSettings();
}

// creates a MediaCycle object (containing the whole application)
// with the appropriate type (audio/image/video/text/mixed/composite/...)
void ACMultiMediaCycleOsgQt::createMediaCycle(ACMediaType _media_type, ACBrowserMode _browser_mode){

	this->media_cycle = new MediaCycle(_media_type,"/tmp/","mediacycle.acl");
	media_cycle->setCallback(mediacycle_callback, (void*)this);

	ui.compositeOsgView->setMediaCycle(this->media_cycle);	
	// SD
	ui.compositeOsgView->prepareFromBrowser();
	this->settingsDialog->setMediaCycle(this->media_cycle);

	// keep track locally of the media and browser modes
	this->media_type = _media_type;
	this->browser_mode = _browser_mode;
	
	#if defined (SUPPORT_AUDIO)
	if (_media_type == MEDIA_TYPE_AUDIO){
		audio_engine = new ACAudioEngine();
		audio_engine->setMediaCycle(media_cycle);
		ui.compositeOsgView->setAudioEngine(audio_engine);
	}	
	#endif //defined (SUPPORT_AUDIO)

	for (int d=0;d<dockWidgets.size();d++){
		dockWidgets[d]->setMediaCycle(media_cycle);
		#if defined (SUPPORT_AUDIO)
			dockWidgets[d]->setAudioEngine(audio_engine);
		#endif //defined (SUPPORT_AUDIO)
		dockWidgets[d]->setOsgView(ui.compositeOsgView);
	}	
}

// destroys the MediaCycle object (containing the whole application)
// it should leave an empty blue frame, just as lauch time
void ACMultiMediaCycleOsgQt::destroyMediaCycle(){
	// XS TODO : remove it from the graphics ?
	#if defined (SUPPORT_AUDIO)
		if (audio_engine) {delete audio_engine; audio_engine = 0;}
	#endif //defined (SUPPORT_AUDIO)
	delete media_cycle;
}

// XS in theory one could select multiple ACL files and concatenate them (not tested yet)
void ACMultiMediaCycleOsgQt::on_actionLoad_ACL_triggered(bool checked){
	if (media_cycle == 0) {
		cerr << "first define the type of application" << endl;
		return;
	}
	
	QString fileName;	
	QFileDialog dialog(this,"Open Library File(s)");
	dialog.setDefaultSuffix ("acl");
	dialog.setNameFilter("Library Files (*.acl)");
	dialog.setFileMode(QFileDialog::ExistingFiles); 
	// changed to ExistingFiles for multiple file handling
	
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	
	QStringList::Iterator file = fileNames.begin();
	while(file != fileNames.end()) {
		fileName = *file;
		std::cout << "Opening ACL file: '" << fileName.toStdString() << "'" << std::endl;
		//fileName = QFileDialog::getOpenFileName(this, "~", );
		
		if (!(fileName.isEmpty())) {
			media_cycle->importACLLibrary(fileName.toStdString());
			std::cout << "ACL file imported" << std::endl;
		}	
		++file;
	}	
		
	// only after loading all ACL files:
	this->updateLibrary();
	
	media_cycle->storeNavigationState(); 
	
	// XS debug
	media_cycle->dumpNavigationLevel();
	media_cycle->dumpLoopNavigationLevels() ;
}

void ACMultiMediaCycleOsgQt::on_actionSave_ACL_triggered(bool checked){
	if (media_cycle == 0) {
		cerr << "first define the type of application" << endl;
		return;
	}
	
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save as MediaCycle Library"),"",tr("MediaCycle Library (*.acl)"));
	QFile file(fileName);
	
	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(this,
							 tr("File error"),
							 tr("Failed to open\n%1").arg(fileName));
	} 
	else {
		string acl_file = fileName.toStdString();
		cout << "saving ACL file: " << acl_file << endl;
		media_cycle->saveACLLibrary(acl_file);
	}
}

// XS in theory one could select multiple XML files and concatenate them (not tested yet)
void ACMultiMediaCycleOsgQt::on_actionLoad_XML_triggered(bool checked){
	this->readXMLConfig();
}

void ACMultiMediaCycleOsgQt::readXMLConfig(string _filename){
	if (_filename==""){
		QString fileName;	
		QFileDialog dialog(this,"Open XML Library File(s)");
		dialog.setDefaultSuffix ("xml");
		dialog.setNameFilter("Library Files (*.xml)");
		dialog.setFileMode(QFileDialog::ExistingFile); 	// change to ExistingFiles for multiple file handling
		QStringList fileNames;
		if (dialog.exec())
			fileNames = dialog.selectedFiles();
		QStringList::Iterator file = fileNames.begin();
		// only one file !
		// while(file != fileNames.end()) {
		fileName = *file;
		_filename=fileName.toStdString();
		
		// std::cout << "Opening XML file: '" << fileName.toStdString() << "'" << std::endl;
		
//		if (!(fileName.isEmpty())) {
//			media_cycle->importXMLLibrary(fileName.toStdString());
//			std::cout << "XML file imported" << std::endl;
//		}	
		//++file;
	}	
	
	// XS TODO add tests !!
	std::cout << "Opening XML config file: '" << _filename << std::endl;

	// 1) read header info
	//TiXmlHandle rootHandle = this->readXMLConfigHeader(_filename);
	if (_filename=="") return;
	TiXmlDocument doc( _filename.c_str() );
	try {
		if (!doc.LoadFile( ))
			throw runtime_error("error reading XML file");
	} catch (const exception& e) {
		cout << e.what( ) << "\n";
		exit(1);
    }	
	
	TiXmlHandle docHandle(&doc);
	TiXmlHandle rootHandle = docHandle.FirstChildElement( "MediaCycle" );
	
	// XS TODO browser mode and media type as string instead of enum
	TiXmlText* browserModeText=rootHandle.FirstChild( "BrowserMode" ).FirstChild().Text();
	std::stringstream tmp;
	// XS TODO if (browserModeText)
	tmp << browserModeText->ValueStr();
	int bm; // ACBrowserMode
	tmp >> bm;
	this->setBrowserMode(ACBrowserMode (bm));
	
	TiXmlText* mediaTypeText=rootHandle.FirstChild( "MediaType" ).FirstChild().Text();
	std::stringstream tmp2;
	tmp2 << mediaTypeText->ValueStr();
	int mt; //ACMediaType
	tmp2 >> mt;
	this->setMediaType(ACMediaType(mt));	
	
	// XS TODO only creat if it did not exist yet -- otherwise change media type
	// 2) create an instance of mediacycle
	if (this->media_cycle){ 
		this->media_cycle->changeMediaType(this->media_type);
		this->media_cycle->changeBrowserMode(this->browser_mode);
	}
	else
		createMediaCycle(this->media_type, this->browser_mode);

	//this->configureDockWidgets(this->media_type);

	// 3) read the meat of this instance
	media_cycle->readXMLConfigFileCore(rootHandle);
	media_cycle->readXMLConfigFilePlugins(rootHandle);

	// XS ---- TEST ----
	media_cycle->normalizeFeatures(1);
	
	// only after loading all XML files:
	this->updateLibrary();
	
	media_cycle->storeNavigationState(); 
	
	// XS debug
	//media_cycle->dumpNavigationLevel();
	//media_cycle->dumpLoopNavigationLevels() ;
}

// read in informationnecessary to create an instance of MediaCycle
// i.e. browser mode, media type, ...
//TiXmlHandle ACMultiMediaCycleOsgQt::readXMLConfigHeader(string _fname){
//	// XS TODO: this is not great, it's the same code as in MediaCycle
//	if (_fname=="") return 0;
//	TiXmlDocument doc( _fname.c_str() );
//	try {
//		if (!doc.LoadFile( ))
//			throw runtime_error("error reading XML file");
//	} catch (const exception& e) {
//		cout << e.what( ) << "\n";
//		exit(1);
//    }	
//	
//	TiXmlHandle docHandle(&doc);
//	TiXmlHandle rootHandle = docHandle.FirstChildElement( "MediaCycle" );
//	
//	// XS TODO browser mode and media type as string instead of enum
//	TiXmlText* browserModeText=rootHandle.FirstChild( "BrowserMode" ).FirstChild().Text();
//	std::stringstream tmp;
//	tmp << browserModeText->ValueStr();
//	int bm; // ACBrowserMode
//	tmp >> bm;
//	this->setBrowserMode(ACBrowserMode (bm));
//	
//	TiXmlText* mediaTypeText=rootHandle.FirstChild( "MediaType" ).FirstChild().Text();
//	std::stringstream tmp2;
//	tmp2 << mediaTypeText->ValueStr();
//	int mt; //ACMediaType
//	tmp2 >> mt;
//	this->setMediaType(ACMediaType(mt));
//	return rootHandle;
//}

void ACMultiMediaCycleOsgQt::on_actionSave_XML_triggered(bool checked){
	// XS TODO what is the use of checked ?
	this->writeXMLConfig();
}

// this saves the XML file "as is" 
// ex: the user quits when all features have'nt been computed yet
// XS TODO make sure this is fine
void ACMultiMediaCycleOsgQt::writeXMLConfig(string _filename){
	if (media_cycle == 0) {
		cerr << "first define the type of application" << endl;
		return;
	}
	
	else {
		if (_filename=="") {
			QString fileName = QFileDialog::getSaveFileName(this, tr("Save Config as XML Library"),"",tr("MediaCycle XML Library (*.xml)"));
			QFile file(fileName);
			
			if (!file.open(QIODevice::WriteOnly)) {
				QMessageBox::warning(this,
									 tr("File error"),
									 tr("Failed to open\n%1").arg(fileName));
			} 
			else {
				_filename = fileName.toStdString();
				cout << "saving config in XML file: " << _filename << endl;
				media_cycle->saveXMLConfigFile(_filename);
			}			
		}
	}
}

// XS TODO: make sure it works if we add a new directory to the existing library ?
void ACMultiMediaCycleOsgQt::on_actionLoad_Media_Directory_triggered(bool checked){
	
	std::vector<string> directories;
	
	if (! hasMediaCycle()) return; 

//	configurePluginDock();
	
	QString selectDir = QFileDialog::getExistingDirectory
	(
	this, 
	 tr("Open Directory"),
	 "",
	 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	 );
	
	if (selectDir=="") return; // e.g. the user pressed "cancel"
	// check if the user wants segments
	bool do_segments = false;
	bool forward_order = true; // only make it false for AudioGarden where media have been presegmented and segments have special names
	int recursive = 1;	
	
	int seg_button = QMessageBox::question(this,
									   tr("Segmentation"),
									   tr("Do you want to segment the media ?"),
									   QMessageBox::Yes | QMessageBox::No);
	if (seg_button == QMessageBox::Yes) {
		// XS TODO: check that segmentation algorithms exist
		do_segments = true;
	}
	
// XS TODO to use progress bar, we need to import files one by one...
// so split the importdDirectory into scanDirectory + importFile
//	pb->show();
//	pb->setRange(0, 100);
//	pb->setValue(2);
	
	directories.push_back((string)selectDir.toStdString());
	
	// non - threaded version
	//int res = media_cycle->importDirectory(selectDir.toStdString(), recursive, forward_order, do_segments);
	
	media_cycle->importDirectoriesThreaded(directories, recursive, forward_order, do_segments);
//	media_cycle->importDirectories(directories, recursive, forward_order, do_segments);

	directories.empty();
	
	//usleep(2000000);
	//media_cycle->setReferenceNode(0);
	//ui.compositeOsgView->prepareFromBrowser();
	//ui.compositeOsgView->prepareFromTimeline();
	//media_cycle->setNeedsDisplay(true);
	//ui.compositeOsgView->setFocus();
	
	this->updateLibrary();
	
	// SD not working with threaded version
	/*

	
	int res = media_cycle->importDirectory(selectDir.toStdString(), recursive, forward_order, do_segments, element3);
	int warn_button = 0;
	if (res > 0){
		statusBar()->showMessage(tr("Directory Imported."), 2000);
		media_cycle->normalizeFeatures();
		statusBar()->showMessage(tr("Features Normalized."), 2000);
		this->updateLibrary();
	}
	else if (res==0) {
		warn_button = QMessageBox::warning(this, "Warning", 
											   "Empty Directory"); //, <#const QString button0Text#>, <#const QString button1Text#>, <#const QString button2Text#>, <#int defaultButtonNumber#>, <#int escapeButtonNumber#>);

	}
	else  {
		warn_button = QMessageBox::warning(this, "Error", 
											   "Problem Importing Directory"); //, <#const QString button0Text#>, <#const QString button1Text#>, <#const QString button2Text#>, <#int defaultButtonNumber#>, <#int escapeButtonNumber#>);
	}
	statusBar()->clearMessage();
	*/

}

void ACMultiMediaCycleOsgQt::on_actionLoad_Media_Files_triggered(bool checked){
	
	std::vector<string> directories;
	
	if (! hasMediaCycle()) return; 

	configurePluginDock();
	
	QString fileName;
	QFileDialog dialog(this,"Open MediaCycle Media File(s)");
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
	//dialog.setDefaultSuffix ("png");
	//dialog.setNameFilter("Image Files (*.jpg *.png *.tif *.tiff)");
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
			//media_cycle->importDirectory(fileName.toStdString(), 0);
			//media_cycle->normalizeFeatures();
			//media_cycle->libraryContentChanged(); no, this is in updatelibrary
			//std::cout <<  fileName.toStdString() << " imported" << std::endl;
		}
	}
	
	if (!(directories.empty())){
		
		// check if the user wants segments
		bool do_segments = false;
		bool forward_order = true; // only make it false for AudioGarden where media have been presegmented and segments have special names
		int recursive = 1;	
		
		int seg_button = QMessageBox::question(this,
											   tr("Segmentation"),
											   tr("Do you want to segment the media ?"),
											   QMessageBox::Yes | QMessageBox::No);
		if (seg_button == QMessageBox::Yes) {
			// XS TODO: check that segmentation algorithms exist
			do_segments = true;
		}
		
		media_cycle->importDirectoriesThreaded(directories, recursive, forward_order, do_segments);
		directories.empty();
	}	
	
	// XS do this only after loading all files (it was in the while loop) !
	// SD not needed anymore
	//this->updateLibrary();
}

void ACMultiMediaCycleOsgQt::on_actionEdit_Config_File_triggered(bool checked){
	cout << "Editing config file with Setting Dialog GUI..." << endl;
	settingsDialog->show();
	settingsDialog->setMediaCycleMainWindow(this);
	settingsDialog->setFocus();
}

void ACMultiMediaCycleOsgQt::on_actionLoad_Config_File_triggered(bool checked){
// XS TODO : no !!! should just load the XML with features and everything.

//	QFileDialog dialog(this,"Open Config File");
//	dialog.setDefaultSuffix ("config");
//	dialog.setNameFilter("Config Files (*.config)");
//	dialog.setFileMode(QFileDialog::ExistingFile); 
//	
//	QStringList fileNames;
//	if (dialog.exec())
//		fileNames = dialog.selectedFiles();
//	QStringList::Iterator file = fileNames.begin(); // only ONE file in fact !
//	QString fileName  = *file;
//	this->config_file_xml = fileName.toStdString();
//	cout << "Loading config file from Setting Dialog GUI : " << this->config_file << endl;
//	ifstream load_config_file(this->config_file.c_str());
	
}


void ACMultiMediaCycleOsgQt::on_actionSave_Config_File_triggered(bool checked){
	// XS TODO :  should just save the XML with features and everything.
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

void ACMultiMediaCycleOsgQt::updateLibrary(){
	if (! hasMediaCycle()) return; 
	media_cycle->libraryContentChanged(); 	
	media_cycle->setReferenceNode(0);
	// XSCF 250310 added these 3
	// media_cycle->pushNavigationState(); // XS 250810 removed this
	//media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
	
	// XS TODO this is sooo ugly:
	// XS TODO updateBrowser()
	media_cycle->getBrowser()->setState(AC_CHANGING);
	
	ui.compositeOsgView->prepareFromBrowser();
	ui.compositeOsgView->prepareFromTimeline();
	//browserOsgView->setPlaying(true);
	media_cycle->setNeedsDisplay(true);

	this->configurePluginDock();

	ui.compositeOsgView->setFocus();
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
	
void ACMultiMediaCycleOsgQt::addPluginItem(QListWidgetItem *_item){
	cout << "adding item : " << _item->text().toStdString() << endl;
	QListWidgetItem * new_item = new QListWidgetItem(*_item);
	//CF ugly, use signals?
	// XS TODO change this
	for (int d=0;d<dockWidgets.size();d++){
		if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersNeighborsDockWidgetQt")
			((ACBrowserControlsClustersNeighborsDockWidgetQt*)dockWidgets[d])->getFeaturesListWidget()->addItem(new_item);
		if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersDockWidgetQt")
			((ACBrowserControlsClustersDockWidgetQt*)dockWidgets[d])->getFeaturesListWidget()->addItem(new_item);
	}
	//////ui.featuresListWidget->addItem(new_item);
}

// adds the plugins in _library pth via mediaCycle's pluginManager
// keeps track of the plugins added by the Settings Dialog
void  ACMultiMediaCycleOsgQt::addPluginsLibrary(string _library){
	if (! hasMediaCycle()) return; 
	if (this->media_cycle->addPluginLibrary(_library)){
		this->plugins_libraries.push_back(_library);
		cout << "added plugin " << _library << endl;
	}
}

void  ACMultiMediaCycleOsgQt::removePluginsLibrary(string _library){
	if (! hasMediaCycle()) return; 
	if (this->media_cycle->removePluginLibrary(_library)){
		vector<string>::iterator f = std::find(this->plugins_libraries.begin(), this->plugins_libraries.end(), _library);
		  if( f != this->plugins_libraries.end() )
			  this->plugins_libraries.erase(f);
		cout << "erased plugin " << _library << endl;
	}
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
		default:
			break;
	}
	
	if (smedia=="none"){
		cerr <<"need to define media type"<< endl;
		return;
	}
	/*if (_media_type == MEDIA_TYPE_NONE || _media_type == MEDIA_TYPE_MIXED || _media_type == MEDIA_TYPE_ALL){
		cerr <<"need to define media type"<< endl;
		return;
	}*/
	
	if (this->media_cycle){ 
		this->media_cycle->changeMediaType(_media_type);
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
	f_plugin = s_path + "/../../plugins/"+smedia+"/mc_"+smedia+".so";
	v_plugin = s_path + "/../../plugins/visualisation/mc_visualisation.so";
	s_plugin = s_path + "/../../plugins/segmentation/mc_segmentation.so";
#endif
	media_cycle->addPluginLibrary(f_plugin);
	media_cycle->addPluginLibrary(v_plugin);
	media_cycle->addPluginLibrary(s_plugin);
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
			
			// telling the setting dialog what we have done
			this->settingsDialog->setMediaType(mt);
		}	
	}
}

void ACMultiMediaCycleOsgQt::on_actionClean_triggered(bool checked) {
	if (! hasMediaCycle()) return; 
//	this->media_cycle->cleanUserLog(); //use cleanBrowser instead

	this->media_cycle->cleanLibrary();
	this->media_cycle->cleanBrowser();

//	this->updateLibrary(); // XS TODO : or cleanLibrary, so we don't call libraryContentChanged each time
	
	//was cleanCheckBoxes()
	for (int d=0;d<dockWidgets.size();d++){
		if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersNeighborsDockWidgetQt") {
			((ACBrowserControlsClustersNeighborsDockWidgetQt*)dockWidgets[d])->cleanCheckBoxes();
		}
		if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersDockWidgetQt") {
			((ACBrowserControlsClustersDockWidgetQt*)dockWidgets[d])->cleanCheckBoxes();
		}
	}	
	plugins_scanned = false;
	
	ui.compositeOsgView->clean();
	ui.compositeOsgView->setFocus();
}	

void  ACMultiMediaCycleOsgQt::showError(std::string s){
	int warn_button;
	const QString qs = QString::fromStdString(s);
	warn_button = QMessageBox::warning(this, "Error", qs);
	cerr << s << endl;
}

bool ACMultiMediaCycleOsgQt::hasMediaCycle(){
	if (media_cycle == 0) {
		this->showError ("First define the type of application");
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
	//QString scf = QString::fromStdString(this->config_file_xml);
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
