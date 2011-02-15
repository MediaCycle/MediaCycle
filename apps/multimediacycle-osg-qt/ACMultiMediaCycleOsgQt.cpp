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

ACMultiMediaCycleOsgQt::ACMultiMediaCycleOsgQt(QWidget *parent) : QMainWindow(parent), 
 features_known(false), plugins_scanned(false),library_loaded(false), config_file("")
{
	ui.setupUi(this); // first thing to do
	this->media_type = MEDIA_TYPE_NONE;
	this->browser_mode = AC_MODE_NONE;
	this->media_cycle = NULL;
	this->audio_engine = NULL;
	
	//ui.compositeOsgView->move(0,20);
	//	ui.compositeOsgView->prepareFromBrowser();
	//browserOsgView->setPlaying(true);
	
	// Docks
	//////mediaConfig = new ACMediaConfigDockWidgetQt();
	//////this->addDockWidget(Qt::LeftDockWidgetArea,mediaConfig);
	/////connect(comboDefaultSettings, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(comboDefaultSettingsChanged()));
	connect(ui.comboDefaultSettings, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(comboDefaultSettingsChanged()));
	///////browserControls = new ACBrowserControlsClustersNeighborsDockWidgetQt();
	///////this->addDockWidget(Qt::LeftDockWidgetArea,browserControls);
	audioControls = new ACAudioControlsDockWidgetQt();
	audioControls->setVisible(false);
	videoControls = new ACVideoControlsDockWidgetQt();
	videoControls->setVisible(false);
	//////lastDocksVisibilities.resize(3); //docks: Media Config, Browser, OSC
	lastDocksVisibilities.resize(2); //docks: Media Config, Browser
	for (int d=0; d<lastDocksVisibilities.size();d++)
		lastDocksVisibilities[d] = 1;
	connect(ui.actionShow_Hide_Controls, SIGNAL(triggered()), this, SLOT(syncControlToggleWithDocks()));
	connect(ui.dockWidgetMediaConfig, SIGNAL(visibilityChanged(bool)), this, SLOT(syncControlToggleWithDocks()));
	connect(ui.dockWidgetBrowser, SIGNAL(visibilityChanged(bool)), this, SLOT(syncControlToggleWithDocks()));
	//////connect(ui.dockWidgetOSC, SIGNAL(visibilityChanged(bool)), this, SLOT(syncControlToggleWithDocks()));
	wasControlsToggleChecked = true;
	
	connect(ui.actionLoad_Media_Directory, SIGNAL(triggered()), this, SLOT(loadMediaDirectory()));
	connect(ui.actionLoad_Media_Files, SIGNAL(triggered()), this, SLOT(loadMediaFiles()));
	connect(ui.actionSave_ACL, SIGNAL(triggered()), this, SLOT(saveACLFile()));
	connect(ui.actionLoad_ACL, SIGNAL(triggered()), this, SLOT(loadACLFile()));
	connect(ui.actionSave_XML, SIGNAL(triggered()), this, SLOT(saveXMLFile()));
	connect(ui.actionLoad_XML, SIGNAL(triggered()), this, SLOT(loadXMLFile()));
	connect(ui.actionEdit_Config_File, SIGNAL(triggered()), this, SLOT(editConfigFile()));
	connect(ui.actionSave_Config_File, SIGNAL(triggered()), this, SLOT(saveConfigFile()));
	connect(ui.actionLoad_Config_File, SIGNAL(triggered()), this, SLOT(loadConfigFile()));
	
	connect(ui.actionClean, SIGNAL(triggered()), this, SLOT(on_pushButtonClean_clicked()));

	connect(ui.featuresListWidget, SIGNAL(itemClicked(QListWidgetItem*)),this, SLOT(modifyListItem(QListWidgetItem*)));

	//	// connect spinBox and slider
	//	connect(ui.spinBoxClusters, SIGNAL(valueChanged(int)), this, SLOT(spinBoxClustersValueChanged(int)));
	//	connect(ui.sliderClusters, SIGNAL(valueChanged(int)), ui.spinBoxClusters , SIGNAL(valueChanged(int)));
	//	
	
	// uses another window for settings = editing the config file
	settingsDialog = new SettingsDialog(parent);
	settingsDialog->setMediaCycleMainWindow(this);
	
	// progress bar
	//pb = new QProgressBar(statusBar());
	//	pb->setTextVisible(false);
	//	pb->hide();
	//	statusBar()->addPermanentWidget(pb);
	
	this->show();
}

