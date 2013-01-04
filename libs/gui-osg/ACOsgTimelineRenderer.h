/**
 * @brief The timeline renderer that handles all media tracks, implemented with OSG
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

#ifndef __ACOSG_TIMELINE_RENDERER_H__
#define __ACOSG_TIMELINE_RENDERER_H__

#include <MediaCycle.h>

#include "ACOsgTrackRenderer.h"

#include <osg/ref_ptr>
#include <osg/Group>

#include <pthread.h>

#include "ACRefId.h"

typedef std::map<long int,ACOsgTrackRenderer*> ACOsgTrackRenderers;

class ACOsgTimelineRenderer {
protected:
    MediaCycle *media_cycle;
    osg::ref_ptr<osg::Group> group;
    osg::ref_ptr<osg::Group> track_group;
    ACOsgTrackRenderers track_renderers;
    osg::ref_ptr<osgText::Font> font;
    int screen_width;
    float height,width;

public:
    ACOsgTimelineRenderer();
    ~ACOsgTimelineRenderer();
    void clean();

    void setMediaCycle(MediaCycle *media_cycle) { this->media_cycle = media_cycle; }
    osg::ref_ptr<osg::Group> getShapes() 	{ return group; }
    ACOsgTrackRenderer* getTrack(int number);
    ACOsgTrackRenderers getTracks(){return track_renderers;}
    //bool addTrack(int media_index);
    void setFont(osg::ref_ptr<osgText::Font> _font){this->font = _font;}
    bool addTrack(ACMedia* _media);
    int getNumberOfTracks(){return track_renderers.size();}

    void prepareTracks(int start=0);
    void updateTracks(double ratio=0.0);
    void setScreenWidth(int _screen_width){screen_width = _screen_width;}
    //void setHeight(float _height){height = _height;}
    void updateScreenWidth(int _screen_width);
    void updateSize(float _width,float _height);
    void setSize(int _width,float _height){width = _width;height = _height;}

    void changeTrackPlaybackThumbnail(int _track, std::string _thumbnail);
    void changeAllTracksPlaybackThumbnail(std::string _thumbnail);
    void changeTrackSummaryThumbnail(int _track, std::string _thumbnail);
    void changeAllTracksSummaryThumbnail(std::string _thumbnail);
    void changeTrackSelectionThumbnail(int _track, std::string _thumbnail);
    void changeAllTracksSelectionThumbnail(std::string _thumbnail);
protected:
    std::string track_playback_thumbnail;
    std::string track_summary_thumbnail;
    std::string track_selection_thumbnail;

private:
    bool removeTracks();

protected:
    /*pthread_mutex_t activity_update_mutex;
    pthread_mutexattr_t activity_update_mutex_attr;*/
    OpenThreads::Mutex activity_update_mutex;

public:
    void mutexLock(){activity_update_mutex.lock();}
    void mutexUnlock(){activity_update_mutex.unlock();}
};

#endif
