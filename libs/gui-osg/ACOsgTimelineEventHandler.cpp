/**
 * @brief The timeline event handler filters user interaction for all media tracks, implemented with OSG
 * @author Christian Frisson
 * @date 28/04/2010
 * @copyright (c) 2010 – UMONS - Numediart
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

#include "ACOsgTimelineEventHandler.h"
#include <iostream>
#include <sstream>
#include <osg/Version>

#ifdef USE_DEBUG
// ----------- uncomment this to display event messages 
//#define DEBUG_HANDLE

// ----------- uncomment this to display picked objects messages 
//#define DEBUG_PICKED
#endif //USE_DEBUG

ACOsgTimelineEventHandler::ACOsgTimelineEventHandler()
    : renderer(0), media_cycle(0)
{
    this->clean();
}

void ACOsgTimelineEventHandler::clean(){
    //media_cycle = 0;
    selecting_zone = false;
    selecting_zone_begin = false;
    selecting_zone_end = false;
    selecting_summary_waveform = false;
    selecting_summary_frames = false;
    selecting_summary_slider = false;
    selecting_segments = false;
    selection = 0;
    selection_begin = 0.0f;
    selection_end = 0.0f;
    pushed_x = 0.0f;
    moved_x = 0.0f;
    pushed_y = 0.0f;
    moved_y = 0.0f;
}

bool ACOsgTimelineEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
    if (media_cycle == 0) return false;
    float pos_x = ea.getX()/ea.getXmax();//[0;1]
    float pos_y = ea.getY()/ea.getYmax();//[0;1]
    if (pos_x>1.0f) pos_x=1.0f;
    if (pos_x<0.0f) pos_x=0.0f;
    if (pos_y>1.0f) pos_y=1.0f;
    if (pos_y<0.0f) pos_y=0.0f;

#if OSG_MIN_VERSION_REQUIRED(3,0,0)
    if (ea.isMultiTouchEvent())
    {
#ifdef DEBUG_HANDLE
        std::cout << "Received multitouch event" << std::endl;
#endif
    }
#endif

    switch(ea.getEventType())
    {
    case(osgGA::GUIEventAdapter::PUSH):
    {
#ifdef DEBUG_HANDLE
        std::cout << "(PUSH) pos x " << pos_x << " y " << pos_y << std::endl;
#endif
        osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
        if (view){
            pick(view,ea,false);

            if(selection){

                // Skipping/Moving the selection zone to the mouse position
                if(selecting_summary_waveform){
                    // Audio feedback
                    int mediaID = renderer->getTrack(selection->getRefId())->getMedia()->getId();
                    if (mediaID > -1){
                        if (media_cycle->getLibrary()->getMedia(mediaID)->getType() == MEDIA_TYPE_AUDIO){
                            std::cout << "Skipping zone to frame: " << (int)(pos_x) << std::endl; //*((ACAudio*) media_cycle->getLibrary()->getMedia(mediaID))->getNFrames())
                            media_cycle->performActionOnMedia("synchro mode",mediaID,"Manual");
                            media_cycle->performActionOnMedia("scale mode",mediaID,"Resample");
                            stringstream s;
                            s << (float) 100.0f*pos_x;
                            media_cycle->performActionOnMedia("scrub",mediaID,s.str());
                        }
                    }
                    // Visual feedback
                    renderer->getTrack(selection->getRefId())->moveSelection(pos_x,pos_y);
#ifdef DEBUG_HANDLE
                    std::cout << "(PUSH) Jumping the selection zone to the mouse position (track " << selection->getRefId() <<")" << std::endl;
#endif
                }

                else if(selecting_summary_slider){
                    // Visual feedback
                    renderer->getTrack(selection->getRefId())->moveSelection(pos_x,pos_y);
#ifdef DEBUG_HANDLE
                    std::cout << "(PUSH) Jumping the selection zone to the mouse position (track " << selection->getRefId() <<")" << std::endl;
#endif
                }

                // Skipping/Moving the current frame to the mouse position
                else if (selecting_summary_frames && !selecting_zone){
                    // Audio feedback
                    int mediaID = renderer->getTrack(selection->getRefId())->getMedia()->getId();
                    if (mediaID > -1){
                        if (media_cycle->getLibrary()->getMedia(mediaID)->getType() == MEDIA_TYPE_AUDIO){
                            std::cout << "Skipping current frame to frame: " << (int)(pos_x) << std::endl; //*((ACAudio*) media_cycle->getLibrary()->getMedia(mediaID))->getNFrames())
                            media_cycle->performActionOnMedia("synchro mode",mediaID,"Manual");
                            media_cycle->performActionOnMedia("scale mode",mediaID,"Resample");
                            stringstream s;
                            s << (float) 100.0f*pos_x;
                            media_cycle->performActionOnMedia("scrub",mediaID,s.str());
                        }
                    }
                    renderer->getTrack(selection->getRefId())->moveSelection(pos_x,pos_y);
#ifdef DEBUG_HANDLE
                    std::cout << "(PUSH) Skipping/Moving the current frame to the mouse position (track " << selection->getRefId() <<")" << std::endl;
#endif
                }

                // Skipping/Moving to the segment under the mouse
                else if (selecting_segments){
                    //                    //Audio feedback
                    //                    int mediaID = renderer->getTrack(selection->getRefId())->getMedia()->getId();
                    //                    if (mediaID > -1){
                    //                        if (media_cycle->getLibrary()->getMedia(mediaID)->getType() == MEDIA_TYPE_AUDIO){
                    //                            //std::cout << "Skipping to frame: " << (int)((pos_x)*((ACAudio*) media_cycle->getLibrary()->getMedia(mediaID))->getNFrames()) << std::endl;
                    //                                media_cycle->performActionOnMedia("synchro mode",mediaID,"Manual");
                    //                                media_cycle->performActionOnMedia("scale mode",mediaID,"Resample");
                    //                                stringstream s;
                    //                                s << (float) 100.0f*(pos_x);
                    //                                media_cycle->performActionOnMedia("scrub",mediaID,s.str());
                    //                        }
                    //                    }
                    renderer->getTrack(selection->getRefId())->moveSelection(pos_x,pos_y); //should be adapted to the segment begin time once segmentation works that way
#ifdef DEBUG_HANDLE
                    std::cout << "(PUSH) Skipping/Moving to segment " << selection->getElementId() <<" (track " << selection->getRefId() <<")" << std::endl;
#endif
                }
            }
        }
        return false;
    }
    case(osgGA::GUIEventAdapter::DRAG):
    {
#ifdef DEBUG_HANDLE
        std::cout << "(DRAG)" << std::endl;
#endif
        //        osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
        //        if (view) pick(view,ea,false);

        if (selection){

            //Resizing the selection zone by moving its left boundary
            if (selecting_zone_begin){
                renderer->getTrack(selection->getRefId())->resizeSelectionFromBegin(pos_x,pos_y);
#ifdef DEBUG_HANDLE
                std::cout << "(DRAG) Resizing the selection zone by moving its left boundary (track " << selection->getRefId() <<")" << std::endl;
#endif
            }

            //Resizing the selection zone by moving its right boundary
            else if(selecting_zone_end){
                renderer->getTrack(selection->getRefId())->resizeSelectionFromEnd(pos_x,pos_y);
#ifdef DEBUG_HANDLE
                std::cout << "(DRAG) Resizing the selection zone by moving its right boundary (track " << selection->getRefId() <<")" << std::endl;
#endif
            }

            //Moving the selection zone (audio tracks)
            else if(selecting_zone){
                int mediaID = renderer->getTrack(selection->getRefId())->getMedia()->getId();
                if (mediaID > -1){
                    if (media_cycle->getLibrary()->getMedia(mediaID)->getType() == MEDIA_TYPE_AUDIO){
                        //std::cout << "Skipping to frame: " << (int)((pos_x)*((ACAudio*) media_cycle->getLibrary()->getMedia(mediaID))->getNFrames()) << std::endl;
#ifdef DEBUG_HANDLE
                        std::cout << "(DRAG) Moving the selection zone (track " << selection->getRefId() <<") to audio pos " << 100.0f*pos_x << std::endl;
#endif
                        media_cycle->performActionOnMedia("synchro mode",mediaID,"Manual");
                        media_cycle->performActionOnMedia("scale mode",mediaID,"Vocode");
                        stringstream s;
                        s << (float) 100.0f*pos_x; // (int)((pos_x)*((ACAudio*) media_cycle->getLibrary()->getMedia(mediaID))->getNFrames())
                        media_cycle->performActionOnMedia("scrub",mediaID,s.str());
                    }
                }
                // Visual feedback
                renderer->getTrack(selection->getRefId())->moveSelection((pos_x+moved_x),(pos_y+moved_y));
            }

            // Moving the current frame to the mouse position
            else if (selecting_summary_frames){
                renderer->getTrack(selection->getRefId())->moveSelection(pos_x,pos_y);
#ifdef DEBUG_HANDLE
                std::cout << "(DRAG) Moving the current frame to the mouse position (track " << selection->getRefId() <<")" << std::endl;
#endif
            }
        }
        return false;
    }
    case(osgGA::GUIEventAdapter::RELEASE):
    {
#ifdef DEBUG_HANDLE
        std::cout << "(RELEASE)" << std::endl;
#endif
        // Skipping/Moving the selection zone to the mouse position
        if(selection && selecting_zone){
            // Audio feedback
            int mediaID = renderer->getTrack(selection->getRefId())->getMedia()->getId();
            if (mediaID > -1){
                if (media_cycle->getLibrary()->getMedia(mediaID)->getType() == MEDIA_TYPE_AUDIO){
                    //std::cout << "Skipping to frame: " << (int)(pos_x*((ACAudio*) media_cycle->getLibrary()->getMedia(mediaID))->getNFrames()) << std::endl;
                    media_cycle->performActionOnMedia("synchro mode",mediaID,"None");
                    media_cycle->performActionOnMedia("scale mode",mediaID,"None");
                }
            }
#ifdef DEBUG_HANDLE
            std::cout << "(RELEASE) Moving the selection zone (track " << selection->getRefId() <<")" << std::endl;
#endif
        }

        osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
        if (view)
            pick(view,ea,false);
        return false;
    }
    case(osgGA::GUIEventAdapter::MOVE):
    {
#ifdef DEBUG_HANDLE
        std::cout << "(MOVE)" << std::endl;
#endif
        //        osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
        //        if (view) pick(view, ea, true);
        return false;
    }
        //    case(osgGA::GUIEventAdapter::KEYDOWN):
        //    {
        //        //std::cout << "Key (OSG) '" << (char)ea.getKey() << "'" << std::endl;
        //        switch( ea.getKey() )
        //        {
        //        case osgGA::GUIEventAdapter::KEY_Space:
        //            break;
        //        default:
        //            break;
        //        }
        //        return false;
        //    }
        //    case(osgGA::GUIEventAdapter::KEYUP):
        //    {
        //        return false;
        //    }
    case(osgGA::GUIEventAdapter::FRAME):
    {
        return false;
    }
    default:
    {
        return false;
    }
    }
}

void ACOsgTimelineEventHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, bool hover)
{
    if (media_cycle == 0) return;
    osgUtil::LineSegmentIntersector::Intersections intersections;

    float x = ea.getX();
    float y = ea.getY();

    //    if (view->computeIntersections(x,y,intersections))
    //    {

    osg::ref_ptr< osgUtil::LineSegmentIntersector > picker = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, x, y);
    osgUtil::IntersectionVisitor iv(picker);
    view->getCamera()->accept(iv);
    if (picker->containsIntersections())
    {
        intersections = picker->getIntersections();

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
                    ACRefId *rid = 0;
                    rid = (ACRefId*)hitr->nodePath.back()->getUserData();
#ifdef DEBUG_PICKED
                    std::cout << "  Widget " << rid->getRefName() << " with object id " << rid->object_id << std::endl;
#endif
                    if(!rid)
                        break;
                    if(rid->getRefId()==-1)
                        break;
                    //if ( (rid->getRefId() >= 0) && (rid->getRefId() < renderer->getNumberOfTracks()) ){
                    //if ((rid->getRefName()=="track selection begin")||(rid->getRefName()=="track selection end")||(rid->getRefName()=="track selection zone")||(rid->getRefName()=="audio track summary waveform")||(rid->getRefName()=="video track summary frames")||(rid->getRefName()=="video track summary slit-scan")||(rid->getRefName()=="video track segments"))
                    //{
                    switch(ea.getEventType())
                    {
                    case(osgGA::GUIEventAdapter::PUSH):
                    {
                        //std::cout << "    Push on '" << rid->getRefName() << "'" << std::endl;
                        if (rid->getRefName()=="track selection begin") selecting_zone_begin = true;
                        else if (rid->getRefName()=="track selection end") selecting_zone_end = true;
                        else if (rid->getRefName()=="track selection zone") selecting_zone = true;
                        else if (rid->getRefName()=="audio track summary waveform") selecting_summary_waveform = true;
                        else if (rid->getRefName()=="video track summary frames") selecting_summary_frames = true;
                        else if (rid->getRefName()=="video track summary slit-scan") selecting_summary_frames = true;
                        else if (rid->getRefName()=="track summary slider") selecting_summary_slider = true;
                        //else if (rid->getRefName()=="video track segments") selecting_segments = true;
                        else if (rid->getRefName()=="video track segment") selecting_segments = true;
                        else if (rid->getRefName()=="audio track segment") selecting_segments = true;

                        //selection = rid;
                        if (selection) {delete selection; selection = 0;}
                        //selection = new ACRefId(rid->getRefId(),rid->getRefName());
                        selection = new ACRefId(*rid);

                        //selection_begin = renderer->getTrack(selection->getRefId())->getSelectionBegin();
                        //selection_end = renderer->getTrack(selection->getRefId())->getSelectionEnd();

                        pushed_x = ea.getX()/ea.getXmax();//ea.getXnormalized();
                        pushed_y = ea.getY()/ea.getYmax(); // CF ea.getYnormalized(); seems to be buggy...

                        //CF temporarily locked to track 0
                        selection->object_id = 0;
                        rid->object_id = 0; //until here

                        moved_x = renderer->getTrack(selection->getRefId())->getSelectionPosX() - ea.getX()/ea.getXmax();
                        moved_y = renderer->getTrack(selection->getRefId())->getSelectionPosY() - ea.getY()/ea.getYmax();
                        if(renderer->getTrack(rid->getRefId()) != 0)
                            renderer->getTrack(rid->getRefId())->setManualSelection(true);
                        //if ((rid->getRefName()!="track selection begin")&&(rid->getRefName()!="track selection end"))

                        break;
                    }
                    case(osgGA::GUIEventAdapter::RELEASE):
                    {
                        selecting_zone = false;
                        selecting_zone_begin = false;
                        selecting_zone_end = false;
                        selecting_summary_waveform = false;
                        selecting_summary_frames = false;
                        selecting_summary_slider = false;
                        selecting_segments = false;
                        if (selection) {delete selection; selection = 0;}
                        selection_begin = 0.0f;
                        selection_end = 0.0f;
                        moved_x = 0.0f;
                        pushed_x = 0.0f;
                        moved_y = 0.0f;
                        pushed_y = 0.0f;
                        if(renderer->getTrack(rid->getRefId()) != 0)
                            renderer->getTrack(rid->getRefId())->setManualSelection(false);
                        break;
                    }

                        //                    case(osgGA::GUIEventAdapter::MOVE):
                        //                    {
                        //                        std::cout << "Moving in" << std::endl;
                        //                        break;
                        //                    }

                    default:
                    {
                        break;
                    }
                    }
                    //}
                    //}
                    //break ; // only pick the first one
                }
            }
        }
        if(selecting_zone_end || selecting_zone_begin){
            selecting_zone = false;
            //selecting_zone_begin = false;
            //selecting_zone_end = false;
            selecting_summary_waveform = false;
            selecting_summary_frames = false;
            selecting_segments = false;
        }
        if(selecting_zone){
            selecting_summary_waveform = false;
            selecting_summary_frames = false;
            selecting_summary_slider = false;
            selecting_segments = false;
        }

        //        if (ea.getEventType()==osgGA::GUIEventAdapter::PUSH){
        //            //std::cout << "  Selecting: zone("<<selecting_zone<<") begin("<<selecting_zone_begin<<") end("<<selecting_zone_end<<") summary("<<selecting_summary_waveform<<")"<<std::endl;
        //            if (selecting_zone && selecting_summary_waveform)
        //                selecting_summary_waveform = false;
        //            //std::cout << "  Selecting: zone("<<selecting_zone<<") begin("<<selecting_zone_begin<<") end("<<selecting_zone_end<<") summary("<<selecting_summary_waveform<<")"<<std::endl;
        //            if ((selecting_zone_begin || selecting_zone_end) && (selecting_zone || selecting_summary_waveform)){
        //                selecting_zone = false;
        //                selecting_summary_waveform = false;
        //            }
        //            //std::cout << "  Selecting: zone("<<selecting_zone<<") begin("<<selecting_zone_begin<<") end("<<selecting_zone_end<<") summary("<<selecting_summary_waveform<<")"<<std::endl;
        //        }
    }
}
