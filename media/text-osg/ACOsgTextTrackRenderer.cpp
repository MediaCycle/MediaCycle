/**
 * @brief The text timeline track renderer class, implemented with OSG
 * @authors Christian Frisson, Thierry Ravet
 * @date 19/06/2012
 * @copyright (c) 2012 – UMONS - Numediart
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

#include "ACOsgTextTrackRenderer.h"
#include <ACText.h>
#include "ACTextSTLData.h"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include <sstream>
#include <osg/Version>
#include <textFile.h>
#include <osg/Notify>

namespace fs = boost::filesystem;

using namespace osg;

ACOsgTextTrackRenderer::ACOsgTextTrackRenderer()
    :ACOsgTrackRenderer()
{
    media_type = MEDIA_TYPE_TEXT;
    text_geode = 0;
    text = 0;
    text_transform = 0;
    selection_slider_transform = 0;
    selection_zone_transform = 0;
    isSliderVisible = false;
    slider_width = 12; //px
}

ACOsgTextTrackRenderer::~ACOsgTextTrackRenderer() {
    text_geode = 0;
    text = 0;
    text_transform = 0;
    selection_slider_transform = 0;
    selection_zone_transform = 0;
}

void ACOsgTextTrackRenderer::textGeode() {

    text_transform = 0;
    text_transform = new osg::MatrixTransform();
    osg::Vec4 textColor(0.9f,0.9f,0.9f,1.0f);
    float textCharacterSize = 14.0f;
    text_geode = new Geode();
    text = new osgText::Text;
    if(font)
        text->setFont(font);
    text->setColor(textColor);
    text->setCharacterSize(textCharacterSize);
    text->setPosition(osg::Vec3(0.0f,0.0f,0.0f));
    text->setLayout(osgText::Text::LEFT_TO_RIGHT);
    text->setFontResolution(textCharacterSize,textCharacterSize);
    text->setAlignment( osgText::Text::LEFT_TOP);
    
    if(media){
        //utf8_string = textFileRead(media->getFileName());//TR to replace to adapt to Navimed and archipel
        //utf8_string=media->getTextMetaData();
        //utf8_string=string("test\ntest\ntest\ntest\ntest\ntest\ntest\ntest");
        if(!media->getMediaData()){
            std::cerr << "ACOsgTextTrackRenderer::textGeode: no media data available" << std::endl;
            return;
        }
        if(!media->getMediaData()->getData()){
            std::cerr << "ACOsgTextTrackRenderer::textGeode: no media data container available" << std::endl;
            return;
        }
        ACTextSTLDataContainer* text_data = dynamic_cast<ACTextSTLDataContainer*>(media->getMediaData()->getData());
        if(!text_data){
            std::cerr << "ACOsgTextTrackRenderer::textGeode: no text data available" << std::endl;
            return;
        }
        utf8_string=(*((string*)text_data->getData()));
        if(utf8_string!=std::string(""))
            text->setText(utf8_string,osgText::String::ENCODING_UTF8);
        else
            std::cerr << "ACOsgTextTrackRenderer::textGeode: text content empty" << std::endl;
    }
    std::cout<<text->getText().createUTF8EncodedString()<<std::endl;
    text->setMaximumWidth(this->width);
    bool test=text_geode->addDrawable(text);
    test=text_transform->addChild(text_geode);
    text_geode->setUserData(new ACRefId(track_index,"track text"));
}

void ACOsgTextTrackRenderer::prepareTracks() {
    text_transform = 0;
    text_geode = 0;
    text=0;
    width_changed = true;

    if(!selection_slider_transform){
        this->boxTransform(selection_slider_transform, slider_width, osg::Vec4(0.0f, 0.0f, 0.0f, 0.4f), "track summary slider");
        track_node->addChild(selection_slider_transform);
    }
    if(!selection_zone_transform){
        this->boxTransform(selection_zone_transform, slider_width, osg::Vec4(0.2f, 0.9f, 0.2f, 0.9f), "track selection zone");
        track_node->addChild(selection_zone_transform);
    }
}

void ACOsgTextTrackRenderer::updateTracks(double ratio) {
    if (media_changed)
    {
        if(text_transform)
            track_node->removeChild(text_transform);
        if(selection_slider_transform)
            track_node->removeChild(selection_slider_transform);
        if(selection_zone_transform)
            track_node->removeChild(selection_zone_transform);
        selection_center_pos_y = 1;
        if(media){
            text_transform = 0;
            text_geode = 0;
            text=0;
            textGeode();
        }
        std::cout<<"test2:"<<text->getText().createUTF8EncodedString()<<std::endl;
        
        track_node->addChild(text_transform);
    }

    if (width_changed || height_changed || selection_center_pos_changed || media_changed)
    {
        if(text){
            float zone_height = height/((text->getLineCount())*text->getCharacterHeight());
            if( zone_height >= 1 ){
                if(isSliderVisible){
                    track_node->removeChild(selection_slider_transform);
                    track_node->removeChild(selection_zone_transform);
                    isSliderVisible = false;
                    text->setMaximumWidth(this->width);
                }
            }
            else{
                if(!isSliderVisible || media_changed){
                    track_node->addChild(selection_slider_transform);
                    track_node->addChild(selection_zone_transform);
                    isSliderVisible = true;
                    text->setMaximumWidth(this->width-2*slider_width);
                }
            }

            if(selection_center_pos_y > 1)
                selection_center_pos_y = 1;
            if(selection_center_pos_y < 0)
                selection_center_pos_y = 0;

            Matrix selection_slider_matrix,selection_zone_matrix,text_matrix;
            selection_slider_matrix.makeTranslate(this->width - slider_width,this->height/2.0f,0.0f);
            selection_slider_matrix = Matrix::scale(1.0f,this->height,1.0f)*selection_slider_matrix;
            selection_slider_transform->setMatrix(selection_slider_matrix);
            selection_zone_matrix.makeTranslate(this->width - slider_width, (1-zone_height/2.0f + selection_center_pos_y-1 )*this->height,0.0f);
            selection_zone_matrix = Matrix::scale(1.0f, zone_height*this->height ,1.0f)*selection_zone_matrix;
            selection_zone_transform->setMatrix(selection_zone_matrix);
            text_matrix.makeTranslate(0.0f,(2-selection_center_pos_y)*this->height,0.0f);
            text_transform->setMatrix(text_matrix);
        }
    }

    // Reset "signals"
    width_changed = false;
    media_changed = false;
    selection_begin_pos_changed = false;
    selection_end_pos_changed = false;
    selection_center_pos_changed = false;
}
