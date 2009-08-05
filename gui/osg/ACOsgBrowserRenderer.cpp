/*
 *  ACOsgBrowserRenderer.cpp
 *  MediaCycle
 *
 *  @author Stéphane Dupont
 *  @date 24/08/09
 *
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

#include "ACOsgBrowserRenderer.h"
#include "ACOsgAudioRenderer.h"
#include "ACOsgImageRenderer.h"
#include "ACOsgVideoRenderer.h"

void ACOsgBrowserRenderer::prepareNodes(int start) {
	
	int media_type;
	
	n = media_cycle->getLibrarySize(); 	
		
	if (media_renderer.size()>n) {
		
		for (i=n;i<media_renderer.size();i++) {
			group->removeChild(i, 1);
			delete media_renderer[i];
		}
		group = 0;
	}
	
	if (!group) {
		group = new Group();
	}
	
	int nn = min(n, 1);
	nn = n;
	
	media_renderer.resize(nn);
	distance_mouse.resize(n);
	
	for (i=start;i<nn;i++) {
		media_type = media_cycle->getMediaType(i);
		switch (media_type) {
			case MEDIA_TYPE_AUDIO:
				media_renderer[i] = new ACOsgAudioRenderer();
				break;
			case MEDIA_TYPE_IMAGE:
				media_renderer[i] = new ACOsgImageRenderer();
				break;
			case MEDIA_TYPE_VIDEO:
				media_renderer[i] = new ACOsgVideoRenderer();
				break;
			default:
				media_renderer[i] = 0;
				break;
		}
		if (media_renderer[i]) {
			media_renderer[i]->setMediaCycle(media_cycle);
			media_renderer[i]->setLoopIndex(i);
			// media_renderer[i]->setActivity(0);
			media_renderer[i]->prepareNodes();
			group->addChild(media_renderer[i]->getNode());
		}
	}
	
}

void ACOsgBrowserRenderer::updateNodes(double ratio) {
	
	if (media_cycle->getNeedsDisplay()) {
		for (i=0;i<media_renderer.size();i++) {
			media_renderer[i]->updateNodes(ratio);
		}
		// SD - the scene graph is changing every time we move something (inclusing the mouse)....
		// media_cycle->setNeedsDisplay(false);
	}
}

int ACOsgBrowserRenderer::computeScreenCoordinates(osgViewer::Viewer* view, double ratio)// XS TODO c : should be image

{		
	int closest_loop;
	float closest_distance;
	closest_distance = 1000000;
	closest_loop = -1;
	
	int i;
	float x, y, z;
	float mx, my;
	
	n = media_cycle->getLibrarySize(); 	
	n = media_renderer.size();
	
	//osg::Matrix modelModel = view->getModelMatrix();
	osg::Matrix viewMatrix = view->getCamera()->getViewMatrix();
	osg::Matrix projectionMatrix = view->getCamera()->getProjectionMatrix();
	//osg::Matrix window = view->getWindowMatrix();
	osg::Matrix VPM = viewMatrix * projectionMatrix;
	
	// convertpoints in model coordinates to view coordinates
	// Not necessary to go to screen coordinated because pick function can get normalized mouse coordinates
	osg::Vec3 modelPoint;
	osg::Vec3 screenPoint;
	
	for(i=0; i<n; i++) {
		
		const ACLoopAttribute &attribute = media_cycle->getLoopAttributes(i);
		const ACPoint &p = attribute.currentPos, &p2 = attribute.nextPos;
		double omr = 1.0-ratio;
		
		x = omr*p.x + ratio*p2.x;
		y = omr*p.y + ratio*p2.y;
		z = 0;
		
		modelPoint = Vec3(x,y,z);
		screenPoint = modelPoint * VPM;
				
		media_cycle->getMouse(&mx, &my);
		
		// compute distance between mouse and media element in view
		distance_mouse[i] = sqrt((screenPoint[0]-mx)*(screenPoint[0]-mx)+(screenPoint[1]-my)*(screenPoint[1]-my));
		media_renderer[i]->setDistanceMouse(distance_mouse[i]);
	
		if (distance_mouse[i]<closest_distance) {
			closest_distance = distance_mouse[i];
			closest_loop = i;
		}
	}	
	
	return closest_loop;
}
