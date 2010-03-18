/*
 *  ACOsgNodeLinkRenderer.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 16/03/2010
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

#ifndef __ACOSG_NODELINK_RENDERER_H__
#define __ACOSG_NODELINK_RENDERER_H__

#include "MediaCycle.h"

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

class ACOsgNodeLinkRenderer {
public:	
	void setMediaCycle(MediaCycle *_media_cycle) { this->media_cycle = _media_cycle; };
	void setNodeIndex(int _node_index) { this->node_index = _node_index; };
	void setDistanceMouse(float _distance_mouse) { this->distance_mouse = _distance_mouse; };
	MatrixTransform* getLink() { return link_node; };
protected:
	MediaCycle* media_cycle;
	MatrixTransform* link_node;
	int node_index;
	float distance_mouse;	
protected:
	Geode* link_geode;	
	void linkGeode(double to_x, double to_y);
public:
	ACOsgNodeLinkRenderer();
	~ACOsgNodeLinkRenderer();
	void prepareLinks();
	void updateLinks(double ratio=0.0);
};

#endif