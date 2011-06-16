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

#include <osgUtil/Tessellator> // to tessellate multiple contours

using namespace osg;

ACOsgAudioTrackRenderer::ACOsgAudioTrackRenderer() : ACOsgTrackRenderer() {

	summary_waveform_geode = 0; summary_cursor_geode = 0; summary_cursor_transform = 0; track_geode = 0;
	playback_waveform_geode = 0;
	//zoom_x = 10.0; zoom_y = 6.0;
	zoom_x = 1.0; zoom_y = 1.0;
	track_left_x = 0.0;
	summary_height = yspan/16.0f; // values [0;yspan/2.0f]
	segments_height = yspan/16.0f; // values [0;yspan/2.0f]
	playback_height = yspan/2.0f - summary_height ; // yspan/2.0f - summary_height - segments_height; // (yspan-2*yspan/8.0f)/2.0f; // values [0;yspan/2.0f]
	summary_center_y = -yspan/2.0f + summary_height; // -yspan/2.0f + summary_height + segments_height; //-yspan/2.0f+yspan/8.0f; // values [-yspan/2.0f;yspan/2.0f]
	segments_center_y = -yspan/2.0f + segments_height;
	playback_center_y = -yspan/2.0f + summary_height + playback_height + segments_height; //yspan/4.0f; // values [-yspan/2.0f;yspan/2.0f]
	//playback_scale = 0.5f;

	samples_hop_threshold = 10;
	samples_n_threshold = screen_width*15;
	samples = 0;

    segments_transform = new MatrixTransform();
	segments_group = new Group();
	segments_number = 0;
}

ACOsgAudioTrackRenderer::~ACOsgAudioTrackRenderer() {
	// track_node->removeChild(0,1);
	if(summary_waveform_geode) summary_waveform_geode=0;
	if(summary_cursor_geode) summary_cursor_geode=0;
	if(summary_cursor_transform) summary_cursor_transform=0;
	if(track_geode) track_geode=0;
	if(selection_begin_geode) selection_begin_geode=0;
	if(selection_zone_geode) selection_zone_geode=0;
	if(selection_end_geode) selection_end_geode=0;
	if(playback_waveform_geode) playback_waveform_geode=0;
	if(segments_transform) segments_transform=0;
}

