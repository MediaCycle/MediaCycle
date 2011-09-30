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

#include "ACOsgHUDRenderer.h"
using namespace osg;

ACOsgHUDRenderer::ACOsgHUDRenderer()
{
	media_cycle_pointer_current_pos.x = 0;
	media_cycle_pointer_current_pos.y = 0;
	media_cycle_pointer_current_pos.z = 0;

	// create a camera to set up the projection and model view matrices, and the subgraph to draw in the HUD
    camera = new osg::Camera;
    // set the projection matrix
    camera->setProjectionMatrix(osg::Matrix::ortho2D(0,1,0,1));
    // set the view matrix
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setViewMatrix(osg::Matrix::identity());
    // only clear the depth buffer
    camera->setClearMask(GL_DEPTH_BUFFER_BIT);
    // draw subgraph after main camera view.
    camera->setRenderOrder(osg::Camera::POST_RENDER);
    // we don't want the camera to grab event focus from the viewers main camera(s).
    camera->setAllowEventFocus(false);

	pointer_renderer.resize(0);
	group = new Group();
	pointer_group = new Group();
	group->addChild(pointer_group);//group->addChild(pointer_group.get());
	camera->addChild(group);
}
//TR NEM2011
void ACOsgHUDRenderer::cleanPointers(){

	for (int i=0;i<pointer_renderer.size();i++){
		pointer_group->removeChild(pointer_renderer[i]->getNode());
		delete pointer_renderer[i];
	}
	pointer_renderer.clear();
	
}

double ACOsgHUDRenderer::getTime() {
	struct timeval  tv = {0, 0};
	struct timezone tz = {0, 0};
	gettimeofday(&tv, &tz);
	return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

void ACOsgHUDRenderer::setMediaCycle(MediaCycle *media_cycle) {

	this->media_cycle = media_cycle;
}

osg::ref_ptr<osg::Camera> ACOsgHUDRenderer::getCamera() {

	return camera;
}

void ACOsgHUDRenderer::preparePointers(osgViewer::View* view) {

	unsigned int n = media_cycle->getNumberOfPointers();

	unsigned int prev_size =  pointer_renderer.size();
	//std::cout << "ACOsgHUDRenderer::preparePointers n " << n << " prev_size " << prev_size << std::endl;

	/*if(n == 1)
	{
		if (media_cycle->getPointerFromIndex(0)->getType() == AC_POINTER_MOUSE)
			std::cout << "ACOsgHUDRenderer::preparePointers mouse yes"<<std::endl;
		else
			std::cout << "ACOsgHUDRenderer::preparePointers mouse no"<<std::endl;
		
	}	*/
//	if(prev_size == 1 || n==1)
//		if (media_cycle->getPointerFromIndex(0)!=0)
//			if (media_cycle->getPointerFromIndex(0)->getType()==AC_POINTER_MOUSE)//TR NEM2011
//				pointer_group->removeChildren(0,pointer_group->getNumChildren());
	
	
	if (pointer_renderer.size()>n) {

		for (unsigned int i=n;i<pointer_renderer.size();i++) {
			//pointer_group->removeChild(i, 1);
			pointer_group->removeChild(pointer_renderer[i]->getNode());
			delete pointer_renderer[i];
		}
		pointer_renderer.resize(n);
	}
	else if (pointer_renderer.size()<n) {

		pointer_renderer.resize(n);

		for (unsigned int i=prev_size;i<n;i++) {
			pointer_renderer[i] = new ACOsgPointerRenderer();
			if (pointer_renderer[i]) {
				pointer_renderer[i]->setMediaCycle(media_cycle);
				pointer_renderer[i]->setNodeIndex(i);
				ACPointer* p = media_cycle->getPointerFromIndex(i);
				if(p){
					std::string txt = p->getText();
					pointer_renderer[i]->setText(media_cycle->getPointerFromIndex(i)->getText());
					//std::cout << "Pointer id " << i << " txt " <<  txt << std::endl;
					pointer_renderer[i]->prepareNodes();
					if( !(/*(prev_size == 1 || n == 1) && */p->getType() == AC_POINTER_MOUSE))
						pointer_group->addChild(pointer_renderer[i]->getNode());
					//if( ((prev_size == 1 || n == 1) && p->getType() == AC_POINTER_MOUSE))
					//	pointer_group->removeChildren(0,pointer_group->getNumChildren());
				}	
				else
					std::cerr << "ACOsgHUDRenderer::preparePointers: couldn't prepare pointer with index" << i << std::endl;

			}
		}
	}
}

//Cocoa - simple OSG viewer
void ACOsgHUDRenderer::updatePointers(osgViewer::Viewer* view) {
	int w, h;
	h = 1; w = 1;
	osgViewer::Viewer::Windows windows;
	if (view->isRealized()) {
		view->getWindows(windows);
		w = windows[0]->getTraits()->width;
		h = windows[0]->getTraits()->height;
	}
	this->updatePointers(w,h);
}

//Qt - composite OSG viewer
void ACOsgHUDRenderer::updatePointers(osgViewer::View* view) {
	//this->preparePointers(view);

	int w, h;
	h = 1; w = 1;
	 if (view->getViewerBase()->isRealized()) {
		 w = view->getCamera()->getViewport()->width();
		 h = view->getCamera()->getViewport()->height();
	 }
	this->updatePointers(w,h);
}

//Common
void ACOsgHUDRenderer::updatePointers(int w, int h) {
	
	unsigned int n = media_cycle->getNumberOfPointers();
	
	unsigned int prev_size =  pointer_renderer.size();
	
	//std::cout << "ACOsgHUDRenderer::updatePointers n " << n << " prev_size " << prev_size << std::endl;
	/*if(n == 1)
	{
		if (media_cycle->getPointerFromIndex(0)->getType() == AC_POINTER_MOUSE)
			std::cout << "ACOsgHUDRenderer::updatePointers mouse yes"<<std::endl;
		else
			std::cout << "ACOsgHUDRenderer::updatePointers mouse no"<<std::endl;
		
	}	*/
	
	if(n!=prev_size)
		this->preparePointers();

	for (unsigned int i=0;i<pointer_renderer.size();i++) {
		ACPointer* p =0;
		p = media_cycle->getPointerFromIndex(i);
		if (p){
			media_cycle_pointer_current_pos = p->getCurrentPosition();
			//printf ("POINTER: %f %f\n", media_cycle_pointer_current_pos.x, media_cycle_pointer_current_pos.y);
			media_cycle_pointer_current_pos.x = (media_cycle_pointer_current_pos.x+1)/2*w;
			media_cycle_pointer_current_pos.y = (media_cycle_pointer_current_pos.y+1)/2*h;
			//printf ("POINTER: %f %f\n", media_cycle_pointer_current_pos.x, media_cycle_pointer_current_pos.y);
			pointer_renderer[i]->setPos(media_cycle_pointer_current_pos);
			pointer_renderer[i]->updateNodes();	
		}
		else
			std::cerr << "ACOsgHUDRenderer::updatePointers pointer at index " << i << " not available" << std::endl;
	}	
}
