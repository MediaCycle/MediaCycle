/**
 * @brief ACOsgPointerRenderer.cpp
 * @author Christian Frisson
 * @date 24/02/2011
 * @copyright (c) 2011 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
*/

#include "ACOsgPointerRenderer.h"

using namespace osg;

ACOsgPointerRenderer::ACOsgPointerRenderer() {
	
	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	text_string = "";
	text_geode = 0;
	pointer_geode = 0;
	pointer_transform = 0;
}

ACOsgPointerRenderer::~ACOsgPointerRenderer() {
	
	if 	(text_geode) { //ref_ptr//text_geode->unref();
		text_geode=0; }
	if 	(pointer_geode) { //ref_ptr//pointer_geode->unref(); 
		pointer_geode=0; }
	if  (pointer_transform) { //ref_ptr//pointer_transform->unref(); 
		pointer_transform=0; }
}

void ACOsgPointerRenderer::textGeode() {
	
	Vec4 textColor(0.9f,0.9f,0.9f,0.9f);
	float textCharacterSize = 96.0f; // 10 pixels ? 
	
	text_geode = new Geode();
	//ref_ptr//text_geode->ref();	
	
	text = new osgText::Text;
	text->setColor(textColor);
	text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
	text->setCharacterSize(textCharacterSize);
	text->setPosition(osg::Vec3(0,0,0));
	text->setLayout(osgText::Text::LEFT_TO_RIGHT);
	text->setFontResolution(64,64);
	text->setAlignment( osgText::Text::CENTER_CENTER );
	text->setAxisAlignment( osgText::Text::SCREEN );
	
	text->setText( text_string );
	//text->setText( "POINTER" );
	
	text_geode->addDrawable(text.get());
}

void ACOsgPointerRenderer::pointerGeode() {
	
	Vec4 color(1.0f, 1.0f, 1.0f, 0.33f);	
	osg::ref_ptr<osg::Vec4Array> colors = new Vec4Array;
	colors->push_back(color);		
	
	StateSet *state;

	pointer_geode = new Geode();
	//ref_ptr//pointer_geode->ref();	

	state = pointer_geode->getOrCreateStateSet();
	state->setMode(GL_NORMALIZE, osg::StateAttribute::ON);	
	state->setMode(GL_BLEND, StateAttribute::ON);
	
	osg::Shape* capsule = new osg::Capsule(osg::Vec3(0.0f, 0.0f, 0.0f), 15.0f, 1.0f);
	osg::ShapeDrawable* capsuleDrawable = new osg::ShapeDrawable(capsule);
	capsuleDrawable->setColor(color);
	
	pointer_geode->addDrawable(capsuleDrawable);
}

void ACOsgPointerRenderer::prepareNodes() {

	textGeode();
	pointerGeode();
	
	pointer_transform = new MatrixTransform();
	//ref_ptr//pointer_transform->ref();
	pointer_transform->addChild(text_geode);
	pointer_transform->addChild(pointer_geode);
	
	media_node->addChild(pointer_transform);
}

void ACOsgPointerRenderer::updateNodes(double ratio) {
		
	Matrix T;
	Matrix Trotate;
		
	T.makeTranslate(Vec3(pos.x, pos.y, pos.z));
	text->setText( text_string );
	
	pointer_transform->setMatrix(T);	
}
