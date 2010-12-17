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

#if !defined (APPLE_IOS)

ACOsgVideoTrackRenderer::ACOsgVideoTrackRenderer() {
	video_stream = 0;
	zoom_x = 1.0; zoom_y = 1.0;
	track_left_x = 0.0;
	summary_center_y = -yspan/2.0f+yspan/8.0f; //[-yspan/2.0f;yspan/2.0f]
	summary_height = yspan/8.0f;//[0;yspan/2.0f]
	playback_center_y = yspan/8.0f; //[-yspan/2.0f;yspan/2.0f]
	playback_height = (yspan-2*yspan/8.0f)/2.0f;//[0;yspan/2.0f]
	
	playback_geode = 0; border_geode = 0; playback_transform = 0;
	
	Vec4 color(1.0f, 1.0f, 1.0f, 0.9f);	
	Vec4 color2(0.2f, 0.8f, 0.2f, 1.0f);	
	Vec4 color3(0.4f, 0.4f, 0.4f, 1.0f);	
	colors = new Vec4Array;
	colors2 = new Vec4Array;
	colors3 = new Vec4Array;
	
	colors->push_back(color);		
	colors2->push_back(color2);	
	colors3->push_back(color3);	
}

ACOsgVideoTrackRenderer::~ACOsgVideoTrackRenderer() {
	/*if (video_stream){
		osg::ImageStream::StreamStatus streamStatus = video_stream->getStatus();
		switch (streamStatus) {
			case osg::ImageStream::INVALID:
				//std::cout << "Image stream invalid status" << std::endl;
				break;
			case osg::ImageStream::PLAYING:
				video_stream->pause();
				break;
			case osg::ImageStream::PAUSED:
				break;
			case osg::ImageStream::REWINDING:
				//std::cout << "Image stream rewinding" << std::endl;
				break;
			default:
				break;
		}
	}*/	
	if (playback_geode) { playback_geode->unref(); playback_geode=0; }
	if (border_geode) { border_geode->unref(); border_geode=0; }
	if (playback_transform) { playback_transform->unref(); playback_transform=0; }
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
	
	if (media_index > -1)	
		playback_texture = (osg::Texture2D*)(media_cycle->getLibrary()->getMedia(media_index)->getThumbnailPtr());
	//playback_texture->setUnRefImageDataAfterApply(true);
	state = playback_geometry->getOrCreateStateSet();
	state->setTextureAttribute(0, playback_texture);
	state->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
	
	playback_geometry->setColorArray(colors);
	playback_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	playback_geode->addDrawable(playback_geometry);
	
	playback_transform->addChild(playback_geode);
	
#ifdef IMAGE_BORDER
	
	border_geode = new Geode();
	border_geometry = new Geometry();	
	
	// border vertices
	vertices = new Vec3Array(5);
	(*vertices)[0] = Vec3(-imagex-xstep, -imagey-xstep, zpos);
	(*vertices)[1] = Vec3(imagex+xstep, -imagey-xstep, zpos);
	(*vertices)[2] = Vec3(imagex+xstep, imagey+xstep, zpos);
	(*vertices)[3] = Vec3(-imagex-xstep, imagey+xstep, zpos);
	(*vertices)[4] = Vec3(-imagex-xstep, -imagey-xstep, zpos);
	border_geometry->setVertexArray(vertices);
	
	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 8);	
	for(i=0; i<4; i++) {
		(*line_p)[2*i] = i;
		(*line_p)[2*i+1] = i+1;
	}
	border_geometry->addPrimitiveSet(line_p);
	
	border_geometry->setColorArray(colors3);// XS was : colors2, but (0.4f, 0.4f, 0.4f, 1.0f)
	border_geometry->setColorBinding(Geometry::BIND_OVERALL);
	state = border_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(2.0));
	
	state = border_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
	
	border_geode->addDrawable(border_geometry);
	
	playback_transform->addChild(border_geode);
	
	//(*colors2)[0] = Vec4(1.0f, 1.0f, 1.0f, 0.5f);
#endif	
	playback_transform->ref();
	//sprintf(name, "some audio element");
	playback_geode->setUserData(new ACRefId(track_index,"video track"));
#ifdef IMAGE_BORDER
	playback_geode->ref();	
	border_geode->ref();
#endif
}

void ACOsgVideoTrackRenderer::prepareTracks() {
	playback_geode = 0;
}

void ACOsgVideoTrackRenderer::updateTracks(double ratio) {
	if (media_changed)
	{
		track_node->removeChild(playback_geode);
		if (media){
		
			playbackGeode();
			track_node->addChild(playback_transform);
			video_stream = (ImageStream*)(((ACVideo*)(media_cycle->getLibrary()->getMedia(media_index)))->getStream());
		}	
		//track_node->addChild(playback_geode);
		media_changed = false;
	}
	
	if (height != 0.0f && width != 0.0f){
		float w = (float)(media_cycle->getWidth(media_index));
		float h = (float)(media_cycle->getHeight(media_index));
		Matrix T;
		if (w/h*height/width<1.0f) // video fits view height
			T =  Matrix::scale(w/h*height/width,1.0f,1.0f);
		else //if (h/w*width/height<1.0f) // video fits view width 
			T =  Matrix::scale(1.0f,h/w*width/height,1.0f);
		playback_transform->setMatrix(T);
	}
}
#endif//CF APPLE_IOS