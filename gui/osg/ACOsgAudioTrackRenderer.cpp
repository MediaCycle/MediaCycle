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

#include "ACOsgAudioTrackRenderer.h"

#include <osgGA/GUIEventHandler>

ACOsgAudioTrackRenderer::ACOsgAudioTrackRenderer() {
	
	waveform_geode = 0; curser_geode = 0; curser_transform = 0; track_geode = 0;
	//zoom_x = 10.0; zoom_y = 6.0;
	zoom_x = 1.0; zoom_y = 1.0;
	translate_x = 0.0;
	displayed_media_index = -1;
	// Magic numbers!
	zpos = 0.01f;
	xstep = 0.0005f;
	yspan = 0.65f;
	xspan = 0.65f;
}

ACOsgAudioTrackRenderer::~ACOsgAudioTrackRenderer() {
	// track_node->removeChild(0,1);
	if 	(waveform_geode) {
		waveform_geode->unref();
		waveform_geode=0;
	}
	if 	(curser_geode) {
		curser_geode->unref();
		curser_geode=0;
	}
	if 	(curser_transform) {
		curser_transform->unref();
		curser_transform=0;
	}
	if 	(track_geode) {
		track_geode->unref();
		track_geode=0;
	}
}

void ACOsgAudioTrackRenderer::waveformGeode() {
	
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
	
	waveform_geode = new Geode();
	samples_geometry = new Geometry();
	frame_geometry = new Geometry();
	border_geometry = new Geometry();
	baseline_geometry = new Geometry();

	if (screen_width !=0){	
		// CF: temporary workaround as the ACUserLog tree and the ACLoopAttributes vector in ACMediaBrowser are not sync'd 
		//int media_index = track_index; // or media_cycle->getBrowser()->getMediaTrack(track_index).getMediaId(); 
		if (media_from_lib && media_cycle->getBrowser()->getMode() == AC_MODE_NEIGHBORS)
			media_index = media_cycle->getBrowser()->getUserLog()->getMediaIdFromNodeId(media_index);//CF can waveformGeode() occur more than once, once media_index is set?
		
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
		 float* samples = ((ACAudio*) media)->getMonoSamples();
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
		std::cout << "Number of points of the waveform: " << width << std::endl;
		
		zoom_x = xspan/xstep/width;// CF autosizefit possible only when displaying one track
		translate_x = -((width+1) * xstep * zoom_x)/2; // CF autocenter possible only when displaying one track
			
		// samples vertices
		//bars
		/*
		vertices = new Vec3Array(2*width+2);
		for(i=0; i<width; i++) {
			(*vertices)[2*i] = Vec3(translate_x + i * xstep * zoom_x, yspan/2.0f * thumbnail[2*i] * zoom_y, zpos);
			(*vertices)[2*i+1] = Vec3(translate_x + i * xstep * zoom_x, yspan/2.0f * thumbnail[2*i+1] * zoom_y, zpos);
		}
		(*vertices)[2*i] = Vec3(translate_x + 0.0, 0.0, zpos);
		(*vertices)[2*i+1] = Vec3(translate_x + (i-1) * xstep * zoom_x, 0.0, zpos);
		samples_geometry->setVertexArray(vertices);
		*/

		//envelope
		/*vertices = new Vec3Array(2*width);
		for(i=0; i<width; i++) {
			(*vertices)[i] = Vec3(translate_x + i * xstep * zoom_x, yspan/2.0f * thumbnail[i] * zoom_y, zpos);
			(*vertices)[i+width] = Vec3(translate_x + (width-i-1) * xstep * zoom_x, yspan/2.0f * thumbnail[2*width-i-1] * zoom_y, zpos);
		}
		//(*vertices)[2*width] = Vec3(translate_x + i * xstep * zoom_x, yspan/2.0f * thumbnail[0] * zoom_y, zpos);
		samples_geometry->setVertexArray(vertices);*/
		
		
		//counter clockwise, polygon cull facing test
		/*vertices = new Vec3Array(2*width);
		for(i=0; i<width; i++) {
			(*vertices)[i] = Vec3(translate_x + i * xstep * zoom_x, yspan/2.0f * thumbnail[i+width] * zoom_y, zpos);
			(*vertices)[i+width] = Vec3(translate_x + (width-i-1) * xstep * zoom_x, yspan/2.0f * thumbnail[width-i-1] * zoom_y, zpos);
		}
		//(*vertices)[2*width] = Vec3(translate_x + i * xstep * zoom_x, yspan/2.0f * thumbnail[0] * zoom_y, zpos);
		samples_geometry->setVertexArray(vertices);*/
		
		// envelope with quads
		vertices = new Vec3Array(4*(width-1));
		for(i=0; i<width; i++) {
			(*vertices)[4*i] = Vec3(translate_x + i * xstep * zoom_x, yspan/2.0f * thumbnail[i] * zoom_y, zpos);
			(*vertices)[4*i+1] = Vec3(translate_x + (i+1) * xstep * zoom_x, yspan/2.0f * thumbnail[i+1] * zoom_y, zpos);
			(*vertices)[4*i+2] = Vec3(translate_x + (i+1) * xstep * zoom_x, yspan/2.0f * thumbnail[i+width+1] * zoom_y, zpos);
			(*vertices)[4*i+3] = Vec3(translate_x + i * xstep * zoom_x, yspan/2.0f * thumbnail[i+width] * zoom_y, zpos);
		}
		samples_geometry->setVertexArray(vertices);
	}
	
	//frame vertices
	vertices = new Vec3Array(4);
	(*vertices)[0] = Vec3(translate_x + 0, -yspan/2.0f * zoom_y, zpos);
	(*vertices)[1] = Vec3(translate_x + width * xstep * zoom_x, -yspan/2.0f * zoom_y, zpos);
	(*vertices)[2] = Vec3(translate_x + width * xstep * zoom_x, yspan/2.0f * zoom_y, zpos);
	(*vertices)[3] = Vec3(translate_x + 0, yspan/2.0f * zoom_y, zpos);
	frame_geometry->setVertexArray(vertices);
	
	//border vertices
	vertices = new Vec3Array(5);
	(*vertices)[0] = Vec3(translate_x + 0, -yspan/2.0f * zoom_y, zpos);
	(*vertices)[1] = Vec3(translate_x + width * xstep * zoom_x, (-yspan/2.0f+xstep) * zoom_y, zpos);
	(*vertices)[2] = Vec3(translate_x + width * xstep * zoom_x, (yspan/2.0f-xstep) * zoom_y, zpos);
	(*vertices)[3] = Vec3(translate_x + 0, yspan/2.0f * zoom_y, zpos);
	(*vertices)[4] = Vec3(translate_x + 0, -yspan/2.0f * zoom_y, zpos);
	border_geometry->setVertexArray(vertices);
	
	//baseline vertices
	vertices = new Vec3Array(2);
	(*vertices)[0] = Vec3(translate_x + 0, 0.0f, zpos+0.01f);
	(*vertices)[1] = Vec3(translate_x + width * xstep * zoom_x, 0.0f, zpos+0.005f);
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
	
	state = waveform_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
#if !defined(APPLE_IOS)
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON); //CF not supported by OpenGL ES 2...
#endif	
	
	state = samples_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(1.0));
	
	state = baseline_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(1.0));
	
	waveform_geode->addDrawable(samples_geometry);
	waveform_geode->addDrawable(border_geometry);
	waveform_geode->addDrawable(frame_geometry);
	waveform_geode->addDrawable(baseline_geometry);
	
	//sprintf(name, "some audio element");
	if (media_from_lib)
		waveform_geode->setUserData(new ACRefId(media_index));
	else
		waveform_geode->setUserData(new ACRefId(-1)); //CF?
	//waveform_geode->setName(name);
	waveform_geode->ref();	
}

