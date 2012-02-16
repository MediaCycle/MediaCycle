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

ACOsgNodeLinkRenderer::ACOsgNodeLinkRenderer(){
	link_node = new MatrixTransform();
	link_geode = 0;
    node_in = 0;
    node_out = 0;
    link_color = Vec4(1,1,0.5,1); //CF seminal yellow
    media_cycle = 0;
}

ACOsgNodeLinkRenderer::~ACOsgNodeLinkRenderer(){
    link_node = 0;
    link_geode = 0;
    node_in = 0;
    node_out = 0;
    media_cycle = 0;
}

void ACOsgNodeLinkRenderer::setMediaCycle(MediaCycle *_media_cycle)
{
    this->media_cycle = _media_cycle;
}

osg::ref_ptr<osg::MatrixTransform> ACOsgNodeLinkRenderer::getLink()
{
    return link_node
}

void ACOsgNodeLinkRenderer::setNodeIn(ACOsgMediaRenderer* _node)
{
    node_in = _node;
}

void ACOsgNodeLinkRenderer::setNodeOut(ACOsgMediaRenderer* _node)
{
    node_out = _node;
}

void ACOsgNodeLinkRenderer::linkGeode(double to_x, double to_y) {	
	int i;
	float zpos = 0;

	StateSet *state;
	Vec3Array* vertices;	
	osg::ref_ptr<DrawElementsUInt> line_p;
	osg::ref_ptr<Geometry> link_geometry;
	
	link_geode = new Geode();
	link_geometry = new Geometry();

	//link vertices
	vertices = new Vec3Array(2);
	(*vertices)[0] = Vec3(0, 0, zpos);
	(*vertices)[1] = Vec3(to_x, to_y, zpos);
	link_geometry->setVertexArray(vertices);
	
	osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array;
    colors->push_back(link_color);

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
}

void ACOsgNodeLinkRenderer::prepareLinks() {
	link_geode = 0;
}

void ACOsgNodeLinkRenderer::updateLinks(){
    if ( node_in && node_out && node_in->getIsDisplayed() && node_out->getIsDisplayed()){
        Matrix T;
        float x, y, z;
        x=node_in->getViewPos().x;
        y=node_in->getViewPos().y;
        z = 0;
        T.makeTranslate(Vec3(x, y, z));
        linkGeode( node_out->getViewPos().x-node_in->getViewPos().x,node_out->getViewPos().y-node_in->getViewPos().y);
        link_node->addChild(link_geode);
        link_node->setMatrix(T);
    }
}
