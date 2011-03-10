/*
 *  ACOsgNodeLinkRenderer.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 16/03/2010
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

#include "ACOsgNodeLinkRenderer.h"
using namespace osg;

ACOsgNodeLinkRenderer::ACOsgNodeLinkRenderer() {
	link_node = new MatrixTransform();
	link_geode = 0;
	node_index = -1;
	distance_mouse = 0.0f;
}

ACOsgNodeLinkRenderer::~ACOsgNodeLinkRenderer() {
	link_node = new MatrixTransform();
	if 	(link_geode) {
		//ref_ptr//link_geode->unref();
		link_geode=0;
	}
}

void ACOsgNodeLinkRenderer::linkGeode(double to_x, double to_y) {
	
	int i;
	float zpos = 0;
//	double xstep = 0.0005, ylim = 0.025;

//	int width = 4;
	
	StateSet *state;
	
	Vec3Array* vertices;	
	DrawElementsUInt* line_p;
	
	Geometry *link_geometry;
	
	link_geode = new Geode();
	link_geometry = new Geometry();

	//link vertices
	vertices = new Vec3Array(2);
	(*vertices)[0] = Vec3(0, 0, zpos);
	(*vertices)[1] = Vec3(to_x, to_y, zpos);
	link_geometry->setVertexArray(vertices);
	
	Vec4 color (1,1,0.5,1);
	osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array;
	colors->push_back(color);

	line_p = new DrawElementsUInt(PrimitiveSet::LINES, 2);	
	for(i=0; i<1; i++) {
		(*line_p)[2*i] = i;
		(*line_p)[2*i+1] = i+1;
	}

	link_geometry->addPrimitiveSet(line_p);
	link_geometry->setColorArray(colors);
	link_geometry->setColorBinding(Geometry::BIND_OVERALL);

	state = link_geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	state->setMode(GL_BLEND, StateAttribute::ON);
	#if !defined (APPLE_IOS)
	state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);
	#endif//CF APPLE_IOS
	state->setAttribute(new LineWidth(0.5));

	link_geode->addDrawable(link_geometry);
	//ref_ptr//link_geode->ref();	
}

void ACOsgNodeLinkRenderer::prepareLinks() {
		
	link_geode = 0;
}

void ACOsgNodeLinkRenderer::updateLinks(double ratio) {
	
//	double xstep = 0.00025;
	
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
		colors_ready = true;
	}
	
	const ACMediaNode &attribute = media_cycle->getMediaNode(node_index);

	// SD TODO - rather than recomputing the geometry, use matrix transforms instead
	if (link_node->getNumChildren() == 1) {
		if (link_geode) {
			link_node->removeChild(0,1);
			//ref_ptr//link_geode->unref();
		}
	}
	
	if ( attribute.isDisplayed() ){
		
		const ACPoint &p = attribute.getCurrentPosition(), &p2 = attribute.getNextPosition();
		double omr = 1.0-ratio;
				
		//omr = 1;
		//ratio = 0;
		
		Matrix T;
		/*Matrix Trotate;
		Matrix curserT;*/

		float x, y, z;
		// Apply "rotation" to compensate camera rotation
		x = omr*p.x + ratio*p2.x;
		y = omr*p.y + ratio*p2.y;
		z = 0;
		T.makeTranslate(Vec3(x, y, z));

		if (media_cycle->getBrowser()->getUserLog()->getParentFromNodeId(node_index) != -1 ) {
			int parentId = media_cycle->getBrowser()->getUserLog()->getParentFromNodeId(node_index);
			const ACMediaNode &to_attribute = media_cycle->getMediaNode( parentId );
			const ACPoint &to_p = to_attribute.getCurrentPosition();
			const ACPoint &to_p2 = to_attribute.getNextPosition();
			float to_x,to_y;
			to_x = omr*to_p.x + ratio*to_p2.x;
			to_y = omr*to_p.y + ratio*to_p2.y;
			if ( (x != 0) || ( y!=0 ) ) //CF prevents false early nodelinks with new nodes, requires testing with animation
			{
				linkGeode(to_x-x,to_y-y);
				link_node->addChild(link_geode);
			}	
	 	}	
		link_node->setMatrix(T);
	}
}
