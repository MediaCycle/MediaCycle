/*
 *  ACImageCycleOsgQt.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 16/02/10
 *  @author Xavier Siebert
 *
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

#include <iostream>
#include "ACImageCycleOsgQt.h"

ACImageCycleOsgQt::ACImageCycleOsgQt(QWidget *parent)
 : QMainWindow(parent), 
 updatedLibrary(false)
{
	ui.setupUi(this); // first thing to do
	features_known = false; // when launching the app we don't know how many features.

	media_cycle = new MediaCycle(MEDIA_TYPE_IMAGE,"/tmp/","mediacycle.acl");
	
	// XS TODO detect if fichier de configuration
	// if not, set default options
	media_cycle->setMode(AC_MODE_CLUSTERS);

	#if defined(__APPLE__)
		std::string build_type ("Release");
		#ifdef USE_DEBUG
			build_type = "Debug";
		#endif
		media_cycle->addPlugin("../../../plugins/image/" + build_type + "/mc_image.dylib");
	#endif
	
	//this->configureCheckBoxes();
	
	ui.browserOsgView->move(0,20);
	ui.browserOsgView->setMediaCycle(media_cycle);
	ui.browserOsgView->prepareFromBrowser();
	//browserOsgView->setPlaying(true);
	
	connect(ui.actionLoad_Media_Directory, SIGNAL(triggered()), this, SLOT(loadMediaDirectory()));
	connect(ui.actionLoad_Media_Files, SIGNAL(triggered()), this, SLOT(loadMediaFiles()));
	connect(ui.actionLoad_ACL, SIGNAL(triggered()), this, SLOT(on_pushButtonLaunch_clicked()));
	connect(ui.actionSave_ACL, SIGNAL(triggered()), this, SLOT(saveACLFile()));
	connect(ui.actionEdit_Config_File, SIGNAL(triggered()), this, SLOT(editConfigFile()));

	// connect spinBox and slider
	connect(ui.spinBoxClusters, SIGNAL(valueChanged(int)), this, SLOT(spinBoxClustersValueChanged(int)));
	connect(ui.sliderClusters, SIGNAL(valueChanged(int)), ui.spinBoxClusters , SIGNAL(valueChanged(int)));

	settingsDialog = new SettingsDialog(parent);
	settingsDialog->setMediaCycleMainWindow(this);

	this->show();
	
	//ui.browserOsgView->setFocus();
}

ACImageCycleOsgQt::~ACImageCycleOsgQt()
{
	delete media_cycle;
}


void ACImageCycleOsgQt::updateLibrary()
{	
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
	updatedLibrary = true;
	
	//XS new, use this carefully 
	this->configureCheckBoxes();
	
	ui.browserOsgView->setFocus();
}

void ACImageCycleOsgQt::on_pushButtonLaunch_clicked() {
	this->loadACLFile();
	//ui.browserOsgView->setFocus();
}

void ACImageCycleOsgQt::on_pushButtonClean_clicked()
{
	media_cycle->cleanUserLog();
	media_cycle->cleanLibrary();
	this->updateLibrary();
	//ui.browserOsgView->setFocus();
}	

void ACImageCycleOsgQt::on_pushButtonRecenter_clicked()
{
	media_cycle->setCameraRecenter();
	//ui.browserOsgView->setFocus();
}	

void ACImageCycleOsgQt::on_pushButtonBack_clicked()
{
	media_cycle->goBack();
	this->synchronizeFeaturesWeights();
//	ui.navigationLineEdit->setText(QString::number(media_cycle->getNavigationLevel()));
	
	// XS debug
	media_cycle->dumpNavigationLevel() ;
	media_cycle->dumpLoopNavigationLevels() ;

	//ui.browserOsgView->setFocus();
}

void ACImageCycleOsgQt::on_pushButtonForward_clicked()
{
	media_cycle->goForward();
	this->synchronizeFeaturesWeights();

//	ui.navigationLineEdit->setText(QString::number(media_cycle->getNavigationLevel()));
	
	// XS debug
	media_cycle->dumpNavigationLevel() ;
	media_cycle->dumpLoopNavigationLevels() ;

	//ui.browserOsgView->setFocus();
}

void ACImageCycleOsgQt::on_sliderClusters_sliderReleased(){
	// for synchronous display of values 
	ui.spinBoxClusters->setValue(ui.sliderClusters->value());
}

void ACImageCycleOsgQt::spinBoxClustersValueChanged(int _value)
{
	ui.sliderClusters->setValue(_value); 	// for synchronous display of values 

	std::cout << "ClusterNumber: " << _value << std::endl;
	if (updatedLibrary){
		media_cycle->setClusterNumber(_value);
		// XSCF251003 added this
		media_cycle->updateDisplay(true); //XS 250310 was: media_cycle->updateClusters(true);
		// XS 310310 removed media_cycle->setNeedsDisplay(true); // now in updateDisplay
		ui.browserOsgView->updateTransformsFromBrowser(1.0);
	}
	//ui.browserOsgView->setFocus();
}

void ACImageCycleOsgQt::loadACLFile(){
	QString fileName;
	
	QFileDialog dialog(this,"Open ImageCycle Library File(s)");
	dialog.setDefaultSuffix ("acl");
	dialog.setNameFilter("ImageCycle Library Files (*.acl)");
	dialog.setFileMode(QFileDialog::ExistingFile); // change to ExistingFiles for multiple file handling
	
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	
	QStringList::Iterator file = fileNames.begin();
	while(file != fileNames.end()) {
		//std::cout << "File library: '" << (*file).toStdString() << "'" << std::endl;
		fileName = *file;
		++file;
	}
	//std::cout << "Will open: '" << fileName.toStdString() << "'" << std::endl;
	//fileName = QFileDialog::getOpenFileName(this, "~", );
	
	if (!(fileName.isEmpty())) {
		media_cycle->importACLLibrary((char*) fileName.toStdString().c_str());
		media_cycle->normalizeFeatures();
		std::cout << "File library imported" << std::endl;
		this->updateLibrary();
	}	
	
	// XS watch this one...
	media_cycle->storeNavigationState(); 
	
	// XS play
//	ui.navigationLineEdit->setText(QString::number(media_cycle->getNavigationLevel()));
	
	// XS debug
	media_cycle->dumpNavigationLevel();
	media_cycle->dumpLoopNavigationLevels() ;
}

void ACImageCycleOsgQt::saveACLFile(){
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
	//ui.browserOsgView->setFocus();
}

void ACImageCycleOsgQt::loadMediaDirectory(){

	QString selectDir = QFileDialog::getExistingDirectory
	(
	 this, 
	 tr("Open Directory"),
	 "",
	 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);
	
	// XS TODO : check if directory exists
	// XS : do not separate directory and files in Qt and let MediaCycle handle it
	
	media_cycle->importDirectory(selectDir.toStdString(), 1);
	// with this function call here, do not import twice!!!
	// XS TODO: what if we add a new directory to the existing library ?
	media_cycle->normalizeFeatures();
	this->updateLibrary();
	//ui.browserOsgView->setFocus();
	
//	QStringList listFilter;
//	listFilter << "*.png";
//	listFilter << "*.jpg";
//	
//	QDirIterator dirIterator(selectDir, listFilter ,QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
//	
//	// Variable qui contiendra tous les fichiers correspondant à notre recherche
//	QStringList fileList; 
//	// Tant qu'on n'est pas arrivé à la fin de l'arborescence...
//	while(dirIterator.hasNext()) 
//	{   
//		// ...on va au prochain fichier correspondant à notre filtre
//		fileList << dirIterator.next(); 
//	}
//	for ( QStringList::Iterator it = fileList.begin(); it != fileList.end(); ++it ) {
//		cout << (*it).toStdString() << endl;
//	}	
}

void ACImageCycleOsgQt::loadMediaFiles(){
	QString fileName;
	
	QFileDialog dialog(this,"Open MediaCycle Image File(s)");
	dialog.setDefaultSuffix ("png");
	dialog.setNameFilter("Image Files (*.jpg *.png *.tif *.tiff)");
	dialog.setFileMode(QFileDialog::ExistingFiles); // ExistingFile(s); "s" is for multiple file handling
	
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	
	QStringList::Iterator file = fileNames.begin();
	while(file != fileNames.end()) {
		//std::cout << "File library: '" << (*file).toStdString() << "'" << std::endl;
		fileName = *file;
		++file;
		std::cout << "Will open: '" << fileName.toStdString() << "'" << std::endl;
		//fileName = QFileDialog::getOpenFileName(this, "~", );
	
		if (!(fileName.isEmpty())) {
			media_cycle->importDirectory((char*) fileName.toStdString().c_str(), 0);
			//media_cycle->normalizeFeatures();
			//media_cycle->libraryContentChanged(); no, this is in updatelibrary
			std::cout <<  fileName.toStdString() << " imported" << std::endl;
		}
	}
	// XS do this only after loading all files (it was in the while loop) !
	this->updateLibrary();
}

void ACImageCycleOsgQt::editConfigFile(){
	cout << "Editing config file with GUI..." << endl;
	settingsDialog->show();
	settingsDialog->setFocus();
}

void ACImageCycleOsgQt::configureCheckBoxes(){
	// dynamic config of checkboxes
	// according to plugins found by plugin manager
	ACPluginManager *acpl = media_cycle->getPluginManager(); //getPlugins
	if (acpl) {
		for (int i=0;i<acpl->getSize();i++) {
			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
				if (acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_FEATURES && acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType() == MEDIA_TYPE_IMAGE) {
					QString s(acpl->getPluginLibrary(i)->getPlugin(j)->getName().c_str());
					QListWidgetItem * item = new QListWidgetItem(s,ui.featuresListWidget);
					item->setCheckState (Qt::Unchecked);
				}
			}
		}
	}
	
	this->synchronizeFeaturesWeights();

	connect(ui.featuresListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(modifyListItem(QListWidgetItem*)));

//	w2.show();
	
	//	QCheckBox* toto = new QCheckBox("toto",ui.groupBoxSimilarity);
	//	QCheckBox* toto2 = new QCheckBox("toto2",ui.groupBoxSimilarity);
	//	QCheckBox* toto3 = new QCheckBox("toto3",ui.groupBoxSimilarity);
	//	QCheckBox* toto4 = new QCheckBox("toto4",ui.groupBoxSimilarity);
	//	QCheckBox* toto5 = new QCheckBox("toto5",ui.groupBoxSimilarity);
	//	QCheckBox* toto6 = new QCheckBox("toto6",ui.groupBoxSimilarity);
	//	QGridLayout *lo = new QGridLayout;
	//	lo->addWidget(toto);
	//	lo->addWidget(toto2);
	//	lo->addWidget(toto3);
	//	lo->addWidget(toto4);
	//	lo->addWidget(toto5);
	//	lo->addWidget(toto6);
	//
	//	ui.groupBoxSimilarity->setLayout(lo);
	
	// XS end test -- need to delete !!!
}
void ACImageCycleOsgQt::cleanCheckBoxes(){
}

// NEW SLOTS
void ACImageCycleOsgQt::modifyListItem(QListWidgetItem *item)
{
	// XS check
	cout << item->text().toStdString() << endl; // isselected...
	cout << ui.featuresListWidget->currentRow() << endl;
	// end XS check 
	
	if (updatedLibrary){
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

void ACImageCycleOsgQt::synchronizeFeaturesWeights(){
	// synchronize weights with what is loaded in mediacycle
	// note: here weights are 1 or 0 (checkbox).
	// conversion: 0 remains 0, and value > 0 becomes 1.
	vector<float> w = media_cycle->getWeightVector();
	int nw = w.size();
	if (ui.featuresListWidget->count() != nw){
		cerr << "Checkboxes in GUI do not match Features in MediaCycle" << endl;
		cerr << ui.featuresListWidget->count() << "!=" << nw << endl;
		exit(1);
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
