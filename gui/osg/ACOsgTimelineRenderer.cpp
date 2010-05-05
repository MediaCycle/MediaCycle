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
#include "ACOsgAudioTrackRenderer.h"

ACOsgTimelineRenderer::ACOsgTimelineRenderer() {
	track_renderer.resize(0);
	group = new Group();
	track_group = new Group();
	group->addChild(track_group.get());
}

void ACOsgTimelineRenderer::prepareTracks(int start) {

	int media_type;
	
	int n = 1; //CF dummy
	
	// XS are these tests necessary ?
	if (track_renderer.size()>n) {
		for (int i=n;i<track_renderer.size();i++) {
			track_group->removeChild(track_renderer[i]->getTrack());
			delete track_renderer[i];
		}
	}

	track_renderer.resize(n);
	distance_mouse.resize(n);
	
	for (int i=start;i<n;i++) {
		media_type = MEDIA_TYPE_AUDIO;//media_cycle->getMediaType(i);
		switch (media_type) {
			case MEDIA_TYPE_AUDIO:
				track_renderer[i] = new ACOsgAudioTrackRenderer();
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
				track_renderer[i] = NULL;
				break;
		}
		if (track_renderer[i] != NULL) {
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

int ACOsgTimelineRenderer::computeScreenCoordinates(osgViewer::View* view, double ratio) //CF: use osgViewer::Viewer* for the simple Viewer
{		
	int closest_track;
	float closest_distance;
	closest_distance = 1000000;
	closest_track = -1;
	
	float x, y, z;
	float mx, my;
	
	int n = 1;// = media_cycle->getLibrarySize(); 	
	n = track_renderer.size();
	
	//osg::Matrix modelModel = view->getModelMatrix();
	osg::Matrix viewMatrix = view->getCamera()->getViewMatrix();
	osg::Matrix projectionMatrix = view->getCamera()->getProjectionMatrix();
	//osg::Matrix window = view->getWindowMatrix();
	osg::Matrix VPM = viewMatrix * projectionMatrix;
	
	// convertpoints in model coordinates to view coordinates
	// Not necessary to go to screen coordinated because pick function can get normalized mouse coordinates
	osg::Vec3 modelPoint;
	osg::Vec3 screenPoint;
	
	for(int i=0; i<n; i++) {
		
		//const ACMediaNode &attribute = media_cycle->getMediaNode(i);
		//const ACPoint &p = attribute.getCurrentPosition(), &p2 = attribute.getNextPosition();
		ACPoint p,p2;
		p.x = p.y = p.z = p2.x = p2.y = p2.z = 0.0f; // CF dummy
		//std::cout << "Timeline p " << p.x << p.y << p.z << " p2 " << p2.x << p2.y << p2.z << std::endl;
		double omr = 1.0-ratio;
		
		x = omr*p.x + ratio*p2.x;
		y = omr*p.y + ratio*p2.y;
		z = 0;
		
		modelPoint = Vec3(x,y,z);
		screenPoint = modelPoint * VPM;
				
		media_cycle->getMouse(&mx, &my);
		
		// compute distance between mouse and media element in view
		distance_mouse[i] = sqrt((screenPoint[0]-mx)*(screenPoint[0]-mx)+(screenPoint[1]-my)*(screenPoint[1]-my));
		track_renderer[i]->setDistanceMouse(distance_mouse[i]);
	
		if (distance_mouse[i]<closest_distance) {
			closest_distance = distance_mouse[i];
			closest_track = i;
		}
	}	
	
	return closest_track;
}
