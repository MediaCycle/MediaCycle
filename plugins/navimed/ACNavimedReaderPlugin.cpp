/*
 *  ACNavimedReaderPlugin.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 27/07/11
 *  @copyright (c) 2011 – UMONS - Numediart
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

#include "ACNavimedReaderPlugin.h"
#include "ACNavimedText.h"
#include "ACNavimedMediaDocument.h"

ACNavimedReaderPlugin::ACNavimedReaderPlugin() : ACMediaReaderPlugin(){

    this->mMediaType = MEDIA_TYPE_MIXED&MEDIA_TYPE_TEXT;
    this->mName = "Navimed Reader";
    this->mDescription = "Navimed Reader plugin";
    this->mId = "";
}

ACMedia* ACNavimedReaderPlugin::mediaFactory(ACMediaType mediaType, const ACMedia* media){
    //if (mediaType&this->mMediaType)
    //    return new ACNavimedMediaDocument();
    if(mediaType&MEDIA_TYPE_TEXT)
        return new ACNavimedText();
    else if(mediaType&MEDIA_TYPE_MIXED)
        return new ACNavimedMediaDocument();
    else {
        return 0;
    }
}

ACMediaData* ACNavimedReaderPlugin::mediaReader(ACMediaType mediaType){
    ACMediaData* media_data(0);
    if(mediaType&MEDIA_TYPE_TEXT){
        media_data = new ACNavimedTextData();
    }
    return media_data;
}

std::map<std::string, ACMediaType> ACNavimedReaderPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string,ACMediaType> extensions;
    if(media_type == MEDIA_TYPE_TEXT || media_type == MEDIA_TYPE_ALL){
        extensions[".xml"] = MEDIA_TYPE_TEXT;
    }
    if(media_type == MEDIA_TYPE_MIXED || media_type == MEDIA_TYPE_ALL){
        extensions[".xml"] = MEDIA_TYPE_MIXED;
    }
    return extensions;
}
