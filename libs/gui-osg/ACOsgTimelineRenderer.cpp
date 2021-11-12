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
#if defined (SUPPORT_AUDIO)
#include "ACOsgAudioTrackRenderer.h"
#endif //defined (SUPPORT_AUDIO)
#if defined (SUPPORT_VIDEO)
#include "ACOsgVideoTrackRenderer.h"
#endif //defined (SUPPORT_VIDEO)
#if defined (SUPPORT_TEXT)
#include "ACOsgTextTrackRenderer.h"
#endif //defined (SUPPORT_TEXT)
#if defined (SUPPORT_MULTIMEDIA)
#include <ACMediaDocument.h>
#endif //defined (SUPPORT_MULTIMEDIA)

using namespace osg;

ACOsgTimelineRenderer::ACOsgTimelineRenderer()
    : media_cycle(0), screen_width(0), height(0.0f), width(0.0f),font(0){
	#if defined (SUPPORT_AUDIO)
		audio_engine = 0;
	#endif//defined (SUPPORT_AUDIO)
	default_video_summary_type = AC_VIDEO_SUMMARY_KEYFRAMES;
	default_video_selection_type = AC_VIDEO_SELECTION_KEYFRAMES;
	default_video_playback_visibility = true;
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

//bool ACOsgTimelineRenderer::addTrack(int media_index){
bool ACOsgTimelineRenderer::addTrack(ACMedia* _media){
    int n = track_renderer.size();
    //ACMediaType media_type = media_cycle->getLibrary()->getMedia(media_index)->getType();

    ACMediaType media_type = _media->getType();
#ifdef SUPPORT_MULTIMEDIA
    if(media_cycle->getMediaType() == MEDIA_TYPE_MIXED && _media->getType() == MEDIA_TYPE_MIXED)
        media_type = ((ACMediaDocument*) _media)->getActiveSubMediaType();
#endif//def SUPPORT_MULTIMEDIA

    switch (media_type) {
    case MEDIA_TYPE_AUDIO:
    {
#if defined (SUPPORT_AUDIO)
        track_renderer.resize(n+1);
        track_renderer[n] = new ACOsgAudioTrackRenderer();

#endif //defined (SUPPORT_AUDIO)
    }
    break;
    case MEDIA_TYPE_VIDEO:
    {
#if defined (SUPPORT_VIDEO)
        track_renderer.resize(n+1);
        track_renderer[n] = new ACOsgVideoTrackRenderer();
#endif //defined (SUPPORT_VIDEO)
    }
    break;
    case MEDIA_TYPE_TEXT:
    {
#if defined (SUPPORT_TEXT)
        track_renderer.resize(n+1);
        track_renderer[n] = new ACOsgTextTrackRenderer();
#endif //defined (SUPPORT_TEXT)
    }
    break;
    
    case MEDIA_TYPE_SENSOR:
    {
#if defined (SUPPORT_SENSOR)
        track_renderer.resize(n+1);
        track_renderer[n] = new ACOsgTextTrackRenderer();
#endif //defined (SUPPORT_SENSOR)
    }
    default:
        break;
    }

    if (track_renderer.size() == n+1) {
        if(track_renderer[n] != 0){
            track_renderer[n]->setScreenWidth(screen_width);
            track_renderer[n]->setSize(width,height);
            track_renderer[n]->setMediaCycle(media_cycle);
#if defined (SUPPORT_AUDIO)
            track_renderer[n]->setAudioEngine(audio_engine);
#endif
            track_renderer[n]->setTrackIndex(n);
            track_renderer[n]->setFont(font);
            //track_renderer[n]->updateMedia(media_index);
            #ifdef SUPPORT_MULTIMEDIA
            if(media_cycle->getMediaType() == MEDIA_TYPE_MIXED && _media->getType() == MEDIA_TYPE_MIXED)
                track_renderer[n]->updateMedia( ((ACMediaDocument*) _media)->getActiveMedia() );
            else
            #endif//def SUPPORT_MULTIMEDIA
                track_renderer[n]->updateMedia(_media);
            track_renderer[n]->prepareTracks();
            track_group->addChild(track_renderer[n]->getTrack());
        }
        else
            track_renderer.resize(n);
    }
}

void ACOsgTimelineRenderer::prepareTracks(int start) {

	ACMediaType media_type;
	int n = 0;

	// XS are these tests necessary ?
	if (track_renderer.size()>n) {
		for (int i=n;i<track_renderer.size();i++) {
			track_group->removeChild(track_renderer[i]->getTrack());
			delete track_renderer[i];
		}
	}

	track_renderer.resize(n);

	for (int i=start;i<n;i++) {
		media_type = media_cycle->getMediaType(i);
		switch (media_type) {
			case MEDIA_TYPE_AUDIO:
				#if defined (SUPPORT_AUDIO)
				track_renderer[i] = new ACOsgAudioTrackRenderer();
				track_renderer[n]->setAudioEngine(audio_engine);
				track_renderer[i]->setScreenWidth(screen_width);
				track_renderer[i]->setSize(width,height);
				#endif //defined (SUPPORT_AUDIO)
				break;
			case MEDIA_TYPE_VIDEO:
				#if defined (SUPPORT_VIDEO)
				track_renderer[i] = new ACOsgVideoTrackRenderer();
				track_renderer[i]->setScreenWidth(screen_width);
				track_renderer[i]->setSize(width,height);
				#endif //defined (SUPPORT_VIDEO)
				break;
		/*
			case MEDIA_TYPE_SENSORDATA:
				track_renderer[i] = new ACOsgSensorDataTrackRenderer();
				break;
			//... ;-)
		*/
			default:
				track_renderer[i] = 0;
				break;
		}
		if (track_renderer[i] != 0) {
			track_renderer[i]->updateSummaryType(default_video_summary_type);
			track_renderer[i]->updateSelectionType(default_video_selection_type);
			track_renderer[i]->updatePlaybackVisibility(default_video_playback_visibility);
			track_renderer[i]->setMediaCycle(media_cycle);
			track_renderer[i]->setTrackIndex(i);
			//track_renderer[i]->setMediaIndex(0);//CF dumb
			// track_renderer[i]->setActivity(0);
			track_renderer[i]->prepareTracks();
			track_group->addChild(track_renderer[i]->getTrack());
		}

	}
}

void ACOsgTimelineRenderer::updateTracks(double ratio) {

	for (unsigned int i=0;i<track_renderer.size();i++) {
		track_renderer[i]->updateSummaryType(default_video_summary_type);
		track_renderer[i]->updateSelectionType(default_video_selection_type);
		track_renderer[i]->updatePlaybackVisibility(default_video_playback_visibility);
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

void ACOsgTimelineRenderer::updateVideoSummaryType(ACVideoSummaryType _type){
	if (default_video_summary_type != _type){
		this->default_video_summary_type = _type;
		for (unsigned int i=0;i<track_renderer.size();i++) {
			track_renderer[i]->updateSummaryType(_type);
		}
	}
}

void ACOsgTimelineRenderer::updateVideoSelectionType(ACVideoSelectionType _type){
	if (default_video_selection_type != _type){
		this->default_video_selection_type = _type;
		for (unsigned int i=0;i<track_renderer.size();i++) {
			track_renderer[i]->updateSelectionType(_type);
		}
	}
}

void ACOsgTimelineRenderer::updateVideoPlaybackVisibility(bool _visibility){
	if (default_video_playback_visibility != _visibility){
		this->default_video_playback_visibility = _visibility;
		for (unsigned int i=0;i<track_renderer.size();i++) {
			track_renderer[i]->updatePlaybackVisibility(_visibility);
		}
	}
}
