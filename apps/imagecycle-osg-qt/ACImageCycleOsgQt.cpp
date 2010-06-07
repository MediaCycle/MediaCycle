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
	media_cycle->pushNavigationState();
	//media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
	media_cycle->getBrowser()->setState(AC_CHANGING);
	
	ui.browserOsgView->prepareFromBrowser();
	//browserOsgView->setPlaying(true);
	media_cycle->setNeedsDisplay(true);
	updatedLibrary = true;
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
	//ui.browserOsgView->setFocus();
}

void ACImageCycleOsgQt::on_pushButtonForward_clicked()
{
	media_cycle->goForward();
	//ui.browserOsgView->setFocus();
}

void ACImageCycleOsgQt::on_checkBoxFeat1_stateChanged(int state)
{
	if (updatedLibrary)
	{
		media_cycle->setWeight(0,state/2.0f);
		media_cycle->updateDisplay(true); //XS 250310 was: media_cycle->updateClusters(true);
		// XS250310 removed mediacycle->setNeedsDisplay(true); // now in updateDisplay

		ui.browserOsgView->updateTransformsFromBrowser(0.0); 
	}
	//ui.browserOsgView->setFocus();
}

void ACImageCycleOsgQt::on_checkBoxFeat2_stateChanged(int state)
{
	if (updatedLibrary)
	{
		media_cycle->setWeight(1,state/2.0f);
		media_cycle->updateDisplay(true); //XS 250310 was: media_cycle->updateClusters(true);
		// XS 310310 removed media_cycle->setNeedsDisplay(true); // now in updateDisplay

		ui.browserOsgView->updateTransformsFromBrowser(0.0); 
	}
	//ui.browserOsgView->setFocus();
}

void ACImageCycleOsgQt::on_checkBoxFeat3_stateChanged(int state)
{
	if (updatedLibrary)
	{
		media_cycle->setWeight(2,state/2.0f);
		media_cycle->updateDisplay(true); //XS 250310 was: media_cycle->updateClusters(true);
		// XS 310310 removed media_cycle->setNeedsDisplay(true); // now in updateDisplay

		ui.browserOsgView->updateTransformsFromBrowser(0.0); 
	}
	//ui.browserOsgView->setFocus();
}

// this one is too slow when many items
//void ACImageCycleOsgQt::on_sliderClusters_sliderMoved(){
//	// for synchronous display of values 
//	ui.spinBoxClusters->setValue(ui.sliderClusters->value());
//}

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
		media_cycle->importLibrary((char*) fileName.toStdString().c_str());
		media_cycle->normalizeFeatures();
		std::cout << "File library imported" << std::endl;
		this->updateLibrary();
	}	
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
	//ui.browserOsgView->setFocus();
}

void ACImageCycleOsgQt::editConfigFile(){
	cout << "Editing config file with GUI..." << endl;
	settingsDialog->show();
	settingsDialog->setFocus();
}