ACMultiMediaCycleOsgQt::~ACMultiMediaCycleOsgQt(){
	this->destroyMediaCycle();
}

// creates a MediaCycle object (containing the whole application)
// with the appropriate type (audio/image/video/text/mixed/composite/...)
void ACMultiMediaCycleOsgQt::createMediaCycle(ACMediaType _media_type, ACBrowserMode _browser_mode){
	this->media_cycle = new MediaCycle(_media_type,"/tmp/","mediacycle.acl");
	ui.compositeOsgView->setMediaCycle(this->media_cycle);	
	this->settingsDialog->setMediaCycle(this->media_cycle);

	// keep track locally of the media and browser modes
	this->media_type = _media_type;
	this->browser_mode = _browser_mode;
	
	audio_engine = new ACAudioEngine();
	audio_engine->setMediaCycle(media_cycle);
	ui.compositeOsgView->setAudioEngine(audio_engine);
	
	audioControls->setMediaCycle(media_cycle);
	audioControls->setAudioEngine(audio_engine);
	videoControls->setMediaCycle(media_cycle);
	videoControls->setOsgView(ui.compositeOsgView);
}

// destroys the MediaCycle object (containing the whole application)
// it should leave an empty blue frame, just as lauch time
void ACMultiMediaCycleOsgQt::destroyMediaCycle(){
	// XS TODO : remove it from the graphics ?
	if (audio_engine) delete audio_engine;
	delete media_cycle;
}

