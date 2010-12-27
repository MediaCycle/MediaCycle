/*
 *  ACOsgVideoTrackRenderer.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 13/12/10
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

#ifndef __ACOSG_VIDEO_TRACK_RENDERER_H__
#define __ACOSG_VIDEO_TRACK_RENDERER_H__

#if !defined (APPLE_IOS)

#include "ACOsgBrowserRenderer.h"
//#include "ACOsgImageRenderer.h"
#include "ACOsgTrackRenderer.h"

using namespace std;
using namespace osg;

class ACOsgVideoTrackRenderer : public ACOsgTrackRenderer {
protected:
	ImageStream* video_stream;
	CvCapture* summary_data;

	MatrixTransform* playback_transform;
	MatrixTransform* frames_transform;
	osg::ref_ptr<osg::MatrixTransform> cursor_transform;
	
	Geode* playback_geode;
	ref_ptr<Group> frames_group;
	Geode* frame_geode;
	Geode* cursor_geode;

	void playbackGeode();
	void framesGeode();
	void cursorGeode();
	
	float zoom_x, zoom_y, track_left_x;
	float summary_center_y,summary_height;
	float playback_center_y,playback_height,playback_scale;
	float frame_min_width, frame_n;
	bool scrubbing;

	static const int NCOLORS ;
	Vec4Array* colors;
	Vec4Array* colors2;
	Vec4Array* colors3;
public:
	ACOsgVideoTrackRenderer();
	~ACOsgVideoTrackRenderer();
	void prepareTracks();
	void updateTracks(double ratio=0.0);
};

#endif//CF APPLE_IOS
#endif
