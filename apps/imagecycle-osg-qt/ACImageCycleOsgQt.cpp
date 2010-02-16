/*
 *  ACImageCycleOsgQt.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 16/02/10
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
		
	ui.browserOsgView->move(0,20);
	ui.browserOsgView->setMediaCycle(media_cycle);
	ui.browserOsgView->prepareFromBrowser();
	//browserOsgView->setPlaying(true);
	
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

void ACImageCycleOsgQt::on_pushButtonLaunch_clicked()
{
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

void ACImageCycleOsgQt::on_pushButtonClean_clicked()
{
	media_cycle->cleanLibrary(); // XS instead of getImageLibrary CHECK THIS
	media_cycle->libraryContentChanged();
	this->updateLibrary();
}	

void ACImageCycleOsgQt::on_pushButtonRecenter_clicked()
{
	media_cycle->setCameraRecenter();
}	

void ACImageCycleOsgQt::on_checkBoxFeat1_stateChanged(int state)
{
	if (updatedLibrary)
	{
		media_cycle->setWeight(1,state/2.0f);
		ui.browserOsgView->updateTransformsFromBrowser(1.0); 
	}
}

void ACImageCycleOsgQt::on_checkBoxFeat2_stateChanged(int state)
{
	if (updatedLibrary)
	{
		media_cycle->setWeight(2,state/2.0f);
		ui.browserOsgView->updateTransformsFromBrowser(1.0); 
	}
}

void ACImageCycleOsgQt::on_checkBoxFeat3_stateChanged(int state)
{
	if (updatedLibrary)
	{
		media_cycle->setWeight(3,state/2.0f);
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
