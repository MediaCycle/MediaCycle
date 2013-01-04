/*
 *  ACArchipelReaderPlugin.cpp
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

#include "ACArchipelReaderPlugin.h"

#include "ACArchipelMediaDocument.h"
#include "ACArchipelText.h"

ACArchipelReaderPlugin::ACArchipelReaderPlugin()
    : ACMediaReaderPlugin(), ACMediaRendererPlugin()
{
    this->mMediaType = MEDIA_TYPE_MIXED;
    this->mName = "Archipel Reader";
    this->mDescription = "Archipel Reader plugin";
    this->mId = "";
}


ACMedia* ACArchipelReaderPlugin::mediaFactory(ACMediaType mediaType, const ACMedia* media){
    if (mediaType&MEDIA_TYPE_MIXED)
        return new ACArchipelMediaDocument();
    else if (mediaType&MEDIA_TYPE_TEXT)
        return new ACArchipelText();
    else {
        return 0;
    }
}

ACMediaData* ACArchipelReaderPlugin::mediaReader(ACMediaType mediaType){
    ACMediaData* media_data(0);
    if(mediaType&MEDIA_TYPE_TEXT){
        media_data = new ACArchipelTextData();
    }
    return media_data;
}

std::map<std::string, ACMediaType> ACArchipelReaderPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string,ACMediaType> extensions;
    /*if(media_type == MEDIA_TYPE_MIXED || media_type == MEDIA_TYPE_ALL){
        extensions[".xml"] = MEDIA_TYPE_MIXED;
    }
    else if(media_type == MEDIA_TYPE_TEXT){
        extensions[".xml"] = MEDIA_TYPE_TEXT;
    }*/
    extensions[".xml"] = MEDIA_TYPE_MIXED|MEDIA_TYPE_TEXT;
    return extensions;
}
