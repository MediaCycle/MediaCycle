/*
 *  ACOsgAudioTrackRenderer.cpp
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

#if defined (SUPPORT_AUDIO)
#include "ACOsgAudioTrackRenderer.h"

#include <osgGA/GUIEventHandler>

using namespace osg;

ACOsgAudioTrackRenderer::ACOsgAudioTrackRenderer() {
	
	summary_waveform_geode = 0; summary_cursor_geode = 0; summary_cursor_transform = 0; track_geode = 0;
	playback_waveform_geode = 0; 
	//zoom_x = 10.0; zoom_y = 6.0;
	zoom_x = 1.0; zoom_y = 1.0;
	track_left_x = 0.0;
	summary_center_y = -yspan/2.0f+yspan/8.0f; //[-yspan/2.0f;yspan/2.0f]
	summary_height = yspan/8.0f;//[0;yspan/2.0f]
	playback_center_y = yspan/8.0f; //[-yspan/2.0f;yspan/2.0f]
	playback_height = (yspan-2*yspan/8.0f)/2.0f;//[0;yspan/2.0f]
	samples_hop_threshold = 10;
	samples_n_threshold = screen_width*15;
	samples = NULL;
}

ACOsgAudioTrackRenderer::~ACOsgAudioTrackRenderer() {
	// track_node->removeChild(0,1);
	if 	(summary_waveform_geode) {
		summary_waveform_geode->unref();
		summary_waveform_geode=0;
	}
	if 	(summary_cursor_geode) {
		summary_cursor_geode->unref();
		summary_cursor_geode=0;
	}
	if 	(summary_cursor_transform) {
		summary_cursor_transform->unref();
		summary_cursor_transform=0;
	}
	if 	(track_geode) {
		track_geode->unref();
		track_geode=0;
	}
	if 	(selection_begin_geode) {
		selection_begin_geode->unref();
		selection_begin_geode=0;
	}
	if 	(selection_zone_geode) {
		selection_zone_geode->unref();
		selection_zone_geode=0;
	}
	if 	(selection_end_geode) {
		selection_end_geode->unref();
		selection_end_geode=0;
	}
	if 	(playback_waveform_geode) {
		playback_waveform_geode->unref();
		playback_waveform_geode=0;
	}
}

void ACOsgAudioTrackRenderer::selectionWaveformGeode() {
	int i;
	
	int width;
	float *thumbnail;
	
	StateSet *state;
	
	Vec3Array* vertices;	
	DrawElementsUInt* line_p;
	
	Geometry *samples_geometry;
	Geometry *frame_geometry;
	Geometry *border_geometry;
	Geometry *baseline_geometry;
	
	summary_waveform_geode = new Geode();
	samples_geometry = new Geometry();
	frame_geometry = new Geometry();
	border_geometry = new Geometry();
	baseline_geometry = new Geometry();

	if (screen_width !=0){	
		// CF: temporary workaround as the ACUserLog tree and the ACLoopAttributes vector in ACMediaBrowser are not sync'd 
		//int media_index = track_index; // or media_cycle->getBrowser()->getMediaTrack(track_index).getMediaId(); 
		if (media_from_lib && media_cycle->getBrowser()->getMode() == AC_MODE_NEIGHBORS)
			media_index = media_cycle->getBrowser()->getUserLog()->getMediaIdFromNodeId(media_index);//CF can selectionWaveformGeode() occur more than once, once media_index is set?
		
		//CF std thumbnail, was for AudioGarden
		/*
		 if (media_from_lib)
		 width = media_cycle->getThumbnailWidth(media_index);
		 else
		 width = media->getThumbnailWidth(); //CF media should be non-null
		 */
		
		//CF full blast waveform
		/*
		 width = ((ACAudio*) media)->getNFrames();// / ((ACAudio*) media)->getSampleRate();
		 */
		
		//CF screen width adapted waveform
		width = screen_width;
		
		//CF std thumbnail, was for AudioGarden
		/*
		 if (media_from_lib)
		 thumbnail = (float*)media_cycle->getThumbnailPtr(media_index);//CF instead of track_index
		 else
		 thumbnail = (float*)media->getThumbnailPtr();
		 */
		
		//CF full blast waveform
		/*
		thumbnail = ((ACAudio*) media)->getMonoSamples();//CF with ACMediaDocuments, stereo tracks should be two different ACMedias
		*/
		
		//CF screen width adapted waveform
		/*
		float* samples = ((ACAudio*) media)->getSamples();
				
		int i, j, k;
		int n_samples_hop;
		float hop = (float)(((ACAudio*) media)->getNFrames()) / (float)(((ACAudio*) media)->getSampleRate()) / (float)(width);
		
		n_samples_hop = hop * ((ACAudio*) media)->getSampleRate();
		
		thumbnail = new float[2 * width];
		k = 0;
		int channels = ((ACAudio*) media)->getChannels();
		for (i=0; i< 2*width-1; i=i+2) {
			thumbnail[i] = 0;
			thumbnail[i+1] = 0;
			for (j=k;j<k+n_samples_hop;j++) {
				if ((samples[channels*j])< thumbnail[i]) {
					thumbnail[i] = samples[channels*j];
				}
				if ( samples[channels*j] > thumbnail[i+1] ) {
					thumbnail[i+1] = samples[channels*j];
				}
			}
			k += n_samples_hop;
		}
		width *= 2;
		 */
		
		//CF screen width adapted waveform, envelope instead of bars
		 //float* samples = ((ACAudio*) media)->getMonoSamples();
		 int i, j, k;
		 int n_samples_hop;
		 float hop = (float)(((ACAudio*) media)->getNFrames()) / (float)(((ACAudio*) media)->getSampleRate()) / (float)(width);
		 
		 n_samples_hop = hop * ((ACAudio*) media)->getSampleRate();
		 
		 thumbnail = new float[2 * width];
		 k = 0;
		 for (i=0; i< width; i++) {
			 thumbnail[i] = samples[k];//0;
			 thumbnail[i+width] = samples[k];//0;
			 for (j=k;j<k+n_samples_hop;j++) {
				 if ((samples[j])> thumbnail[i]) {
					 thumbnail[i] = samples[j];
					 //if (thumbnail[i]<0)
					//	 std::cout << "Error not positive" << std::endl;
				 }
				 if ( samples[j] < thumbnail[i+width] ) {
					 thumbnail[i+width] = samples[j];
					 //if (thumbnail[i]>0)
					//	 std::cout << "Error not negative" << std::endl;
				 }
			 }
			 if (thumbnail[i] < thumbnail[i+width])
				 std::cout << "Mismatch at " << i << std::endl;
			 k += n_samples_hop;
		 }
		width *= 2;
		
		///// 
		
		width = width / 2;
		//std::cout << "Number of points of the waveform: " << width << std::endl;
		
		zoom_x = xspan/xstep/width;// CF autosizefit possible only when displaying one track
		track_left_x = -((width) * xstep * zoom_x)/2; // CF autocenter possible only when displaying one track
			
		// samples vertices
		//bars
		/*
		vertices = new Vec3Array(2*width+2);
		for(i=0; i<width; i++) {
			(*vertices)[2*i] = Vec3(track_left_x + i * xstep * zoom_x, summary_center_y + summary_height * thumbnail[2*i] * zoom_y, zpos);
			(*vertices)[2*i+1] = Vec3(track_left_x + i * xstep * zoom_x, summary_center_y + summary_height * thumbnail[2*i+1] * zoom_y, zpos);
		}
		(*vertices)[2*i] = Vec3(track_left_x + 0.0, 0.0, zpos);
		(*vertices)[2*i+1] = Vec3(track_left_x + (i-1) * xstep * zoom_x, 0.0, zpos);
		samples_geometry->setVertexArray(vertices);
		*/

		//envelope
		/*vertices = new Vec3Array(2*width);
		for(i=0; i<width; i++) {
			(*vertices)[i] = Vec3(track_left_x + i * xstep * zoom_x, summary_center_y + summary_height * thumbnail[i] * zoom_y, zpos);
			(*vertices)[i+width] = Vec3(track_left_x + (width-i-1) * xstep * zoom_x, summary_center_y + summary_height * thumbnail[2*width-i-1] * zoom_y, zpos);
		}
		//(*vertices)[2*width] = Vec3(track_left_x + i * xstep * zoom_x, summary_center_y + summary_height * thumbnail[0] * zoom_y, zpos);
		samples_geometry->setVertexArray(vertices);*/
		
		
		//counter clockwise, polygon cull facing test
		/*vertices = new Vec3Array(2*width);
		for(i=0; i<width; i++) {
			(*vertices)[i] = Vec3(track_left_x + i * xstep * zoom_x, summary_center_y + summary_height * thumbnail[i+width] * zoom_y, zpos);
			(*vertices)[i+width] = Vec3(track_left_x + (width-i-1) * xstep * zoom_x, summary_center_y + summary_height * thumbnail[width-i-1] * zoom_y, zpos);
		}
		//(*vertices)[2*width] = Vec3(track_left_x + i * xstep * zoom_x, summary_center_y + summary_height * thumbnail[0] * zoom_y, zpos);
		samples_geometry->setVertexArray(vertices);*/
		
		// envelope with quads
		vertices = new Vec3Array(4*(width-1));
		for(i=0; i<width; i++) {
			(*vertices)[4*i] = Vec3(track_left_x + i * xstep * zoom_x, summary_center_y + summary_height * thumbnail[i] * zoom_y, zpos);
			(*vertices)[4*i+1] = Vec3(track_left_x + (i+1) * xstep * zoom_x, summary_center_y + summary_height * thumbnail[i+1] * zoom_y, zpos);
			(*vertices)[4*i+2] = Vec3(track_left_x + (i+1) * xstep * zoom_x, summary_center_y + summary_height * thumbnail[i+width+1] * zoom_y, zpos);
			(*vertices)[4*i+3] = Vec3(track_left_x + i * xstep * zoom_x, summary_center_y + summary_height * thumbnail[i+width] * zoom_y, zpos);
		}
		samples_geometry->setVertexArray(vertices);
	}
	
	//frame vertices
	vertices = new Vec3Array(4);
	(*vertices)[0] = Vec3(track_left_x + 0, (summary_center_y - summary_height) * zoom_y, zpos);
	(*vertices)[1] = Vec3(track_left_x + width * xstep * zoom_x, (summary_center_y - summary_height) * zoom_y, zpos);
	(*vertices)[2] = Vec3(track_left_x + width * xstep * zoom_x, (summary_center_y + summary_height) * zoom_y, zpos);
	(*vertices)[3] = Vec3(track_left_x + 0, (summary_center_y + summary_height) * zoom_y, zpos);
	frame_geometry->setVertexArray(vertices);
	
	//border vertices
	vertices = new Vec3Array(5);
	(*vertices)[0] = Vec3(track_left_x + 0, (summary_center_y - summary_height) * zoom_y, zpos);
	(*vertices)[1] = Vec3(track_left_x + width * xstep * zoom_x, ((summary_center_y - summary_height)+xstep) * zoom_y, zpos);
	(*vertices)[2] = Vec3(track_left_x + width * xstep * zoom_x, ((summary_center_y + summary_height)-xstep) * zoom_y, zpos);
	(*vertices)[3] = Vec3(track_left_x + 0, (summary_center_y + summary_height) * zoom_y, zpos);
	(*vertices)[4] = Vec3(track_left_x + 0, (summary_center_y - summary_height) * zoom_y, zpos);
	border_geometry->setVertexArray(vertices);
	
	//baseline vertices
	vertices = new Vec3Array(2);
	(*vertices)[0] = Vec3(track_left_x + 0, summary_center_y * zoom_y, zpos);
	(*vertices)[1] = Vec3(track_left_x + width * xstep * zoom_x, summary_center_y * zoom_y, zpos);
	baseline_geometry->setVertexArray(vertices);
	
	Vec4 color(0.9f, 0.9f, 0.9f, 0.9f);	
	Vec4Array* colors = new Vec4Array;
	colors->push_back(color);
	

	if (screen_width !=0){
		// bars
		/*line_p = new DrawElementsUInt(PrimitiveSet::LINES, 2*width+2);
		for(i=0; i<width+1; i++) {
			(*line_p)[2*i] = 2*i;
			(*line_p)[2*i+1] = 2*i+1;
		}
		samples_geometry->addPrimitiveSet(line_p);*/
		
		//envelope 
		/*line_p = new DrawElementsUInt(PrimitiveSet::POLYGON, 2*width); //POLYGON or LINE_LOOP
		for(i=0; i<2*width; i++) {
			(*line_p)[i] = i;
		}
		samples_geometry->addPrimitiveSet(line_p);*/
		 
		//envelope with quads
		line_p = new DrawElementsUInt(PrimitiveSet::QUADS, 4*(width+1));
		for(i=0; i<4*(width+1); i++) {
			line_p->push_back(i);
		}
		samples_geometry->addPrimitiveSet(line_p);
	}
	
	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 8);	
	for(i=0; i<4; i++) {
		(*line_p)[2*i] = i;
		(*line_p)[2*i+1] = i+1;
	}
	border_geometry->addPrimitiveSet(line_p);
	
	DrawElementsUInt *poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
	poly->push_back(0);
	poly->push_back(1);
	poly->push_back(2);
	poly->push_back(3);
	frame_geometry->addPrimitiveSet(poly);
	
	samples_geometry->setColorArray(colors);
	samples_geometry->setColorBinding(Geometry::BIND_OVERALL);//BIND_OFF, BIND_OVERALL, BIND_PER_PRIMITIVE_SET, BIND_PER_PRIMITIVE,BIND_PER_VERTEX 
	
	// set the normal in the same way color.
	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
	samples_geometry->setNormalArray(normals);
	samples_geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	
	border_geometry->setColorArray(colors);
	border_geometry->setColorBinding(Geometry::BIND_OVERALL);
		
	colors = new Vec4Array(1);
	(*colors)[0] = Vec4(0.0, 0.0, 0.0, 0.3f);
	frame_geometry->setColorArray(colors);
	frame_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 2);	
	(*line_p)[0] = 0;
	(*line_p)[1] = 1;
	baseline_geometry->addPrimitiveSet(line_p);

	colors = new Vec4Array(1);
	(*colors)[0] = Vec4(1.0, 1.0, 1.0, 1.0f);
	baseline_geometry->setColorArray(colors);
	baseline_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	state = summary_waveform_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
