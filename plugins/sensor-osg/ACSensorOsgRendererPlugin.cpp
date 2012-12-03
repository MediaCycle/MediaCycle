/*
 *  ACSensorOsgRendererPlugin.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 04/11/12
 *  @copyright (c) 2012 – UMONS - Numediart
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

#include "ACSensorOsgRendererPlugin.h"
#include "ACOsgSensorRenderer.h"

#include <iostream>

#include <osgDB/Registry>
#include <osgDB/ReaderWriter>
#include <osgDB/FileNameUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/PluginQuery>

using namespace std;

// uncomment this to parse formats dynamically
//#define PARSE_FORMATS_DYNAMICALLY

// uncomment this to parse formats in verbose mode
//#define PARSE_FORMATS_VERBOSE

ACSensorOsgRendererPlugin::ACSensorOsgRendererPlugin() : ACOsgRendererPlugin(){
    this->mName = "Sensor Renderer (OSG)";
    this->mDescription ="Plugin for rendering Sensor files with OpenSceneGraph";
    this->mMediaType = MEDIA_TYPE_SENSOR;
}

ACSensorOsgRendererPlugin::~ACSensorOsgRendererPlugin(){
}

std::map<std::string,ACMediaType> ACSensorOsgRendererPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string, ACMediaType> extensions;
    return extensions;
}

std::vector<ACMediaType> ACSensorOsgRendererPlugin::getSupportedMediaTypes(){
    std::vector<ACMediaType> media_types;
    media_types.push_back(MEDIA_TYPE_SENSOR);
    return media_types;
}

ACOsgMediaRenderer* ACSensorOsgRendererPlugin::createMediaRenderer(ACMediaType media_type){
    ACOsgMediaRenderer* renderer = 0;
    if(media_type == MEDIA_TYPE_SENSOR)
        renderer = new ACOsgSensorRenderer();
    return renderer;
}
