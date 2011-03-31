/*
 *  ACOsgTimelineControlsRenderer.h
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

#ifndef __ACOSG_TIMELINE_CONTROLS_RENDERER_H__
#define __ACOSG_TIMELINE_CONTROLS_RENDERER_H__

#include "MediaCycle.h"

#include "ACOsgTrackControlsRenderer.h"

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


class ACOsgTimelineControlsRenderer {
protected:
	MediaCycle				*media_cycle;
	osg::ref_ptr<osg::Group>				 group;
	osg::ref_ptr<osg::Group>				 track_group;
	std::vector<ACOsgTrackControlsRenderer*>  track_renderer;
	std::vector<float>				 distance_mouse;

public:
	ACOsgTimelineControlsRenderer();
	~ACOsgTimelineControlsRenderer();
	void clean();	
		
	void setMediaCycle(MediaCycle *media_cycle) { this->media_cycle = media_cycle; };
	osg::ref_ptr<osg::Group> getShapes() 	{ return group; };
	ACOsgTrackControlsRenderer* getControls(int number){if ( (number>=0) && (number<track_renderer.size()) ) return track_renderer[number];}
	//bool addControl(int position,ACMediaType media_type = -1);
	
	void prepareControls(int start=0);
	void updateControls(double ratio=0.0);

	int computeScreenCoordinates(osgViewer::View* view, double ratio=0.0); //CF: use osgViewer::Viewer* for simple Viewers
	std::vector<float> getDistanceMouse() { return distance_mouse; };

private:
	bool removeControls(int _first=0, int _last=0){};
};

#endif
