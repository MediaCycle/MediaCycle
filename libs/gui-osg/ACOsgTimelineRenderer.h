/*
 *  ACOsgTimelineRenderer.h
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

#ifndef __ACOSG_TIMELINE_RENDERER_H__
#define __ACOSG_TIMELINE_RENDERER_H__

#include "MediaCycle.h"
#if defined (SUPPORT_AUDIO)
#include <ACAudioEngine.h>
#endif //defined (SUPPORT_AUDIO)

#include "ACOsgTrackRenderer.h"

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

class ACOsgTimelineRenderer {
protected:
	MediaCycle				*media_cycle;
	#if defined (SUPPORT_AUDIO)
		ACAudioEngine *audio_engine;
	#endif //defined (SUPPORT_AUDIO)
	osg::ref_ptr<osg::Group>				 group;
	osg::ref_ptr<osg::Group>				 track_group;
	std::vector<ACOsgTrackRenderer*>  track_renderer;
	int screen_width;
	float height,width;

public:
	ACOsgTimelineRenderer();
	~ACOsgTimelineRenderer();
	void clean();

	void setMediaCycle(MediaCycle *media_cycle) { this->media_cycle = media_cycle; };
	#if defined (SUPPORT_AUDIO)
		void setAudioEngine(ACAudioEngine *engine){audio_engine=engine;}
	#endif //defined (SUPPORT_AUDIO)
	osg::ref_ptr<osg::Group> getShapes() 	{ return group; };
	ACOsgTrackRenderer* getTrack(int number){if ( (number>=0) && (number<track_renderer.size()) ) return track_renderer[number];}
	bool addTrack(int media_index);
	int getNumberOfTracks(){return track_renderer.size();}

	void prepareTracks(int start=0);
	void updateTracks(double ratio=0.0);
	void setScreenWidth(int _screen_width){screen_width = _screen_width;}
	//void setHeight(float _height){height = _height;}
	void updateScreenWidth(int _screen_width);
	void updateSize(float _width,float _height);
	void setSize(int _width,float _height){width = _width;height = _height;}

private:
	bool removeTracks(int _first=0, int _last=0);
};

#endif
