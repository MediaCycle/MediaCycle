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
#if defined (SUPPORT_AUDIO)
	#include "ACOsgAudioRenderer.h"
#endif //defined (SUPPORT_AUDIO)
#if defined (SUPPORT_IMAGE)
#include "ACOsgImageRenderer.h"
#endif //defined (SUPPORT_IMAGE)
#if defined (SUPPORT_VIDEO)
#include "ACOsgVideoRenderer.h"
#endif //defined (SUPPORT_VIDEO)
#if defined (SUPPORT_3DMODEL)
#include "ACOsg3DModelRenderer.h"
#endif //defined (SUPPORT_3DMODEL)
//#include "ACOsgTextRenderer.h"

#include <osgDB/Registry>
#include <osgDB/ReaderWriter>
#include <osgDB/FileNameUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/PluginQuery>

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

using namespace osg;

ACOsgBrowserRenderer::ACOsgBrowserRenderer()
//: displayed_nodes(0)
{
	group = new Group();
	//ref_ptr//group->ref();
	media_group = new Group();
	//ref_ptr//media_group->ref();
	link_group = new Group();
	//ref_ptr//link_group->ref();
	label_group = new Group();
	//ref_ptr//label_group->ref();
	group->addChild(label_group);		// SD TODO - check this get(), was needed to compile on OSG v2.4 (used by AM)
	group->addChild(media_group);
	group->addChild(link_group);
	this->clean();

	
	/*
	std::string ffmpegLib = osgDB::Registry::instance()->createLibraryNameForExtension("ffmpeg");
	
	osgDB::Registry::LoadStatus ffmpegStatus = osgDB::Registry::instance()->loadLibrary(ffmpegLib);
	
	std::string qtLib = osgDB::Registry::instance()->createLibraryNameForExtension("qt");
	
	osgDB::Registry::LoadStatus qtStatus = osgDB::Registry::instance()->loadLibrary(qtLib); 
	
	std::cout << "OSG supports the following extensions:" << std::endl;
	osgDB::Registry::ReaderWriterList readerWriterList = osgDB::Registry::instance()->getReaderWriterList();
	for (int r=0; r<readerWriterList.size();r++)
	{
		osgDB::ReaderWriter::FormatDescriptionMap fDM = readerWriterList[r]->supportedExtensions();
		osgDB::ReaderWriter::FormatDescriptionMap::iterator iter = fDM.begin();
		for(;iter!=fDM.end();++iter)
			std::cout << "-- (*." << iter->first << ") "<< iter->second << std::endl;
	}
	 */
}

ACOsgBrowserRenderer::~ACOsgBrowserRenderer(){
	this->removeNodes();
	this->removeLinks();
	this->removeLabels();
	media_cycle = 0;
}	

void ACOsgBrowserRenderer::clean(){
	// SD - Results from centralized request to MediaCycle - GLOBAL
	media_cycle_time = 0.0;
	media_cycle_prevtime = 0.0;
	media_cycle_deltatime = 0.0;
	media_cycle_zoom = 0.0f;
	media_cycle_angle = 0.0f;
	media_cycle_mode = 0;
	media_cycle_global_navigation_level = 0;
	
	// SD - Results from centralized request to MediaCycle - NODE SPECIFIC
	media_cycle_node = ACMediaNode();
	media_cycle_isdisplayed = false;
	media_cycle_current_pos.x = 0;
	media_cycle_current_pos.y = 0;
	media_cycle_current_pos.z = 0;
	media_cycle_view_pos.x = 0;
	media_cycle_view_pos.y = 0;
	media_cycle_view_pos.z = 0;
	media_cycle_next_pos.x = 0;
	media_cycle_next_pos.y = 0;
	media_cycle_next_pos.z = 0;
	media_cycle_navigation_level = 0;
	media_cycle_activity = 0;
	node_index = -1;
	media_index = -1;
	prev_media_index = -1;	
	media_cycle_filename = "";
	distance_mouse.clear();
	nodes_prepared = 0;
	this->removeNodes();
	this->removeLinks();
	this->removeLabels();
}

