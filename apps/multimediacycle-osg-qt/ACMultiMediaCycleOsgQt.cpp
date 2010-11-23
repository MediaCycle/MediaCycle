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

ACMultiMediaCycleOsgQt::ACMultiMediaCycleOsgQt(QWidget *parent) : QMainWindow(parent), 
 features_known(false), plugins_scanned(false),library_loaded(false)
{
	ui.setupUi(this); // first thing to do
	this->media_type = MEDIA_TYPE_NONE;
	this->browser_mode = AC_MODE_NONE;
	this->media_cycle = NULL;
	
#if defined(__APPLE__)
	std::string build_type ("Release");
#ifdef USE_DEBUG
	build_type = "Debug";
#endif
	//	media_cycle->addPlugin(QApplication::applicationDirPath().toStdString() + "/../../../plugins/image/" + build_type + "/mc_image.dylib");
	//#elif defined (__WIN32__)
	//	media_cycle->addPlugin(QApplication::applicationDirPath().toStdString() + "\..\..\plugins\image\mc_image.dll");
	//#else
	//	media_cycle->addPlugin(QApplication::applicationDirPath().toStdString() + "/../../plugins/image/mc_image.so");
#endif
	
	ui.browserOsgView->move(0,20);
	//	ui.browserOsgView->prepareFromBrowser();
	//browserOsgView->setPlaying(true);
	
	connect(ui.actionLoad_Media_Directory, SIGNAL(triggered()), this, SLOT(loadMediaDirectory()));
	connect(ui.actionLoad_Media_Files, SIGNAL(triggered()), this, SLOT(loadMediaFiles()));
	//	connect(ui.actionLoad_ACL, SIGNAL(triggered()), this, SLOT(on_pushButtonLaunch_clicked()));
	connect(ui.actionSave_ACL, SIGNAL(triggered()), this, SLOT(saveACLFile()));
	connect(ui.actionEdit_Config_File, SIGNAL(triggered()), this, SLOT(editConfigFile()));
	connect(ui.featuresListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(modifyListItem(QListWidgetItem*)));

	//	// connect spinBox and slider
	//	connect(ui.spinBoxClusters, SIGNAL(valueChanged(int)), this, SLOT(spinBoxClustersValueChanged(int)));
	//	connect(ui.sliderClusters, SIGNAL(valueChanged(int)), ui.spinBoxClusters , SIGNAL(valueChanged(int)));
	//	
	
	// uses another window for settings (using the class settingsDialog)
	settingsDialog = new SettingsDialog(parent);
	settingsDialog->setMediaCycleMainWindow(this);
	
	this->show();
}

ACMultiMediaCycleOsgQt::~ACMultiMediaCycleOsgQt(){
}

// creates a MediaCycle object (containing the whole application)
// with the appropriate type (audio/image/video/text/mixed/composite/...)
void ACMultiMediaCycleOsgQt::createMediaCycle(ACMediaType _media_type, ACBrowserMode _browser_mode){
	media_cycle = new MediaCycle(_media_type,"/tmp/","mediacycle.acl");
	ui.browserOsgView->setMediaCycle(media_cycle);	
	
	// keep track locall of the media and browser modes
	this->media_type = _media_type;
	this->browser_mode = _browser_mode;
}

// destroys the MediaCycle object (containing the whole application)
// it should leave an empty blue frame, just as lauch time
void ACMultiMediaCycleOsgQt::destroyMediaCycle(){
	// XS TODO : remove it from the graphics ?
	delete media_cycle;
}

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
	cout << "Saving ACL File..." << endl;
	
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

// XS TODO: make sure it works if we add a new directory to the existing library ?
void ACMultiMediaCycleOsgQt::loadMediaDirectory(){
	if (media_cycle == NULL) {
		cerr << "first define the type of application" << endl;
		return;
	}
	
	QString selectDir = QFileDialog::getExistingDirectory
	(
	 this, 
	 tr("Open Directory"),
	 "",
	 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	 );
	
	// check if directory exists
	if (media_cycle->importDirectory(selectDir.toStdString(), 1) > 0){
		media_cycle->normalizeFeatures();
		this->updateLibrary();
	}
	//ui.browserOsgView->setFocus();
}

void ACMultiMediaCycleOsgQt::loadMediaFiles(){
	if (media_cycle == NULL) {
		cerr << "first define the type of application" << endl;
		return;
	}

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
	cout << "Editing config file with GUI..." << endl;
	settingsDialog->show();
	settingsDialog->setFocus();
}


void ACMultiMediaCycleOsgQt::updateLibrary(){
	if (media_cycle == NULL) {
		cerr << "first define the type of application" << endl;
		return;
	}	
	media_cycle->libraryContentChanged(); 	
	media_cycle->setReferenceNode(0);
	// XSCF 250310 added these 3
	// media_cycle->pushNavigationState(); // XS 250810 removed this
	//media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
	
	// XS TODO this is sooo ugly:
	media_cycle->getBrowser()->setState(AC_CHANGING);
	
	ui.browserOsgView->prepareFromBrowser();
	//browserOsgView->setPlaying(true);
	media_cycle->setNeedsDisplay(true);
	
	// do not re-scan the directory for plugins once they have been loaded
//	if (!plugins_scanned) this->configureCheckBoxes();
	
	library_loaded = true;
	ui.browserOsgView->setFocus();
}

void ACMultiMediaCycleOsgQt::on_sliderClusters_sliderReleased(){
	// for synchronous display of values 
	if (media_cycle == NULL) {
		cerr << "first define the type of application" << endl;
		return;
	}	
	ui.spinBoxClusters->setValue(ui.sliderClusters->value());
}

void ACMultiMediaCycleOsgQt::spinBoxClustersValueChanged(int _value)
{
	if (media_cycle == NULL) {
		cerr << "first define the type of application" << endl;
		return;
	}	
	
	ui.sliderClusters->setValue(_value); 	// for synchronous display of values 
	std::cout << "ClusterNumber: " << _value << std::endl;
	if (library_loaded){
		media_cycle->setClusterNumber(_value);
		// XSCF251003 added this
		media_cycle->updateDisplay(true); //XS 250310 was: media_cycle->updateClusters(true);
		// XS 310310 removed media_cycle->setNeedsDisplay(true); // now in updateDisplay
		ui.browserOsgView->updateTransformsFromBrowser(1.0);
	}
	//ui.browserOsgView->setFocus();
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
		ui.browserOsgView->updateTransformsFromBrowser(0.0); 
	}
}

// synchronize weights with what is loaded in mediacycle
// note: here weights are 1 or 0 (checkbox).
// conversion: 0 remains 0, and value > 0 becomes 1.
void ACMultiMediaCycleOsgQt::synchronizeFeaturesWeights(){
	vector<float> w = media_cycle->getWeightVector();
	int nw = w.size();
	if (ui.featuresListWidget->count() != nw){
		cerr << "Warning: Checkboxes in GUI do not match Features in MediaCycle" << endl;
		cerr << ui.featuresListWidget->count() << "!=" << nw << endl;
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