void ACOsgAudioTrackRenderer::selectionWaveformGeode() {
	int i;

	int width;
	float *thumbnail;

	StateSet *state;

	Vec3Array* vertices;	
	osg::ref_ptr<DrawElementsUInt> line_p;

	osg::ref_ptr<Geometry> samples_geometry;
	osg::ref_ptr<Geometry> frame_geometry;
	osg::ref_ptr<Geometry> border_geometry;
	osg::ref_ptr<Geometry> baseline_geometry;

	summary_waveform_geode = new Geode();
	samples_geometry = new Geometry();
	frame_geometry = new Geometry();
	border_geometry = new Geometry();
	baseline_geometry = new Geometry();

	float sample_rate = (float)(((ACAudio*) media)->getSampleRate());
	float n_frames = (float)(((ACAudio*) media)->getNFrames());

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
		int n_samples_hop = 0;

		float hop = n_frames / sample_rate / (float)(width);

		n_samples_hop = hop * sample_rate;

		if (n_frames >= 10*screen_width){
			thumbnail = new float[2 * width];
			int k = 0;
			for (int i=0; i< width; i++) {
				 thumbnail[i] = samples[k];//0;
				 thumbnail[i+width] = samples[k];//0;
				 for (int j=k;j<k+n_samples_hop;j++) {
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
		}
        else {
			width = (int)(n_frames);
			thumbnail = new float[width];
			int k = 0;
			for (int i=0; i< width; i++) {
				if ((k+i<0)||(k+i>=width))
					thumbnail[i]=0.0f;
				else	
					thumbnail[i]=samples[k+i];
			}
		}

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

        if (n_frames >= 10*screen_width){
		    //envelope
		    vertices = new Vec3Array(2*width);
		    for(int i=0; i<width; i++) {
		    	(*vertices)[i] = Vec3(track_left_x + i * xstep * zoom_x, summary_center_y + summary_height * thumbnail[i] * zoom_y, zpos);
		    	(*vertices)[i+width] = Vec3(track_left_x + (width-i-1) * xstep * zoom_x, summary_center_y + summary_height * thumbnail[2*width-i-1] * zoom_y, zpos);
		    }
		    //(*vertices)[2*width] = Vec3(track_left_x + i * xstep * zoom_x, summary_center_y + summary_height * thumbnail[0] * zoom_y, zpos);
        }
        else {
			vertices = new Vec3Array(width);
			for(int i=0; i<width; i++) {
				(*vertices)[i] = Vec3(track_left_x + i * xstep * zoom_x, summary_center_y + summary_height * thumbnail[i] * zoom_y, zpos);
			}
		}
        samples_geometry->setVertexArray(vertices);

		//counter clockwise, polygon cull facing test
		/*vertices = new Vec3Array(2*width);
		for(i=0; i<width; i++) {
			(*vertices)[i] = Vec3(track_left_x + i * xstep * zoom_x, summary_center_y + summary_height * thumbnail[i+width] * zoom_y, zpos);
			(*vertices)[i+width] = Vec3(track_left_x + (width-i-1) * xstep * zoom_x, summary_center_y + summary_height * thumbnail[width-i-1] * zoom_y, zpos);
		}
		//(*vertices)[2*width] = Vec3(track_left_x + i * xstep * zoom_x, summary_center_y + summary_height * thumbnail[0] * zoom_y, zpos);
		samples_geometry->setVertexArray(vertices);*/

		// envelope with quads
		/*
		vertices = new Vec3Array(4*(width));
		for(i=0; i<width-1; i++) {
			(*vertices)[4*i] = Vec3(track_left_x + i * xstep * zoom_x, summary_center_y + summary_height * thumbnail[i] * zoom_y, zpos);
			(*vertices)[4*i+1] = Vec3(track_left_x + (i+1) * xstep * zoom_x, summary_center_y + summary_height * thumbnail[i+1] * zoom_y, zpos);
			(*vertices)[4*i+2] = Vec3(track_left_x + (i+1) * xstep * zoom_x, summary_center_y + summary_height * thumbnail[i+width+1] * zoom_y, zpos);
			(*vertices)[4*i+3] = Vec3(track_left_x + i * xstep * zoom_x, summary_center_y + summary_height * thumbnail[i+width] * zoom_y, zpos);
		}
		samples_geometry->setVertexArray(vertices);*/
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
	osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array;
	colors->push_back(color);


	if (samples && screen_width !=0){
		// bars
		/*line_p = new DrawElementsUInt(PrimitiveSet::LINES, 2*width+2);
		for(i=0; i<width+1; i++) {
			(*line_p)[2*i] = 2*i;
			(*line_p)[2*i+1] = 2*i+1;
		}
		samples_geometry->addPrimitiveSet(line_p);*/

        if (n_frames >= 10*screen_width){
		    //envelope
		    line_p = new DrawElementsUInt(PrimitiveSet::POLYGON, 2*width); //POLYGON or LINE_LOOP
		    for(int i=0; i<2*width; i++) {
			    (*line_p)[i] = i;
		    }
		    samples_geometry->addPrimitiveSet(line_p);
		    // We use a Tessellator to produce the tessellation required once only 
		    // and the contours are discarded.
		    //std::cout << "Tesselating summary..." << std::endl;
		    osg::ref_ptr<osgUtil::Tessellator> tscx=new osgUtil::Tessellator; // the v1.2 multi-contour Tessellator.
		    tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
		    tscx->setBoundaryOnly(false);
		    tscx->setWindingType( osgUtil::Tessellator::TESS_WINDING_ODD); // so that first change in wind type makes the commonest tessellation - ODD.
		    tscx->retessellatePolygons(*samples_geometry);
		    //std::cout << "Tesselation summary done." << std::endl;
        }
        else {
			//envelope
			line_p = new DrawElementsUInt(PrimitiveSet::LINE_STRIP, width); //POLYGON or LINE_LOOP
			for(int i=0; i<width; i++) {
				(*line_p)[i] = i;
			}
			samples_geometry->addPrimitiveSet(line_p);
		}
		
		//envelope with quads
		/*line_p = new DrawElementsUInt(PrimitiveSet::QUADS, 4*(width-1));
		for(i=0; i<4*(width-1); i++) {
			line_p->push_back(i);
		}
		samples_geometry->addPrimitiveSet(line_p);*/
	}

	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 8);
	for(int i=0; i<4; i++) {
		(*line_p)[2*i] = i;
		(*line_p)[2*i+1] = i+1;
	}
	border_geometry->addPrimitiveSet(line_p);

	osg::ref_ptr<DrawElementsUInt> poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
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
	//summary_waveform_geode->addDrawable(frame_geometry);// disabled frame
	summary_waveform_geode->addDrawable(baseline_geometry);

	//sprintf(name, "some audio element");
	summary_waveform_geode->setUserData(new ACRefId(track_index,"audio track summary waveform"));
	//summary_waveform_geode->setName(name);
	//ref_ptr//summary_waveform_geode->ref();
}

void ACOsgAudioTrackRenderer::selectionCursorGeode() {
	StateSet *state;

	Vec3Array* vertices;

	osg::ref_ptr<DrawElementsUInt> line_p;

	osg::ref_ptr<Geometry> summary_cursor_geometry;

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
	osg::ref_ptr<osg::Vec4Array> summary_cursor_colors = new Vec4Array;
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

	summary_cursor_transform->addChild(summary_cursor_geode);

	//sprintf(name, "some audio element");
	//summary_cursor_transform->setName(name);
	//ref_ptr//summary_cursor_transform->ref();
	summary_cursor_geode->setUserData(new ACRefId(track_index,"audio track cursor"));
	//summary_cursor_geode->setName(name);
	//ref_ptr//summary_cursor_geode->ref();
}

void ACOsgAudioTrackRenderer::selectionZoneGeode() {
	StateSet *state;

	Vec3Array* vertices;
	osg::ref_ptr<DrawElementsUInt> line_p;

	osg::ref_ptr<Geometry> selection_zone_border_geometry;
	osg::ref_ptr<Geometry> selection_zone_frame_geometry;

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
	osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array;
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

	selection_zone_transform->addChild(selection_zone_geode);

	//sprintf(name, "some audio element");
	//selection_transform->setName(name);
	//ref_ptr//selection_zone_transform->ref();
	selection_zone_geode->setUserData(new ACRefId(track_index,"track selection zone"));
	//summary_cursor_geode->setName(name);
	//ref_ptr//selection_zone_geode->ref();
}

void ACOsgAudioTrackRenderer::selectionBeginGeode() {
	StateSet *state;

	Vec3Array* vertices;
	osg::ref_ptr<DrawElementsUInt> line_p;

	osg::ref_ptr<Geometry> selection_begin_geometry;

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
	Vec4 color(0.2f, 0.9f, 0.2f, 0.9f);//(1.0f, 0.0f, 0.0f, 0.4f);
	osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array;
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

	selection_begin_transform->addChild(selection_begin_geode);

	//sprintf(name, "some audio element");
	//selection_begin_transform->setName(name);
	//ref_ptr//selection_begin_transform->ref();
	selection_begin_geode->setUserData(new ACRefId(track_index,"track selection begin"));
	//summary_cursor_geode->setName(name);
	//ref_ptr//selection_begin_geode->ref();
}

void ACOsgAudioTrackRenderer::selectionEndGeode() {
	StateSet *state;

	Vec3Array* vertices;
	osg::ref_ptr<DrawElementsUInt> line_p;

	osg::ref_ptr<Geometry> selection_end_geometry;

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
	Vec4 color(0.2f, 0.9f, 0.2f, 0.9f);//(1.0f, 0.0f, 0.0f, 0.4f);
	osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array;
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

	selection_end_transform->addChild(selection_end_geode);

	//sprintf(name, "some audio element");
	//selection_end_transform->setName(name);
	//ref_ptr//selection_end_transform->ref();
	selection_end_geode->setUserData(new ACRefId(track_index,"track selection end"));
	//summary_cursor_geode->setName(name);
	//ref_ptr//selection_end_geode->ref();
}

void ACOsgAudioTrackRenderer::playbackWaveformGeode() {
	int width;
	float *thumbnail;

	StateSet *state;

	Vec3Array* vertices;
	osg::ref_ptr<DrawElementsUInt> line_p;

	osg::ref_ptr<Geometry> samples_geometry;
	osg::ref_ptr<Geometry> frame_geometry;
	osg::ref_ptr<Geometry> border_geometry;
	osg::ref_ptr<Geometry> baseline_geometry;
	osg::ref_ptr<Geometry> cursor_geometry;

	playback_waveform_geode = new Geode();
	samples_geometry = new Geometry();
	frame_geometry = new Geometry();
	border_geometry = new Geometry();
	baseline_geometry = new Geometry();
	cursor_geometry = new Geometry();

	int n_samples_hop = 0;
	int n_playback_samples = 0;
	if (samples && screen_width !=0){
		// CF: temporary workaround as the ACUserLog tree and the ACLoopAttributes vector in ACMediaBrowser are not sync'd 
		//int media_index = track_index; // or media_cycle->getBrowser()->getMediaTrack(track_index).getMediaId(); 
		if (media_from_lib && media_cycle->getBrowser()->getMode() == AC_MODE_NEIGHBORS)
			media_index = media_cycle->getBrowser()->getUserLog()->getMediaIdFromNodeId(media_index);//CF can selectionWaveformGeode() occur more than once, once media_index is set?

		width = screen_width;

		//CF screen width adapted waveform, envelope instead of bars
		//float* samples = ((ACAudio*) media)->getMonoSamples();
		float n_frames = (float)(((ACAudio*) media)->getNFrames());
		float sample_rate = (float)(((ACAudio*) media)->getSampleRate());
		float hop = n_frames * (selection_end_pos-selection_begin_pos)/xspan / sample_rate / (float)(width);
		n_playback_samples = (int)(n_frames * (selection_end_pos-selection_begin_pos)/xspan);
		n_samples_hop = hop * sample_rate;
		samples_n_threshold = screen_width*15;//magic number...

		//if (n_samples_hop >= 1){
		if (n_playback_samples >= 10*screen_width){
			thumbnail = new float[2 * width];
			int k = (int)( n_frames *(selection_begin_pos+xspan/2.0f)/xspan);
			for (int i=0; i< width; i++) {
				if ((k < 0)||(k>n_frames)){
					thumbnail[i] = 0.0f;
					thumbnail[i+width] = 0.0f;
				}
				else{
					thumbnail[i] = samples[k];//0;
					thumbnail[i+width] = samples[k];//0;
				}
				for (int j=k;j<k+n_samples_hop;j++) {
					if ((j<0)||(k>n_frames)){
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
			width = (int)(n_frames * (selection_end_pos-selection_begin_pos)/xspan);
			thumbnail = new float[width];
			int k = (int)( n_frames *(selection_begin_pos+xspan/2.0f)/xspan);
			for (int i=0; i< width; i++) {
				if ((k+i<0)||(k+i>=width))
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
		else */if (n_playback_samples >= 10*screen_width){
		//if (n_samples_hop >= 1){
			//envelope
			vertices = new Vec3Array(2*width);
			for(int i=0; i<width; i++) {
				(*vertices)[i] = Vec3(track_left_x + i * xstep * zoom_x, playback_center_y + playback_height * thumbnail[i] * zoom_y, zpos);
				(*vertices)[i+width] = Vec3(track_left_x + (width-i-1) * xstep * zoom_x, playback_center_y + playback_height * thumbnail[2*width-i-1] * zoom_y, zpos);
			}
		}
		else {
			vertices = new Vec3Array(width);
			for(int i=0; i<width; i++) {
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

	if (samples && screen_width !=0){
		//if (n_samples_hop > samples_hop_threshold){
		/*if (n_playback_samples > samples_n_threshold){
			//envelope with quads
			line_p = new DrawElementsUInt(PrimitiveSet::QUADS, 4*(width+1));
			for(i=0; i<4*(width+1); i++) {
				line_p->push_back(i);
			}
		}
		else */if (n_playback_samples >= 10*screen_width){
			//if (n_samples_hop >= 1){
			//envelope 
			//std::cout << "Playback (POLYGON)" << std::endl;
			line_p = new DrawElementsUInt(PrimitiveSet::POLYGON, 2*width); //POLYGON or LINE_LOOP
			for(int i=0; i<2*width; i++) {
				(*line_p)[i] = i;
			}
			samples_geometry->addPrimitiveSet(line_p);
			// We use a Tessellator to produce the tessellation required once only 
			// and the contours are discarded.
			//std::cout << "Tesselating playback..." << std::endl;
			osg::ref_ptr<osgUtil::Tessellator> tscx=new osgUtil::Tessellator; // the v1.2 multi-contour Tessellator.
			tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
			tscx->setBoundaryOnly(false);
			tscx->setWindingType( osgUtil::Tessellator::TESS_WINDING_ODD); // so that first change in wind type makes the commonest tessellation - ODD.
			tscx->retessellatePolygons(*samples_geometry);
			//std::cout << "Tesselation playback done." << std::endl;
		}
		else {
			//envelope
			line_p = new DrawElementsUInt(PrimitiveSet::LINE_STRIP, width); //POLYGON or LINE_LOOP
			for(int i=0; i<width; i++) {
				(*line_p)[i] = i;
			}
			samples_geometry->addPrimitiveSet(line_p);
		}
	}

	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 8);
	for(int i=0; i<4; i++) {
		(*line_p)[2*i] = i;
		(*line_p)[2*i+1] = i+1;
	}
	border_geometry->addPrimitiveSet(line_p);

	osg::ref_ptr<DrawElementsUInt> poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
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
	osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array;
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
	//playback_waveform_geode->addDrawable(border_geometry);
	//playback_waveform_geode->addDrawable(frame_geometry);
	playback_waveform_geode->addDrawable(baseline_geometry);
	playback_waveform_geode->addDrawable(cursor_geometry);

	//sprintf(name, "some audio element");
	playback_waveform_geode->setUserData(new ACRefId(track_index,"audio track playback waveform"));
	//playback_waveform_geode->setName(name);
	//ref_ptr//playback_waveform_geode->ref();
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
	//ref_ptr//track_geode->ref();
}

void ACOsgAudioTrackRenderer::segmentsGeode() {
	segments_group->removeChildren(0,	segments_group->getNumChildren());

	/*std::vector< osg::ref_ptr<osg::Geode> >::iterator segments_geodes_iter;
	for (segments_geodes_iter=segments_geodes.begin();segments_geodes_iter!=segments_geodes.begin();segments_geodes_iter++)
		(*segments_geodes_iter)=0;*/

	int segments_n = media->getNumberOfSegments();
	StateSet *state;

	float media_length = media->getEnd() - media->getStart();

	for (int s=0;s<segments_n;s++){
		segments_geodes.resize(segments_geodes.size()+1);
		segments_geodes[s] = new Geode;
		TessellationHints *hints = new TessellationHints();
		hints->setDetailRatio(0.0);

		state = segments_geodes[s]->getOrCreateStateSet();
		state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
		state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
		state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
		state->setMode(GL_BLEND, StateAttribute::ON);
		//state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);

		/*
		// AudioRenderer colors
		colors[0] = Vec4(1,1,0.5,1);
		colors[1] = Vec4(1,0.5,1,1);
		colors[2] = Vec4(0.5,1,1,1);
		colors[3] = Vec4(1,0.5,0.5,1);
		colors[4] = Vec4(0.5,1,0.5,1);*/

		Vec4 segment_color;
		if ( (float)s/2.0f != s/2) // odd segment index
			segment_color = Vec4(1,1,0.5,0.5f);//Vec4(0.0f, 0.0f, 1.0f, 1.0f);
		else // even segment index
			segment_color = Vec4(1,0.5,1,0.5f);//Vec4(1.0f, 0.0f, 0.0f, 1.0f);
		osg::ref_ptr<osg::Vec4Array> segment_colors = new Vec4Array;
		segment_colors->push_back(segment_color);

		//segments_geodes[s]->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(-xspan/2.0f+ (media->getSegment(s)->getStart()+ media->getSegment(s)->getEnd())/2.0f*xspan/media_length,0.0f,0.0f) , (media->getSegment(s)->getEnd()-media->getSegment(s)->getStart())*xspan/media_length , yspan , 0.0f), hints));
		segments_geodes[s]->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(-xspan/2.0f+ (media->getSegment(s)->getStart()+ media->getSegment(s)->getEnd())/2.0f*xspan/media_length,segments_center_y,0.0f) , (media->getSegment(s)->getEnd()-media->getSegment(s)->getStart())*xspan/media_length , segments_height*2 , 0.0f), hints));
		//std::cout << "Segment " << s << " start " << media->getSegment(s)->getStart()/media_length << " end " << media->getSegment(s)->getEnd()/media_length << " width " << (media->getSegment(s)->getEnd()-media->getSegment(s)->getStart())/media_length << std::endl;
		((ShapeDrawable*)(segments_geodes[s])->getDrawable(0))->setColor(segment_color);
		segments_geodes[s]->setUserData(new ACRefId(track_index,"video track segments"));
		//ref_ptr//segments_geodes[s]->ref();
		segments_group->addChild(segments_geodes[s]);
	}
	if(segments_n>0){
		//ref_ptr//segments_group->ref();
		segments_transform->addChild(segments_group);
		//ref_ptr//segments_transform->ref();
	}
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
		//track_node->removeChild(summary_cursor_transform);
		track_node->removeChild(summary_waveform_geode);
	}
	if (media_changed || screen_width_changed || selection_begin_pos_changed || selection_end_pos_changed)
	{
		track_node->removeChild(playback_waveform_geode);
	}
	if (media_changed || selection_begin_pos_changed || selection_end_pos_changed)
	{
		track_node->removeChild(selection_begin_transform);
		track_node->removeChild(selection_zone_transform);
		track_node->removeChild(selection_end_transform);
		/*track_node->removeChild(selection_begin_geode);
		 track_node->removeChild(selection_zone_geode);
		 track_node->removeChild(selection_end_geode);*/
		track_node->removeChild(summary_cursor_transform);
		track_node->removeChild(playback_waveform_geode);
	}
	if (media){
		if (media_changed)
		{
			if (samples) delete samples;
			samples = ((ACAudio*) media)->getMonoSamples();
			playback_min_width = screen_width*xspan/(((ACAudio*) media)->getNFrames()); // one frame per pixel

			//CF debug
			std::cout << "Updating audio track with ACAudio of";
			std::cout << " samplerate: " << ((ACAudio*) media)->getSampleRate() << "," ;
			std::cout << " "<<((ACAudio*) media)->getNFrames() <<" frames,";
			std::cout << " start " << ((ACAudio*) media)->getStart() <<",";
			std::cout << " end " << ((ACAudio*) media)->getEnd() << ",";
			std::cout << " from lib " << media_from_lib  << ",";
			std::cout << " " << media->getNumberOfSegments()  << " segments";
			std::cout << std::endl;
			std::cout << "Audio track with ";
			std::cout << " selection_begin_pos " << selection_begin_pos  << ",";
			std::cout << " selection_center_pos " << selection_center_pos  << ",";
			std::cout << " selection_end_pos " << selection_end_pos  << ",";
			std::cout << std::endl;

			//CF: dummy segments for testing
			/*if (media->getNumberOfSegments()==0){
				for (int s=0;s<4;s++){
					ACMedia* seg = ACMediaFactory::getInstance().create(media);
					seg->setParentId(media->getId());
					media->addSegment(seg);//dummy
				}
				float media_start = media->getStart();
				float media_end = media->getEnd();
				media->getSegment(0)->setStart(media_start);
				media->getSegment(0)->setEnd((media_end-media_start)/4.0f);
				media->getSegment(1)->setStart((media_end-media_start)/4.0f);
				media->getSegment(1)->setEnd(3*(media_end-media_start)/8.0f);
				media->getSegment(2)->setStart(3*(media_end-media_start)/8.0f);
				media->getSegment(2)->setEnd((media_end-media_start)/2.0f);
				media->getSegment(3)->setStart((media_end-media_start)/2.0f);
				media->getSegment(3)->setEnd(media_end);
			}*/
			track_node->removeChild(segments_transform);
			
            if (media->getNumberOfSegments()>0){//////CF dangerous if a new media has the same number of segments than the previous one:  && segments_number != media->getNumberOfSegments()){
				/////CF track_node->removeChild(segments_transform);
				//if (frame_n != floor(width/frame_min_width)){
				//double segments_start = getTime();
				std::cout << "Generating segments... ";
				segments_transform->removeChild(segments_group);
				segmentsGeode();
				//std::cout << getTime()-segments_start << " sec." << std::endl;
				segments_number = media->getNumberOfSegments();
			}
			if (media->getNumberOfSegments()>0)
				track_node->addChild(segments_transform);
		}
		if (media_changed || screen_width_changed)
		{
			selectionWaveformGeode();//CF origin of polygon artifacts
			//selectionCursorGeode();
			track_node->addChild(summary_waveform_geode);
			//track_node->addChild(summary_cursor_transform);
		}
		if (media_changed || screen_width_changed || selection_begin_pos_changed || selection_end_pos_changed)
		{
			playbackWaveformGeode();
			track_node->addChild(playback_waveform_geode);
		}
		if (media_changed || selection_begin_pos_changed || selection_end_pos_changed)
		{
			selectionZoneGeode();
			track_node->addChild(selection_zone_transform);
			if (media_changed || selection_begin_pos_changed)
				selectionBeginGeode();
			track_node->addChild(selection_begin_transform);
			if (media_changed || selection_end_pos_changed)
				selectionEndGeode();
			track_node->addChild(selection_end_transform);
			selectionCursorGeode();
			track_node->addChild(summary_cursor_transform);
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
	/*Matrix curserT;
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
