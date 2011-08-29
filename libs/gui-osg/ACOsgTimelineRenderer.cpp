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

using namespace osg;

ACOsgTimelineRenderer::ACOsgTimelineRenderer(): media_cycle(0), screen_width(0), height(0.0f) {
	#if defined (SUPPORT_AUDIO)
		audio_engine = 0;
	#endif//defined (SUPPORT_AUDIO)
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
	
	switch (media_type) {
		case MEDIA_TYPE_AUDIO:
			#if defined (SUPPORT_AUDIO)
			track_renderer.resize(n+1);
			track_renderer[n] = new ACOsgAudioTrackRenderer();
			track_renderer[n]->setScreenWidth(screen_width);
			track_renderer[n]->setSize(width,height);
			if (track_renderer[n] != 0) {
				track_renderer[n]->setMediaCycle(media_cycle);
				track_renderer[n]->setAudioEngine(audio_engine);
				track_renderer[n]->setTrackIndex(n);
				//track_renderer[n]->updateMedia(media_index);
				track_renderer[n]->updateMedia(_media);
				track_renderer[n]->prepareTracks();
				track_group->addChild(track_renderer[n]->getTrack());
			}
			#endif //defined (SUPPORT_AUDIO)
			break;
		case MEDIA_TYPE_VIDEO:
			#if defined (SUPPORT_VIDEO)
			track_renderer.resize(n+1);
			track_renderer[n] = new ACOsgVideoTrackRenderer();
			track_renderer[n]->setScreenWidth(screen_width);
			track_renderer[n]->setSize(width,height);
			if (track_renderer[n] != 0) {
				track_renderer[n]->setMediaCycle(media_cycle);
				track_renderer[n]->setTrackIndex(n);
				//track_renderer[n]->updateMedia(media_index);
				track_renderer[n]->updateMedia(_media);
				track_renderer[n]->prepareTracks();
				track_group->addChild(track_renderer[n]->getTrack());
			}
			#endif //defined (SUPPORT_VIDEO)
			break;
			/*
			 case MEDIA_TYPE_SENSORDATA:
			 track_renderer[i] = new ACOsgSensorDataTrackRenderer();
			 break;
			 //... ;-)
			 */
		default:
			track_renderer[n] = 0;
			break;
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
	if (( height != _height) || ( width != _width)){
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
