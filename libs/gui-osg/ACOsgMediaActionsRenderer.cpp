/**
 * @brief A class that displays the last media action performed.
 * @author Christian Frisson
 * @date 05/05/2013
 * @copyright (c) 2013 – UMONS - Numediart
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

#include "ACOsgMediaActionsRenderer.h"
#include <osg/Version>
using namespace osg;

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

ACOsgMediaActionsRenderer::ACOsgMediaActionsRenderer()
    :ACOsgMediaRenderer(),ACEventListener()
{
    width = 0;
    height = 0;
    this->changeSetting(this->setting);
    refresh = 2;
    last_time = 0;
}

void ACOsgMediaActionsRenderer::init()
{
    node_color = Vec4(0.4f, 0.4f, 0.4f, 1.0f);

    media_action_renderer.caption = "";

    media_action_node = new MatrixTransform();
    media_node->addChild(media_action_node);
}

void ACOsgMediaActionsRenderer::changeSetting(ACSettingType _setting)
{
    // Init setting
    if(_setting == AC_SETTING_INSTALLATION){
        max_side_size = 128; //pixels
        font_size = 32; // pixels
        line_sep = font_size; //pixels
    }
    else{
        max_side_size = 64; //pixels
        font_size = 16; // pixels
        line_sep = font_size; //pixels
    }
    this->init();

    // Change setting
    if(this->setting == _setting)
        return;
    this->setting = _setting;

    //this->updateNodes();
}

ACOsgMediaActionsRenderer::~ACOsgMediaActionsRenderer() {
    font = 0;
}

void ACOsgMediaActionsRenderer::textGeode(std::string _string, osg::ref_ptr<osgText::Text>& _text, osg::ref_ptr<osg::Geode>& _geode,osg::Vec3 pos)
{
    _geode = 0;
    Vec4 textColor(0.9f,0.9f,0.9f,0.9f);
    _geode = new Geode();
    _text = new osgText::Text;
    _text->setColor(textColor);
    //_text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
    _text->setCharacterSize(font_size);
    //_text->setPosition(osg::Vec3(50,25,0.0));
    _text->setPosition(pos);
    _text->setLayout(osgText::Text::LEFT_TO_RIGHT);
    //#if OSG_MIN_VERSION_REQUIRED(3,0,0)
    if(font)
        _text->setFont(font);
    _text->setFontResolution(font_size,font_size);
    //#else
    //    _text->setFontResolution(12,12);
    //#endif
    _text->setAlignment( osgText::Text::LEFT_BASE_LINE );
    _text->setAxisAlignment( osgText::Text::SCREEN );

    osgText::String osg_string = osgText::String(_string,osgText::String::ENCODING_SIGNATURE);
    std::string utf8_string = osg_string.createUTF8EncodedString();

    _text->setText( utf8_string,osgText::String::ENCODING_UTF8 );
    _geode->addDrawable(_text);
    //_geode->setCullingActive(false);
}

void ACOsgMediaActionsRenderer::prepareNodes() {
    //medias_caption = ACMediaFactory::getInstance().getNormalCaseStringFromMediaType(this->media_cycle->getMediaType()) + "s";
}

void ACOsgMediaActionsRenderer::updateTextRenderer(ACOsgLibraryTextRenderer& _renderer, std::string _value, osg::ref_ptr<osg::MatrixTransform> _node)
{
    //if(_renderer.value!=_value){
    _renderer.value=_value;
    if(_renderer.geode)
        _node->removeChild(_renderer.geode);
    textGeode(_renderer.caption+_renderer.value,_renderer.text,_renderer.geode,osg::Vec3(_renderer.x,_renderer.y,0.0));
    _node->addChild(_renderer.geode);
    //}
}

void ACOsgMediaActionsRenderer::updateNodes(double ratio) {
    if(media_cycle && media_cycle->getLibrarySize()>0){
        if(getTime() > refresh + last_time)
            this->updateTextRenderer(media_action_renderer,"",media_action_node);
        else
            this->updateTextRenderer(media_action_renderer,media_action_renderer.value,media_action_node);
    }
    //CF be careful with positionning, overlapping text geodes explode CPU usage
    float media_action_x,media_action_y,media_action_z;
    media_action_x = max_side_size/2;
    media_action_y = 3*line_sep-max_side_size/2;
    media_action_z = 0.0f;

#ifdef AUTO_TRANSFORM
    media_action_node->setPosition(Vec3(media_action_x,media_action_y,media_action_z));
    //media_action_node->setRotation(Quat(0.0, 0.0, 1.0, -media_cycle_angle));
    //media_action_node->setScale(Vec3(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom));
#else

    Matrix media_action_matrix;
    media_action_matrix.makeTranslate(Vec3(media_action_x, media_action_y, media_action_z)); // omr*p.z + ratio*p2.z));
    /*T =  Matrix::rotate(-media_cycle_angle,Vec3(0.0,0.0,1.0))
   * Matrix::scale(localscale/media_cycle_zoom,localscale/media_cycle_zoom,localscale/media_cycle_zoom)
                        * T;*/
    media_action_node->setMatrix(media_action_matrix);

#endif //AUTO_TRANSFORM
}

void ACOsgMediaActionsRenderer::mediaActionPerformed(std::string action, long int mediaId, std::vector<boost::any> arguments){
    std::stringstream text;
    text << "Action " << action << " on media " << mediaId;
    if(arguments.size()==1)
        text << " argument ";
    else if(arguments.size()==1)
        text << " arguments ";
    for(std::vector<boost::any>::iterator argument = arguments.begin(); argument != arguments.end(); argument++){
        std::string new_value = "";
        try{
            new_value = boost::any_cast<std::string>(*argument);
            text << " " << new_value;
        }
        catch(const boost::bad_any_cast &){
            //std::cerr << "ACOsgMediaActionsRenderer::mediaActionPerformed: couldn't convert to string, aborting..."<< std::endl;
            float float_arg (0.0f);
            try{
                float_arg = boost::any_cast<float>(*argument);
                text << " " << float_arg;
            }
            catch(const boost::bad_any_cast &){
                //std::cerr << "ACOsgMediaActionsRenderer::mediaActionPerformed: couldn't convert to float, aborting..."<< std::endl;
                int int_arg (-1);
                try{
                    int_arg = boost::any_cast<int>(*argument);
                    text << " " << int_arg;
                }
                catch(const boost::bad_any_cast &){
                    //std::cerr << "ACOsgMediaActionsRenderer::mediaActionPerformed: couldn't convert to int, aborting..."<< std::endl;
                }
            }
        }
    }
    media_action_renderer.value = text.str();
    //this->updateTextRenderer(media_action_renderer,text.str(),media_action_node);
    last_time = getTime();
}

void ACOsgMediaActionsRenderer::updateSize(int w, int h){
    this->width = w;
    this->height = h;
}

void ACOsgMediaActionsRenderer::setMediaCycle(MediaCycle *_media_cycle)
{
    this->media_cycle = _media_cycle;
    if(this->media_cycle)
        this->media_cycle->addListener(this);
}

