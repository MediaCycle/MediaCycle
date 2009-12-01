/**
 * @brief ACMainWindowQT.cpp
 * @author Christian Frisson
 * @date 01/12/2009
 * @copyright (c) 2009 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
*/

#include <iostream>
#include "ACMainWindowQT.h"
//MediaCycle *media_cycle;

TiOscReceiverRef mOscReceiver;

/*
static void userdata()
{};
*/

static void osc_callback(TiOscReceiverRef, const char *tagName, void *userData)
{
	ACMainWindowQT *window = (ACMainWindowQT*)userData;
	
	printf("osc received tag: %s\n", tagName);
	std::cout << "tagName: " << tagName << std::endl;
	/*		
	 if(strcasecmp(tagName, "/imagecycle/1/browser/1/move/xy") == 0)
	 {
	 int x = 0, y = 0;
	 
	 TiOscReceiverReadInt(mOscReceiver, &x);
	 TiOscReceiverReadInt(mOscReceiver, &y);
	 
	 media_cycle->setCameraPosition((int)x, (int)y);
	 //media_cycle->getAudioBrowser()->incrementLoopNavigationLevels(loop);
	 //media_cycle->getAudioBrowser()->setSelectedObject(loop);
	 //media_cycle->getAudioBrowser()->updateClusters(true);
	 media_cycle->getAudioBrowser()->setNeedsDisplay(true);
	 }
	 else if(strcasecmp(tagName, "/imagecycle/1/browser/1/move/zoom") == 0)
	 {
	 int zoom = 0;
	 
	 TiOscReceiverReadInt(mOscReceiver, &zoom);
	 
	 printf("Zoom: %i",zoom);		
	 media_cycle->setCameraZoom((float)zoom);
	 media_cycle->getAudioBrowser()->setNeedsDisplay(true);
	 }
	 else if(strcasecmp(tagName, "/imagecycle/1/browser/library/load") == 0)
	 {
	 char *lib_path = NULL;
	 lib_path = new char[100];
	 TiOscReceiverReadString(mOscReceiver, lib_path, 100);
	 //media_cycle->getAudioLibrary()->openLibrary(lib_path);
	 //media_cycle->getAudioBrowser()->libraryContentChanged();
	 
	 
	 
	 
	 
	 
	 
	 std::cout << "Importing file library..." << std::endl;
	 //media_cycle->importDirectory("./", 1);
	 media_cycle->getAudioLibrary()->openLibrary(lib_path);
	 std::cout << "File library imported" << std::endl;
	 
	 //media_cycle->getAudioBrowser()->libraryContentChanged();
	 
	 
	 if (window->isLibraryUpToDate())
	 {
	 std::cout << "Other launch" << std::endl;
	 media_cycle->getAudioBrowser()->libraryContentChanged();
	 }
	 else
	 {
	 std::cout << "First launch" << std::endl;
	 // with this function call here, do not import twice!!!
	 //media_cycle->getAudioLibrary()->normalizeFeatures();
	 
	 window->getViewerWindow()->prepareFromBrowser();
	 media_cycle->getAudioBrowser()->setSelectedObject(0);
	 window->getViewerWindow()->updateTransformsFromBrowser(1.0); 
	 window->updateLibrary(true);
	 }	
	 
	 }
	 else if(strcasecmp(tagName, "/imagecycle/1/browser/library/clear") == 0)
	 {
	 //media_cycle->cleanLibrary(); // XS instead of getImageLibrary CHECK THIS
	 media_cycle->getAudioBrowser()->libraryContentChanged();
	 }	
	 else if(strcasecmp(tagName, "/imagecycle/1/browser/recenter") == 0)
	 {
	 //media_cycle->setCameraRecenter();
	 }
	 
	 // ...
	 */
}

static void init()
{
	mOscReceiver = TiOscReceiverCreate("localhost", 12346);
	TiOscReceiverSetUserData(mOscReceiver, NULL);
	TiOscReceiverSetCallback(mOscReceiver, osc_callback);
	TiOscReceiverStart(mOscReceiver);
};

