/**
 * @brief The HUD pointer renderer class, implemented with OSG
 * @author Stéphane Dupont
 * @date 16/11/2010
 * @copyright (c) 2010 – UMONS - Numediart
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

#include <osg/Version>
using namespace osg;

ACOsgPointerRenderer::ACOsgPointerRenderer() : ACOsgMediaRenderer()
{
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    text_string = "";
    text_geode = 0;
    pointer_geode = 0;
    pointer_transform = 0;
    font = 0;
    text = 0;
}

ACOsgPointerRenderer::~ACOsgPointerRenderer()
{	
    text_geode=0;
    pointer_geode=0;
    pointer_transform=0;
    font = 0;
    text = 0;
}

void ACOsgPointerRenderer::textGeode()
{
    Vec4 textColor(0.9f,0.9f,0.9f,0.9f);
    float textCharacterSize = 96.0f; // 10 pixels ?
#if OSG_MIN_VERSION_REQUIRED(2,9,11)
    textCharacterSize = 12.0f;
#endif

    text_geode = new Geode();

    text = new osgText::Text;
    text->setColor(textColor);
    text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
    text->setCharacterSize(textCharacterSize);
    //text->setPosition(osg::Vec3(0,0,0));
    text->setPosition(osg::Vec3(0,25,0.0));
    text->setLayout(osgText::Text::LEFT_TO_RIGHT);
#if OSG_MIN_VERSION_REQUIRED(2,9,11)
    text->setFontResolution(12,12);
#else
    text->setFontResolution(64,64);
#endif
    text->setAlignment( osgText::Text::CENTER_CENTER );
    text->setAxisAlignment( osgText::Text::SCREEN );

    text->setText( text_string );
    //text->setText( "POINTER" );

    text_geode->addDrawable(text);
    text_geode->setCullingActive(false);
}

void ACOsgPointerRenderer::pointerGeode()
{
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
    pointer_geode->setCullingActive(false);
}

void ACOsgPointerRenderer::prepareNodes()
{
    if(!hide_information)
        textGeode();
    pointerGeode();

    pointer_transform = new MatrixTransform();
    //ref_ptr//pointer_transform->ref();
    if(!hide_information)
        pointer_transform->addChild(text_geode);
    pointer_transform->addChild(pointer_geode);

    pointer_transform->setCullingActive(false);

    if(media_node->getNumChildren() == 0)
        media_node->addChild(pointer_transform);
}

void ACOsgPointerRenderer::updateNodes(double ratio)
{
    Matrix T;
    Matrix Trotate;

    T.makeTranslate(Vec3(pos.x, pos.y, pos.z));
    /*if(!hide_information)
        text->setText( text_string );*/

    if(pointer_transform)
        pointer_transform->setMatrix(T);
}