void ACOsgAudioTrackRenderer::curserGeode() {
	
	float czpos = 0.02;
	
	StateSet *state;
	
	Vec3Array* vertices;
	
	DrawElementsUInt* line_p;
	
	Geometry *curser_geometry;
	
	curser_transform = new MatrixTransform();
	curser_geode = new Geode();
	curser_geometry = new Geometry();
	
	vertices = new Vec3Array(2);
	(*vertices)[0] = Vec3(translate_x + 0, (-yspan/2.0f+xstep) * zoom_y, czpos+0.00005);
	(*vertices)[1] = Vec3(translate_x + 0, (yspan/2.0f-xstep) * zoom_y, czpos+0.00005);	
	curser_geometry->setVertexArray(vertices);
	
	Vec4 curser_color(0.2f, 0.9f, 0.2f, 0.9f);	
	Vec4Array* curser_colors = new Vec4Array;
	curser_colors->push_back(curser_color);		
	curser_geometry->setColorArray(curser_colors);
	curser_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 3);	
	(*line_p)[0] = 0;
	(*line_p)[1] = 1;	
	curser_geometry->addPrimitiveSet(line_p);
	
	state = curser_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(2.0));
	
	state = curser_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
#if !defined(APPLE_IOS)
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON); //CF not supported by OpenGL ES 2...
#endif	
	
	state = curser_transform->getOrCreateStateSet();
	state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	
	curser_geode->addDrawable(curser_geometry);
	
	curser_transform->addChild(curser_geode);
	
	//sprintf(name, "some audio element");
	if (media_from_lib)
		curser_transform->setUserData(new ACRefId(media_index));
	else
		curser_transform->setUserData(new ACRefId(-1));//CF?
	//curser_transform->setName(name);
	curser_transform->ref();
	if (media_from_lib)
		curser_geode->setUserData(new ACRefId(media_index));
	else
		curser_geode->setUserData(new ACRefId(-1));//CF?
	//curser_geode->setName(name);
	curser_geode->ref();
}

void ACOsgAudioTrackRenderer::trackGeode() {
	
	StateSet *state;
	
	track_geode = new Geode();
	
	TessellationHints *hints = new TessellationHints();
	hints->setDetailRatio(0.0);
	
	state = track_geode->getOrCreateStateSet();
	state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);	
	
	track_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),xspan,yspan,0.0), hints)); //draws a square // Vintage AudioCycle
	//track_geode->setName(name);
	track_geode->ref();	
}

void ACOsgAudioTrackRenderer::prepareTracks() {
	waveform_geode = 0;
	curser_transform = 0;
	curser_geode = 0;
	track_geode = 0;
}

void ACOsgAudioTrackRenderer::updateTracks(double ratio) {
	Matrix curserT;
	if (media_changed || screen_width_changed)
	{
		track_node->removeChild(curser_transform);
		track_node->removeChild(waveform_geode);
		if (media){
			waveformGeode();
			curserGeode();
			track_node->addChild(waveform_geode);
			track_node->addChild(curser_transform);
			displayed_media_index = media_index;
			media_changed = false;
		}	
	}
	//CF solve this for media outside lib
	if (media_from_lib) 
	{
		if (media_index > -1)
		{
			const ACMediaNode &attribute = media_cycle->getMediaNode(media_index);
			if ( attribute.getActivity()==1) 
			{	
				curserT.makeTranslate(Vec3(attribute.getCursor() * xstep/2.0f * screen_width/media->getThumbnailWidth(), 0.0f, 0.0f));			// curserT =  Matrix::scale(0.5/zoom,0.5/zoom,0.5/zoom) * curserT;
				curser_transform->setMatrix(curserT);
			}
		}	
	}
}
