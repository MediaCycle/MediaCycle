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
#include "ACOsgTextRenderer.h"

ACOsgBrowserRenderer::ACOsgBrowserRenderer() {
	media_renderer.resize(0);
	label_renderer.resize(0);
	group = new Group();
	media_group = new Group();
	label_group = new Group();
	group->addChild(media_group);
	group->addChild(label_group);
}

void ACOsgBrowserRenderer::prepareNodes(int start) {
	
	int media_type;
	
	n = media_cycle->getLibrarySize(); 	
		
	if (media_renderer.size()>n) {
		
		for (i=n;i<media_renderer.size();i++) {
			media_group->removeChild(i, 1);
			delete media_renderer[i];
		}
	}
	
	/*if (!media_group) {
		media_group = new Group();
	}*/
	
	media_renderer.resize(n);
	distance_mouse.resize(n);
	
	for (i=start;i<n;i++) {
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
			case MEDIA_TYPE_TEXT:
				media_renderer[i] = new ACOsgTextRenderer();
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
			media_group->addChild(media_renderer[i]->getNode());
		}
	}
	
}

void ACOsgBrowserRenderer::updateNodes(double ratio) {
	
	for (i=0;i<media_renderer.size();i++) {
		media_renderer[i]->updateNodes(ratio);
	}
}

void ACOsgBrowserRenderer::prepareLabels(int start) {

	n = media_cycle->getLabelSize(); 	
	// int n = 1;
	
	if (label_renderer.size()>n) {
		
		for (i=n;i<label_renderer.size();i++) {
			label_group->removeChild(i, 1);
			delete label_renderer[i];
		}
	}
	
	/*if (!label_group) {
		label_group = new Group();
	}*/
	
	label_renderer.resize(n);
	
	for (i=start;i<n;i++) {
		label_renderer[i] = new ACOsgTextRenderer();
		if (label_renderer[i]) {
			((ACOsgTextRenderer*)label_renderer[i])->setText(media_cycle->getLabelText(i));
			((ACOsgTextRenderer*)label_renderer[i])->setPos(media_cycle->getLabelPos(i));
			label_renderer[i]->setMediaCycle(media_cycle);
			label_renderer[i]->setLoopIndex(i);
			label_renderer[i]->prepareNodes();
			label_group->addChild(label_renderer[i]->getNode());
		}
	}
}

void ACOsgBrowserRenderer::updateLabels(double ratio) {

	for (i=0;i<label_renderer.size();i++) {
		label_renderer[i]->updateNodes(ratio);
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
