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
	
	media_cycle->addPlugin("/Users/xavier/development/Fall09/ticore-app/Applications/Numediart/MediaCycle/src/Builds/darwin-xcode/plugins/image/Debug/libimage.dylib");
	
	ui.browserOsgView->move(0,20);
	ui.browserOsgView->setMediaCycle(media_cycle);
	ui.browserOsgView->prepareFromBrowser();
	//browserOsgView->setPlaying(true);
	
	connect(ui.actionLoad_Media_Directory, SIGNAL(triggered()), this, SLOT(loadMediaDirectory()));
	connect(ui.actionLoad_Media_Files, SIGNAL(triggered()), this, SLOT(loadMediaFiles()));
	connect(ui.actionLoad_ACL, SIGNAL(triggered()), this, SLOT(on_pushButtonLaunch_clicked()));
	connect(ui.actionSave_ACL, SIGNAL(triggered()), this, SLOT(saveACLFile()));

	this->show();
}

ACImageCycleOsgQt::~ACImageCycleOsgQt()
{
	delete media_cycle;
}


void ACImageCycleOsgQt::updateLibrary()
{	
	media_cycle->setSelectedObject(0);
	ui.browserOsgView->prepareFromBrowser();
	//browserOsgView->setPlaying(true);
	media_cycle->setNeedsDisplay(true);
	updatedLibrary = true;
}

void ACImageCycleOsgQt::on_pushButtonLaunch_clicked() {
	this->loadACLFile();
}

void ACImageCycleOsgQt::on_pushButtonClean_clicked()
{
	media_cycle->cleanLibrary();
	media_cycle->libraryContentChanged();
	this->updateLibrary();
}	

void ACImageCycleOsgQt::on_pushButtonRecenter_clicked()
{
	media_cycle->setCameraRecenter();
}	

void ACImageCycleOsgQt::on_pushButtonBack_clicked()
{
	media_cycle->setBack();
}

void ACImageCycleOsgQt::on_pushButtonForward_clicked()
{
	media_cycle->setForward();
}

void ACImageCycleOsgQt::on_checkBoxFeat1_stateChanged(int state)
{
	if (updatedLibrary)
	{
		media_cycle->setWeight(0,state/2.0f);
		ui.browserOsgView->updateTransformsFromBrowser(1.0); 
	}
}

void ACImageCycleOsgQt::on_checkBoxFeat2_stateChanged(int state)
{
	if (updatedLibrary)
	{
		media_cycle->setWeight(1,state/2.0f);
		ui.browserOsgView->updateTransformsFromBrowser(1.0); 
	}
}

void ACImageCycleOsgQt::on_checkBoxFeat3_stateChanged(int state)
{
	if (updatedLibrary)
	{
		media_cycle->setWeight(2,state/2.0f);
		ui.browserOsgView->updateTransformsFromBrowser(1.0); 
	}
}

void ACImageCycleOsgQt::on_sliderClusters_sliderReleased()
{
	std::cout << "ClusterNumber: " << ui.sliderClusters->value() << std::endl;
	if (updatedLibrary){
		media_cycle->setClusterNumber(ui.sliderClusters->value());
		ui.browserOsgView->updateTransformsFromBrowser(1.0);
	}
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
	
	media_cycle->importLibrary((char*) fileName.toStdString().c_str());
	std::cout << "File library imported" << std::endl;
	this->updateLibrary();

}

void ACImageCycleOsgQt::saveACLFile(){
	cout << "Saving ACL File..." << endl;
	
	QString fileName = QFileDialog::getSaveFileName(this);
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
	media_cycle->normalizeFeatures();

	// media_cycle->libraryContentChanged(); // XS already in importDirectory

	this->updateLibrary();

	
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
}