// XS in theory one could select multiple ACL files and concatenate them (not tested yet)
void ACMultiMediaCycleOsgQt::loadACLFile(){
	if (media_cycle == NULL) {
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

void ACMultiMediaCycleOsgQt::saveACLFile(){
	if (media_cycle == NULL) {
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
void ACMultiMediaCycleOsgQt::loadXMLFile(){
	if (media_cycle == NULL) {
		cerr << "first define the type of application" << endl;
		return;
	}
	
	QString fileName;	
	QFileDialog dialog(this,"Open XML Library File(s)");
	dialog.setDefaultSuffix ("xml");
	dialog.setNameFilter("Library Files (*.xml)");
	dialog.setFileMode(QFileDialog::ExistingFiles); 
	// changed to ExistingFiles for multiple file handling
	
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	
	QStringList::Iterator file = fileNames.begin();
	while(file != fileNames.end()) {
		fileName = *file;
		std::cout << "Opening XML file: '" << fileName.toStdString() << "'" << std::endl;
		
		if (!(fileName.isEmpty())) {
			media_cycle->importXMLLibrary(fileName.toStdString());
			std::cout << "XML file imported" << std::endl;
		}	
		++file;
	}	
	
	// only after loading all XML files:
	this->updateLibrary();
	
	media_cycle->storeNavigationState(); 
	
	// XS debug
	media_cycle->dumpNavigationLevel();
	media_cycle->dumpLoopNavigationLevels() ;
}

void ACMultiMediaCycleOsgQt::saveXMLFile(){
	if (media_cycle == NULL) {
		cerr << "first define the type of application" << endl;
		return;
	}
	
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save as XML Library"),"",tr("MediaCycle XML Library (*.xml)"));
	QFile file(fileName);
	
	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(this,
							 tr("File error"),
							 tr("Failed to open\n%1").arg(fileName));
	} 
	else {
		string acl_file = fileName.toStdString();
		cout << "saving XML file: " << acl_file << endl;
		media_cycle->saveXMLLibrary(acl_file);
	}
}

// XS TODO: make sure it works if we add a new directory to the existing library ?
void ACMultiMediaCycleOsgQt::loadMediaDirectory(){
	if (! hasMediaCycle()) return; 
	statusBar()->showMessage(tr("Loading Directory..."), 0);

	QString selectDir = QFileDialog::getExistingDirectory
	(
	 this, 
	 tr("Open Directory"),
	 "",
	 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	 );
	
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
	int res = media_cycle->importDirectory(selectDir.toStdString(), recursive, forward_order, do_segments);
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

}

void ACMultiMediaCycleOsgQt::loadMediaFiles(){
	if (! hasMediaCycle()) return; 

	QString fileName;
	QFileDialog dialog(this,"Open MediaCycle Image File(s)");
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
		//fileName = QFileDialog::getOpenFileName(this, "~", );
		
		if (!(fileName.isEmpty())) {
			media_cycle->importDirectory(fileName.toStdString(), 0);
			//media_cycle->normalizeFeatures();
			//media_cycle->libraryContentChanged(); no, this is in updatelibrary
			std::cout <<  fileName.toStdString() << " imported" << std::endl;
		}
	}
	// XS do this only after loading all files (it was in the while loop) !
	this->updateLibrary();
}

void ACMultiMediaCycleOsgQt::editConfigFile(){
	cout << "Editing config file with Setting Dialog GUI..." << endl;
	settingsDialog->show();
	settingsDialog->setMediaCycleMainWindow(this);
	settingsDialog->setFocus();
}

void ACMultiMediaCycleOsgQt::loadConfigFile(){
	QFileDialog dialog(this,"Open Config File");
	dialog.setDefaultSuffix ("config");
	dialog.setNameFilter("Config Files (*.config)");
	dialog.setFileMode(QFileDialog::ExistingFile); 
	
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	QStringList::Iterator file = fileNames.begin();
	QString fileName  = *file;
	this->config_file = fileName.toStdString();
	cout << "Loading config file from Setting Dialog GUI : " << this->config_file << endl;
	ifstream load_config_file(this->config_file.c_str());
	
	string comment;
		
	string s_media_type;
	std::getline(load_config_file, s_media_type, '|');
	getline(load_config_file, comment);
	this->media_type = ACMediaType (atoi(s_media_type.c_str()));
	cout << "media type : " << this->media_type << endl;
	
	string s_browser_mode;
	getline(load_config_file, s_browser_mode, '|');
	getline(load_config_file, comment);
	this->browser_mode = ACBrowserMode (atoi(s_browser_mode.c_str()));
	cout << "browser mode : " << this->browser_mode << endl;
	
	this->createMediaCycle(this->media_type, this->browser_mode);

	// rstrip : strips the white spaces or tabs that can appear at the right of the name
	string splugin, splugin_rstrip;
	while(!std::getline(load_config_file, splugin, '|').eof()) {
		cout << "plugin : " << splugin << endl;
		splugin_rstrip= this->rstrip(splugin);
		this->addPluginsLibrary(splugin_rstrip);
		if (getline(load_config_file, comment).eof()) break;
	}
	
	// XS TODO: add ACL file, ...
	// structure the load/save with XML.
	load_config_file.close();

}

//http://doc.qt.nokia.com/qt-maemo-4.6/mainwindows-application.html
//Saving a file is very similar to loading one. Here, the QFile::Text flag ensures that on Windows, "\n" is converted into "\r\n" to conform to the Windows convension.
bool ACMultiMediaCycleOsgQt::saveFile(const QString &fileName) {
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Application"),
							 tr("Cannot write file %1:\n%2.")
							 .arg(fileName)
							 .arg(file.errorString()));
		return false;
	}
	
	
	this->config_file = fileName.toStdString();
	ofstream save_config_file (this->config_file.c_str());
	
	save_config_file << setw(20) << this->media_type   << " | type of media" << endl;
	save_config_file << setw(20) << this->browser_mode << " | browsing mode" << endl;
	
	vector<string>::iterator iter;
	for (iter = plugins_libraries.begin(); iter != plugins_libraries.end(); iter++) {
		save_config_file << (*iter) << " | plugins library " << endl;
	}
	
	statusBar()->showMessage(tr("File saved"), 2000);
	return true;
}

bool ACMultiMediaCycleOsgQt::saveConfigFile(){

	QString _configFile = QFileDialog::getSaveFileName(this);
	if (_configFile.isEmpty())
		return false;
	
	return saveFile(_configFile);
}

