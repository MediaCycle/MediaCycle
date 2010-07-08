/*
 *  ACOsgTrackRenderer.h
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

#ifndef __ACOSG_TRACK_RENDERER_H__
#define __ACOSG_TRACK_RENDERER_H__

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

class ACOsgTrackRenderer {
protected:
	MediaCycle* media_cycle;
	MatrixTransform* track_node;
	int track_index, media_index;
	float distance_mouse;
	// int	media_activity;
	ACMedia* media;
	bool media_from_lib;
	bool media_changed;
	
public:
	ACOsgTrackRenderer();
	virtual ~ACOsgTrackRenderer() {};

	void setMediaCycle(MediaCycle *_media_cycle) { this->media_cycle = _media_cycle; };
	void setTrackIndex(int _track_index) { this->track_index = _track_index; };
	void setMediaIndex(int _media_index) { this->media_index = _media_index; };
	int getMediaIndex() { return media_index; }
	void updateMedia(ACMedia* _media);
	void updateMedia(int _media_index);
	void clearMedia();
	ACMedia* getMedia(){return media;}
	void setDistanceMouse(float _distance_mouse) { this->distance_mouse = _distance_mouse; };
	//void setActivity(int _media_activity) { this->media_activity = _media_activity; }
	MatrixTransform* getTrack() { return track_node; };
	
	virtual void prepareTracks()=0;
	virtual void updateTracks(double ratio=0.0)=0;
	
};

#endif