ACMainWindowQT::ACMainWindowQT(QWidget *parent)
 : QMainWindow(parent), 
 updatedLibrary(false)
{
	//int counter = 0;
	//std::cout << "Passed: " << ++counter << std::endl;
	ui.setupUi(this); // A faire en premier
	media_cycle = new MediaCycle(MEDIA_TYPE_IMAGE,"/tmp/","mediacycle.acl");
	browserOsgView = new ACOsgBrowserViewQT(ui.groupBoxView);
	browserOsgView->setGeometry(10,20,450,440);
	browserOsgView->setMediaCycle(media_cycle);
	////browserOsgView->prepareFromBrowser();
	//browserOsgView->setPlaying(true);
	this->show();
	init();
}

ACMainWindowQT::~ACMainWindowQT()
{
	//browserOsgView->setPlaying(false);
	free(media_cycle);
	TiOscReceiverStop(mOscReceiver);
	TiOscReceiverRelease(mOscReceiver);
}

void ACMainWindowQT::updateLibrary()
{	
	media_cycle->setSelectedObject(0);
	browserOsgView->prepareFromBrowser();
	//browserOsgView->setPlaying(true);
	media_cycle->setNeedsDisplay(true);
}

void ACMainWindowQT::on_pushButtonLaunch_clicked()
{

	std::cout << "Importing file library..." << std::endl;
	//media_cycle->importDirectory("./", 1);
	//media_cycle->importLibrary("./zero-g-pro-pack-test-frisson.acl");//("./zero-g-pro-pack-test-frisson.acl");//zero-g-pro-pack-test-frisson.acl");
	//media_cycle->importLibrary("./zero-g-pro-pack-test-frisson.acl");
	media_cycle->importLibrary((char*) "/dupont/vistex_final.acl");
	std::cout << "File library imported" << std::endl;
	this->updateLibrary();
	
}

void ACMainWindowQT::on_horizontalSliderRhythm_sliderReleased()
{
	std::cout << "WeightRhythm: " << ui.horizontalSliderRhythm->value()/100.0f << std::endl;
	if (updatedLibrary)
	{
		//media_cycle->setWeightRhythm(ui.horizontalSliderRhythm->value()/100.0f);
		browserOsgView->updateTransformsFromBrowser(1.0); 
	}

}

void ACMainWindowQT::on_horizontalSliderTimbre_sliderReleased()
{
	std::cout << "WeightTimbre: " << ui.horizontalSliderTimbre->value()/100.0f << std::endl;
	if (updatedLibrary){
		//media_cycle->setWeightTimbre(ui.horizontalSliderTimbre->value()/100.0f);
		//browserOsgView->updateTransformsFromBrowser(1.0);
	}
}

void ACMainWindowQT::on_horizontalSliderHarmony_sliderReleased()
{
	std::cout << "WeightHarmony: " << ui.horizontalSliderHarmony->value()/100.0f << std::endl;
	if (updatedLibrary){
		//media_cycle->setWeightHarmony(ui.horizontalSliderHarmony->value()/100.0f);
		//browserOsgView->updateTransformsFromBrowser(1.0);
	}

}

void ACMainWindowQT::on_horizontalSliderClusters_sliderReleased()
{
	std::cout << "ClusterNumber: " << ui.horizontalSliderClusters->value() << std::endl;
	if (updatedLibrary){
		//media_cycle->setClusterNumber(ui.horizontalSliderClusters->value());
		//browserOsgView->updateTransformsFromBrowser(1.0);
	}
}

void ACMainWindowQT::on_horizontalSliderBPM_valueChanged()
{
	std::cout << "BPM: " << ui.horizontalSliderBPM->value() << std::endl;
	if (updatedLibrary){
		//media_cycle->setBPM(ui.horizontalSliderBPM->value());
	}
}

void ACMainWindowQT::on_horizontalSliderKey_valueChanged()
{
	std::cout << "Key: " << ui.horizontalSliderKey->value() << std::endl;
	if (updatedLibrary){
		//media_cycle->setKey(ui.horizontalSliderKey->value());
	}
}