double ACOsgBrowserRenderer::getTime() {
	struct timeval  tv = {0, 0};
	struct timezone tz = {0, 0};
	gettimeofday(&tv, &tz);
	return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

// adds/removes nodes to the node_renderer
void ACOsgBrowserRenderer::prepareNodes(int _start) {
	
	int start;
	
	int n = media_cycle->getNumberOfMediaNodes(); //XS was: getLibrarySize(); 
	
	if (_start) {
		start = _start;
	}
	else {
		start = node_renderer.size();
	}
	
	if (node_renderer.size()>n) {
		this->removeNodes(n, node_renderer.size());
	}
	else if (node_renderer.size()<n) {
		this->addNodes(start,n);
		
	}
	
	// XS  TODO why this ?
	if ((n-start)>0)
		nodes_prepared = 1;
}

void ACOsgBrowserRenderer::updateNodes(double ratio) {
	
	if (!nodes_prepared) {
		return;
	}
	//CF prepareNodes
	/*
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
	
	for (int i=0;i<n;i++) {
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
				node_renderer[i] = 0;
				break;
		}
		if (node_renderer[i] != 0) {
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
	*/
	
	// SD 2010 OCT - This animation has moved from Browser to Renderer
	/*
	#define CUB_FRAC(x) (x*x*(-2.0*x + 3.0))
	#define TI_CLAMP(x,a,b) ((x)<(a)?(a):(x)>(b)?(b):(x))
	double t = getTime();
	double frac;
	double andur = 2.0;	
	*/
	
	media_cycle_time = getTime();
	media_cycle_deltatime = media_cycle_time - media_cycle_prevtime;
	media_cycle_prevtime = media_cycle_time;
	media_cycle_zoom = media_cycle->getCameraZoom();		// SD TODO - why still part of mediacycle? 
	media_cycle_angle = media_cycle->getCameraRotation();
	media_cycle_mode = media_cycle->getBrowser()->getMode();
	media_cycle_global_navigation_level = media_cycle->getNavigationLevel();
	
	for (unsigned int i=0;i<node_renderer.size();i++) {
		
		media_cycle_node = media_cycle->getMediaNode(i);
		media_cycle_isdisplayed = media_cycle_node.isDisplayed();
		media_cycle_current_pos = media_cycle_node.getCurrentPosition();
		media_cycle_next_pos = media_cycle_node.getNextPosition();
		media_cycle_navigation_level = media_cycle_node.getNavigationLevel();
		media_cycle_activity = media_cycle_node.getActivity();		
		node_index = node_renderer[i]->getNodeIndex();
		media_index = node_index; 
		if (media_cycle_mode == AC_MODE_NEIGHBORS)
			media_index = media_cycle->getBrowser()->getUserLog()->getMediaIdFromNodeId(node_index);	
		if (media_index<0)
			media_index = 0;
		media_cycle_filename = media_cycle->getMediaFileName(media_index);	
		
		if (media_cycle_isdisplayed) {
			
			// GLOBAL
			node_renderer[i]->setDeltaTime(media_cycle_deltatime);
			node_renderer[i]->setZoomAngle(media_cycle_zoom, media_cycle_angle);
			node_renderer[i]->setMode(media_cycle_mode);
			node_renderer[i]->setGlobalNavigation(media_cycle_global_navigation_level);
			
			// NODE SPECIFIC
			node_renderer[i]->setIsDisplayed(media_cycle_isdisplayed);
			// SD 2010 OCT
			//node_renderer[i]->setPos(media_cycle_current_pos, media_cycle_next_pos);
			node_renderer[i]->setNavigation(media_cycle_navigation_level);
			node_renderer[i]->setActivity(media_cycle_activity);
			node_renderer[i]->setMediaIndex(media_index);
			node_renderer[i]->setFilename(media_cycle_filename);
			
			// UPDATE
			node_renderer[i]->updateNodes(ratio);
		}
	}
		
	/*	
	//if (media_cycle && media_cycle->hasBrowser() && media_cycle->getBrowser()->getNumberOfLoopsToDisplay()>0)
		layout_renderer->updateLayout(ratio);
	*/
	//CF or visible and updated only if AC_LAYOUT_TYPE_NODELINK
	// SD
	/*
	if (media_cycle->getBrowser()->getLayout() == AC_LAYOUT_TYPE_NODELINK) {
		for (unsigned int i=0;i<link_renderer.size();i++) {
			link_renderer[i]->updateLinks(ratio);
		}
	}
	 */
}

void ACOsgBrowserRenderer::prepareLabels(int start) {

	int n = media_cycle->getLabelSize(); 	
	// int n = 1;
	
	if (label_renderer.size()>n) {
		this->removeLabels(n, label_renderer.size());
	}
	else {
		this->addLabels(start,n);
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
	
	// SD 2010 OCT - This animation has moved from Browser to Renderer
	#define CUB_FRAC(x) (x*x*(-2.0*x + 3.0))
	#define TI_CLAMP(x,a,b) ((x)<(a)?(a):(x)>(b)?(b):(x))
	double t = getTime();
	double frac;
	double andur = 1.0;	
//	double alpha = 0.99;
	double omr;
	
	media_cycle->getMouse(&mx, &my);
	//printf ("MOUSE: %f %f\n", mx, my);

	for(int i=0; i<n; i++) {
		
		ACMediaNode &attribute = media_cycle->getMediaNode(i);
		
		/*
		const ACPoint &p = attribute.getCurrentPosition();
		const ACPoint &p2 = attribute.getNextPosition();
		double refTime = attribute.getNextTime();
		
		frac = (t-refTime)/andur;
		if (frac<1) {
			frac = CUB_FRAC(frac);
		}
		frac = TI_CLAMP(frac, 0, 1);
		
		omr = 1.0-frac;
		x = omr*p.x + frac*p2.x;
		y = omr*p.y + frac*p2.y;
		z = 0;		
		*/
		
		/*
		 if (i==1) {
		 printf ("POS: %f, %f, %f\n",p.x,p2.x,frac);
		 }
		 */
		
		if (attribute.getChanged()) {
			if (node_renderer[i]->getInitialized()) {
				node_renderer[i]->setCurrentPos(node_renderer[i]->getViewPos());
			}
			else {
				node_renderer[i]->setCurrentPos(attribute.getCurrentPosition());
				node_renderer[i]->setViewPos(attribute.getCurrentPosition());
			}
			node_renderer[i]->setNextPos(attribute.getNextPosition());
			attribute.setChanged(0);
		}
		
		const ACPoint &p = node_renderer[i]->getCurrentPos();
		const ACPoint &p2 = node_renderer[i]->getNextPos();
		double refTime = attribute.getNextTime();
		
		frac = (t-refTime)/andur;
		if (frac<1) {
			//frac = CUB_FRAC(frac);
		}
		frac = TI_CLAMP(frac, 0, 1);
		
		omr = 1.0-frac;
		x = omr*p.x + frac*p2.x;
		y = omr*p.y + frac*p2.y;
		z = 0;		
		
		media_cycle_view_pos.x = x;
		media_cycle_view_pos.y = y;
		
		node_renderer[i]->setFrac(frac);
		node_renderer[i]->setViewPos(media_cycle_view_pos);
		//attribute.setViewPosition(media_cycle_view_pos);
		
		modelPoint = Vec3(x,y,z);
		screenPoint = modelPoint * VPM;
						
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

// private methods

// Clean up properly by calling destructor of each *
bool ACOsgBrowserRenderer::removeNodes(int _first, int _last){
	bool ok = false;
	if (_first < 0 || _last > node_renderer.size() || _last < _first){
		cerr << "<ACOsgBrowserRenderer::removeNodes> : wrong index / out of bounds : " << _first << " - " << _last  << endl;
		ok = false;
	}	
	// (default) remove ALL nodes
	else if (_first == 0 && _last==0) {
		std::vector<ACOsgMediaRenderer*>::iterator iterm; 
		for (iterm = node_renderer.begin(); iterm != node_renderer.end(); iterm++) { 
			media_group->removeChild((*iterm)->getNode());
			delete *iterm; 
		}
		node_renderer.clear();	
		ok = true;
	}
	else {
		for (int i=_first;i<_last;i++) {
			media_group->removeChild(node_renderer[i]->getNode());
			delete node_renderer[i];
		}
		node_renderer.resize(_first);
		ok = true;		
	}
	return ok;
}

bool ACOsgBrowserRenderer::addNodes(int _first, int _last){
	bool ok = false;
	if (_first < 0 || _last < _first){
		cerr << "<ACOsgBrowserRenderer::addNodes> : wrong index / out of bounds : " << _first << " - " << _last  << endl;
		ok = false;
	}	
	else {
		node_renderer.resize(_last);
		//if (media_cycle->getBrowser()->getLayout() == AC_LAYOUT_TYPE_NODELINK)
		//link_renderer.resize(_last);
		distance_mouse.resize(_last);
		
		ACMediaType media_type;
		for (int i=_first;i<_last;i++) {
			media_type = media_cycle->getMediaType(i);
			switch (media_type) {
				case MEDIA_TYPE_AUDIO:
					#if defined (SUPPORT_AUDIO)
					node_renderer[i] = new ACOsgAudioRenderer();
					#endif //defined (SUPPORT_AUDIO)
					break;
				case MEDIA_TYPE_IMAGE:
					#if defined (SUPPORT_IMAGE)
					node_renderer[i] = new ACOsgImageRenderer();
					#endif //defined (SUPPORT_IMAGE)
					break;
				case MEDIA_TYPE_VIDEO:
					#if defined (SUPPORT_VIDEO)
					node_renderer[i] = new ACOsgVideoRenderer();
					#endif //defined (SUPPORT_VIDEO)
					break;
				case MEDIA_TYPE_3DMODEL:
					#if defined (SUPPORT_3DMODEL)
					node_renderer[i] = new ACOsg3DModelRenderer();
					#endif //defined (SUPPORT_3DMODEL)
					break;
				case MEDIA_TYPE_TEXT:
					#if defined (SUPPORT_TEXT)
					node_renderer[i] = new ACOsgTextRenderer();
					#endif //defined (SUPPORT_TEXT)
					break;
				default:
					node_renderer[i] = 0;
					break;
			}
			if (node_renderer[i] != 0) {
				
				node_renderer[i]->setMediaCycle(media_cycle);
				node_renderer[i]->setNodeIndex(i);
				
				media_cycle_node = media_cycle->getMediaNode(i);
				node_index = node_renderer[i]->getNodeIndex();
				//XS TODO chipo !
				media_index = node_index; 
				if (media_cycle_mode == AC_MODE_NEIGHBORS)
					media_index = media_cycle->getBrowser()->getUserLog()->getMediaIdFromNodeId(node_index);	
				if (media_index<0)
					media_index = 0;
				media_cycle_filename = media_cycle->getMediaFileName(media_index);
				node_renderer[i]->setMediaIndex(media_index);
				node_renderer[i]->setFilename(media_cycle_filename);
				
				// node_renderer[i]->setActivity(0);
				node_renderer[i]->prepareNodes();
				media_group->addChild(node_renderer[i]->getNode());
			}
			
			//if (media_cycle->getBrowser()->getLayout() == AC_LAYOUT_TYPE_NODELINK) {
			// SD
			/*
			 link_renderer[i] = new ACOsgNodeLinkRenderer();
			 if (link_renderer[i]) {
			 link_renderer[i]->setMediaCycle(media_cycle);
			 link_renderer[i]->setNodeIndex(i);
			 // node_renderer[i]->setActivity(0);
			 link_renderer[i]->prepareLinks();
			 link_group->addChild(link_renderer[i]->getLink());
			 }
			 */
			//}	
			
			/*
			 if (link_renderer.size()>n){//media_cycle->getBrowser()->getLayout() == AC_LAYOUT_TYPE_NODELINK && ) {
			 for (int i=n;i<link_renderer.size();i++) {
			 link_group->removeChild(link_renderer[i]->getLink());
			 delete link_renderer[i];
			 }
			 link_renderer.resize(n);
			 }
			 */
			
			/*
			 layout_renderer = new ACOsgLayoutRenderer();
			 layout_renderer->setMediaCycle(media_cycle); 
			 group->addChild(layout_renderer->getGroup());
			 layout_renderer->prepareLayout(start);
			 */
			
		}
	}
}

// Clean up properly by calling destructor of each *
bool ACOsgBrowserRenderer::removeLinks(int _first, int _last){
	bool ok = false;
	if (_first < 0 || _last > link_renderer.size() || _last < _first){
		cerr << "<ACOsgBrowserRenderer::removeLinks> : wrong index / out of bounds : " << _first << " - " << _last  << endl;
		ok = false;
	}	
	// (default) remove ALL nodes
	else if (_first == 0 && _last==0) {
		std::vector<ACOsgNodeLinkRenderer*>::iterator itern; 
		for (itern = link_renderer.begin(); itern != link_renderer.end(); itern++) { 
			link_group->removeChild((*itern)->getLink());
			delete *itern; 
		}
		link_renderer.clear();	
		ok = true;
	}
	else {
		for (int i=_first;i<_last;i++) {
			link_group->removeChild(link_renderer[i]->getLink());
			delete link_renderer[i];
		}
		link_renderer.resize(_first);
		ok = true;		
	}
	return ok;
}

// Clean up properly by calling destructor of each *
bool ACOsgBrowserRenderer::removeLabels(int _first, int _last){
	bool ok = false;
	if (_first < 0 || _last > label_renderer.size() || _last < _first){
		cerr << "<ACOsgBrowserRenderer::removeLabels> : wrong index / out of bounds : " << _first << " - " << _last  << endl;
		ok = false;
	}	
	// (default) remove ALL nodes
	else if (_first == 0 && _last==0) {
		std::vector<ACOsgTextRenderer*>::iterator iterm; 
		for (iterm = label_renderer.begin(); iterm != label_renderer.end(); iterm++) { 
			label_group->removeChild((*iterm)->getNode());
			delete *iterm; 
		}
		label_renderer.clear();	
		ok = true;
	}
	else {
		for (int i=_first;i<_last;i++) {
			label_group->removeChild(label_renderer[i]->getNode());
			delete label_renderer[i];
		}
		label_renderer.resize(_first);
		ok = true;		
	}
	return ok;
}

bool ACOsgBrowserRenderer::addLabels(int _first, int _last){
	bool ok = false;
	if (_first < 0 || _last < _first){
		cerr << "<ACOsgBrowserRenderer::addLabels> : wrong index / out of bounds : " << _first << " - " << _last  << endl;
		ok = false;
	}	
	else {
		label_renderer.resize(_last);
		
		for (unsigned int i=_first;i<_last;i++) {
			label_renderer[i] = new ACOsgTextRenderer();
			if (label_renderer[i]) {
				label_renderer[i]->setText(media_cycle->getLabelText(i));
				label_renderer[i]->setPos(media_cycle->getLabelPos(i));
				label_renderer[i]->setMediaCycle(media_cycle);
				label_renderer[i]->setNodeIndex(i);
				label_renderer[i]->prepareNodes();
				label_group->addChild(label_renderer[i]->getNode());
			}
		}
	}
}
		
		