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

ACOsgTimelineEventHandler::ACOsgTimelineEventHandler()
: media_cycle(0),renderer(0),selecting_zone(false),selecting_ends(false),selection(0),
selection_begin(0.0f),selection_end(0.0f),pushed_x(0.0f)
{}

void ACOsgTimelineEventHandler::picked_object_callback(int pid) {
	media_cycle->pickedObjectCallback(pid);
}

void ACOsgTimelineEventHandler::hover_object_callback(int pid) {
	media_cycle->hoverObjectCallback(pid);
}

void ACOsgTimelineEventHandler::hover_callback(float xx, float yy) {
	media_cycle->hoverCallback(xx, yy);
}
	
bool ACOsgTimelineEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	float x = ea.getX();
	float y = ea.getY();
	float xx = ea.getXnormalized();
	float yy = ea.getYnormalized();
	
	switch(ea.getEventType())
	{
		case(osgGA::GUIEventAdapter::PUSH):
		{
			//printf("TIMELINE PUSH (%f, %f) (%f, %f) \n", x, y, xx, yy);
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (view) pick(view,ea,false);
			return false;
		}
		case(osgGA::GUIEventAdapter::DRAG):
		{
			//printf("TIMELINE DRAG (%f, %f) (%f, %f) \n", x, y, xx, yy);
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			//if (view) pick(view,ea,false);
			if(selecting_zone || selecting_ends)// && selection != 0)
			{
				//std::cout << "TIMELINE DRAGGING SELECTION " << selection->getName() << std::endl;
				
				/*
				osg::Vec3 local_intersect = hitr->getLocalIntersectPoint();
				osg::Vec3 world_intersect = hitr->getWorldIntersectPoint();
				std::cout << " Local " << local_intersect.x() << " " << local_intersect.y() << " " << local_intersect.z();// << std::endl;
				std::cout << " World " << world_intersect.x() << " " << world_intersect.y() << " " << world_intersect.z() << std::endl;
				*/
				float xspan = 0.65f; // magic number, track width in OSG coordinates, see ACOsgAudioTrackRenderer
				
				float pos = xx;
				if (xx>1) pos=1;
				if (xx<-1) pos=-1;
				
				//if (renderer->getTrack(selection->getID())->getSelectionEnd() >= renderer->getTrack(selection->getID())->getSelectionBegin()+xspan/10)// min width of the slider xspan/10 to refine
				//if (selection_end >= selection_begin+xspan/10)// min width of the slider xspan/10 to refine
				//{	
					if (selection->getName()=="track selection begin geode"){
						renderer->getTrack(selection->getID())->setSelectionBegin(xspan*pos/2.0f);
						renderer->getTrack(selection->getID())->setSelectionEnd(selection_end-(xspan*pos/2.0f-selection_begin));
						//std::cout << "Moving begin geode of track " << selection->getID() << std::endl;
					}	
					else if(selection->getName()=="track selection end geode"){
						renderer->getTrack(selection->getID())->setSelectionBegin(selection_begin-(xspan*pos/2.0f-selection_end));
						renderer->getTrack(selection->getID())->setSelectionEnd(xspan*pos/2.0f);
						//std::cout << "Moving end geode of track " << selection->getID() << std::endl;
					}
					else if(selection->getName()=="track selection zone geode"){
						if ( (selection_begin+xspan*(pos-pushed_x)/2.0f > -xspan/2.0f) && (selection_end+xspan*(pos-pushed_x)/2.0f < xspan/2.0f)){//selection can't slide out the screen
							renderer->getTrack(selection->getID())->setSelectionBegin(selection_begin+xspan*(pos-pushed_x)/2.0f);
							renderer->getTrack(selection->getID())->setSelectionEnd(selection_end+xspan*(pos-pushed_x)/2.0f);
						}	
					}	
				//}	
				
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
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			//if (view) pick(view, ea, true);
			return false;
		}/*   
		case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			std::cout << "Key (OSG) '" << (char)ea.getKey() << "'" << std::endl;
/*
			switch( ea.getKey() )
			{
				case osgGA::GUIEventAdapter::KEY_Space:
					break;
				default:
					break;
			}
 *//*
			return false;
		} 
		case(osgGA::GUIEventAdapter::KEYUP):
		{		
			return false;
		}    */
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
		
		std::cout << "Number of intersections: " <<intersections.size() << std::endl;
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
					
					if ((rid->getName()=="track selection begin geode")||(rid->getName()=="track selection end geode")||(rid->getName()=="track selection zone geode"))
					{
						switch(ea.getEventType())
						{
							case(osgGA::GUIEventAdapter::PUSH):
							{
								std::cout << "    Push on " << rid->getName() << std::endl;
								if ((rid->getName()=="track selection begin geode")||(rid->getName()=="track selection end geode"))
									selecting_ends = true;
								else {
									selecting_zone = true;
								}
								selection = rid;
								selection_begin = renderer->getTrack(selection->getID())->getSelectionBegin();
								selection_end = renderer->getTrack(selection->getID())->getSelectionEnd();
								pushed_x = ea.getXnormalized();
								break;
							}	
							case(osgGA::GUIEventAdapter::RELEASE):
							{
								std::cout << "    Release on " << rid->getName() << std::endl;
								selecting_zone = false;
								selecting_ends = false;
								selection = 0;
								selection_begin = 0.0f;
								selection_end = 0.0f;
								pushed_x = 0.0f;
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
		if (selecting_ends && selecting_zone)
			selecting_zone = false;
	}
}