void ACMultiMediaCycleOsgQt::syncControlToggleWithDocks(){
	int docksVisibilitiesSum = 0;
	docksVisibilitiesSum += ui.dockWidgetMediaConfig->isVisible();
	docksVisibilitiesSum += ui.dockWidgetBrowser->isVisible();
	//docksVisibilitiesSum += ui.dockWidgetOSC->isVisible();
	switch (media_type) {
		case MEDIA_TYPE_AUDIO:
			docksVisibilitiesSum += audioControls->isVisible();
			break;
		case MEDIA_TYPE_IMAGE:
			break;
		case MEDIA_TYPE_VIDEO:
			docksVisibilitiesSum += videoControls->isVisible();
			break;
		default:
			break;
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
			ui.dockWidgetMediaConfig->setVisible(true);
			ui.dockWidgetBrowser->setVisible(true);
			//ui.dockWidgetOSC->setVisible(true);
			switch (media_type) {
				case MEDIA_TYPE_AUDIO:
					audioControls->setVisible(true);
					break;
				case MEDIA_TYPE_IMAGE:
					break;
				case MEDIA_TYPE_VIDEO:
					videoControls->setVisible(true);
					break;
				default:
					break;
			}
		}	
		else {
			if (!wasControlsToggleChecked){
				ui.dockWidgetMediaConfig->setVisible((bool)(lastDocksVisibilities[0]));
				ui.dockWidgetBrowser->setVisible((bool)(lastDocksVisibilities[1]));
				//ui.dockWidgetOSC->setVisible((bool)(lastDocksVisibilities[2]));
				switch (media_type) {
					case MEDIA_TYPE_AUDIO:
						audioControls->setVisible((bool)(lastDocksVisibilities[2]));
						break;
					case MEDIA_TYPE_IMAGE:
						break;
					case MEDIA_TYPE_VIDEO:
						videoControls->setVisible((bool)(lastDocksVisibilities[2]));
						break;
					default:
						break;
				}
			}
		}
		lastDocksVisibilities[0]=ui.dockWidgetMediaConfig->isVisible();
		lastDocksVisibilities[1]=ui.dockWidgetBrowser->isVisible();
		//lastDocksVisibilities[2]=ui.dockWidgetOSC->isVisible();
		switch (media_type) {
			case MEDIA_TYPE_AUDIO:
				lastDocksVisibilities[2]=audioControls->isVisible();
				break;
			case MEDIA_TYPE_IMAGE:
				break;
			case MEDIA_TYPE_VIDEO:
				lastDocksVisibilities[2]=videoControls->isVisible();
				break;
			default:
				break;
		}
	}
	else {
		ui.dockWidgetMediaConfig->setVisible(false);
		ui.dockWidgetBrowser->setVisible(false);
		//ui.dockWidgetOSC->setVisible(false);
		switch (media_type) {
			case MEDIA_TYPE_AUDIO:
				audioControls->setVisible(false);
				break;
			case MEDIA_TYPE_IMAGE:
				break;
			case MEDIA_TYPE_VIDEO:
				videoControls->setVisible(false);
				break;
			default:
				break;
		}
	}
	wasControlsToggleChecked = ui.actionShow_Hide_Controls->isChecked();
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
	
	// do not re-scan the directory for plugins once they have been loaded
//	if (!plugins_scanned) this->configureCheckBoxes();
	
	library_loaded = true;
	ui.compositeOsgView->setFocus();
}

// for synchronous display of values 
void ACMultiMediaCycleOsgQt::on_sliderClusters_sliderReleased(){
	if (! hasMediaCycle()) return; 
	ui.spinBoxClusters->setValue(ui.sliderClusters->value());
}

void ACMultiMediaCycleOsgQt::spinBoxClustersValueChanged(int _value){
	if (! hasMediaCycle()) return; 
	
	ui.sliderClusters->setValue(_value); 	// for synchronous display of values 
	std::cout << "ClusterNumber: " << _value << std::endl;
	if (library_loaded){
		media_cycle->setClusterNumber(_value);
		// XSCF251003 added this
		media_cycle->updateDisplay(true); //XS 250310 was: media_cycle->updateClusters(true);
		// XS 310310 removed media_cycle->setNeedsDisplay(true); // now in updateDisplay
		ui.compositeOsgView->updateTransformsFromBrowser(1.0);
	}
	//ui.compositeOsgView->setFocus();
}

void ACMultiMediaCycleOsgQt::addPluginItem(QListWidgetItem *_item){
	cout << "adding item : " << _item->text().toStdString() << endl;
	QListWidgetItem * new_item = new QListWidgetItem(*_item);
	ui.featuresListWidget->addItem(new_item);
}

void ACMultiMediaCycleOsgQt::modifyListItem(QListWidgetItem *item) {
	// XS check
	cout << item->text().toStdString() << endl; // isselected...
	cout << ui.featuresListWidget->currentRow() << endl;
	// end XS check 
	
	if (library_loaded){
		float w;
		if (item->checkState() == Qt::Unchecked) w = 0.0;
		else w = 1.0 ;
		int f =  ui.featuresListWidget->currentRow(); // index of selected feature
		media_cycle->setWeight(f,w);
		media_cycle->updateDisplay(true); 
		//XS 250310 was: media_cycle->updateClusters(true);
		// XS250310 removed mediacycle->setNeedsDisplay(true); // now in updateDisplay
		ui.compositeOsgView->updateTransformsFromBrowser(0.0); 
	}
}

