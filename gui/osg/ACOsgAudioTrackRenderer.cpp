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

ACOsgAudioTrackRenderer::ACOsgAudioTrackRenderer() {

	waveform_geode = 0; curser_geode = 0; curser_transform = 0; track_geode = 0;
	//zoom_x = 10.0; zoom_y = 6.0;
	zoom_x = 1.0; zoom_y = 1.0;
	translate_x = 0.0;
	displayed_media_index = -1;
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
	float zpos = 0.01;
	double xstep = 0.0005, ylim = 0.25;//0.025
	
	int width;
	float *thumbnail;
	
	StateSet *state;
	
	Vec3Array* vertices;	
	DrawElementsUInt* line_p;
	
	Geometry *samples_geometry;
	Geometry *frame_geometry;
	Geometry *border_geometry;
	
	waveform_geode = new Geode();
	samples_geometry = new Geometry();
	frame_geometry = new Geometry();
	border_geometry = new Geometry();
	
	// CF: temporary workaround as the ACUserLog tree and the ACLoopAttributes vector in ACMediaBrowser are not sync'd 
	//int media_index = track_index; // or media_cycle->getBrowser()->getMediaTrack(track_index).getMediaId(); 
	if (media_from_lib && media_cycle->getBrowser()->getMode() == AC_MODE_NEIGHBORS)
		media_index = media_cycle->getBrowser()->getUserLog()->getMediaIdFromNodeId(media_index);//CF can waveformGeode() occur more than once, once media_index is set?
	
	if (media_from_lib)
		width = media_cycle->getThumbnailWidth(media_index);//CF instead of track_index
	else
		width = media->getThumbnailWidth(); //CF media should be non-null
	
	std::cout << "Number of points of the waveform: " << width << std::endl;
	width = width / 2;
	
	zoom_x = 5.0*240.0/width;// CF autosizefit possible only when displaying one track
	translate_x = -(width * xstep * zoom_x)/2; // CF autocenter possible only when displaying one track

	if (media_from_lib)
		thumbnail = (float*)media_cycle->getThumbnailPtr(media_index);//CF instead of track_index
	else
		thumbnail = (float*)media->getThumbnailPtr();
		
	// samples vertices
	vertices = new Vec3Array(2*width+2);
	for(i=0; i<width; i++) {
		(*vertices)[2*i] = Vec3(translate_x + i * xstep * zoom_x, ylim * thumbnail[2*i] * zoom_y, zpos);
		(*vertices)[2*i+1] = Vec3(translate_x + i * xstep * zoom_x, ylim * thumbnail[2*i+1] * zoom_y, zpos);
	}
	(*vertices)[2*i] = Vec3(translate_x + 0.0, 0.0, zpos);
	(*vertices)[2*i+1] = Vec3(translate_x + (i-1) * xstep * zoom_x, 0.0, zpos);
	samples_geometry->setVertexArray(vertices);
	
	//frame vertices
	vertices = new Vec3Array(4);
	(*vertices)[0] = Vec3(translate_x + 0, -ylim * zoom_y, zpos);
	(*vertices)[1] = Vec3(translate_x + width * xstep * zoom_x, -ylim * zoom_y, zpos);
	(*vertices)[2] = Vec3(translate_x + width * xstep * zoom_x, ylim * zoom_y, zpos);
	(*vertices)[3] = Vec3(translate_x + 0, ylim * zoom_y, zpos);
	frame_geometry->setVertexArray(vertices);

	//border vertices
	vertices = new Vec3Array(5);
	(*vertices)[0] = Vec3(translate_x + 0, -ylim * zoom_y, zpos);
	(*vertices)[1] = Vec3(translate_x + width * xstep * zoom_x, (-ylim+xstep) * zoom_y, zpos);
	(*vertices)[2] = Vec3(translate_x + width * xstep * zoom_x, (ylim-xstep) * zoom_y, zpos);
	(*vertices)[3] = Vec3(translate_x + 0, ylim * zoom_y, zpos);
	(*vertices)[4] = Vec3(translate_x + 0, -ylim * zoom_y, zpos);
	border_geometry->setVertexArray(vertices);
	
	Vec4 color(0.9f, 0.9f, 0.9f, 0.9f);	
	Vec4Array* colors = new Vec4Array;
	colors->push_back(color);

	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 2*width+2);
	for(i=0; i<width+1; i++) {
		(*line_p)[2*i] = 2*i;
		(*line_p)[2*i+1] = 2*i+1;
	}
	samples_geometry->addPrimitiveSet(line_p);
	
	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 8);	
	for(i=0; i<4; i++) {
		(*line_p)[2*i] = i;
		(*line_p)[2*i+1] = i+1;
	}
	border_geometry->addPrimitiveSet(line_p); // CF, bounding box temporarily disabled
	
	DrawElementsUInt *poly = new DrawElementsUInt(PrimitiveSet::QUADS, 4);
	poly->push_back(0);
	poly->push_back(1);
	poly->push_back(2);
	poly->push_back(3);
	frame_geometry->addPrimitiveSet(poly);
	
	samples_geometry->setColorArray(colors);
	samples_geometry->setColorBinding(Geometry::BIND_OVERALL);
	border_geometry->setColorArray(colors);
	border_geometry->setColorBinding(Geometry::BIND_OVERALL);
	
	colors = new Vec4Array(1);
	(*colors)[0] = Vec4(0.0, 0.0, 0.0, 0.3);
	frame_geometry->setColorArray(colors);
	frame_geometry->setColorBinding(Geometry::BIND_OVERALL);

	state = waveform_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
		
	state = samples_geometry->getOrCreateStateSet();
	state->setAttribute(new LineWidth(1.0));
	
	waveform_geode->addDrawable(samples_geometry);
	waveform_geode->addDrawable(border_geometry);
	waveform_geode->addDrawable(frame_geometry);
	
	//sprintf(name, "some audio element");
	if (media_from_lib)
		waveform_geode->setUserData(new ACRefId(media_index));
	else
		waveform_geode->setUserData(new ACRefId(-1)); //CF?
	//waveform_geode->setName(name);
	waveform_geode->ref();	
}

