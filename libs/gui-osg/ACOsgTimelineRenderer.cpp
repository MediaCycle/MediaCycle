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

#include "ACOsgTimelineRenderer.h"
#include "ACOsgTrackRenderer.h"
#if defined (SUPPORT_MULTIMEDIA)
#include <ACMediaDocument.h>
#endif //defined (SUPPORT_MULTIMEDIA)

#include "ACOsgRendererFactory.h"

using namespace osg;

ACOsgTimelineRenderer::ACOsgTimelineRenderer()
    : ACAbstractTimelineRenderer(), media_cycle(0), screen_width(0), height(0.0f), width(0.0f),font(0)
{
    group = new Group();
	track_group = new Group();
	group->addChild(track_group);
    track_playback_thumbnail = "";
    track_summary_thumbnail = "";
    track_selection_thumbnail = "";
}

ACOsgTimelineRenderer::~ACOsgTimelineRenderer(){
	this->removeTracks();
}

void ACOsgTimelineRenderer::clean(){
	this->removeTracks();
}

ACOsgTrackRenderer* ACOsgTimelineRenderer::getTrack(int number){
    ACOsgTrackRenderers::iterator track = track_renderers.find(number);
    if(track != track_renderers.end())
        return track_renderers[number];
    else
        return 0;
}

//bool ACOsgTimelineRenderer::addTrack(int media_index){
bool ACOsgTimelineRenderer::addTrack(ACMedia* _media){
    bool ok = true;
    //ACMediaType media_type = media_cycle->getLibrary()->getMedia(media_index)->getType();

    if(!_media){
        std::cerr << "ACOsgTimelineRenderer::addTrack: provided media is null" << std::endl;
        return false;
    }

    /*if(this->getTrack(_media->getId())){
        std::cerr << "ACOsgTimelineRenderer::addTrack: track already exists for media id " << _media->getId() << ", aborting" << std::endl;
        return false;
    }*/ // For now we allow only one track

    ACMediaType media_type = _media->getType();

#ifdef SUPPORT_MULTIMEDIA
    if(media_cycle->getMediaType() == MEDIA_TYPE_MIXED && _media->getType() == MEDIA_TYPE_MIXED)
        media_type = ((ACMediaDocument*) _media)->getActiveSubMediaType();
#endif//def SUPPORT_MULTIMEDIA

    // This thumbnail contains an image(stream) or texture to be shared between the browser and the timeline
    // This should be mirrored in the timeline renderer in case the browser is not used
    std::string shared_thumbnail_name("");
    shared_thumbnail_name = ACOsgRendererFactory::getInstance().sharedThumbnailName(_media->getType());
    if(_media->getThumbnail(shared_thumbnail_name)==0){
        ACMediaThumbnail* shared_thumbnail = 0;
        shared_thumbnail = ACOsgRendererFactory::getInstance().createSharedThumbnail(_media);
        if(shared_thumbnail)
            _media->addThumbnail(shared_thumbnail);
    }

    ACOsgTrackRenderer* renderer = 0;
    renderer = ACOsgRendererFactory::getInstance().createTrackRenderer(media_type);
    if(renderer){
        renderer->setSharedThumbnailName(shared_thumbnail_name);
        renderer->setScreenWidth(screen_width);
        renderer->setSize(width,height);
        renderer->setMediaCycle(media_cycle);
        renderer->setTrackIndex(_media->getId());
        renderer->setFont(font);
        //renderer->updateMedia(media_index);

        //track_renderers[_media->getId()] = renderer;
        track_renderers[track_renderers.size()] = renderer; //CF for now, only one track, assigned by index

#ifdef SUPPORT_MULTIMEDIA
        if(media_cycle->getMediaType() == MEDIA_TYPE_MIXED && _media->getType() == MEDIA_TYPE_MIXED)
            renderer->updateMedia( ((ACMediaDocument*) _media)->getActiveMedia() );
        else
#endif//def SUPPORT_MULTIMEDIA
            renderer->updateMedia(_media);
        renderer->prepareTracks();
        if(renderer->getTrack())
            track_group->addChild(renderer->getTrack());
    }
    else
        ok = false;
    return ok;
}

