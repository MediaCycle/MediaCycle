/*
 *  ACOSCDockWidgetQt.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 05/04/11
 *  Filled by Christian Frisson on 03/05/11.
 *  @copyright (c) 2011 – UMONS - Numediart
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

#include "ACOSCDockWidgetQt.h"

static void osc_callback(const char *tagName, void *userData)
{
	printf("osc receiving tag: %s\n", tagName);
	ACOSCDockWidgetQt *widget = (ACOSCDockWidgetQt*)userData;
	//printf("osc received tag: %s\n", tagName);
	//std::cout << "tagName: " << tagName << std::endl;
	widget->processOscMessage(tagName);
	//printf("osc processed tag: %s\n", tagName);
}

ACOSCDockWidgetQt::ACOSCDockWidgetQt(QWidget *parent)
: ACAbstractDockWidgetQt(parent, MEDIA_TYPE_ALL,"ACOSCDockWidgetQt")
{
	ui.setupUi(this); // first thing to do
	this->show();	
	osc_browser = 0;
	osc_feedback = 0;
	osc_browser = new ACOscBrowser();
	osc_feedback = new ACOscFeedback();
}

ACOSCDockWidgetQt::~ACOSCDockWidgetQt(){
	if (osc_browser) {
		osc_browser->stop();		
		osc_browser->release();
		delete osc_browser;
		osc_browser = 0;
	}
	if (osc_feedback) {
		osc_feedback->release();
		delete osc_feedback;
		osc_feedback = 0;
	}
}

void ACOSCDockWidgetQt::on_pushButtonControlStart_clicked() {
	std::cout << "Control IP: " << ui.lineEditControlIP->text().toStdString() << std::endl;
	std::cout << "Control Port: " << ui.lineEditControlPort->text().toInt() << std::endl;
	if ( ui.pushButtonControlStart->text().toStdString() == "Start") {
		osc_browser = new ACOscBrowser();
		osc_browser->create(ui.lineEditControlIP->text().toStdString().c_str(), ui.lineEditControlPort->text().toInt());
		osc_browser->setUserData(this);
		osc_browser->setCallback(osc_callback);
		osc_browser->start();
		ui.pushButtonControlStart->setText("Stop");
	}	
	else if ( ui.pushButtonControlStart->text().toStdString() == "Stop") {
		osc_browser->stop();
		osc_browser->release();
		delete osc_browser;
		osc_browser = 0;
		ui.pushButtonControlStart->setText("Start");
	}
}	

void ACOSCDockWidgetQt::on_pushButtonFeedbackStart_clicked() {
	std::cout << "Feedback IP: " << ui.lineEditFeedbackIP->text().toStdString() << std::endl;
	std::cout << "Feedback Port: " << ui.lineEditFeedbackPort->text().toInt() << std::endl;
	if ( ui.pushButtonFeedbackStart->text().toStdString() == "Start") {	
		osc_feedback = new ACOscFeedback();
		osc_feedback->create(ui.lineEditFeedbackIP->text().toStdString().c_str(), ui.lineEditFeedbackPort->text().toInt());
		osc_feedback->messageBegin("test");
		std::cout << "sending test messages to " << ui.lineEditFeedbackIP->text().toStdString().c_str() << " on port " << ui.lineEditFeedbackPort->text().toInt() << " ..." << endl;
		osc_feedback->messageSend();
		ui.pushButtonFeedbackStart->setText("Stop");
	}	
	else if ( ui.pushButtonFeedbackStart->text().toStdString() == "Stop") {	
		osc_feedback->release();
		delete osc_feedback;
		osc_feedback = 0;
		ui.pushButtonFeedbackStart->setText("Start");
	}
}	

void ACOSCDockWidgetQt::processOscMessage(const char* tagName) {
	std::string tag = std::string(tagName);
	std::cout << "OSC message: '" <<  tag << "'" << std::endl;
	bool ac = (tag.find("/audiocycle",0)!= string::npos);
	bool mc = (tag.find("/mediacycle",0)!= string::npos);
	if(!ac && !mc)//we don't process messages not containing /audiocycle or /mediacycle
		return;

	//if(!media_cycle || !this->getOsgView())
	//	return;
	
	if(tag.find("/test",0)!= string::npos)
	{
		std::cout << "OSC communication established" << std::endl;
		
		if (osc_feedback && osc_feedback->isActive())
		{
			osc_feedback->messageBegin("/audiocycle/received");
			osc_feedback->messageEnd();
			osc_feedback->messageSend();
		}
		
	}
	else if(tag.find("/fullscreen",0)!= string::npos)
	{
		int fullscreen = 0;
		osc_browser->readInt(&fullscreen);
		std::cout << "Fullscreen? " << fullscreen << std::endl;
		/*if (fullscreen == 1)
			ui.groupControls->hide();
		else
			ui.groupControls->show();*/	
	}
	
	// BROWSER CONTROLS
	else if(tag.find("/browser",0)!= string::npos)
	{
		if(tag.find("/move",0)!= string::npos)
		{
			if(tag.find("/xy",0)!= string::npos)
			{
				float x = 0.0, y = 0.0;
				media_cycle->getCameraPosition(x,y);
				osc_browser->readFloat(&x);
				osc_browser->readFloat(&y);
				
				float zoom = media_cycle->getCameraZoom();
				float angle = media_cycle->getCameraRotation();
				float xmove = x*cos(-angle)-y*sin(-angle);
				float ymove = y*cos(-angle)+x*sin(-angle);
				media_cycle->setCameraPosition(xmove/2/zoom , ymove/2/zoom); // norm [-1;1] = 2 (instead of 100 from mediacycle-osg)
				media_cycle->setNeedsDisplay(true);
			}
			else if(tag.find("/zoom",0)!= string::npos)
			{
				float zoom;//, refzoom = media_cycle->getCameraZoom();
				osc_browser->readFloat(&zoom);
				//zoom = zoom*600/50; // refzoom +
				media_cycle->setCameraZoom((float)zoom);
				media_cycle->setNeedsDisplay(true);
			}
			else if(tag.find("/angle",0)!= string::npos)
			{
				float angle;//, refangle = media_cycle->getCameraRotation();
				osc_browser->readFloat(&angle);
				media_cycle->setCameraRotation((float)angle);
				media_cycle->setNeedsDisplay(true);
			}
		}
		else if(tag.find("/hover/xy",0)!= string::npos)
		{
			float x = 0.0, y = 0.0;
			osc_browser->readFloat(&x);
			osc_browser->readFloat(&y);
			
			media_cycle->hoverCallback(x,y);
			int closest_node = media_cycle->getClosestNode();
			float distance = this->getOsgView()->getBrowserRenderer()->getDistanceMouse()[closest_node];
			if (osc_feedback && osc_feedback->isActive())
			{
				osc_feedback->messageBegin("/audiocycle/closest_node_at");
				osc_feedback->messageAppendFloat(distance);
				osc_feedback->messageEnd();
				osc_feedback->messageSend();
			}	
			media_cycle->setNeedsDisplay(true);
		}
		else if(tag.find("/recenter",0)!= string::npos)
		{
			media_cycle->setCameraRecenter();
		}
		else if(tag.find("/recluster",0)!= string::npos)
		{		
			//int node = media_cycle->getClickedNode();
			int node = media_cycle->getClosestNode();
			if (media_cycle->getLibrary()->getSize()>0 && media_cycle->getBrowser()->getMode() == AC_MODE_CLUSTERS && node > -1)
			{
				media_cycle->setReferenceNode(node);
				// media_cycle->pushNavigationState(); XS 250810 removed
				media_cycle->updateDisplay(true);
			}
		}
		else if(tag.find("/back",0)!= string::npos)
		{		
			media_cycle->goBack();
		}
		else if(tag.find("/forward",0)!= string::npos)
		{		
			media_cycle->goForward();
		}
		else if(tag.find("/library/load",0)!= string::npos)
		{
			std::cerr << "Library loading thru OSC not yet implemented" << std::endl;
		}
		else if(tag.find("/library/clear",0)!= string::npos)
		{
			std::cerr << "Library cleaning thru OSC not yet implemented" << std::endl;
			
			/*this->media_cycle->cleanLibrary();
			this->media_cycle->cleanBrowser();
		
			//CF make the following accessible from a dock manager
		
			//was cleanCheckBoxes()
			//for (int d=0;d<dockWidgets.size();d++){
			//	if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersNeighborsDockWidgetQt") {
			//		((ACBrowserControlsClustersNeighborsDockWidgetQt*)dockWidgets[d])->cleanCheckBoxes();
			//	}
			//	if (dockWidgets[d]->getClassName() == "ACBrowserControlsClustersDockWidgetQt") {
			//		((ACBrowserControlsClustersDockWidgetQt*)dockWidgets[d])->cleanCheckBoxes();
			//	}
			//}	
		
			// XS TODO : remove the boxes specific to the media that was loaded
			// e.g. ACAudioControlDockWidgets
			// modify the DockWidget's API to allow this
			//plugins_scanned = false;
		
			this->getOsgView()->clean();
			this->getOsgView()->setFocus();*/
		}	
	}
	else if(tag.find("/player",0)!= string::npos)
	{
		if (media_cycle->getLibrary()->getMediaType() == MEDIA_TYPE_AUDIO && !this->getAudioEngine())
			return;
		
		if(tag.find("/playclosestloop",0)!= string::npos)
		{	
			media_cycle->pickedObjectCallback(-1);
		}
		else if(tag.find("/muteall",0)!= string::npos)
		{	
			media_cycle->muteAllSources();
		}
		else if(tag.find("/bpm",0)!= string::npos)
		{
			float bpm;
			osc_browser->readFloat(&bpm);
			
			//int node = media_cycle->getClickedNode();
			//int node = media_cycle->getClosestNode();
			int node = media_cycle->getLastSelectedNode();
			
			if (node > -1)
			{
				audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeAutoBeat);
				audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);
				audio_engine->setBPM((float)bpm);
			}
		}	
		else if(tag.find("/scrub",0)!= string::npos)
		{
			float scrub;
			osc_browser->readFloat(&scrub);
			
			//int node = media_cycle->getClickedNode();
			//int node = media_cycle->getClosestNode();
			int node = media_cycle->getLastSelectedNode();
			
			if (node > -1)
			{
				//media_cycle->pickedObjectCallback(-1);
				audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeManual);
				audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);//ACAudioEngineScaleModeVocode
				audio_engine->setScrub((float)scrub*100); // temporary hack to scrub between 0 an 1
			}
		}
		else if(tag.find("/pitch",0)!= string::npos)
		{
			float pitch;
			osc_browser->readFloat(&pitch);
			
			//int node = media_cycle->getClickedNode();
			//int node = media_cycle->getClosestNode();
			int node = media_cycle->getLastSelectedNode();
			
			if (node > -1)
			{
				/*
				 if (!is_pitching)
				 {	
				 is_pitching = true;
				 is_scrubing = false;
				 */ 
				//media_cycle->pickedObjectCallback(-1);
				audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeAutoBeat);
				audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);
				//}
				audio_engine->setSourcePitch(node, (float) pitch); 
			}
			
		}
	}
	//std::cout << "End of OSC process messages" << std::endl;
}