#if !defined(APPLE_IOS)
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON); //CF not supported by OpenGL ES 2...
#endif	
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	state = samples_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(1.0));
	
	state = baseline_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(1.0));
	
	summary_waveform_geode->addDrawable(samples_geometry);
	//summary_waveform_geode->addDrawable(border_geometry);
	summary_waveform_geode->addDrawable(frame_geometry);
	summary_waveform_geode->addDrawable(baseline_geometry);
	
	//sprintf(name, "some audio element");
	summary_waveform_geode->setUserData(new ACRefId(track_index,"audio track summary waveform"));
	//summary_waveform_geode->setName(name);
	summary_waveform_geode->ref();	
}

void ACOsgAudioTrackRenderer::selectionCursorGeode() {
	StateSet *state;
	
	Vec3Array* vertices;
	
	DrawElementsUInt* line_p;
	
	Geometry *summary_cursor_geometry;
	
	summary_cursor_transform = new MatrixTransform();
	summary_cursor_geode = new Geode();
	summary_cursor_geometry = new Geometry();
	
	vertices = new Vec3Array(2);
	/*(*vertices)[0] = Vec3(track_left_x + 0, ((summary_center_y - summary_height)+xstep) * zoom_y, zpos+0.00005);
	(*vertices)[1] = Vec3(track_left_x + 0, ((summary_center_y + summary_height)-xstep) * zoom_y, zpos+0.00005);*/
	(*vertices)[0] = Vec3((selection_end_pos+selection_begin_pos)/2.0f, ((summary_center_y - summary_height)+xstep) * zoom_y, zpos+0.00005);
	(*vertices)[1] = Vec3((selection_end_pos+selection_begin_pos)/2.0f, ((summary_center_y + summary_height)-xstep) * zoom_y, zpos+0.00005);
	summary_cursor_geometry->setVertexArray(vertices);
	
	Vec4 summary_cursor_color(0.2f, 0.9f, 0.2f, 0.9f);	
	Vec4Array* summary_cursor_colors = new Vec4Array;
	summary_cursor_colors->push_back(summary_cursor_color);		
	summary_cursor_geometry->setColorArray(summary_cursor_colors);
	summary_cursor_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 2);	
	(*line_p)[0] = 0;
	(*line_p)[1] = 1;	
	summary_cursor_geometry->addPrimitiveSet(line_p);
	
	state = summary_cursor_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(2.0));
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	state = summary_cursor_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
#if !defined(APPLE_IOS)
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON); //CF not supported by OpenGL ES 2...
#endif	
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	state = summary_cursor_transform->getOrCreateStateSet();
	state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	summary_cursor_geode->addDrawable(summary_cursor_geometry);
	
	summary_cursor_transform.get()->addChild(summary_cursor_geode);
	
	//sprintf(name, "some audio element");
	//summary_cursor_transform->setName(name);
	summary_cursor_transform->ref();
	summary_cursor_geode->setUserData(new ACRefId(track_index,"audio track cursor"));
	//summary_cursor_geode->setName(name);
	summary_cursor_geode->ref();
}

