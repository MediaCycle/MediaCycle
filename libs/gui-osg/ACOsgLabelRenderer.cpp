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

ACOsgLabelRenderer::ACOsgLabelRenderer():ACOsgMediaRenderer() {
    media_type = MEDIA_TYPE_TEXT;
    text_geode = 0;
    text=0;
    text_string = "";
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
}

ACOsgLabelRenderer::~ACOsgLabelRenderer() {
    text_geode=0;
    text=0;
}

void ACOsgLabelRenderer::textGeode() {

    //	StateSet *state;

    osg::Vec4 textColor(0.9f,0.9f,0.9f,0.9f);
    float textCharacterSize = 24.0f; // 10 pixels ?

    text_geode = new Geode();
    text = new osgText::Text;
    if(font)
        text->setFont(font);
    text->setColor(textColor);
    text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
    text->setCharacterSize(textCharacterSize);
    text->setPosition(osg::Vec3(0,0,0));
    //	text->setPosition(osg::Vec3(pos.x,pos.y,pos.z));
    text->setLayout(osgText::Text::LEFT_TO_RIGHT);
    text->setFontResolution(textCharacterSize,textCharacterSize);
    text->setAlignment( osgText::Text::CENTER_CENTER );
    text->setAxisAlignment( osgText::Text::SCREEN );

    text->setText( text_string );

    //state = text_geode->getOrCreateStateSet();
    //state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
    //state->setMode(GL_BLEND, StateAttribute::ON);
    //state->setMode(GL_LINE_SMOOTH, StateAttribute::ON);

    text_geode->addDrawable(text);
}

void ACOsgLabelRenderer::prepareNodes() {
    if(!media_cycle) return;
    if(!media_cycle->getBrowser()) return;

    if(text_geode)
        media_node->removeChild(text_geode);
    text=0;
    text_geode = 0;
    textGeode();
}

void ACOsgLabelRenderer::updateNodes(double ratio) {
    if(!media_cycle) return;
    if(!media_cycle->getBrowser()) return;

    if(media_cycle->getBrowser()->isLabelDisplayed(this->node_index) ){
        if( media_node->getNumChildren() == 0){
            media_node->addChild(text_geode);
        }
    }
    else{
        if(media_node->getNumChildren() > 0){
            media_node->removeChildren(0,media_node->getNumChildren());
        }
    }

    if(media_cycle->getBrowser()->isLabelDisplayed(this->node_index)){
        float x, y, z;
        if(media_cycle && text){
            if(media_cycle->getBrowserMode() == AC_MODE_CLUSTERS){
                if(cluster_colors.size()>0)
                    text->setColor(cluster_colors[this->node_index%cluster_colors.size()]);
                else
                    text->setColor(node_color);
            }
        }
        if (user_defined_color && text)
            text->setColor(node_color);
        x = pos.x;
        y = pos.y;
        z = pos.z;
#ifdef AUTO_TRANSFORM
        media_node->setPosition(Vec3(x,y,z));
#else
        Matrix T;
        Matrix Trotate;
        T.makeTranslate(Vec3(x, y, z));
        media_node->setMatrix(T);
#endif
    }

}
