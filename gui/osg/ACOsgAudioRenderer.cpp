/*
 *  ACOsgBrowserRenderer.cpp
 *  MediaCycle
 *
 *  @author Stéphane Dupont
 *  @date 24/08/09
 *
 *  @copyright (c) 2009 – UMONS - Numediart
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

#include "ACOsgAudioRenderer.h"

ACOsgAudioRenderer::ACOsgAudioRenderer() {

	waveform_geode = 0; curser_geode = 0; curser_transform = 0; entry_geode = 0;
}

ACOsgAudioRenderer::~ACOsgAudioRenderer() {
	// media_node->removeChild(0,1);
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
	if 	(entry_geode) {
		entry_geode->unref();
		entry_geode=0;
	}
}

void ACOsgAudioRenderer::waveformGeode() {
	
	int i;
	float zpos = 0.04; //CF sphere hack instead of 0.02 for boxes
	double xstep = 0.0005, ylim = 0.025;

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
		
	width = media_cycle->getThumbnailWidth(node_index);
	width = width / 2;
	thumbnail = (float*)media_cycle->getThumbnailPtr(node_index);

	// samples vertices
	vertices = new Vec3Array(2*width+2);
	for(i=0; i<width; i++) {
		(*vertices)[2*i] = Vec3(i * xstep, ylim * thumbnail[2*i], zpos);
		(*vertices)[2*i+1] = Vec3(i * xstep, ylim * thumbnail[2*i+1], zpos);
	}
	(*vertices)[2*i] = Vec3(0.0, 0.0, zpos);
	(*vertices)[2*i+1] = Vec3((i-1) * xstep, 0.0, zpos);
	samples_geometry->setVertexArray(vertices);
	
	//frame vertices
	vertices = new Vec3Array(4);
	(*vertices)[0] = Vec3(0, -ylim, zpos);
	(*vertices)[1] = Vec3(width * xstep, -ylim, zpos);
	(*vertices)[2] = Vec3(width * xstep, ylim, zpos);
	(*vertices)[3] = Vec3(0, ylim, zpos);
	frame_geometry->setVertexArray(vertices);

	//border vertices
	vertices = new Vec3Array(5);
	(*vertices)[0] = Vec3(0, -ylim, zpos);
	(*vertices)[1] = Vec3(width * xstep, -ylim+xstep, zpos);
	(*vertices)[2] = Vec3(width * xstep, ylim-xstep, zpos);
	(*vertices)[3] = Vec3(0, ylim, zpos);
	(*vertices)[4] = Vec3(0, -ylim, zpos);
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
	waveform_geode->setUserData(new ACRefId(node_index));
	//waveform_geode->setName(name);
	waveform_geode->ref();	
}

void ACOsgAudioRenderer::curserGeode() {
	
	float zpos = 0.04; //CF sphere hack instead of 0.02
	double xstep = 0.0005, ylim = 0.025;
	
	StateSet *state;
	
	Vec3Array* vertices;
	
	DrawElementsUInt* line_p;
	
	Geometry *curser_geometry;
	
	curser_transform = new MatrixTransform();
	curser_geode = new Geode();
	curser_geometry = new Geometry();

	vertices = new Vec3Array(2);
	(*vertices)[0] = Vec3(0, -ylim+xstep, zpos+0.00005);
	(*vertices)[1] = Vec3(0, ylim-xstep, zpos+0.00005);	
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
	curser_transform->setUserData(new ACRefId(node_index));
	//curser_transform->setName(name);
	curser_transform->ref();
	curser_geode->setUserData(new ACRefId(node_index));
	//curser_geode->setName(name);
	curser_geode->ref();
}

void ACOsgAudioRenderer::entryGeode() {
	
	StateSet *state;
	
	entry_geode = new Geode();

	TessellationHints *hints = new TessellationHints();
	hints->setDetailRatio(0.0);
	
	state = entry_geode->getOrCreateStateSet();
	state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);	

	//entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f,0.0f,0.0f),0.01), hints)); //draws a square // Vintage AudioCycle
	entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),0.01), hints)); // draws a sphere // MultiMediaCycle
	//entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.0f,0.0f,0.0f),0.01, 0.0f), hints)); // draws a disc
	//entry_geode->addDrawable(new osg::ShapeDrawable(new osg::Capsule(osg::Vec3(0.0f,0.0f,0.0f),0.01, 0.005f), hints)); // draws a sphere
	//sprintf(name, "some audio element");
	entry_geode->setUserData(new ACRefId(node_index));
	//entry_geode->setName(name);
	entry_geode->ref();	
}

void ACOsgAudioRenderer::prepareNodes() {
		
	waveform_geode = 0;
	curser_transform = 0;
	curser_geode = 0;
	entry_geode = 0;
	
	//waveformGeode();
	//curserGeode();
	if  (media_cycle->getMediaNode(node_index).isDisplayed()){
		entryGeode();
		media_node->addChild(entry_geode);
	}	
}

void ACOsgAudioRenderer::updateNodes(double ratio) {
	
	double xstep = 0.00025;
	
#define NCOLORS 5
	static Vec4 colors[NCOLORS];
	static bool colors_ready = false;
	
	if(!colors_ready)
	{
		colors[0] = Vec4(1,1,0.5,1);
		colors[1] = Vec4(1,0.5,1,1);
		colors[2] = Vec4(0.5,1,1,1);
		colors[3] = Vec4(1,0.5,0.5,1);
		colors[4] = Vec4(0.5,1,0.5,1);
	}
	
	const ACMediaNode &attribute = media_cycle->getMediaNode(node_index);

	if ( attribute.isDisplayed() ){
		const ACPoint &p = attribute.getCurrentPosition(), &p2 = attribute.getNextPosition();
		double omr = 1.0-ratio;
				
		omr = 1;
		ratio = 0;
		
		float zoom = media_cycle->getCameraZoom();
		float angle = media_cycle->getCameraRotation();

		Matrix T;
		Matrix Trotate;
		Matrix curserT;

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
		// localscale = 0.5;
		
		if (attribute.getActivity()==1) {	// with waveform
		//if (0) {	// without waveform
			localscale = 0.5;
			
			if(waveform_geode == 0) {
				waveformGeode();
			}
			if (curser_transform == 0) {
				curserGeode();
			}
			
			if(media_node->getNumChildren() == 1 && media_node->getChild(0) == entry_geode) {
				media_node->setChild(0, waveform_geode);
				media_node->addChild(curser_transform);
			}
			
			// curserT.makeTranslate(Vec3(omr*p.x + ratio*p2.x + attribute.curser * xstep * 0.5 / zoom, omr*p.y + ratio*p2.y, 0.0)); // omr*p.z + ratio*p2.z));
			// curserT =  Matrix::scale(0.5/zoom,0.5/zoom,0.5/zoom) * curserT;
			curserT.makeTranslate(Vec3(attribute.getCursor() * xstep, 0.0, 0.0)); 
			curser_transform->setMatrix(curserT);
		
			T =  Matrix::rotate(-angle,Vec3(0.0,0.0,1.0)) * Matrix::scale(localscale/zoom,localscale/zoom,localscale/zoom) * T;
		}
		else {
			if(media_node->getNumChildren() == 2) {
				media_node->setChild(0, entry_geode);
				media_node->removeChild(1, 1);
			}
					
			((ShapeDrawable*)entry_geode->getDrawable(0))->setColor(colors[attribute.getClusterId()%NCOLORS]);
			
			T =  Matrix::rotate(-angle,Vec3(0.0,0.0,1.0)) * Matrix::scale(localscale/zoom,localscale/zoom,localscale/zoom) * T;
		}
				
		unsigned int mask = (unsigned int)-1;
		if(attribute.getNavigationLevel() >= media_cycle->getNavigationLevel()) {
			entry_geode->setNodeMask(mask);
		}
		else {
			entry_geode->setNodeMask(0);
		}
		
		media_node->setMatrix(T);
	}	
}