void ACOsgAudioTrackRenderer::selectionZoneGeode() {
	StateSet *state;
	
	Vec3Array* vertices;	
	DrawElementsUInt* line_p;
	
	Geometry *selection_zone_border_geometry;
	Geometry *selection_zone_frame_geometry;
	
	selection_zone_geode = new Geode();
	selection_zone_border_geometry = new Geometry();
	selection_zone_frame_geometry = new Geometry();
	selection_zone_transform = new MatrixTransform();
	
	int width = screen_width;
	
	zoom_x = xspan/xstep/width;// CF autosizefit possible only when displaying one track
	track_left_x = -((width+1) * xstep * zoom_x)/2; // CF autocenter possible only when displaying one track
	
	//border vertices
	vertices = new Vec3Array(5);
	(*vertices)[0] = Vec3(selection_begin_pos, (summary_center_y - summary_height) * zoom_y, zpos);
	(*vertices)[1] = Vec3(selection_end_pos, ((summary_center_y - summary_height)+xstep) * zoom_y, zpos);
	(*vertices)[2] = Vec3(selection_end_pos, ((summary_center_y + summary_height)-xstep) * zoom_y, zpos);
	(*vertices)[3] = Vec3(selection_begin_pos, (summary_center_y + summary_height) * zoom_y, zpos);
	(*vertices)[4] = Vec3(selection_begin_pos, (summary_center_y - summary_height) * zoom_y, zpos);
	selection_zone_border_geometry->setVertexArray(vertices);
	
	vertices = new Vec3Array(4);
	(*vertices)[0] = Vec3(selection_begin_pos+selection_sensing_width, (summary_center_y - summary_height) * zoom_y, zpos);
	(*vertices)[1] = Vec3(selection_end_pos-selection_sensing_width, (summary_center_y - summary_height) * zoom_y, zpos);
	(*vertices)[2] = Vec3(selection_end_pos-selection_sensing_width, (summary_center_y + summary_height) * zoom_y, zpos);
	(*vertices)[3] = Vec3(selection_begin_pos+selection_sensing_width, (summary_center_y + summary_height) * zoom_y, zpos);
	selection_zone_frame_geometry->setVertexArray(vertices);
	
	/*
	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 8);	
	for(int i=0; i<4; i++) {
		(*line_p)[2*i] = i;
		(*line_p)[2*i+1] = i+1;
	}
	*/
	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 4);	
	for(int i=0; i<4; i++) {
		(*line_p)[i] = i;
	}
	selection_zone_border_geometry->addPrimitiveSet(line_p);
	
	line_p = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
	line_p->push_back(0);
	line_p->push_back(1);
	line_p->push_back(2);
	line_p->push_back(3);
	selection_zone_frame_geometry->addPrimitiveSet(line_p);
	
	Vec4 color(0.0f, 0.0f, 0.0f, 0.2f);	
	Vec4Array* colors = new Vec4Array;
	colors->push_back(color);
	
	selection_zone_frame_geometry->setColorArray(colors);
	selection_zone_frame_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	color = Vec4(1.0f, 0.0f, 0.0f, 1.0f);	
	colors = new Vec4Array;
	colors->push_back(color);
	
	selection_zone_border_geometry->setColorArray(colors);
	selection_zone_border_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	// set the normal in the same way color.
	/*
	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
	selection_zone_geometry->setNormalArray(normals);
	selection_zone_geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	*/
	
	state = selection_zone_frame_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(1.0));
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	state = selection_zone_border_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(1.0));
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	state = selection_zone_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
#if !defined(APPLE_IOS)
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON); //CF not supported by OpenGL ES 2...
#endif	
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	state = selection_zone_transform->getOrCreateStateSet();
	state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	//selection_zone_geode->addDrawable(selection_zone_border_geometry);
	selection_zone_geode->addDrawable(selection_zone_frame_geometry);
	
	selection_zone_transform.get()->addChild(selection_zone_geode);
	
	//sprintf(name, "some audio element");
	//selection_transform->setName(name);
	selection_zone_transform->ref();
	selection_zone_geode->setUserData(new ACRefId(track_index,"track selection zone"));
	//summary_cursor_geode->setName(name);
	selection_zone_geode->ref();
}

