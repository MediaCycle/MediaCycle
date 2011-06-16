/*
 *  ACOsgTimelineControlsRenderer.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 17/11/10
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

#include "ACOsgTimelineControlsRenderer.h"
#if defined (SUPPORT_AUDIO)
	#include "ACOsgAudioTrackControlsRenderer.h"
#endif //defined (SUPPORT_AUDIO)
using namespace osg;

ACOsgTimelineControlsRenderer::ACOsgTimelineControlsRenderer() {
	track_renderer.resize(0);
	group = new Group();
	track_group = new Group();
	group->addChild(track_group);//group->addChild(track_group.get());
}

ACOsgTimelineControlsRenderer::~ACOsgTimelineControlsRenderer(){
	this->removeControls();
}

void ACOsgTimelineControlsRenderer::clean(){
	this->removeControls();
}

void ACOsgTimelineControlsRenderer::prepareControls(int start) {

	int media_type;

	int n = 1; //CF dummy

	// XS are these tests necessary ?
	if (track_renderer.size()>n) {
		for (int i=n;i<track_renderer.size();i++) {
			track_group->removeChild(track_renderer[i]->getControls());
			delete track_renderer[i];
		}
	}

	track_renderer.resize(n);

	for (int i=start;i<n;i++) {
		media_type = MEDIA_TYPE_AUDIO;//media_cycle->getMediaType(i);
		switch (media_type) {
			case MEDIA_TYPE_AUDIO:
				#if defined (SUPPORT_AUDIO)
				track_renderer[i] = new ACOsgAudioTrackControlsRenderer();
				#endif //defined (SUPPORT_AUDIO)
				break;
		/*
			case MEDIA_TYPE_VIDEO:
				track_renderer[i] = new ACOsgVideoTrackRenderer();
				break;
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
			track_renderer[i]->prepareControls();
			track_group->addChild(track_renderer[i]->getControls());
		}

	}
}

void ACOsgTimelineControlsRenderer::updateControls(double ratio) {

	for (unsigned int i=0;i<track_renderer.size();i++) {
		track_renderer[i]->updateControls(ratio);
	}
}
