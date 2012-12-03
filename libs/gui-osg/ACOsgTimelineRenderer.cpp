/*
 *  ACOsgTimelineRenderer.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 28/04/10
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

#include "ACOsgTimelineRenderer.h"
#include "ACOsgTrackRenderer.h"
#if defined (SUPPORT_MULTIMEDIA)
#include <ACMediaDocument.h>
#endif //defined (SUPPORT_MULTIMEDIA)

#include "ACOsgRendererFactory.h"

using namespace osg;

ACOsgTimelineRenderer::ACOsgTimelineRenderer()
    : media_cycle(0), screen_width(0), height(0.0f), width(0.0f),font(0){
    track_renderer.resize(0);
    group = new Group();
	track_group = new Group();
	group->addChild(track_group);
}

ACOsgTimelineRenderer::~ACOsgTimelineRenderer(){
	this->removeTracks();
}

void ACOsgTimelineRenderer::clean(){
	this->removeTracks();
}

ACOsgTrackRenderer* ACOsgTimelineRenderer::getTrack(int number){
    if ( (number>=0) && (number<track_renderer.size()) )
        return track_renderer[number];
    else
        return 0;
}

//bool ACOsgTimelineRenderer::addTrack(int media_index){
bool ACOsgTimelineRenderer::addTrack(ACMedia* _media){
    bool ok = true;
    int n = track_renderer.size();
    //ACMediaType media_type = media_cycle->getLibrary()->getMedia(media_index)->getType();

    ACMediaType media_type = _media->getType();

#ifdef SUPPORT_MULTIMEDIA
    if(media_cycle->getMediaType() == MEDIA_TYPE_MIXED && _media->getType() == MEDIA_TYPE_MIXED)
        media_type = ((ACMediaDocument*) _media)->getActiveSubMediaType();
#endif//def SUPPORT_MULTIMEDIA

    ACOsgTrackRenderer* renderer = 0;
    renderer = ACOsgRendererFactory::getInstance().createTrackRenderer(media_type);
    if(renderer){
        track_renderer.push_back(renderer);
        renderer->setScreenWidth(screen_width);
        renderer->setSize(width,height);
        renderer->setMediaCycle(media_cycle);
        renderer->setTrackIndex(n);
        renderer->setFont(font);
        //renderer->updateMedia(media_index);
#ifdef SUPPORT_MULTIMEDIA
        if(media_cycle->getMediaType() == MEDIA_TYPE_MIXED && _media->getType() == MEDIA_TYPE_MIXED)
            renderer->updateMedia( ((ACMediaDocument*) _media)->getActiveMedia() );
        else
#endif//def SUPPORT_MULTIMEDIA
            renderer->updateMedia(_media);
        renderer->prepareTracks();
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
    if (track_renderer.size()>n) {
        for (int i=n;i<track_renderer.size();i++) {
            track_group->removeChild(track_renderer[i]->getTrack());
            delete track_renderer[i];
        }
    }

    track_renderer.resize(n);
}

void ACOsgTimelineRenderer::updateTracks(double ratio) {

    for (unsigned int i=0;i<track_renderer.size();i++) {
        track_renderer[i]->updateTracks(ratio);
	}
}

void ACOsgTimelineRenderer::updateScreenWidth(int _screen_width)
{
	if ( screen_width != _screen_width){
		this->screen_width = _screen_width;
        for (unsigned int i=0;i<track_renderer.size();i++) {
            track_renderer[i]->updateScreenWidth(_screen_width);
		}
	}
}

void ACOsgTimelineRenderer::updateSize(float _width,float _height)
{
	if (( this->height != _height) || ( this->width != _width)){
		this->height = _height;
		this->width = _width;
        for (unsigned int i=0;i<track_renderer.size();i++) {
            track_renderer[i]->updateSize(_width,_height);
		}
	}
}

// private methods

// Clean up properly by calling destructor of each *
bool ACOsgTimelineRenderer::removeTracks(int _first, int _last){
	bool ok = false;
	if (_first < 0 || _last > track_renderer.size() || _last < _first){
		cerr << "<ACOsgTimelineRenderer::removeTracks> : wrong index / out of bounds : " << _first << " - " << _last  << endl;
		ok = false;
	}
	// (default) remove ALL tracks
	else if (_first == 0 && _last==0) {
        std::vector<ACOsgTrackRenderer*>::iterator iterm;
        for (iterm = track_renderer.begin(); iterm != track_renderer.end(); iterm++) {
            track_group->removeChild((*iterm)->getTrack());
			delete *iterm;
		}
		track_renderer.clear();
		ok = true;
	}
	else {
		for (int i=_first;i<_last;i++) {
			track_group->removeChild(track_renderer[i]->getTrack());
			delete track_renderer[i];
		}
		track_renderer.resize(_first);
		ok = true;
	}
	return ok;
}