void ACOsgAudioTrackRenderer::selectionBeginGeode() {
	StateSet *state;
	
	Vec3Array* vertices;	
	DrawElementsUInt* line_p;
	
	Geometry *selection_begin_geometry;
	
	selection_begin_geode = new Geode();
	selection_begin_geometry = new Geometry();
	selection_begin_transform = new MatrixTransform();
	
	int width = screen_width;
	
	zoom_x = xspan/xstep/width;// CF autosizefit possible only when displaying one track
	track_left_x = -((width+1) * xstep * zoom_x)/2; // CF autocenter possible only when displaying one track
		
	vertices = new Vec3Array(4);
	(*vertices)[0] = Vec3(selection_begin_pos, (summary_center_y - summary_height) * zoom_y, zpos);
	(*vertices)[1] = Vec3(selection_begin_pos+selection_sensing_width, (summary_center_y - summary_height) * zoom_y, zpos);
	(*vertices)[2] = Vec3(selection_begin_pos+selection_sensing_width, (summary_center_y + summary_height) * zoom_y, zpos);
	(*vertices)[3] = Vec3(selection_begin_pos, (summary_center_y + summary_height) * zoom_y, zpos);
	selection_begin_geometry->setVertexArray(vertices);
	
	//Vec4 color(0.0f, 1.0f, 0.0f, 1.0f);	
	Vec4 color(1.0f, 0.0f, 0.0f, 0.4f);	
	Vec4Array* colors = new Vec4Array;
	colors->push_back(color);

	line_p = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
	line_p->push_back(0);
	line_p->push_back(1);
	line_p->push_back(2);
	line_p->push_back(3);
	selection_begin_geometry->addPrimitiveSet(line_p);
	
	selection_begin_geometry->setColorArray(colors);
	selection_begin_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	// set the normal in the same way color.
	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
	selection_begin_geometry->setNormalArray(normals);
	selection_begin_geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	
	state = selection_begin_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(1.0));
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	state = selection_begin_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
#if !defined(APPLE_IOS)
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON); //CF not supported by OpenGL ES 2...
#endif	
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	state = selection_begin_transform->getOrCreateStateSet();
	state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	selection_begin_geode->addDrawable(selection_begin_geometry);
	
	selection_begin_transform.get()->addChild(selection_begin_geode);
	
	//sprintf(name, "some audio element");
	//selection_begin_transform->setName(name);
	selection_begin_transform->ref();
	selection_begin_geode->setUserData(new ACRefId(track_index,"track selection begin"));
	//summary_cursor_geode->setName(name);
	selection_begin_geode->ref();
}	

