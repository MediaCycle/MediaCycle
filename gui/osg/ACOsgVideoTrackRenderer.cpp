/*
 *  ACOsgVideoTrackRenderer.cpp
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

#include "ACOsgVideoTrackRenderer.h"
#include "ACVideo.h"
#include "ACImage.h"
#include <cmath>
#include <osg/ImageUtils>


#if !defined (APPLE_IOS)


 static double getTime()
 {
 struct timeval tv = {0, 0};
 struct timezone tz = {0, 0};
 
 gettimeofday(&tv, &tz);
 
 return (double)tv.tv_sec + tv.tv_usec / 1000000.0;
 }
 

ACOsgVideoTrackRenderer::ACOsgVideoTrackRenderer() {
	video_stream = 0;
	//summary_stream = 0;
	zoom_x = 1.0; zoom_y = 1.0;
	track_left_x = 0.0;
	summary_center_y = -yspan/2.0f+yspan/8.0f; //[-yspan/2.0f;yspan/2.0f]
	summary_height = yspan/8.0f;//[0;yspan/2.0f]
	playback_center_y = yspan/4.0f; //[-yspan/2.0f;yspan/2.0f]
	playback_height = yspan/2.0f;//[0;yspan]
	playback_scale = 0.5f;
	playback_geode = 0; playback_transform = 0;
	frame_min_width = 32;
	frame_n = 1;
	selection_center_pos = -xspan/2.0f;
	selection_begin_pos = selection_center_pos;
	selection_end_pos = selection_center_pos;
	scrubbing = false;
	
	Vec4 color(1.0f, 1.0f, 1.0f, 0.9f);	
	Vec4 color2(0.2f, 0.8f, 0.2f, 1.0f);	
	Vec4 color3(0.4f, 0.4f, 0.4f, 1.0f);	
	colors = new Vec4Array;
	colors2 = new Vec4Array;
	colors3 = new Vec4Array;
	
	colors->push_back(color);		
	colors2->push_back(color2);	
	colors3->push_back(color3);	
	
	frames_transform = new MatrixTransform();
	frames_group = new Group();
}

ACOsgVideoTrackRenderer::~ACOsgVideoTrackRenderer() {
	if (video_stream) video_stream->quit();
	if (playback_geode) { playback_geode->unref(); playback_geode=0; }
	if (playback_transform) { playback_transform->unref(); playback_transform=0; }
	if (cursor_geode) {	cursor_geode->unref(); cursor_geode=0; }
}

void ACOsgVideoTrackRenderer::playbackGeode() {
	StateSet *state;
	Vec3Array* vertices;
	Vec2Array* texcoord;
	Geometry *playback_geometry;
	Texture2D *playback_texture;
	
	playback_transform = new MatrixTransform();
	playback_geode = new Geode();
	playback_geometry = new Geometry();	
	
	double imagey = yspan/2.0f;
	double imagex = xspan/2.0f;
	
	vertices = new Vec3Array(4);
	(*vertices)[0] = Vec3(-imagex, -imagey, zpos);
	(*vertices)[1] = Vec3(imagex, -imagey, zpos);
	(*vertices)[2] = Vec3(imagex, imagey, zpos);
	(*vertices)[3] = Vec3(-imagex, imagey, zpos);
	playback_geometry->setVertexArray(vertices);
	
	// Primitive Set
	DrawElementsUInt *poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
	poly->push_back(0);
	poly->push_back(1);
	poly->push_back(2);
	poly->push_back(3);
	playback_geometry->addPrimitiveSet(poly);
	
	// State Set
	state = playback_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
	
	// Texture Coordinates
	texcoord = new Vec2Array;
	float a = 0.0;
	float b = 1.0-a;
	bool flip = true;
	texcoord->push_back(osg::Vec2(a, flip ? b : a));
	texcoord->push_back(osg::Vec2(b, flip ? b : a));
	texcoord->push_back(osg::Vec2(b, flip ? a : b));
	texcoord->push_back(osg::Vec2(a, flip ? a : b));
	playback_geometry->setTexCoordArray(0, texcoord);	
	
	if (media_index > -1){
		playback_texture = (osg::Texture2D*)(media_cycle->getLibrary()->getMedia(media_index)->getThumbnailPtr());
		playback_texture->setResizeNonPowerOfTwoHint(false);
		//playback_texture->setUnRefImageDataAfterApply(true);
		state = playback_geometry->getOrCreateStateSet();
		state->setTextureAttribute(0, playback_texture);
		state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
		playback_geometry->setColorArray(colors);
		playback_geometry->setColorBinding(Geometry::BIND_OVERALL);
		playback_geode->addDrawable(playback_geometry);
		playback_transform->addChild(playback_geode);
		playback_transform->ref();
		playback_geode->setUserData(new ACRefId(track_index,"video track"));
	}
}

void ACOsgVideoTrackRenderer::cursorGeode() {
	StateSet *state;	
	Vec3Array* vertices;
	DrawElementsUInt* line_p;
	Geometry *cursor_geometry;
	
	cursor_transform = new MatrixTransform();
	cursor_geode = new Geode();
	cursor_geometry = new Geometry();
	
	vertices = new Vec3Array(2);
	(*vertices)[0] = Vec3(0.0f, -yspan/2.0f, 0.0f);
	(*vertices)[1] = Vec3(0.0f, yspan/2.0f, 0.0f);	
	cursor_geometry->setVertexArray(vertices);
	
	Vec4 cursor_color(0.2f, 0.9f, 0.2f, 0.9f);	
	Vec4Array* cursor_colors = new Vec4Array;
	cursor_colors->push_back(cursor_color);		
	cursor_geometry->setColorArray(cursor_colors);
	cursor_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 2);	
	(*line_p)[0] = 0;
	(*line_p)[1] = 1;	
	cursor_geometry->addPrimitiveSet(line_p);
	
	state = cursor_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(2.0));
	
	state = cursor_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
#if !defined(APPLE_IOS)
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON); //CF not supported by OpenGL ES 2...
#endif		
	state = cursor_transform->getOrCreateStateSet();
	state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	
	cursor_geode->addDrawable(cursor_geometry);
	cursor_transform->addChild(cursor_geode);
	cursor_transform->ref();
	cursor_geode->setUserData(new ACRefId(track_index,"video track cursor"));
	cursor_geode->ref();
}

void ACOsgVideoTrackRenderer::framesGeode() {
	frames_group->removeChildren(0,	frames_group->getNumChildren ());
	frame_n = floor(width/frame_min_width);
	StateSet *state;
	
	//for (int f=frame_n-1;f>=0;f--){ // hysteresis test
	for (int f=0;f<frame_n;f++){
		frame_geode = new Geode;
		TessellationHints *hints = new TessellationHints();
		hints->setDetailRatio(0.0);
		state = frame_geode->getOrCreateStateSet();
		state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);	
		state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
		int total_frames = (int) cvGetCaptureProperty( summary_data, CV_CAP_PROP_FRAME_COUNT);
		
		cvSetCaptureProperty( summary_data, CV_CAP_PROP_POS_FRAMES, (int)((f+0.5f)*total_frames/(float)frame_n) );// +0.5f means we're taking the center frame as a representative
		if(!cvGrabFrame(summary_data)){
			cerr << "<ACVideoTrackRenderer::updateTrack> Could not find frame " << f << endl;
		}
		else {
			IplImage* tmp = cvRetrieveFrame(summary_data);
			
			osg::Image* tmposg = Convert_OpenCV_TO_OSG_IMAGE(tmp);
			osg::Image* thumbnail = new osg::Image;
			thumbnail->allocateImage(tmposg->s(), tmposg->t(), tmposg->r(), GL_RGB, tmposg->getDataType());
			osg::copyImage(tmposg, 0, 0, 0, tmposg->s(), tmposg->t(), tmposg->r(),thumbnail, 0, 0, 0, false);
			
			StateSet *state;
			Vec3Array* vertices;
			Vec2Array* texcoord;
			Geometry *frame_geometry;
			Texture2D *frame_texture = new osg::Texture2D;
			frame_texture->setImage(thumbnail);
			frame_texture->setResizeNonPowerOfTwoHint(false);
			frame_geometry = new Geometry();	
			
			//CF this is a hack, shouldn't be, maybe there's a delay while accessing frames from the video
			//int g = f-1;
			//if (f == 0) g = frame_n-1;
			int g = f;
			
			vertices = new Vec3Array(4);
			(*vertices)[0] = Vec3(-xspan/2.0f+2*g*xspan/frame_n/2.0f, -yspan/2.0f, 0.0);
			(*vertices)[1] = Vec3(-xspan/2.0f+(2*g+2)*xspan/frame_n/2.0f, -yspan/2.0f, 0.0);
			(*vertices)[2] = Vec3(-xspan/2.0f+(2*g+2)*xspan/frame_n/2.0f, yspan/2.0f, 0.0);
			(*vertices)[3] = Vec3(-xspan/2.0f+2*g*xspan/frame_n/2.0f, yspan/2.0f, 0.0);
			frame_geometry->setVertexArray(vertices);
			
			// Primitive Set
			DrawElementsUInt *poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
			poly->push_back(0);
			poly->push_back(1);
			poly->push_back(2);
			poly->push_back(3);
			frame_geometry->addPrimitiveSet(poly);
			
			// State Set
			state = frame_geode->getOrCreateStateSet();
			state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
			state->setMode(GL_BLEND, StateAttribute::ON);
			state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
			
			// Texture Coordinates
			texcoord = new Vec2Array;
			float a = 0.0;
			float b = 1.0-a;
			bool flip = true;
			texcoord->push_back(osg::Vec2(a, flip ? b : a));
			texcoord->push_back(osg::Vec2(b, flip ? b : a));
			texcoord->push_back(osg::Vec2(b, flip ? a : b));
			texcoord->push_back(osg::Vec2(a, flip ? a : b));
			frame_geometry->setTexCoordArray(0, texcoord);	
			
			state = frame_geometry->getOrCreateStateSet();
			state->setTextureAttribute(0, frame_texture);
			state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
			//summary_geometry->setColorArray(colors[0]);
			frame_geometry->setColorBinding(Geometry::BIND_OVERALL);
			frame_geode->addDrawable(frame_geometry);
			frame_geode->setUserData(new ACRefId(track_index,"track summary frames"));
		}
		frame_geode->ref();
		frames_group->addChild(frame_geode);
	}
	frames_group->ref();
	frames_transform->addChild(frames_group);
	frames_transform->ref();
}

void ACOsgVideoTrackRenderer::prepareTracks() {
	playback_geode = 0;
	cursor_transform = 0;
	cursor_geode = 0;
}

void ACOsgVideoTrackRenderer::updateTracks(double ratio) {
	if (media_changed)
	{
		track_node->removeChild(playback_geode);
		if (media){
		
			playbackGeode();
			track_node->addChild(playback_transform);
			
			//if (video_stream) delete video_stream;
			std::cout << "Getting video stream... ";
			double video_stream_in = getTime();
			video_stream = (ImageStream*)(((ACVideo*)(media_cycle->getLibrary()->getMedia(media_index)))->getStream());
			std::cout << getTime()-video_stream_in << " sec." << std::endl;
			
			//if (summary_data) delete summary_data;
			std::cout << "Getting summary data... ";
			double summary_data_in = getTime();
			summary_data = ((ACVideo*)(media_cycle->getLibrary()->getMedia(media_index)))->getData();
			std::cout << getTime()-summary_data_in << " sec." << std::endl;
		}	
		//track_node->addChild(playback_geode);
	}
	
	static Vec4 colors[2];
	static bool colors_ready = false;
	
	if(!colors_ready)
	{
		colors[0] = Vec4(1,0,0,1);
		colors[1] = Vec4(0,1,0,1);
		colors_ready = true;
	}

	if (height != 0.0f && width != 0.0f){
		float w = (float)(media_cycle->getWidth(media_index));
		float h = (float)(media_cycle->getHeight(media_index));
		
		if (frame_n != floor(width/frame_min_width)){
			double summary_start = getTime();
			std::cout << "Generating frames... ";
			track_node->removeChild(frames_transform);
			frames_transform->removeChild(frames_group);
			framesGeode();
			track_node->addChild(frames_transform);
			std::cout << getTime()-summary_start << " sec." << std::endl;
		}
				
		Matrix T;
		Matrix G;

		summary_height = yspan*(h/height * width/w/frame_n);
		playback_scale = (yspan-summary_height)/yspan;
		playback_height = height * playback_scale;
		playback_center_y = yspan/2.0f-(yspan-summary_height)/2.0f;
				
		G.makeTranslate(0.0f,-yspan/2.0f+summary_height/2.0f,0.0f);
		G = Matrix::scale(1.0f,summary_height/yspan,1.0f)*G;
		
		T.makeTranslate(0.0f,playback_center_y,0.0f);
		if (w/h*playback_height/width<1.0f){ // video fits view height
			T = Matrix::scale(w/h*playback_height/width,playback_scale,1.0f)*T;
			//std::cout << "video fits view height" << std::endl;
		}
		else{ //if (h/w*width/playback_height<1.0f) // video fits view width 
			T = Matrix::scale(1.0f,h/w*width/height,1.0f)*T;
			//std::cout << "video fits view width" << std::endl;
		}
		playback_transform->setMatrix(T);
		frames_transform->setMatrix(G);
		//cursor_transform->setMatrix(G);
	}
	
	track_node->removeChild(cursor_transform.get());
	if (media_changed)
	{
		selection_center_pos = -xspan/2.0f;
		cursorGeode();
		media_changed = false;
	}
	track_node->addChild(cursor_transform.get());
	
	if (media){
		if (video_stream){
			if (height != 0.0f && width != 0.0f){
				Matrix cursorT;
				osg::ImageStream::StreamStatus streamStatus = video_stream->getStatus();
				if (manual_selection){
					video_stream->seek((selection_center_pos/(xspan/2.0f)+1)/2.0f*video_stream->getLength());
					if (streamStatus == osg::ImageStream::PAUSED){
						scrubbing = true;
						video_stream->play();
					}
				}	
				else {
					if (scrubbing){
						scrubbing = false;
						video_stream->pause();
					}	
					if (streamStatus == osg::ImageStream::PLAYING){
						selection_center_pos = -xspan/2.0f+video_stream->getReferenceTime()/video_stream->getLength()*xspan;
						selection_begin_pos = selection_center_pos;
						selection_end_pos = selection_center_pos;
					}
				}
				cursorT.makeTranslate(selection_center_pos,-yspan/2.0f+summary_height/2.0f,0.0f);	
				cursorT = Matrix::scale(1.0f,summary_height/yspan,1.0f)*cursorT;
				cursor_transform->setMatrix(cursorT);
			}	
		}
			
	}		
}
#endif//CF APPLE_IOS