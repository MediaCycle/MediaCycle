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
	node_renderer.resize(0);
	link_renderer.resize(0);
	label_renderer.resize(0);
	group = new Group();
	media_group = new Group();
	label_group = new Group();
	link_group = new Group();
	group->addChild(label_group.get());		// SD TODO - check this get(), was needed to compile on OSG v2.4 (used by AM)
	group->addChild(media_group.get());
	group->addChild(link_group.get());
}

void ACOsgBrowserRenderer::prepareNodes(int start) {
	// XS 180310: nodes are added here in the node_renderer
	// previously media were added into media_renderer
	// TODO: check it is incremental
	int media_type;
	
	int n = media_cycle->getNumberOfMediaNodes(); //XS was: getLibrarySize(); 
	
	// XS are these tests necessary ?
	if (node_renderer.size()>n) {
		for (int i=n;i<node_renderer.size();i++) {
			media_group->removeChild(node_renderer[i]->getNode());
			delete node_renderer[i];
		}
	}

	if (link_renderer.size()>n){//media_cycle->getBrowser()->getLayout() == AC_LAYOUT_TYPE_NODELINK && ) {
		for (int i=n;i<link_renderer.size();i++) {
			link_group->removeChild(link_renderer[i]->getLink());
			delete link_renderer[i];
		}
	}
	
	
	node_renderer.resize(n);
	//if (media_cycle->getBrowser()->getLayout() == AC_LAYOUT_TYPE_NODELINK)
		link_renderer.resize(n);
	distance_mouse.resize(n);
	
	for (int i=start;i<n;i++) {
		media_type = media_cycle->getMediaType(i);
		switch (media_type) {
			case MEDIA_TYPE_AUDIO:
				node_renderer[i] = new ACOsgAudioRenderer();
				break;
			case MEDIA_TYPE_IMAGE:
				node_renderer[i] = new ACOsgImageRenderer();
				break;
			case MEDIA_TYPE_VIDEO:
				node_renderer[i] = new ACOsgVideoRenderer();
				break;
			case MEDIA_TYPE_TEXT:
				node_renderer[i] = new ACOsgTextRenderer();
				break;
			default:
				node_renderer[i] = NULL;
				break;
		}
		if (node_renderer[i] != NULL) {
			node_renderer[i]->setMediaCycle(media_cycle);
			node_renderer[i]->setNodeIndex(i);
			// node_renderer[i]->setActivity(0);
			node_renderer[i]->prepareNodes();
			media_group->addChild(node_renderer[i]->getNode());
		}
		
		//if (media_cycle->getBrowser()->getLayout() == AC_LAYOUT_TYPE_NODELINK) {
			link_renderer[i] = new ACOsgNodeLinkRenderer();
			if (link_renderer[i]) {
				link_renderer[i]->setMediaCycle(media_cycle);
				link_renderer[i]->setNodeIndex(i);
				// node_renderer[i]->setActivity(0);
				link_renderer[i]->prepareLinks();
				link_group->addChild(link_renderer[i]->getLink());
			}
		//}	
	}
	/*
	layout_renderer = new ACOsgLayoutRenderer();
	layout_renderer->setMediaCycle(media_cycle); 
	group->addChild(layout_renderer->getGroup());
	layout_renderer->prepareLayout(start);
	 */
}

void ACOsgBrowserRenderer::updateNodes(double ratio) {
	
	for (unsigned int i=0;i<node_renderer.size();i++) {
		node_renderer[i]->updateNodes(ratio);
	}
	/*	
	//if (media_cycle && media_cycle->hasBrowser() && media_cycle->getBrowser()->getNumberOfLoopsToDisplay()>0)
		layout_renderer->updateLayout(ratio);
	*/
	//CF or visible and updated only if AC_LAYOUT_TYPE_NODELINK
	if (media_cycle->getBrowser()->getLayout() == AC_LAYOUT_TYPE_NODELINK) {
		for (unsigned int i=0;i<link_renderer.size();i++) {
			link_renderer[i]->updateLinks(ratio);
		}
	}
}

void ACOsgBrowserRenderer::prepareLabels(int start) {

	int n = media_cycle->getLabelSize(); 	
	// int n = 1;
	
	if (label_renderer.size()>n) {
		
		for (unsigned int i=n;i<label_renderer.size();i++) {
			label_group->removeChild(i, 1);
			delete label_renderer[i];
		}
	}
	
	/*if (!label_group) {
		label_group = new Group();
	}*/
	
	label_renderer.resize(n);
	
	for (unsigned int i=start;i<n;i++) {
		label_renderer[i] = new ACOsgTextRenderer();
		if (label_renderer[i]) {
			((ACOsgTextRenderer*)label_renderer[i])->setText(media_cycle->getLabelText(i));
			((ACOsgTextRenderer*)label_renderer[i])->setPos(media_cycle->getLabelPos(i));
			label_renderer[i]->setMediaCycle(media_cycle);
			label_renderer[i]->setNodeIndex(i);
			label_renderer[i]->prepareNodes();
			label_group->addChild(label_renderer[i]->getNode());
		}
	}
}

void ACOsgBrowserRenderer::updateLabels(double ratio) {

	for (unsigned int i=0;i<label_renderer.size();i++) {
		label_renderer[i]->updateNodes(ratio);
	}
}

int ACOsgBrowserRenderer::computeScreenCoordinates(osgViewer::View* view, double ratio) //CF: use osgViewer::Viewer* for the simple Viewer
{		
	int closest_node;
	float closest_distance;
	closest_distance = 1000000;
	closest_node = -1;
	
	float x, y, z;
	float mx, my;
	
	int n = media_cycle->getLibrarySize(); 	
	n = node_renderer.size();
	
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
		
		const ACMediaNode &attribute = media_cycle->getMediaNode(i);
		const ACPoint &p = attribute.getCurrentPosition(), &p2 = attribute.getNextPosition();
		double omr = 1.0-ratio;
		
		x = omr*p.x + ratio*p2.x;
		y = omr*p.y + ratio*p2.y;
		z = 0;
		
		modelPoint = Vec3(x,y,z);
		screenPoint = modelPoint * VPM;
				
		media_cycle->getMouse(&mx, &my);
		
		// compute distance between mouse and media element in view
		distance_mouse[i] = sqrt((screenPoint[0]-mx)*(screenPoint[0]-mx)+(screenPoint[1]-my)*(screenPoint[1]-my));
		node_renderer[i]->setDistanceMouse(distance_mouse[i]);
		if (media_cycle->getBrowser()->getLayout() == AC_LAYOUT_TYPE_NODELINK)
			link_renderer[i]->setDistanceMouse(distance_mouse[i]);
	
		if (distance_mouse[i]<closest_distance) {
			closest_distance = distance_mouse[i];
			closest_node = i;
		}
	}	
	
	return closest_node;
}