void ACOsgAudioTrackRenderer::selectionEndGeode() {
	StateSet *state;
	
	Vec3Array* vertices;	
	DrawElementsUInt* line_p;
	
	Geometry *selection_end_geometry;
	
	selection_end_geode = new Geode();
	selection_end_geometry = new Geometry();
	selection_end_transform = new MatrixTransform();
	
	int width = screen_width;
	
	zoom_x = xspan/xstep/width;// CF autosizefit possible only when displaying one track
	track_left_x = -((width+1) * xstep * zoom_x)/2; // CF autocenter possible only when displaying one track
	
	vertices = new Vec3Array(4);
	(*vertices)[0] = Vec3(selection_end_pos-selection_sensing_width, (summary_center_y - summary_height) * zoom_y, zpos);
	(*vertices)[1] = Vec3(selection_end_pos, (summary_center_y - summary_height) * zoom_y, zpos);
	(*vertices)[2] = Vec3(selection_end_pos, (summary_center_y + summary_height) * zoom_y, zpos);
	(*vertices)[3] = Vec3(selection_end_pos-selection_sensing_width, (summary_center_y + summary_height) * zoom_y, zpos);
	selection_end_geometry->setVertexArray(vertices);
	
	//Vec4 color(0.0f, 1.0f, 0.0f, 1.0f);	
	Vec4 color(1.0f, 0.0f, 0.0f, 0.4f);
	Vec4Array* colors = new Vec4Array;
	colors->push_back(color);
	
	line_p = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
	line_p->push_back(0);
	line_p->push_back(1);
	line_p->push_back(2);
	line_p->push_back(3);
	selection_end_geometry->addPrimitiveSet(line_p);
	
	selection_end_geometry->setColorArray(colors);
	selection_end_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	// set the normal in the same way color.
	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
	selection_end_geometry->setNormalArray(normals);
	selection_end_geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	
	state = selection_end_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(1.0));
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	state = selection_end_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
#if !defined(APPLE_IOS)
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON); //CF not supported by OpenGL ES 2...
#endif	
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	state = selection_end_transform->getOrCreateStateSet();
	state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	selection_end_geode->addDrawable(selection_end_geometry);
	
	selection_end_transform.get()->addChild(selection_end_geode);
	
	//sprintf(name, "some audio element");
	//selection_end_transform->setName(name);
	selection_end_transform->ref();
	selection_end_geode->setUserData(new ACRefId(track_index,"track selection end"));
	//summary_cursor_geode->setName(name);
	selection_end_geode->ref();
}	

