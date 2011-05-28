/*
 *  ACOscBrowser.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 03/04/11
 *
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

#include "ACOscBrowser.h"

using namespace std;

void error(int num, const char *msg, const char *path)
{
    std::cout << "liblo server error " << num << " in path " << path << ": " << msg << std::endl;
    fflush(stdout);
}

void ACOscBrowser::create(const char *hostname, int port)
{
	char portchar[6];
  	sprintf(portchar,"%d",port);
	server_thread = lo_server_thread_new(portchar, error);
	
	this->setUserData(this);
	this->setCallback(static_mess_handler);
}

void ACOscBrowser::release()
{
	if (server_thread){
		lo_server_thread_free(server_thread);
		server_thread = 0;
	}
}

void ACOscBrowser::start()
{
	lo_server_thread_start(server_thread);
}

void ACOscBrowser::stop()
{
	lo_server_thread_stop(server_thread);
}

void ACOscBrowser::setUserData(void *_user_data)
{
	user_data = _user_data;
}

void ACOscBrowser::setCallback(ACOscBrowserCallback* _callback)
{
	callback = _callback;
	lo_server_thread_add_method(server_thread, NULL, NULL, *_callback, user_data);
}

// these can be called only from the callback
void ACOscBrowser::readFloat(float *val)
{
}

void ACOscBrowser::readInt(int *val)
{
}

void ACOscBrowser::readString(char *val, int maxlen)
{	
}

int ACOscBrowser::static_mess_handler(const char *path, const char *types, lo_arg **argv,int argc, void *data, void *user_data){
	ACOscBrowser* widget = (ACOscBrowser*)user_data;
	widget->process_mess(path,types,argv,argc);
}

int ACOscBrowser::process_mess(const char *path, const char *types, lo_arg **argv,int argc){
	std::string tag = std::string(path);
	//std::cout << "OSC message: '" << tag << "'" << std::endl;
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
		//if (fullscreen == 1)
		//	ui.groupControls->hide();
		//else
		//	ui.groupControls->show();
	}
	
	// BROWSER CONTROLS
	else if(tag.find("/browser",0)!= string::npos && media_cycle)
	{
		//Extracting the pointer/device integer id between "/browser/" and subsequent "/..." strings
		int id = -1;
		std::string prefix = ("/browser/");
		std::string suffix = ("/");
		size_t prefix_found = tag.find(prefix,0);
		if(prefix_found!= string::npos){
			size_t suffix_found = tag.find(suffix,prefix_found+prefix.size());
			if (suffix_found!= string::npos){
				std::string id_string = tag.substr(prefix_found+prefix.size(),suffix_found-(prefix_found+prefix.size()));
				
				//id = atoi(id_string.c_str());// can't check errors with this
				
				istringstream id_ss(id_string);
				if (!(id_ss>>id))
					std::cerr << "ACOSCDockWidgetQt: wrong pointer id" << std::endl;
				
				//try{
				//	id = boost::lexical_cast<int>(id_string);
				//}
				//catch(boost::bad_lexical_cast &){
				//	std::cerr << "ACOSCDockWidgetQt: wrong pointer id" << std::endl;
				//}
				//std::cout << "id_string " << id_string << " id " << id << std::endl;
			}
		}
		
		if(tag.find("/activated",0)!= string::npos)
		{
			//std::cout << "OSC message: '" << tag << "'" << std::endl;
			media_cycle->resetPointers();//CF temp: hack, when /released messages aren't received
			media_cycle->addPointer(id);
			//Ugly
			//osg_view->getHUDRenderer()->preparePointers();
			media_cycle->setNeedsDisplay(true);
		}
		else if(tag.find("/released",0)!= string::npos)
		{
			//std::cout << "OSC message: '" << tag << "'" << std::endl;
			media_cycle->removePointer(id);//CF hack
			//Ugly
			//osg_view->getHUDRenderer()->preparePointers();
			media_cycle->setNeedsDisplay(true);
		}
		else if(tag.find("/reset_pointers",0)!= string::npos)
		{
			//std::cout << "OSC message: '" << tag << "'" << std::endl;
			media_cycle->resetPointers();
			//Ugly
			//osg_view->getHUDRenderer()->preparePointers();
			media_cycle->setNeedsDisplay(true);
			std::cout << "Reset to " << media_cycle->getNumberOfPointers() << " pointer(s)" << std::endl;
		}
		else if(tag.find("/move/xy",0)!= string::npos)
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
		else if((tag.find("/rotate",0)!= string::npos)||(tag.find("/angle",0)!= string::npos))
		{
			float angle = 0.0f;
			angle = argv[0]->f;
			media_cycle->setCameraRotation((float)angle);
			media_cycle->setNeedsDisplay(true);
		}
		else if(tag.find("/hover/xy",0)!= string::npos)
		{
			float x = 0.0, y = 0.0;
			if(argc == 2 && types[0] == LO_FLOAT && types[1] == LO_FLOAT){
				x = argv[0]->f;
				y = argv[1]->f;
				
				//media_cycle->hoverCallback(x,y,id);
				ACPoint p;
				p.x = x;
				p.y = y;
				p.z = 0;
				media_cycle->getPointerFromId(id)->setCurrentPosition(p);
				if (media_cycle && media_cycle->getLibrary()->getSize() > 0){
					
					////////////////Ugly
					media_cycle->setAutoPlay(1);
					
					//CF hack
					//int closest_node = media_cycle->getClosestNode();
					//float distance = this->getOsgView()->getBrowserRenderer()->getDistanceMouse()[closest_node];
					//if (osc_feedback)
					//{
					//	if (ac)
					//		osc_feedback->messageBegin("/audiocycle/closest_node_at");
					//	else
					//		osc_feedback->messageBegin("/mediacycle/closest_node_at");
					//	osc_feedback->messageAppendFloat(distance);
					//	osc_feedback->messageEnd();
					//	osc_feedback->messageSend();
					//}
				}
				//media_cycle->setNeedsDisplay(true);
			}
			else {
				std::cout << "ACOSCDockWidgetQt: error with tag'" << tag << "'";
				for (int a=0;a<argc;a++){
					std::cout << " <" << types[a] << ">";
				}
				std::cout << std::endl;
			}
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
			std::cerr << "ACOSCDockWidgetQt: Library loading thru OSC not yet implemented" << std::endl;
		}
		else if(tag.find("/library/clear",0)!= string::npos)
		{
			std::cerr << "ACOSCDockWidgetQt: Library cleaning thru OSC not yet implemented" << std::endl;
			
			//this->media_cycle->cleanLibrary();
			//this->media_cycle->cleanBrowser();
			
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
			
			//this->getOsgView()->clean();
			//this->getOsgView()->setFocus();
		}
	}

	else if(tag.find("/player",0)!= string::npos && media_cycle)
	{
		#if defined (SUPPORT_AUDIO)
		if(media_cycle->getLibrary()->getMediaType() == MEDIA_TYPE_AUDIO && !this->getAudioEngine())
			return 1;
		#endif //defined (SUPPORT_AUDIO)
		
		if(tag.find("/playclosestloop",0)!= string::npos)
		{
			media_cycle->pickedObjectCallback(-1);
		}
		else if(tag.find("/muteall",0)!= string::npos)
		{
			//if(this->getMediaType()==MEDIA_TYPE_AUDIO)
				media_cycle->resetPointers(); //CF hack dirty!
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
				#if defined (SUPPORT_AUDIO)
				audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeAutoBeat);
				audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);
				audio_engine->setBPM((float)bpm);
				#endif //defined (SUPPORT_AUDIO)
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
				#if defined (SUPPORT_AUDIO)
				audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeManual);
				audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);//ACAudioEngineScaleModeVocode
				audio_engine->setScrub((float)scrub*100); // temporary hack to scrub between 0 an 1
				#endif //defined (SUPPORT_AUDIO)
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
				//if (!is_pitching)
				// {
				//is_pitching = true;
				// is_scrubing = false;
				//media_cycle->pickedObjectCallback(-1);
				#if defined (SUPPORT_AUDIO)
				audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeAutoBeat);
				audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);
				//}
				audio_engine->setSourcePitch(node, (float) pitch);
				#endif //defined (SUPPORT_AUDIO)
			}
			
		}
	}
	
	return 1;
	//std::cout << "End of OSC process messages" << std::endl;
}
