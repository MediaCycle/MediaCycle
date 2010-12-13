/*
 *  ACOsgTimelineEventHandler.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 28/05/09
 *  @copyright (c) 2009 – UMONS - Numediart
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

#include "ACOsgTimelineEventHandler.h"
#include <iostream>
#include <sstream>
#include "ACAudio.h"

ACOsgTimelineEventHandler::ACOsgTimelineEventHandler()
: media_cycle(0),renderer(0),selecting_zone(false),selecting_zone_begin(false),selecting_zone_end(false),selecting_summary_waveform(false),selection(0),
selection_begin(0.0f),selection_end(0.0f),pushed_x(0.0f)
{
	audio_engine = NULL;
}

bool ACOsgTimelineEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	float x = ea.getX();
	float y = ea.getY();
	float xx = ea.getXnormalized();
	float yy = ea.getYnormalized();
	float pos = xx;
	if (xx>1) pos=1.0f;
	if (xx<-1) pos=-1.0f;
	float xspan = 0.65f; // magic number, track width in OSG coordinates, see ACOsgAudioTrackRenderer
	
	switch(ea.getEventType())
	{
		case(osgGA::GUIEventAdapter::PUSH):
		{
			//printf("TIMELINE PUSH (%f, %f) (%f, %f) \n", x, y, xx, yy);
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (view){
				pick(view,ea,false);
				// Skipping/Moving the selection zone to the mouse position
				if(selecting_summary_waveform){
					// Audio feedback
					/*int mediaID = renderer->getTrack(selection->getID())->getMedia()->getId();
					if (mediaID > -1){
						if (media_cycle->getLibrary()->getMedia(mediaID)->getType() == MEDIA_TYPE_AUDIO){
							//std::cout << "Skipping to frame: " << (int)((pos+1.0f)*((ACAudio*) media_cycle->getLibrary()->getMedia(mediaID))->getNFrames()) << std::endl;
							if(audio_engine){
								audio_engine->setLoopSynchroMode(mediaID, ACAudioEngineSynchroModeAutoBeat);
								audio_engine->setLoopScaleMode(mediaID, ACAudioEngineScaleModeResample);
								audio_engine->setScrub(pos+1.0f);
							}
						}	
					}*/
					// Visual feedback
					float selection_width = selection_end - selection_begin;
					/*if ( xspan*pos/2.0f < -xspan/2.0f + selection_width/2.0f )
						pos = (-xspan/2.0f + selection_width/2.0f)*2.0f/xspan;
					else if ( xspan*pos/2.0f > xspan/2.0f - selection_width/2.0f )
						pos = (xspan/2.0f - selection_width/2.0f)*2.0f/xspan;*/
					renderer->getTrack(selection->getID())->setSelectionBegin(xspan*pos/2.0f-selection_width/2.0f);
					renderer->getTrack(selection->getID())->setSelectionEnd(xspan*pos/2.0f+selection_width/2.0f);
				}
			}	
			return false;
		}
		case(osgGA::GUIEventAdapter::DRAG):
		{
			//printf("TIMELINE DRAG (%f, %f) (%f, %f) \n", x, y, xx, yy);
			/*osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (view) pick(view,ea,false);*/
			
			//Resizing the selection zone by moving its left boundary
			if (selecting_zone_begin){
				renderer->getTrack(selection->getID())->setSelectionBegin(xspan*pos/2.0f);
				renderer->getTrack(selection->getID())->setSelectionEnd(selection_end-(xspan*pos/2.0f-selection_begin));
				//std::cout << "Moving begin geode of track " << selection->getID() << std::endl;
			}
			//Resizing the selection zone by moving its right boundary
			else if(selecting_zone_end){
				renderer->getTrack(selection->getID())->setSelectionBegin(selection_begin-(xspan*pos/2.0f-selection_end));
				renderer->getTrack(selection->getID())->setSelectionEnd(xspan*pos/2.0f);
				//std::cout << "Moving end geode of track " << selection->getID() << std::endl;
			}
			//Moving the selection zone
			else if(selecting_zone){
				if ( (selection_begin+xspan*(pos-pushed_x)/2.0f > -xspan/2.0f) && (selection_end+xspan*(pos-pushed_x)/2.0f < xspan/2.0f)){//selection can't slide out the screen
					// Audio feedback
					/*int mediaID = renderer->getTrack(selection->getID())->getMedia()->getId();
					if (mediaID > -1){
						if (media_cycle->getLibrary()->getMedia(mediaID)->getType() == MEDIA_TYPE_AUDIO){
							//std::cout << "Skipping to frame: " << (int)((pos+1.0f)*((ACAudio*) media_cycle->getLibrary()->getMedia(mediaID))->getNFrames()) << std::endl;
							if(audio_engine){
								audio_engine->setLoopSynchroMode(mediaID, ACAudioEngineSynchroModeAutoBeat);
								audio_engine->setLoopScaleMode(mediaID, ACAudioEngineScaleModeResample);
								//audio_engine->setScrub((int)((pos+1.0f)*((ACAudio*) media_cycle->getLibrary()->getMedia(mediaID))->getNFrames())); 
								audio_engine->setScrub(pos+1.0f);
							}
						}	
					}*/
					// Visual feedback
					renderer->getTrack(selection->getID())->setSelectionBegin(selection_begin+xspan*(pos-pushed_x)/2.0f);
					renderer->getTrack(selection->getID())->setSelectionEnd(selection_end+xspan*(pos-pushed_x)/2.0f);
				}	
			}	
			return false;
		}
		case(osgGA::GUIEventAdapter::RELEASE):
		{
			//printf("TIMELINE RELEASE (%f, %f) (%f, %f) \n", x, y, xx, yy);
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (view) pick(view,ea,false);
			return false;
		}
		case(GUIEventAdapter::MOVE):
		{
			//printf("TIMELINE MOVE (%f, %f) (%f, %f) \n", x, y, xx, yy);
			/*osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (view) pick(view, ea, true);*/
			return false;
		}/*   
		case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			//std::cout << "Key (OSG) '" << (char)ea.getKey() << "'" << std::endl;
			switch( ea.getKey() )
			{
				case osgGA::GUIEventAdapter::KEY_Space:
					break;
				default:
					break;
			}
			return false;
		} 
		case(osgGA::GUIEventAdapter::KEYUP):
		{		
			return false;
		}*/
		case(GUIEventAdapter::FRAME):
		{
			//XS : not used, was in RS code
			//render_callback();
			
			return false;
		}
		default:
		{	
			//printf("received event: %d\n", ea.getEventType());
			return false;
		}	
	}
}

void ACOsgTimelineEventHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, bool hover)
{
	osgUtil::LineSegmentIntersector::Intersections intersections;
	
	float x = ea.getX();
	float y = ea.getY();
	float xx = ea.getXnormalized();
	float yy = ea.getYnormalized();
	//printf("pick (%f, %f)\n", x, y);
	//printf ("MOUSE: %f %f\n", x, y);
/*	
	if (view->computeIntersections(x,y,intersections))
	{
*/
	osg::ref_ptr< osgUtil::LineSegmentIntersector > picker = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, x, y);
    osgUtil::IntersectionVisitor iv(picker.get());
    view->getCamera()->accept(iv);
    if (picker->containsIntersections())
    {
        intersections = picker->getIntersections();
		//printf("got intersections\n");
		
		//std::cout << "Number of intersections: " <<intersections.size() << std::endl;
		for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
		    hitr != intersections.end();
		    ++hitr)
		{
			std::ostringstream os;
			if (!hitr->nodePath.empty() /*&& !(hitr->nodePath.back()->getName().empty())*/)
			{
				if(hitr->nodePath.back()->getUserData())
				{
					ACRefId *rid = (ACRefId*)hitr->nodePath.back()->getUserData();
					//std::cout << "  Widget " << rid->getName() << " with object id " << rid->object_id << std::endl;
					if ((rid->getName()=="track selection begin")||(rid->getName()=="track selection end")||(rid->getName()=="track selection zone")||(rid->getName()=="track summary waveform"))
					{
						switch(ea.getEventType())
						{
							case(osgGA::GUIEventAdapter::PUSH):
							{
								//std::cout << "    Push on " << rid->getName() << std::endl;
								if (rid->getName()=="track selection begin") selecting_zone_begin = true;
								else if (rid->getName()=="track selection end") selecting_zone_end = true;
								else if (rid->getName()=="track selection zone") selecting_zone = true;
								else if (rid->getName()=="track summary waveform") selecting_summary_waveform = true;								
								selection = rid;
								selection_begin = renderer->getTrack(selection->getID())->getSelectionBegin();
								selection_end = renderer->getTrack(selection->getID())->getSelectionEnd();
								pushed_x = ea.getXnormalized();
								//if ((rid->getName()!="track selection begin")&&(rid->getName()!="track selection end"))
									renderer->getTrack(rid->getID())->setManualSelection(true);
								break;
							}	
							case(osgGA::GUIEventAdapter::RELEASE):
							{
								//std::cout << "    Release on " << rid->getName() << std::endl;
								if(selecting_zone){
									int mediaID = renderer->getTrack(selection->getID())->getMedia()->getId();
									if (mediaID > -1){
										if (media_cycle->getLibrary()->getMedia(mediaID)->getType() == MEDIA_TYPE_AUDIO){
												audio_engine->setLoopSynchroMode(mediaID, ACAudioEngineSynchroModeAutoBeat);
												audio_engine->setLoopScaleMode(mediaID, ACAudioEngineScaleModeResample);
										}
									}
								}	
								selecting_zone = false;
								selecting_zone_begin = false;
								selecting_zone_end = false;
								selecting_summary_waveform = false;
								selection = 0;
								selection_begin = 0.0f;
								selection_end = 0.0f;
								pushed_x = 0.0f;
								renderer->getTrack(rid->getID())->setManualSelection(false);
								break;
							}
							/*	
							case(osgGA::GUIEventAdapter::MOVE):
							{	
								std::cout << "Moving in" << std::endl;
								break;
							}
							*/ 
							default:
							{															  
								break;
							}
									
						}																  
					}
					//break ; // only pick the first one
				}
				//printf("picked %s\n", hitr->nodePath.back()->getName().c_str());
			}
		}
		if (ea.getEventType()==osgGA::GUIEventAdapter::PUSH){
			//std::cout << "  Selecting: zone("<<selecting_zone<<") begin("<<selecting_zone_begin<<") end("<<selecting_zone_end<<") summary("<<selecting_summary_waveform<<")"<<std::endl;
			if (selecting_zone && selecting_summary_waveform)
				selecting_summary_waveform = false;
			//std::cout << "  Selecting: zone("<<selecting_zone<<") begin("<<selecting_zone_begin<<") end("<<selecting_zone_end<<") summary("<<selecting_summary_waveform<<")"<<std::endl;
			if ((selecting_zone_begin || selecting_zone_end) && (selecting_zone || selecting_summary_waveform)){
				selecting_zone = false;
				selecting_summary_waveform = false;
			}
			//std::cout << "  Selecting: zone("<<selecting_zone<<") begin("<<selecting_zone_begin<<") end("<<selecting_zone_end<<") summary("<<selecting_summary_waveform<<")"<<std::endl;
		}
	}
}