void ACOsgAudioTrackRenderer::curserGeode() {
	
	float zpos = 0.02; //CF sphere hack instead of 0.02
	double xstep = 0.0005, ylim = 0.25;//0.025
	
	StateSet *state;
	
	Vec3Array* vertices;
	
	DrawElementsUInt* line_p;
	
	Geometry *curser_geometry;
	
	curser_transform = new MatrixTransform();
	curser_geode = new Geode();
	curser_geometry = new Geometry();

	vertices = new Vec3Array(2);
	(*vertices)[0] = Vec3(translate_x + 0, (-ylim+xstep) * zoom_y, zpos+0.00005);
	(*vertices)[1] = Vec3(translate_x + 0, (ylim-xstep) * zoom_y, zpos+0.00005);	
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
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
	
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

	//track_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),0.65,0.65,0.0), hints)); //draws a square // Vintage AudioCycle
	track_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),0.6,0.5,0.0), hints)); //draws a square // Vintage AudioCycle
	//sprintf(name, "some audio element");
	//track_geode->setUserData(new ACRefId(media_index));
	//track_geode->setName(name);
	track_geode->ref();	
}

void ACOsgAudioTrackRenderer::prepareTracks() {
		
	waveform_geode = 0;
	curser_transform = 0;
	curser_geode = 0;
	track_geode = 0;
	
	trackGeode();
	track_node->addChild(track_geode);
	((ShapeDrawable*)track_geode->getDrawable(0))->setColor(Vec4(0.2,0.2,0.4,1.0));
	std::cout << "track_node_addChild(track_geode)" << std::endl;
	
}

void ACOsgAudioTrackRenderer::updateTracks(double ratio) {
	double xstep = 0.00025;
/*	
	ACPoint p,p2;//CF dummy
	p.x = p.y = p.z = p2.x = p2.y = p2.z = 0.0f; // CF dummy

	double omr = 1.0-ratio;
				
	omr = 1;
	ratio = 0;
		
	float zoom = 1.0f;
	float angle = 0.0f;

	Matrix T;
	Matrix Trotate;
	Matrix curserT;
	Matrix waveT;

	float x, y, z;
	float localscale;
	float maxdistance = 0.2;
	float maxscale = 1.5;
	float minscale = 0.33;				
	// Apply "rotation" to compensate camera rotation
	x = omr*p.x + ratio*p2.x;
	y = omr*p.y + ratio*p2.y;
	z = 0;
	T.makeTranslate(Vec3(x, y, z));
	localscale = maxscale - distance_mouse * (maxscale - minscale) / maxdistance ;
	localscale = max(localscale,minscale);

 	localscale = 1.0;//CF
	*/	
	
	Matrix curserT;
	
	if (media_from_lib) 
	{
		if (media_index > -1) {//CF dummy
			//localscale = 1.0;//CF
			
			//CF: if we have updated the media from the library
			if (media_changed || displayed_media_index != media_index)//CF try to remove the 2nd arg
			{
				track_node->removeChild(curser_transform);
				track_node->removeChild(waveform_geode);
				waveformGeode();
				curserGeode();
				track_node->addChild(waveform_geode);
				track_node->addChild(curser_transform);
				displayed_media_index = media_index;
				media_changed = false;
			}
			
			const ACMediaNode &attribute = media_cycle->getMediaNode(media_index);
			if ( attribute.getActivity()==1) 
			{	
				curserT.makeTranslate(Vec3(attribute.getCursor() * xstep * zoom_x, 0.0, 0.0));			// curserT =  Matrix::scale(0.5/zoom,0.5/zoom,0.5/zoom) * curserT;
				curser_transform->setMatrix(curserT);
			}	
		}
	}	
	else
	{
		if (media_changed)
		{
			std::cout << "Media change outside lib" << std::endl;
			track_node->removeChild(curser_transform);
			track_node->removeChild(waveform_geode);
			waveformGeode();
			curserGeode();
			track_node->addChild(waveform_geode);
			track_node->addChild(curser_transform);
			media_changed = false;		
		}
	}	
	//track_node->setMatrix(T);
}
