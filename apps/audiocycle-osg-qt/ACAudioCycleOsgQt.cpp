/*
 *  ACAudioCycleOsgQt.cpp
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
#include "ACAudioCycleOsgQt.h"

static void osc_callback(ACOscBrowserRef, const char *tagName, void *userData)
{
	ACAudioCycleOsgQt *window = (ACAudioCycleOsgQt*)userData;
	//printf("osc received tag: %s\n", tagName);
	//std::cout << "tagName: " << tagName << std::endl;
	window->processOscMessage(tagName);
}

ACAudioCycleOsgQt::ACAudioCycleOsgQt(QWidget *parent)
 : QMainWindow(parent), 
 updatedLibrary(false)
{
	ui.setupUi(this); // first thing to do
	media_cycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/tmp/","mediacycle.acl");
	//media_cycle->addPlugin ("/Users/frisson/Dev/numediart/MediaCycle/ticore-app/Applications/Numediart/MediaCycle/src/Builds/plugins/visualisation/Debug/mc_visualisation.dylib");
	//media_cycle->setVisualisationPlugin("Visualisation");
	//media_cycle->setNeighborhoodsPlugin("RandomNeighborhoods");
	//media_cycle->setPositionsPlugin("RandomPositions");

	audio_engine = new ACAudioFeedback();
	audio_engine->setMediaCycle(media_cycle);

	osc_feedback = NULL;
	osc_browser = NULL;
	
	ui.browserOsgView->move(0,20);
	ui.browserOsgView->setMediaCycle(media_cycle);
	ui.browserOsgView->prepareFromBrowser();
	//browserOsgView->setPlaying(true);
	
	this->show();

/*	
	osc_browser = new ACOscBrowser();
	mOscReceiver = osc_browser->create("localhost", 12345);
	osc_browser->setUserData(mOscReceiver, this);
	osc_browser->setCallback(mOscReceiver, osc_callback);
	osc_browser->start(mOscReceiver);	
	osc_feedback = new ACOscFeedback();
	mOscFeeder = osc_feedback->create("localhost", 12346);
*/

}

ACAudioCycleOsgQt::~ACAudioCycleOsgQt()
{
	//browserOsgView->setPlaying(false);
	//free(media_cycle);
	if (osc_browser) {
		osc_browser->stop(mOscReceiver);		
		//osc_browser->release(mOscReceiver);//should be in destructor ?
	}

	delete osc_browser;
	delete osc_feedback;//osc_feedback destructor calls ACOscFeedback::release()
	delete audio_engine;
	delete media_cycle;
	
	//delete mOscReceiver;
	//delete mOscFeeder;
}


void ACAudioCycleOsgQt::updateLibrary()
{	
	media_cycle->setSelectedObject(0);
	ui.browserOsgView->prepareFromBrowser();
	//browserOsgView->setPlaying(true);
	media_cycle->setNeedsDisplay(true);
	updatedLibrary = true;
}