void ACOsgAudioTrackRenderer::playbackWaveformGeode() {
	int i;
	
	int width;
	float *thumbnail;
	
	StateSet *state;
	
	Vec3Array* vertices;	
	DrawElementsUInt* line_p;
	
	Geometry *samples_geometry;
	Geometry *frame_geometry;
	Geometry *border_geometry;
	Geometry *baseline_geometry;
	Geometry *cursor_geometry;
	
	playback_waveform_geode = new Geode();
	samples_geometry = new Geometry();
	frame_geometry = new Geometry();
	border_geometry = new Geometry();
	baseline_geometry = new Geometry();
	cursor_geometry = new Geometry();
	
	int n_samples_hop = 0;
	int n_playback_samples = 0;
	if (screen_width !=0){	
		// CF: temporary workaround as the ACUserLog tree and the ACLoopAttributes vector in ACMediaBrowser are not sync'd 
		//int media_index = track_index; // or media_cycle->getBrowser()->getMediaTrack(track_index).getMediaId(); 
		if (media_from_lib && media_cycle->getBrowser()->getMode() == AC_MODE_NEIGHBORS)
			media_index = media_cycle->getBrowser()->getUserLog()->getMediaIdFromNodeId(media_index);//CF can selectionWaveformGeode() occur more than once, once media_index is set?
		
		width = screen_width;
		
		//CF screen width adapted waveform, envelope instead of bars
		//float* samples = ((ACAudio*) media)->getMonoSamples();
		int i, j, k;
		float hop = (float)(((ACAudio*) media)->getNFrames()) * (selection_end_pos-selection_begin_pos)/xspan / (float)(((ACAudio*) media)->getSampleRate()) / (float)(width);
		n_playback_samples = (int)((float)(((ACAudio*) media)->getNFrames()) * (selection_end_pos-selection_begin_pos)/xspan);
		n_samples_hop = hop * ((ACAudio*) media)->getSampleRate();
		samples_n_threshold = screen_width*15;//magic number...
		
		if (n_samples_hop >= 1){
			thumbnail = new float[2 * width];
			k = (int)( ((ACAudio*) media)->getNFrames() *(selection_begin_pos+xspan/2.0f)/xspan);
			for (i=0; i< width; i++) {
				if ((k < 0)||(k>((ACAudio*) media)->getNFrames())){
					thumbnail[i] = 0.0f;
					thumbnail[i+width] = 0.0f;
				}
				else{	
					thumbnail[i] = samples[k];//0;
					thumbnail[i+width] = samples[k];//0;
				}	
				for (j=k;j<k+n_samples_hop;j++) {
					if ((j<0)||(k>((ACAudio*) media)->getNFrames())){
						thumbnail[i] = 0.0f;
						thumbnail[i+width] = 0.0f;
					}
					else {
						if ((samples[j])> thumbnail[i]) {
							thumbnail[i] = samples[j];
						}
						if ( samples[j] < thumbnail[i+width] ) {
							thumbnail[i+width] = samples[j];
						}
					}					
				}
				if (thumbnail[i] < thumbnail[i+width])
					std::cout << "Mismatch at " << i << std::endl;
				k += n_samples_hop;
			}
		}
		else {
			width = (int)((((ACAudio*) media)->getNFrames()) * (selection_end_pos-selection_begin_pos)/xspan);
			thumbnail = new float[width];
			k = (int)( ((ACAudio*) media)->getNFrames() *(selection_begin_pos+xspan/2.0f)/xspan);
			for (i=0; i< width; i++) {
				if (k+i<0)
					thumbnail[i]=0.0f;
				else	
					thumbnail[i]=samples[k+i];
			}	
		}
		
		/*std::cout << "Number of" << std::endl;
		std::cout << "  pixels of width: " << width << std::endl;
		std::cout << "  signal frames: " << ((ACAudio*) media)->getNFrames() << std::endl;
		std::cout << "  playback frames: " << (((ACAudio*) media)->getNFrames()) * (selection_end_pos-selection_begin_pos)/xspan << std::endl;
		std::cout << "  hop'ed samples: " << n_samples_hop << " (hop: " << hop <<")"<< std::endl;*/
		
		zoom_x = xspan/xstep/width;// CF autosizefit possible only when displaying one track
		track_left_x = -((width) * xstep * zoom_x)/2; // CF autocenter possible only when displaying one track
		
		//if (n_samples_hop > samples_hop_threshold){
		/*if (n_playback_samples > samples_n_threshold){
			// envelope with quads
			vertices = new Vec3Array(4*(width-1));
			for(i=0; i<width; i++) {
				(*vertices)[4*i] = Vec3(track_left_x + i * xstep * zoom_x, playback_center_y + playback_height * thumbnail[i] * zoom_y, zpos);
				(*vertices)[4*i+1] = Vec3(track_left_x + (i+1) * xstep * zoom_x, playback_center_y + playback_height * thumbnail[i+1] * zoom_y, zpos);
				(*vertices)[4*i+2] = Vec3(track_left_x + (i+1) * xstep * zoom_x, playback_center_y + playback_height * thumbnail[i+width+1] * zoom_y, zpos);
				(*vertices)[4*i+3] = Vec3(track_left_x + i * xstep * zoom_x, playback_center_y + playback_height * thumbnail[i+width] * zoom_y, zpos);
			}
		}
		else */if (n_samples_hop >= 1){
			//envelope
			vertices = new Vec3Array(2*width);
			for(i=0; i<width; i++) {
				(*vertices)[i] = Vec3(track_left_x + i * xstep * zoom_x, playback_center_y + playback_height * thumbnail[i] * zoom_y, zpos);
				(*vertices)[i+width] = Vec3(track_left_x + (width-i-1) * xstep * zoom_x, playback_center_y + playback_height * thumbnail[2*width-i-1] * zoom_y, zpos);
			}
		}
		else {
			vertices = new Vec3Array(width);
			for(i=0; i<width; i++) {
				(*vertices)[i] = Vec3(track_left_x + i * xstep * zoom_x, playback_center_y + playback_height * thumbnail[i] * zoom_y, zpos);
			}
		}	
		samples_geometry->setVertexArray(vertices);
	}
	
	//frame vertices
	vertices = new Vec3Array(4);
	(*vertices)[0] = Vec3(track_left_x + 0, (playback_center_y - playback_height) * zoom_y, zpos);
	(*vertices)[1] = Vec3(track_left_x + width * xstep * zoom_x, (playback_center_y - playback_height) * zoom_y, zpos);
	(*vertices)[2] = Vec3(track_left_x + width * xstep * zoom_x, (playback_center_y + playback_height) * zoom_y, zpos);
	(*vertices)[3] = Vec3(track_left_x + 0, (playback_center_y + playback_height) * zoom_y, zpos);
	frame_geometry->setVertexArray(vertices);
	
	//border vertices
	vertices = new Vec3Array(5);
	(*vertices)[0] = Vec3(track_left_x + 0, (playback_center_y - playback_height) * zoom_y, zpos);
	(*vertices)[1] = Vec3(track_left_x + width * xstep * zoom_x, ((playback_center_y - playback_height)+xstep) * zoom_y, zpos);
	(*vertices)[2] = Vec3(track_left_x + width * xstep * zoom_x, ((playback_center_y + playback_height)-xstep) * zoom_y, zpos);
	(*vertices)[3] = Vec3(track_left_x + 0, (playback_center_y + playback_height) * zoom_y, zpos);
	(*vertices)[4] = Vec3(track_left_x + 0, (playback_center_y - playback_height) * zoom_y, zpos);
	border_geometry->setVertexArray(vertices);
	
	//baseline vertices
	vertices = new Vec3Array(2);
	(*vertices)[0] = Vec3(track_left_x + 0, playback_center_y * zoom_y, zpos);
	(*vertices)[1] = Vec3(track_left_x + width * xstep * zoom_x, playback_center_y * zoom_y, zpos);
	baseline_geometry->setVertexArray(vertices);
	
	//cursor vertices
	vertices = new Vec3Array(2);
	(*vertices)[0] = Vec3(0, (playback_center_y - playback_height) * zoom_y, zpos);
	(*vertices)[1] = Vec3(0, (playback_center_y + playback_height) * zoom_y, zpos);
	cursor_geometry->setVertexArray(vertices);
	
	if (screen_width !=0){
		//if (n_samples_hop > samples_hop_threshold){
		/*if (n_playback_samples > samples_n_threshold){	
			//envelope with quads
			line_p = new DrawElementsUInt(PrimitiveSet::QUADS, 4*(width+1));
			for(i=0; i<4*(width+1); i++) {
				line_p->push_back(i);
			}
		}
		else */if (n_samples_hop >= 1){
			//envelope 
			line_p = new DrawElementsUInt(PrimitiveSet::LINE_LOOP, 2*width); //POLYGON or LINE_LOOP
			for(i=0; i<2*width; i++) {
				(*line_p)[i] = i;
			}
		}
		else {
			//envelope 
			line_p = new DrawElementsUInt(PrimitiveSet::LINE_LOOP, width); //POLYGON or LINE_LOOP
			for(i=0; i<width; i++) {
				(*line_p)[i] = i;
			}
		}
		samples_geometry->addPrimitiveSet(line_p);
	}
	
	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 8);	
	for(i=0; i<4; i++) {
		(*line_p)[2*i] = i;
		(*line_p)[2*i+1] = i+1;
	}
	border_geometry->addPrimitiveSet(line_p);
	
	DrawElementsUInt *poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
	poly->push_back(0);
	poly->push_back(1);
	poly->push_back(2);
	poly->push_back(3);
	frame_geometry->addPrimitiveSet(poly);
	
	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 2);	
	(*line_p)[0] = 0;
	(*line_p)[1] = 1;	
	cursor_geometry->addPrimitiveSet(line_p);
	
	Vec4 color(0.9f, 0.9f, 0.9f, 0.9f);	
	Vec4Array* colors = new Vec4Array;
	colors->push_back(color);
	samples_geometry->setColorArray(colors);
	samples_geometry->setColorBinding(Geometry::BIND_OVERALL);//BIND_OFF, BIND_OVERALL, BIND_PER_PRIMITIVE_SET, BIND_PER_PRIMITIVE,BIND_PER_VERTEX 
	
	// set the normal in the same way color.
	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
	samples_geometry->setNormalArray(normals);
	samples_geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	
	colors = new Vec4Array(1);
	(*colors)[0] = Vec4(1.0f, 0.0, 0.0, 1.0f);
	border_geometry->setColorArray(colors);
	border_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	colors = new Vec4Array(1);
	(*colors)[0] = Vec4(0.0, 0.0, 0.0, 0.3f);
	frame_geometry->setColorArray(colors);
	frame_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 2);	
	(*line_p)[0] = 0;
	(*line_p)[1] = 1;
	baseline_geometry->addPrimitiveSet(line_p);
	
	colors = new Vec4Array(1);
	(*colors)[0] = Vec4(1.0, 1.0, 1.0, 1.0f);
	baseline_geometry->setColorArray(colors);
	baseline_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	colors = new Vec4Array(1);
	(*colors)[0] = Vec4(0.2f, 0.9f, 0.2f, 0.9f);
	cursor_geometry->setColorArray(colors);
	cursor_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	state = playback_waveform_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
