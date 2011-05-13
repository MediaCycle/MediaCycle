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

ACOSCDockWidgetQt::ACOSCDockWidgetQt(QWidget *parent)
: ACAbstractDockWidgetQt(parent, MEDIA_TYPE_ALL,"ACOSCDockWidgetQt")
{
	#if defined (USE_OSC)	
	ui.setupUi(this); // first thing to do
	this->show();
	osc_browser = 0;
	osc_feedback = 0;
	osc_browser = new ACOscBrowser();
	osc_feedback = new ACOscFeedback();
	#endif //defined (USE_OSC)	
}

ACOSCDockWidgetQt::~ACOSCDockWidgetQt(){
	#if defined (USE_OSC)	
	if (osc_browser) {
		osc_browser->release();
		delete osc_browser;
		osc_browser = 0;
	}
	if (osc_feedback) {
		osc_feedback->release();
		delete osc_feedback;
		osc_feedback = 0;
	}
	#endif //defined (USE_OSC)	
}

#if defined (USE_OSC)
void ACOSCDockWidgetQt::on_pushButtonControlStart_clicked() {
	std::cout << "Control IP: " << ui.lineEditControlIP->text().toStdString() << std::endl;
	std::cout << "Control Port: " << ui.spinBoxControlPort->value() << std::endl;
	if ( ui.pushButtonControlStart->text().toStdString() == "Start") {
		osc_browser = new ACOscBrowser();
		osc_browser->create(ui.lineEditControlIP->text().toStdString().c_str(), ui.spinBoxControlPort->value());
		osc_browser->setUserData(this);
		osc_browser->setCallback(static_mess_handler);
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
	std::cout << "Feedback Port: " << ui.spinBoxFeedbackPort->value() << std::endl;
	if ( ui.pushButtonFeedbackStart->text().toStdString() == "Start") {	
		osc_feedback = new ACOscFeedback();
		osc_feedback->create(ui.lineEditFeedbackIP->text().toStdString().c_str(), ui.spinBoxFeedbackPort->value());
		osc_feedback->messageBegin("test");
		std::cout << "sending test messages to " << ui.lineEditFeedbackIP->text().toStdString().c_str() << " on port " << ui.spinBoxFeedbackPort->value() << " ..." << endl;
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

int ACOSCDockWidgetQt::static_mess_handler(const char *path, const char *types, lo_arg **argv,int argc, void *data, void *user_data){
	ACOSCDockWidgetQt* widget = (ACOSCDockWidgetQt*)user_data;
	widget->process_mess(path,types,argv,argc);
}

int ACOSCDockWidgetQt::process_mess(const char *path, const char *types, lo_arg **argv,int argc){	
	std::string tag = std::string(path);
	//std::cout << "OSC message: '" <<  tag << "'" << std::endl;
	bool ac = (tag.find("/audiocycle",0)!= string::npos);
	bool mc = (tag.find("/mediacycle",0)!= string::npos);
	if(!ac && !mc)//we don't process messages not containing /audiocycle or /mediacycle
		return 1;

	//if(!media_cycle || !this->getOsgView())
	//	return;
	
	if(tag.find("/test",0)!= string::npos)
	{
		std::cout << "OSC communication established" << std::endl;
		
		if (osc_feedback)
		{
			if (ac)
				osc_feedback->messageBegin("/audiocycle/received");
			else
				osc_feedback->messageBegin("/mediacycle/received");	
			osc_feedback->messageEnd();
			osc_feedback->messageSend();
		}
	}
	else if(tag.find("/fullscreen",0)!= string::npos)
	{
		int fullscreen = 0;
		fullscreen = argv[0]->i;
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
				x = argv[0]->f;
				y = argv[1]->f;
				
				float zoom = media_cycle->getCameraZoom();
				float angle = media_cycle->getCameraRotation();
				float xmove = x*cos(-angle)-y*sin(-angle);
				float ymove = y*cos(-angle)+x*sin(-angle);
				media_cycle->setCameraPosition(xmove/2/zoom , ymove/2/zoom); // norm [-1;1] = 2 (instead of 100 from mediacycle-osg)
				media_cycle->setNeedsDisplay(true);
			}
			else if(tag.find("/zoom",0)!= string::npos)
			{
				float zoom = 0.0f;
				zoom = argv[0]->f;
				//zoom = zoom*600/50; // refzoom +
				media_cycle->setCameraZoom((float)zoom);
				media_cycle->setNeedsDisplay(true);
			}
			else if(tag.find("/angle",0)!= string::npos)
			{
				float angle = 0.0f;
				angle = argv[0]->f;
				media_cycle->setCameraRotation((float)angle);
				media_cycle->setNeedsDisplay(true);
			}
		}
		else if(tag.find("/hover/xy",0)!= string::npos)
		{
			float x = 0.0, y = 0.0;
			x = argv[0]->f;
			y = argv[1]->f;
			
			media_cycle->hoverCallback(x,y);
			int closest_node = media_cycle->getClosestNode();
			float distance = this->getOsgView()->getBrowserRenderer()->getDistanceMouse()[closest_node];
			if (osc_feedback)
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
			return 1;
		
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
			bpm = argv[0]->f;
			
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
			scrub = argv[0]->f;
			
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
			pitch = argv[0]->f;
			
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

	return 1;	
	//std::cout << "End of OSC process messages" << std::endl;
}

#endif //defined (USE_OSC)