void ACAudioCycleOsgQt::on_pushButtonLaunch_clicked()
{
/*
	std::cout << "Importing file library..." << std::endl;
	media_cycle->importLibrary((char*) "/Users/frisson/Videodrome/numediart/DataSets/AudioCycle/AudioCycleProPackTest/zero-g-pro-pack-mc-frisson-100105.acl");
 */
	QString fileName;
	
	QFileDialog dialog(this,"Open AudioCycle Library File(s)");
	dialog.setDefaultSuffix ("acl");
	dialog.setNameFilter("AudioCycle Library Files (*.acl)");
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

void ACAudioCycleOsgQt::on_pushButtonMuteAll_clicked()
{
	media_cycle->muteAllSources();
}

void ACAudioCycleOsgQt::on_pushButtonClean_clicked()
{
	media_cycle->cleanLibrary(); // XS instead of getImageLibrary CHECK THIS
	media_cycle->libraryContentChanged();
	this->updateLibrary();
}	

void ACAudioCycleOsgQt::on_pushButtonRecenter_clicked()
{
	media_cycle->setCameraRecenter();
}

void ACAudioCycleOsgQt::on_pushButtonBack_clicked()
{
	//std::cout << media_cycle->getNavigationLevel() << std::endl;
	media_cycle->setBack();
}

void ACAudioCycleOsgQt::on_pushButtonForward_clicked()
{
	//std::cout << media_cycle->getNavigationLevel() << std::endl;
	media_cycle->setForward();
}

void ACAudioCycleOsgQt::on_pushButtonControlStart_clicked()
{
	std::cout << "IP: " << ui.lineEditControlIP->text().toStdString() << std::endl;
	std::cout << "Port: " << ui.lineEditControlPort->text().toInt() << std::endl;
	if ( ui.pushButtonControlStart->text().toStdString() == "Start")
	{	
		osc_browser = new ACOscBrowser();
		mOscReceiver = osc_browser->create(ui.lineEditControlIP->text().toStdString().c_str(), ui.lineEditControlPort->text().toInt());
		osc_browser->setUserData(mOscReceiver, this);
		osc_browser->setCallback(mOscReceiver, osc_callback);
		osc_browser->start(mOscReceiver);
		ui.pushButtonControlStart->setText("Stop");
		 
	}	
	else if ( ui.pushButtonControlStart->text().toStdString() == "Stop")
	{	
		osc_browser->stop(mOscReceiver);
		ui.pushButtonControlStart->setText("Start");
	}	
}	

void ACAudioCycleOsgQt::on_pushButtonFeedbackStart_clicked()
{
	std::cout << "IP: " << ui.lineEditFeedbackIP->text().toStdString() << std::endl;
	std::cout << "Port: " << ui.lineEditFeedbackPort->text().toInt() << std::endl;
	if ( ui.pushButtonFeedbackStart->text().toStdString() == "Start")
	{	
		osc_feedback = new ACOscFeedback();
		osc_feedback->create(ui.lineEditFeedbackIP->text().toStdString().c_str(), ui.lineEditFeedbackPort->text().toInt());
		ui.pushButtonFeedbackStart->setText("Stop");
	}	
	else if ( ui.pushButtonFeedbackStart->text().toStdString() == "Stop")
	{	
		osc_feedback->release();//mOscFeeder);
		ui.pushButtonFeedbackStart->setText("Start");
	}
}	

void ACAudioCycleOsgQt::on_checkBoxRhythm_stateChanged(int state)
{
	if (updatedLibrary)
	{
		media_cycle->setWeight(1,state/2.0f);
		ui.browserOsgView->updateTransformsFromBrowser(1.0); 
	}
}

void ACAudioCycleOsgQt::on_checkBoxTimbre_stateChanged(int state)
{
	if (updatedLibrary)
	{
		media_cycle->setWeight(2,state/2.0f);
		ui.browserOsgView->updateTransformsFromBrowser(1.0); 
	}
}

void ACAudioCycleOsgQt::on_checkBoxHarmony_stateChanged(int state)
{
	if (updatedLibrary)
	{
		media_cycle->setWeight(3,state/2.0f);
		ui.browserOsgView->updateTransformsFromBrowser(1.0); 
	}
}
	
/*
void ACAudioCycleOsgQt::on_sliderRhythm_sliderReleased()
{
	std::cout << "WeightRhythm: " << ui.sliderRhythm->value()/100.0f << std::endl;
	if (updatedLibrary)
	{
		media_cycle->setWeight(1,ui.sliderRhythm->value()/100.0f);
		ui.browserOsgView->updateTransformsFromBrowser(1.0); 
	}
}

void ACAudioCycleOsgQt::on_sliderTimbre_sliderReleased()
{
	std::cout << "WeightTimbre: " << ui.sliderTimbre->value()/100.0f << std::endl;
	if (updatedLibrary){
		media_cycle->setWeight(2,ui.sliderTimbre->value()/100.0f);
		ui.browserOsgView->updateTransformsFromBrowser(1.0);
	}
}

void ACAudioCycleOsgQt::on_sliderHarmony_sliderReleased()
{
	std::cout << "WeightHarmony: " << ui.sliderHarmony->value()/100.0f << std::endl;
	if (updatedLibrary){
		media_cycle->setWeight(3,ui.sliderHarmony->value()/100.0f);
		ui.browserOsgView->updateTransformsFromBrowser(1.0);
	}
}
*/ 

void ACAudioCycleOsgQt::on_sliderClusters_sliderReleased()
{
	std::cout << "ClusterNumber: " << ui.sliderClusters->value() << std::endl;
	if (updatedLibrary){
		media_cycle->setClusterNumber(ui.sliderClusters->value());
		ui.browserOsgView->updateTransformsFromBrowser(1.0);
	}
}

void ACAudioCycleOsgQt::on_sliderBPM_valueChanged()
{
	std::cout << "BPM: " << ui.sliderBPM->value() << std::endl;
	//if (updatedLibrary){
		int clicked_loop = media_cycle->getClickedLoop();
		if (clicked_loop > -1)
		{
			audio_engine->setLoopSynchroMode(clicked_loop, ACAudioEngineSynchroModeAutoBeat);
			audio_engine->setLoopScaleMode(clicked_loop, ACAudioEngineScaleModeResample);
			audio_engine->setBPM(ui.sliderBPM->value());
		}
	//}
}

void ACAudioCycleOsgQt::on_sliderKey_valueChanged()
{
	std::cout << "Key: " << ui.sliderKey->value() << std::endl;
	//if (updatedLibrary){
		int clicked_loop = media_cycle->getClickedLoop();
		if (clicked_loop > -1)
		{
			/*
			 if (!is_pitching)
			 {	
			 is_pitching = true;
			 is_scrubing = false;
			 */ 
			//media_cycle->pickedObjectCallback(-1);
			audio_engine->setLoopSynchroMode(clicked_loop, ACAudioEngineSynchroModeAutoBeat);
			audio_engine->setLoopScaleMode(clicked_loop, ACAudioEngineScaleModeResample);
			//}
			audio_engine->setSourcePitch(clicked_loop, ui.sliderKey->value()); 
		}
	//}
}

void ACAudioCycleOsgQt::processOscMessage(const char* tagName)
{	
	if(strcasecmp(tagName, "/audiocycle/test") == 0)
	{
		std::cout << "OSC communication established" << std::endl;

		if (osc_feedback)
		{
			osc_feedback->messageBegin("/audiocycle/received");
			osc_feedback->messageEnd();
			osc_feedback->messageSend();
		}

		
	}
	else if(strcasecmp(tagName, "/audiocycle/fullscreen") == 0)
	{
		int fullscreen = 0;
		osc_browser->readInt(mOscReceiver, &fullscreen);
		if (fullscreen == 1)
			ui.groupControls->hide();
		else
			ui.groupControls->show();		
	}	
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/xy") == 0)
	{
		float x = 0.0, y = 0.0;
		media_cycle->getCameraPosition(x,y);
		osc_browser->readFloat(mOscReceiver, &x);
		osc_browser->readFloat(mOscReceiver, &y);
		
		float zoom = media_cycle->getCameraZoom();
		float angle = media_cycle->getCameraRotation();
		float xmove = x*cos(-angle)-y*sin(-angle);
		float ymove = y*cos(-angle)+x*sin(-angle);
		media_cycle->setCameraPosition(xmove/2/zoom , ymove/2/zoom); // norm [-1;1] = 2 (instead of 100 from mediacycle-osg)
		media_cycle->setNeedsDisplay(1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/hover/xy") == 0)
	{
		float x = 0.0, y = 0.0;
		osc_browser->readFloat(mOscReceiver, &x);
		osc_browser->readFloat(mOscReceiver, &y);
		
		media_cycle->hoverCallback(x,y);
		int closest_loop = media_cycle->getClosestLoop();
		float distance = ui.browserOsgView->getRenderer()->getDistanceMouse()[closest_loop];
		if (osc_feedback)
		{
			osc_feedback->messageBegin("/audiocycle/closest_loop_at");
			osc_feedback->messageAppendFloat(distance);
			osc_feedback->messageEnd();
			osc_feedback->messageSend();
		}	
		//media_cycle->setNeedsDisplay(1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/zoom") == 0)
	{
		float zoom;
		osc_browser->readFloat(mOscReceiver, &zoom);
		//zoom = zoom*600/50; // refzoom +
		media_cycle->setCameraZoom((float)zoom);
		media_cycle->setNeedsDisplay(1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/angle") == 0)
	{
		float angle;//, refangle = media_cycle->getCameraRotation();
		osc_browser->readFloat(mOscReceiver, &angle);
		media_cycle->setCameraRotation((float)angle);
		media_cycle->setNeedsDisplay(1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/library/load") == 0)
	{
		char *lib_path = NULL;
		lib_path = new char[500]; // wrong magic number!
		osc_browser->readString(mOscReceiver, lib_path, 500); // wrong magic number!
		std::cout << "Importing file library '" << lib_path << "'..." << std::endl;
		media_cycle->importLibrary(lib_path); // XS instead of getImageLibrary CHECK THIS
		//updateLibrary();
		std::cout << "File library imported" << std::endl;
		
		media_cycle->setSelectedObject(0);
		ui.browserOsgView->prepareFromBrowser();
		media_cycle->setNeedsDisplay(true);
		updatedLibrary = true;
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/library/clear") == 0)
	{
		media_cycle->cleanLibrary(); // XS instead of getImageLibrary CHECK THIS
		media_cycle->libraryContentChanged();
		
		media_cycle->setSelectedObject(0);
		ui.browserOsgView->prepareFromBrowser();
		media_cycle->setNeedsDisplay(true);
		updatedLibrary = true;
	}	
	else if(strcasecmp(tagName, "/audiocycle/1/browser/recenter") == 0)
	{
		media_cycle->setCameraRecenter();
	}
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/playclosestloop") == 0)
	{	
		media_cycle->pickedObjectCallback(-1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/muteall") == 0)
	{	
		media_cycle->muteAllSources();
	}
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/bpm") == 0)
	{
		float bpm;
		osc_browser->readFloat(mOscReceiver, &bpm);
		int clicked_loop = media_cycle->getClickedLoop();
		if (clicked_loop > -1)
		{
			audio_engine->setLoopSynchroMode(clicked_loop, ACAudioEngineSynchroModeAutoBeat);
			audio_engine->setLoopScaleMode(clicked_loop, ACAudioEngineScaleModeResample);
			audio_engine->setBPM((float)bpm);
		}
	}	
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/scrub") == 0)
	{
		float scrub;
		osc_browser->readFloat(mOscReceiver, &scrub);
		
		int clicked_loop = media_cycle->getClickedLoop();
		if (clicked_loop > -1)
		{
			//media_cycle->pickedObjectCallback(-1);
			audio_engine->setLoopSynchroMode(clicked_loop, ACAudioEngineSynchroModeManual);
			audio_engine->setLoopScaleMode(clicked_loop, ACAudioEngineScaleModeVocode);
			audio_engine->setScrub((float)scrub*10000); // temporary hack to scrub between 0 an 1
		}
	}
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/pitch") == 0)
	{
		float pitch;
		osc_browser->readFloat(mOscReceiver, &pitch);
		
		int clicked_loop = media_cycle->getClickedLoop();
		if (clicked_loop > -1)
		{
			/*
			 if (!is_pitching)
			 {	
			 is_pitching = true;
			 is_scrubing = false;
			 */ 
			//media_cycle->pickedObjectCallback(-1);
			audio_engine->setLoopSynchroMode(clicked_loop, ACAudioEngineSynchroModeAutoBeat);
			audio_engine->setLoopScaleMode(clicked_loop, ACAudioEngineScaleModeResample);
			//}
			audio_engine->setSourcePitch(clicked_loop, (float) pitch); 
		}
		
	}
	//std::cout << "End of OSC process messages" << std::endl;
}
