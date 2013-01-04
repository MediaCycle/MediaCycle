/*
 *  ACSensorReaderPlugin.cpp
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

#include "ACSensorReaderPlugin.h"
#include "ACSensor.h"
#include <iostream>

using namespace std;

ACSensorReaderPlugin::ACSensorReaderPlugin() : ACMediaReaderPlugin(){
    this->mName = "Sensor Reader";
    this->mDescription ="Plugin for reading sensor files";
    this->mMediaType = MEDIA_TYPE_SENSOR;
}

ACSensorReaderPlugin::~ACSensorReaderPlugin(){
}

ACMedia* ACSensorReaderPlugin::mediaFactory(ACMediaType mediaType, const ACMedia* copy){
    ACMedia* media(0);
    if(mediaType&MEDIA_TYPE_SENSOR){
        if(copy)
            media = new ACSensor((ACSensor&)(*copy));
        else
            media = new ACSensor();
    }
    return media;
}

ACMediaData* ACSensorReaderPlugin::mediaReader(ACMediaType mediaType){
    ACMediaData* media_data(0);
    if(mediaType&MEDIA_TYPE_SENSOR){
        media_data = new ACSensorData();
    }
    return media_data;
}

std::map<std::string, ACMediaType> ACSensorReaderPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string, ACMediaType> extensions;
    if(media_type == MEDIA_TYPE_SENSOR || media_type == MEDIA_TYPE_ALL){
        extensions[".xml"] = MEDIA_TYPE_SENSOR;
    }
    return extensions;
}

