/*
 *  ACOsgLabelRenderer.cpp
 *  MediaCycle
 *
 *  @author Stéphane Dupont
 *  @date 02/11/09
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

#include "ACOsgLabelRenderer.h"

using namespace osg;

ACOsgLabelRenderer::ACOsgLabelRenderer() {
	media_type = MEDIA_TYPE_TEXT;
	text_geode = 0;
	text_string = "";
	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
}

ACOsgLabelRenderer::~ACOsgLabelRenderer() {
	
	if 	(text_geode) { //ref_ptr//text_geode->unref();
		text_geode=0; }
}

void ACOsgLabelRenderer::textGeode() {
	
//	StateSet *state;
	
	osg::Vec4 textColor(0.9f,0.9f,0.9f,0.9f);
	float textCharacterSize = 10.0f; // 10 pixels ? 
	
	text_geode = new Geode();

	text = new osgText::Text;
	//font = osgText::readFontFile("fonts/arial.ttf");
	//text->setFont( font.get() );
	text->setColor(textColor);
	text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
	text->setCharacterSize(textCharacterSize);
	text->setPosition(osg::Vec3(0,0,0.1));
	//	text->setPosition(osg::Vec3(pos.x,pos.y,pos.z));
	text->setLayout(osgText::Text::LEFT_TO_RIGHT);
	text->setFontResolution(64,64);
	text->setAlignment( osgText::Text::CENTER_CENTER );
	text->setAxisAlignment( osgText::Text::SCREEN );
	
	// SD TODO - Find text from media library
	text->setText( text_string );
	
	//state = text_geode->getOrCreateStateSet();
	//state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
	//state->setMode(GL_BLEND, StateAttribute::ON);
	//state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);

	//TODO check this .get() (see also ACOsgBrowserRenderer.cpp)
	//".get()" is necessary for compilation under linux (OSG v2.4)
	text_geode->addDrawable(text.get());
	
	//ref_ptr//text_geode->ref();	
}

void ACOsgLabelRenderer::prepareNodes() {
		
	text_geode = 0;
	
	textGeode();
	
	media_node->addChild(text_geode);
}

void ACOsgLabelRenderer::updateNodes(double ratio) {
		
	float x, y, z;
	
	Matrix T;
	Matrix Trotate;
	
	osg::Vec4 textColor(0.9f,0.9f,0.9f,0.9f);
	//((ShapeDrawable*)text_geode->getDrawable(0))->setColor(textColor);
	
	x = pos.x;
	y = pos.y;
	z = pos.z;
	
	T.makeTranslate(Vec3(x, y, z));
	text->setText( text_string );

#ifdef AUTO_TRANSFORM
	media_node->setPosition(Vec3(x,y,z));
#else
	media_node->setMatrix(T);
#endif

}