void ACOsgTimelineRenderer::prepareTracks(int start) {

    ACMediaType media_type;
    int n = 0;

    // For now we allow only one track and delete it every time the browser is re-"prepared"
    //this->removeTracks();
}

void ACOsgTimelineRenderer::updateTracks(double ratio) {
    /*for(ACOsgTrackRenderers::iterator track = track_renderers.begin();track!=track_renderers.end();track++){
        track->second->updateTracks(ratio);
    }*/
}

void ACOsgTimelineRenderer::updateScreenWidth(int _screen_width)
{
	if ( screen_width != _screen_width){
		this->screen_width = _screen_width;
        for(ACOsgTrackRenderers::iterator track = track_renderers.begin();track!=track_renderers.end();track++){
            track->second->updateScreenWidth(_screen_width);
		}
	}
}

void ACOsgTimelineRenderer::updateSize(float _width,float _height)
{
	if (( this->height != _height) || ( this->width != _width)){
		this->height = _height;
		this->width = _width;
        for(ACOsgTrackRenderers::iterator track = track_renderers.begin();track!=track_renderers.end();track++){
            track->second->updateSize(_width,_height);
		}
	}
}

// private methods

bool ACOsgTimelineRenderer::removeTracks(){//private method
    ACOsgTrackRenderers::iterator iterm;
    for (iterm = track_renderers.begin(); iterm != track_renderers.end(); iterm++) {
        if(iterm->second){
            long tempId=iterm->first;
            track_group->removeChild(iterm->second->getTrack());
            delete iterm->second;
            iterm->second=0;
        }
    }
    track_renderers.clear();
    return true;
}

void ACOsgTimelineRenderer::changeTrackPlaybackThumbnail(int _track, std::string _thumbnail)
{
    activity_update_mutex.lock();
    if(!this->getTrack(_track))
        return;
    track_renderers[_track]->changePlaybackThumbnail(_thumbnail);
    activity_update_mutex.unlock();
}

void ACOsgTimelineRenderer::changeAllTracksPlaybackThumbnail(std::string _thumbnail)
{
    activity_update_mutex.lock();
    for(ACOsgTrackRenderers::iterator track_renderer = track_renderers.begin();track_renderer!=track_renderers.end();track_renderer++){
        track_renderer->second->changePlaybackThumbnail(_thumbnail);
    }
    track_playback_thumbnail = _thumbnail;
    activity_update_mutex.unlock();
}

void ACOsgTimelineRenderer::changeTrackSummaryThumbnail(int _track, std::string _thumbnail)
{
    activity_update_mutex.lock();
    if(!this->getTrack(_track))
        return;
    track_renderers[_track]->changeSummaryThumbnail(_thumbnail);
    activity_update_mutex.unlock();
}

void ACOsgTimelineRenderer::changeAllTracksSummaryThumbnail(std::string _thumbnail)
{
    activity_update_mutex.lock();
    for(ACOsgTrackRenderers::iterator track_renderer = track_renderers.begin();track_renderer!=track_renderers.end();track_renderer++){
        track_renderer->second->changeSummaryThumbnail(_thumbnail);
    }
    track_summary_thumbnail = _thumbnail;
    activity_update_mutex.unlock();
}

void ACOsgTimelineRenderer::changeTrackSelectionThumbnail(int _track, std::string _thumbnail)
{
    activity_update_mutex.lock();
    if(!this->getTrack(_track))
        return;
    track_renderers[_track]->changeSelectionThumbnail(_thumbnail);
    activity_update_mutex.unlock();
}

void ACOsgTimelineRenderer::changeAllTracksSelectionThumbnail(std::string _thumbnail)
{
    activity_update_mutex.lock();
    for(ACOsgTrackRenderers::iterator track_renderer = track_renderers.begin();track_renderer!=track_renderers.end();track_renderer++){
        track_renderer->second->changeSelectionThumbnail(_thumbnail);
    }
    track_selection_thumbnail = _thumbnail;
    activity_update_mutex.unlock();
}

