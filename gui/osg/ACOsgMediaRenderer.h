/*
 *  ACOsgBrowserRenderer.h
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

#ifndef __ACOSG_MEDIA_RENDERER_H__
#define __ACOSG_MEDIA_RENDERER_H__

#include "MediaCycleLight.h"

#include <osg/ref_ptr>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Geode>
#include <osg/Group>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/BlendFunc>
#include <osg/BlendColor>
#include <osg/Texture2D>
#include <osg/Image>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgUtil/SceneView>
#include <osgViewer/Viewer>

#include "ACRefId.h"

using namespace std;
using namespace osg;

class ACOsgMediaRenderer {
protected:
	MediaCycle	*media_cycle;
	MatrixTransform		*media_node;
	int loop_index;
	float distance_mouse;
	// int	media_activity;
	
public:
	ACOsgMediaRenderer();
	virtual ~ACOsgMediaRenderer() {};

	void setMediaCycle(MediaCycle *media_cycle) { this->media_cycle = media_cycle; };
	void setLoopIndex(int loop_index) { this->loop_index = loop_index; };
	void setDistanceMouse(float distance_mouse) { this->distance_mouse = distance_mouse; };
	//void setActivity(int media_activity) { this->media_activity = media_activity; }
	MatrixTransform* getNode() { return media_node; };
	
	virtual void prepareNodes()=0;
	virtual void updateNodes(double ratio=0.0)=0;
	
};

#endif