/*
 *  ACTextOsgRendererPlugin.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 14/10/12
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

#include "ACTextOsgRendererPlugin.h"
#include "ACOsgTextRenderer.h"
#include "ACOsgTextTrackRenderer.h"

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

ACTextOsgRendererPlugin::ACTextOsgRendererPlugin() : ACOsgRendererPlugin(){
    this->mName = "Text Renderer (OSG)";
    this->mDescription ="Plugin for rendering Text files with OpenSceneGraph";
    this->mMediaType = MEDIA_TYPE_TEXT;
}

ACTextOsgRendererPlugin::~ACTextOsgRendererPlugin(){
}

std::map<std::string,ACMediaType> ACTextOsgRendererPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string,ACMediaType> extensions;
    extensions[".txt"] = MEDIA_TYPE_TEXT;
    extensions[".xml"] = MEDIA_TYPE_TEXT;
    return extensions;
}

std::vector<ACMediaType> ACTextOsgRendererPlugin::getSupportedMediaTypes(){
    std::vector<ACMediaType> media_types;
    media_types.push_back(MEDIA_TYPE_TEXT);
    return media_types;
}

ACOsgMediaRenderer* ACTextOsgRendererPlugin::createMediaRenderer(ACMediaType media_type){
    ACOsgMediaRenderer* renderer = 0;
    if(media_type == MEDIA_TYPE_TEXT)
        renderer = new ACOsgTextRenderer();
    return renderer;
}

ACOsgTrackRenderer* ACTextOsgRendererPlugin::createTrackRenderer(ACMediaType media_type){
    ACOsgTrackRenderer* renderer = 0;
    if(media_type == MEDIA_TYPE_TEXT)
        renderer = new ACOsgTextTrackRenderer();
    return renderer;
}