#if !defined(APPLE_IOS)
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON); //CF not supported by OpenGL ES 2...
#endif	
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	state = samples_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(1.0));
	
	state = baseline_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(1.0));
	
	state = cursor_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(2.0));
	
	playback_waveform_geode->addDrawable(samples_geometry);
	playback_waveform_geode->addDrawable(border_geometry);
	//playback_waveform_geode->addDrawable(frame_geometry);
	playback_waveform_geode->addDrawable(baseline_geometry);
	playback_waveform_geode->addDrawable(cursor_geometry);
	
	//sprintf(name, "some audio element");
	playback_waveform_geode->setUserData(new ACRefId(track_index,"audio track playback waveform"));
	//playback_waveform_geode->setName(name);
	playback_waveform_geode->ref();	
}

void ACOsgAudioTrackRenderer::trackGeode() {
	StateSet *state;
	
	track_geode = new Geode();
	
	TessellationHints *hints = new TessellationHints();
	hints->setDetailRatio(0.0);
	
	state = track_geode->getOrCreateStateSet();
	state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);	
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	
	track_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),xspan,yspan,0.0), hints)); //draws a square // Vintage AudioCycle
	//track_geode->setName(name);
	track_geode->ref();	
}

void ACOsgAudioTrackRenderer::prepareTracks() {
	summary_waveform_geode = 0;
	summary_cursor_transform = 0;
	summary_cursor_geode = 0;
	track_geode = 0;
	selection_begin_geode = 0;
	selection_zone_geode = 0;
	selection_end_geode = 0;
	playback_waveform_geode = 0;
}

