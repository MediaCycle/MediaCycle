/*
 *  ACMediaBrowserOsgEventHandler.cpp
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

#include "ACOsgBrowserEventHandler.h"
#include <iostream>

bool ACOsgBrowserEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	if (media_cycle == 0) return false;
	switch(ea.getEventType())
	{
		case(osgGA::GUIEventAdapter::PUSH):
		{
			//printf("event PUSH: aa=%x\n", &aa);
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			//if (view) pick(view,ea,false);
			if (view) clickNode(view,ea,false);
            
			return false;
		}
		case(osgGA::GUIEventAdapter::RELEASE):
		{
			//printf("event RELEASE: aa=%x\n", &aa);
            osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (view) pick(view,ea,false);
            releaseClickedNode();
			
			return false;
		}
		case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			#if defined (USE_DEBUG)
                //std::cout << "Key (OSG) '" << (char)ea.getKey() << "'" << std::endl;
			#endif //defined (USE_DEBUG)
/*
			switch( ea.getKey() )
			{
				case osgGA::GUIEventAdapter::KEY_Space:
					break;
				default:
					break;
			}
 */
			return false;
		}
		case(osgGA::GUIEventAdapter::KEYUP):
		{
			return false;
		}
		case(osgGA::GUIEventAdapter::FRAME):
		{
			//XS : not used, was in RS code
			//render_callback();
			//std::cout << "Browser Event Handler FRAME" << std::endl;
			//std::cout << "Mouse move X "<<ea.getX()<<" ["<<ea.getXmin()<<";"<<ea.getXmax()<<"] Y "<< ea.getY()<<" ["<<ea.getYmin()<<";"<<ea.getYmax()<<"]" << std::endl;
			return false;
		}
		case(osgGA::GUIEventAdapter::MOVE):
		{
			//printf("mouse moved in browser\n");
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (view) pick(view, ea, true);
			//std::cout << "Browser Event Handler MOVE" << std::endl;
			//std::cout << "Mouse move X "<<ea.getX()<<" ["<<ea.getXmin()<<";"<<ea.getXmax()<<"] Y "<< ea.getY()<<" ["<<ea.getYmin()<<";"<<ea.getYmax()<<"]" << std::endl;
			/*int border = 10; //px
			if ( ea.getX()>ea.getXmax()-border || ea.getX()<ea.getXmin()+border || ea.getY()>ea.getYmax()-border || ea.getY()<ea.getYmin()+border )
				std::cout << "Mouse out " << std::endl;
			else
				std::cout << "Mouse in " << std::endl;
			*/
			return false;
		}
		case(osgGA::GUIEventAdapter::DRAG):
		{
			//printf("mouse dragged in browser\n");
			// SD NOTE - On 2D touch devices, there is no such thing as a move
			// So pointer move has to be handled here
#if defined(APPLE_IOS)
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (view) pick(view, ea, true);
#endif
			return false;
		}
		case(osgGA::GUIEventAdapter::DOUBLECLICK):
		{
			return false;
		}
		case(osgGA::GUIEventAdapter::RESIZE):
		{
			return false;
		}
#if !defined(APPLE_IOS)
		case(osgGA::GUIEventAdapter::SCROLL):
		{
			switch(ea.getScrollingMotion()) {
				case(osgGA::GUIEventAdapter::SCROLL_UP):
				{
					return false;
				}
				case(osgGA::GUIEventAdapter::SCROLL_DOWN):
				{
					return false;
				}
				case(osgGA::GUIEventAdapter::SCROLL_LEFT):
				{
					return false;
				}
				case(osgGA::GUIEventAdapter::SCROLL_RIGHT):
				{
					return false;
				}
				default:
				{
					//printf("received event of type 'scroll': %d\n", ea.getScrollingMotion());
					return false;
				}
			}
		}
#endif
		default:
		{
			printf("received event: %d\n", ea.getEventType());
			return false;
		}
	}
}

void ACOsgBrowserEventHandler::clickNode(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, bool hover)
{
	if (media_cycle == 0) return;
	osgUtil::LineSegmentIntersector::Intersections intersections;
	//osgUtil::PolytopeIntersector::Intersections intersections;
    
	std::string gdlist="";
	float x = ea.getX();
	float y = ea.getY();
    
	float xx = ea.getXnormalized();
	float yy = ea.getYnormalized();
    
	//printf("pick (%f, %f)\n", x, y);
    
	//printf ("MOUSE: %f %f\n", x, y);
    
	if(hover) {
		// SD TODO - OSG computeIntersections seems to crash often - avoid doing it while howering
		//CF should we loop on the hoverWithPointer for each pointer?
		if (media_cycle->getPointerFromId(-1))
			media_cycle->getPointerFromId(-1)->setCurrentPosition(xx,yy);
        //		media_cycle->hoverWithPointerId(xx, yy, -1);//mouse//TR NEM2011 1! Mouse Constructor call (cf		void ACOsgCompositeViewQt::updateGL()
		return;
	}
    
	if (view->computeIntersections(x,y,intersections))
	{
		//printf("got intersections\n");
        
		for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
		    hitr != intersections.end();
		    ++hitr)
		{
			std::ostringstream os;
			if (!hitr->nodePath.empty() /*&& !(hitr->nodePath.back()->getName().empty())*/)
			{
				// the geodes are identified by name.
				//os<<"Object \""<<hitr->nodePath.back()->getName()<<"\""<<std::endl;
                
				if(hitr->nodePath.back()->getUserData())
				{
					ACRefId *rid = (ACRefId*)hitr->nodePath.back()->getUserData();
                    
					if(!hover)
						media_cycle->setClickedNode(rid->object_id);
					break ; // only pick the first one
				}
                
				//printf("picked %s\n", hitr->nodePath.back()->getName().c_str());
			}
			else if (hitr->drawable.valid())
			{
				//os<<"Object \""<<hitr->drawable->className()<<"\""<<std::endl;
			}
            
			/*os<<"        local coords vertex("<< hitr->getLocalIntersectPoint()<<")"<<"  normal("<<hitr->getLocalIntersectNormal()<<")"<<std::endl;
			 os<<"        world coords vertex("<< hitr->getWorldIntersectPoint()<<")"<<"  normal("<<hitr->getWorldIntersectNormal()<<")"<<std::endl;*/
			const osgUtil::LineSegmentIntersector::Intersection::IndexList& vil = hitr->indexList;
			for(unsigned int i=0;i<vil.size();++i)
			{
				os<<"        vertex indices ["<<i<<"] = "<<vil[i]<<std::endl;
			}
            
			gdlist += os.str();
		}
	}
	//setLabel(gdlist);
}

