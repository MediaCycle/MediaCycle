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

#ifndef __ACOSG_BROWSER_RENDERER_H__
#define __ACOSG_BROWSER_RENDERER_H__

#include "MediaCycle.h"

#include "ACOsgMediaRenderer.h"
//#include "ACOsgLayoutRenderer.h"
#include "ACOsgNodeLinkRenderer.h"

#include <osgDB/ReadFile>
//#include <osgDB/WriteFile>

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
//#include <osg/Material>
#include <osg/Texture2D>
//#include <osg/TextureRectangle>
//#include <osg/TextureCubeMap>
//#include <osg/TexMat>
//#include <osg/CullFace>
#include <osg/Image>
#include <osg/ImageStream>

#include <osg/io_utils>
#include <osg/LineWidth>
#include <osg/ShapeDrawable>
//#include <osg/BlendFunc>
//#include <osg/BlendColor>

#include <osgUtil/SceneView>
#include <osgViewer/Viewer>

#include "ACRefId.h"

//#include "ACPlugin.h"

using namespace std;
using namespace osg;

class ACOsgBrowserRenderer {
protected:
	int i;
	int n;
	MediaCycle				*media_cycle;
	ref_ptr<Group>				 group;
	ref_ptr<Group>				 media_group;
	ref_ptr<Group>				 label_group;
	ref_ptr<Group>				 link_group;
	vector<ACOsgMediaRenderer*>  media_renderer;
	vector<ACOsgMediaRenderer*>  label_renderer;
	vector<ACOsgNodeLinkRenderer*>  link_renderer;
	//ACOsgLayoutRenderer*		layout_renderer;
	//vector<bool>				 media_selected;
	vector<float>				 distance_mouse;
	//ACPlugin* mLayoutPlugin;
	//ACOsgLayoutType layout_type;

public:
	ACOsgBrowserRenderer();
	~ACOsgBrowserRenderer() {};
		
	void setMediaCycle(MediaCycle *media_cycle) { this->media_cycle = media_cycle; };
	//void setLayoutPlugin(ACPlugin* acpl){mLayoutPlugin=acpl;};
	//void setLayout(ACOsgBrowserLayoutType _type){layout_type = _type;}
	Group *getShapes() 	{ return group.get(); };
	
	void prepareNodes(int start=0);
	void updateNodes(double ratio=0.0);

	void prepareLabels(int start=0);
	void updateLabels(double ratio=0.0);
		
	int computeScreenCoordinates(osgViewer::Viewer* view, double ratio=0.0);
	vector<float> getDistanceMouse() { return distance_mouse; };
};

#endif