// synchronize weights with what is loaded in mediacycle
// note: here weights are 1 or 0 (checkbox).
// conversion: 0 remains 0, and value > 0 becomes 1.
void ACMultiMediaCycleOsgQt::synchronizeFeaturesWeights(){
	if (! hasMediaCycle()) return; 
	vector<float> w = media_cycle->getWeightVector();
	int nw = w.size();
	if (nw==0){
		cout << "features not yet computed from plugins; setting all weights to 0" << endl;
		for (int i=0; i< ui.featuresListWidget->count(); i++){
			ui.featuresListWidget->item(i)->setCheckState (Qt::Unchecked);
		}
		return;
	}
	else if (ui.featuresListWidget->count() != nw){
		cerr << "Warning: Checkboxes in GUI do not match Features in MediaCycle" << endl;
		cerr << ui.featuresListWidget->count() << "!=" << nw << endl;
		return;
		//exit(1);
	}
	else {
		for (int i=0; i< nw; i++){
			if (w[i]==0) 
				ui.featuresListWidget->item(i)->setCheckState (Qt::Unchecked);
			else
				ui.featuresListWidget->item(i)->setCheckState (Qt::Checked);		
		}
	}
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

// load default ("vintage") config for different media.
// 1) creates media_cycle (destroying any previous settings)
// 2) loads default features plugins
// XS assumes for the moment that viewing mmode is clusters 
void ACMultiMediaCycleOsgQt::loadDefaultConfig(ACMediaType _media_type, ACBrowserMode _browser_mode){
	disconnect(audioControls, SIGNAL(visibilityChanged(bool)), this, SLOT(syncControlToggleWithDocks()));
	disconnect(videoControls, SIGNAL(visibilityChanged(bool)), this, SLOT(syncControlToggleWithDocks()));
	this->removeDockWidget(audioControls);
	this->removeDockWidget(videoControls);
	string smedia = "none";
	switch (_media_type) {
		case MEDIA_TYPE_AUDIO:
			smedia="audio";
			this->addDockWidget(Qt::LeftDockWidgetArea,audioControls);
			lastDocksVisibilities.resize(3); //docks: Media Config, Browser, audioControls
			for (int d=0; d<lastDocksVisibilities.size();d++)
				lastDocksVisibilities[d] = 1;
			connect(audioControls, SIGNAL(visibilityChanged(bool)), this, SLOT(syncControlToggleWithDocks()));
			audioControls->show();
			break;
		case MEDIA_TYPE_IMAGE:
			smedia="image";
			lastDocksVisibilities.resize(2); //docks: Media Config, Browser
			for (int d=0; d<lastDocksVisibilities.size();d++)
				lastDocksVisibilities[d] = 1;
			break;
		case MEDIA_TYPE_VIDEO:
			smedia="video";
			this->addDockWidget(Qt::LeftDockWidgetArea,videoControls);
			lastDocksVisibilities.resize(3); //docks: Media Config, Browser, videoControls
			for (int d=0; d<lastDocksVisibilities.size();d++)
				lastDocksVisibilities[d] = 1;
			connect(videoControls, SIGNAL(visibilityChanged(bool)), this, SLOT(syncControlToggleWithDocks()));
			videoControls->show();
			break;
		default:
			break;
	} 
	
	if (smedia=="none"){
		cerr <<"need to define media type"<< endl;
		return;
	}
	if (media_cycle) destroyMediaCycle();
	createMediaCycle(_media_type, _browser_mode);

	// -- media-specific features plugin + generic segmentation and visualisation plugins--
	std::string f_plugin, s_plugin, v_plugin;
	std::string s_path = QApplication::applicationDirPath().toStdString();
	
	std::string build_type ("Release");
#ifdef USE_DEBUG
	build_type = "Debug";
#endif //USE_DEBUG
#if defined(__APPLE__)	
	f_plugin = s_path + "/../../../plugins/"+ smedia + "/" + build_type + "/mc_" + smedia +".dylib";
	v_plugin = s_path + "/../../../plugins/visualisation/" + build_type + "/mc_visualisation.dylib";
	s_plugin = s_path + "/../../../plugins/segmentation/" + build_type + "/mc_segmentation.dylib";

	// common to all media, but only for mac...
#elif defined (__WIN32__)
	f_plugin = s_path + "\..\..\..\plugins\\" + smedia + "\mc_"+smedia+".dll";
	v_plugin = s_path + "/../../../plugins/visualisation/" + build_type + "/mc_visualisation.dll";
	s_plugin = s_path + "/../../../plugins/segmentation/" + build_type + "/mc_segmentation.dll";
#else
	f_plugin = s_path + "/../../../plugins/"+smedia+"/mc_"+smedia+".so";
	v_plugin = s_path + "/../../../plugins/visualisation/" + build_type + "/mc_visualisation.so";
	s_plugin = s_path + "/../../../plugins/segmentation/" + build_type + "/mc_segmentation.so";
#endif
	media_cycle->addPluginLibrary(f_plugin);
	media_cycle->addPluginLibrary(v_plugin);
	media_cycle->addPluginLibrary(s_plugin);
}


void ACMultiMediaCycleOsgQt::comboDefaultSettingsChanged(){
	string mt = ui.comboDefaultSettings->currentText().toStdString();
	
	// custom settings = edit config file
	if (mt == "Custom"){
		cout << "editing configuration file" << endl;
		this->editConfigFile();
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

void ACMultiMediaCycleOsgQt::on_pushButtonClean_clicked() {
	if (! hasMediaCycle()) return; 
//	this->media_cycle->cleanUserLog(); //use cleanBrowser instead

	this->media_cycle->cleanLibrary();
	this->media_cycle->cleanBrowser();

//	this->updateLibrary(); // XS TODO : or cleanLibrary, so we don't call libraryContentChanged each time
	this->cleanCheckBoxes();
	this->library_loaded = false;
	ui.compositeOsgView->clean();
	ui.compositeOsgView->setFocus();
	

}	

void ACMultiMediaCycleOsgQt::on_pushButtonRecenter_clicked() {
	if (! hasMediaCycle()) return; 
	this->media_cycle->setCameraRecenter();
}	

void ACMultiMediaCycleOsgQt::on_pushButtonBack_clicked() {
	if (! hasMediaCycle()) return; 
	this->media_cycle->goBack();
	this->synchronizeFeaturesWeights();
	//	ui.navigationLineEdit->setText(QString::number(media_cycle->getNavigationLevel()));
	
	// XS debug
	this->media_cycle->dumpNavigationLevel() ;
	this->media_cycle->dumpLoopNavigationLevels() ;
}

void ACMultiMediaCycleOsgQt::on_pushButtonForward_clicked() {
	if (! hasMediaCycle()) return; 

	this->media_cycle->goForward();
	this->synchronizeFeaturesWeights();
	
	//	ui.navigationLineEdit->setText(QString::number(media_cycle->getNavigationLevel()));
	
	// XS debug
	this->media_cycle->dumpNavigationLevel() ;
	this->media_cycle->dumpLoopNavigationLevels() ;
}

void ACMultiMediaCycleOsgQt::configureCheckBoxes(){
	// dynamic config of checkboxes
	// according to plugins found by plugin manager
	
	ACPluginManager *acpl = this->media_cycle->getPluginManager(); //getPlugins
	if (acpl) {
		for (int i=0;i<acpl->getSize();i++) {
			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
				if (acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_FEATURES && acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType() == this->media_type) {
					QString s(acpl->getPluginLibrary(i)->getPlugin(j)->getName().c_str());
					QListWidgetItem * item = new QListWidgetItem(s,ui.featuresListWidget);
					item->setCheckState (Qt::Unchecked);
				}
			}
		}
	}
	
	plugins_scanned = true;
	this->synchronizeFeaturesWeights();
	
	connect(ui.featuresListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(modifyListItem(QListWidgetItem*)));
	
}

// XS TODO define when this is called, really clean
void ACMultiMediaCycleOsgQt::cleanCheckBoxes(){
	// XS TODO: does this clear the list ?
	ui.featuresListWidget->clear();
	plugins_scanned = false;
}

void  ACMultiMediaCycleOsgQt::showError(std::string s){
	int warn_button;
	const QString qs = QString::fromStdString(s);
	warn_button = QMessageBox::warning(this, "Error", qs);
	cerr << s << endl;
}

bool ACMultiMediaCycleOsgQt::hasMediaCycle(){
	if (media_cycle == NULL) {
		this->showError ("First define the type of application");
		return false;
	}
	return true;
}