void ACOsgAudioTrackRenderer::updateTracks(double ratio) {	
	if (!manual_selection && media && media_from_lib) 
	{
		if (media_index > -1)
		{
			const ACMediaNode &attribute = media_cycle->getMediaNode(media_index);
			if ( attribute.getActivity()==1) 
			{	
				float selection_width = selection_end_pos-selection_begin_pos;
				float selection_pos = -xspan/2.0f + (float) attribute.getCurrentFrame() / (float)(((ACAudio*) media)->getNFrames())*xspan;
				this->setSelectionBegin(selection_pos - selection_width/2.0f);
				this->setSelectionEnd(selection_pos + selection_width/2.0f);
			}
		}
	}
	//manual_selection = false;
	
	//Matrix curserT;
	if (media_changed || screen_width_changed)
	{
		//track_node->removeChild(summary_cursor_transform.get());
		track_node->removeChild(summary_waveform_geode);
	}
	if (media_changed || screen_width_changed || selection_begin_pos_changed || selection_end_pos_changed)
	{
		track_node->removeChild(playback_waveform_geode);
	}		
	if (media_changed || selection_begin_pos_changed || selection_end_pos_changed)
	{	
		track_node->removeChild(selection_begin_transform.get());
		track_node->removeChild(selection_zone_transform.get());
		track_node->removeChild(selection_end_transform.get());
		/*track_node->removeChild(selection_begin_geode);
		 track_node->removeChild(selection_zone_geode);
		 track_node->removeChild(selection_end_geode);*/
		track_node->removeChild(summary_cursor_transform.get());
		track_node->removeChild(playback_waveform_geode);
	}
	if (media){
		if (media_changed)
		{	
			if (samples) delete samples;
			samples = ((ACAudio*) media)->getMonoSamples();
			playback_min_width = screen_width*xspan/(((ACAudio*) media)->getNFrames()); // one frame per pixel
		}	
		if (media_changed || screen_width_changed)
		{
			selectionWaveformGeode();
			//selectionCursorGeode();
			track_node->addChild(summary_waveform_geode);
			//track_node->addChild(summary_cursor_transform.get());
		}
		if (media_changed || screen_width_changed || selection_begin_pos_changed || selection_end_pos_changed)
		{
			playbackWaveformGeode();
			track_node->addChild(playback_waveform_geode);
		}
		if (media_changed || selection_begin_pos_changed || selection_end_pos_changed)
		{	
			selectionZoneGeode();
			track_node->addChild(selection_zone_transform.get());
			if (media_changed || selection_begin_pos_changed)
				selectionBeginGeode();
			track_node->addChild(selection_begin_transform.get());
			if (media_changed || selection_end_pos_changed)
				selectionEndGeode();
			track_node->addChild(selection_end_transform.get());
			selectionCursorGeode();
			track_node->addChild(summary_cursor_transform.get());
		}
		selection_begin_pos_changed = false;
		selection_end_pos_changed = false;
		/*track_node->addChild(selection_zone_geode);
		track_node->addChild(selection_begin_geode);
		track_node->addChild(selection_end_geode);*/
		displayed_media_index = media_index;
		media_changed = false;
	}	
	//CF solve this for media outside lib
	/*
	if (media_from_lib) 
	{
		if (media_index > -1)
		{
			const ACMediaNode &attribute = media_cycle->getMediaNode(media_index);
			if ( attribute.getActivity()==1) 
			{	
				curserT.makeTranslate(Vec3(attribute.getCursor() * xstep/2.0f * screen_width/media->getThumbnailWidth(), 0.0f, 0.0f));			// curserT =  Matrix::scale(0.5/zoom,0.5/zoom,0.5/zoom) * curserT;
				summary_cursor_transform->setMatrix(curserT);
			}
		}	
	}
	*/ 
}
#endif //defined (SUPPORT_AUDIO)