void ACOsgBrowserEventHandler::releaseClickedNode(){
	if (media_cycle == 0) return;
    if (media_cycle->getClickedNode()>-1)
        media_cycle->setClickedNode(-1);
    
}

void ACOsgBrowserEventHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, bool hover)
{
	if (media_cycle == 0) return;
	osgUtil::LineSegmentIntersector::Intersections intersections;
	//osgUtil::PolytopeIntersector::Intersections intersections;

	std::string gdlist="";
	float x = ea.getX();
	float y = ea.getY();

	float xx = ea.getXnormalized();
	float yy = ea.getYnormalized();

	//printf("pick (%f, %f)\n", x, y);

	//printf ("MOUSE: %f %f\n", x, y);

	if(hover) {
		// SD TODO - OSG computeIntersections seems to crash often - avoid doing it while howering
		//CF should we loop on the hoverWithPointer for each pointer?
		if (media_cycle->getPointerFromId(-1))
			media_cycle->getPointerFromId(-1)->setCurrentPosition(xx,yy);
//		media_cycle->hoverWithPointerId(xx, yy, -1);//mouse//TR NEM2011 1! Mouse Constructor call (cf		void ACOsgCompositeViewQt::updateGL()
		return;
	}

	if (view->computeIntersections(x,y,intersections))
	{
		//printf("got intersections\n");

		for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
		    hitr != intersections.end();
		    ++hitr)
		{
			std::ostringstream os;
			if (!hitr->nodePath.empty() /*&& !(hitr->nodePath.back()->getName().empty())*/)
			{
				// the geodes are identified by name.
				//os<<"Object \""<<hitr->nodePath.back()->getName()<<"\""<<std::endl;

				if(hitr->nodePath.back()->getUserData())
				{
					ACRefId *rid = (ACRefId*)hitr->nodePath.back()->getUserData();

					if(!hover)
						media_cycle->pickedObjectCallback(rid->object_id);
					break ; // only pick the first one
				}

				//printf("picked %s\n", hitr->nodePath.back()->getName().c_str());
			}
			else if (hitr->drawable.valid())
			{
				//os<<"Object \""<<hitr->drawable->className()<<"\""<<std::endl;
			}

			/*os<<"        local coords vertex("<< hitr->getLocalIntersectPoint()<<")"<<"  normal("<<hitr->getLocalIntersectNormal()<<")"<<std::endl;
			 os<<"        world coords vertex("<< hitr->getWorldIntersectPoint()<<")"<<"  normal("<<hitr->getWorldIntersectNormal()<<")"<<std::endl;*/
			const osgUtil::LineSegmentIntersector::Intersection::IndexList& vil = hitr->indexList;
			for(unsigned int i=0;i<vil.size();++i)
			{
				os<<"        vertex indices ["<<i<<"] = "<<vil[i]<<std::endl;
			}

			gdlist += os.str();
		}
	}
	//setLabel(gdlist);
}

// SD - Other pick possibilities, exploring Polytope intersector, first idea for zoooming thumbnails, replaced by
//		computation of distance_mouse

/*
void ACImageBrowserOsgEventHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, bool hover)
{
	if (media_cycle == 0) return;
	std::string gdlist="";
	float x = ea.getXnormalized();
	float y = ea.getYnormalized();

	double w(.05), h(.05);

	osgUtil::PolytopeIntersector *picker = new osgUtil::PolytopeIntersector(osgUtil::Intersector::PROJECTION,x-w, y-h, x+w, y+h);
	osgUtil::IntersectionVisitor iv(picker);
	view->getCamera()->accept(iv);

	if(hover) media_cycle->hoverWithPointerId(x,y,-1);//mouse

	if (picker->containsIntersections()) {

		osgUtil::PolytopeIntersector::Intersections intersections = picker->getIntersections();
		osgUtil::PolytopeIntersector::Intersections::iterator intersection_iter = intersections.begin();

		for (;intersection_iter != intersections.end(); intersection_iter++) {

			if(intersection_iter->nodePath.back()->getUserData()) {

				ACRefId *rid = (ACRefId*)intersection_iter->nodePath.back()->getUserData();
				if(!hover)
					media_cycle->pickedObjectCallback(rid->object_id);
				break ; // only pick the first one
			}
		}
	}
}
*/
