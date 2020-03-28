/**
 * @brief A plugin that provides media and media data instances to parse and read images using OpenCV.
 * @author Christian Frisson
 * @date 12/03/2015
 * @copyright (c) 2015 – UMONS - Numediart
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

#include "ACImageOpenCVReaderPlugin.h"
#include "ACImage.h"
#include "ACImageOpenCVData.h"
#include <iostream>

using namespace std;

// uncomment this to parse formats dynamically
//#define PARSE_FORMATS_DYNAMICALLY

// uncomment this to parse formats in verbose mode
//#define PARSE_FORMATS_VERBOSE

ACImageOpenCVReaderPlugin::ACImageOpenCVReaderPlugin() : ACMediaReaderPlugin(){
    this->mName = "Image Reader (OpenCV)";
    this->mDescription ="Plugin for reading image files with OpenCV";
    this->mMediaType = MEDIA_TYPE_IMAGE;
}

ACImageOpenCVReaderPlugin::~ACImageOpenCVReaderPlugin(){
}

ACMedia* ACImageOpenCVReaderPlugin::mediaFactory(ACMediaType mediaType, const ACMedia* copy){
    ACMedia* media(0);
    if(mediaType&MEDIA_TYPE_IMAGE){
        if(copy)
            media = new ACImage((ACImage&)(*copy));
        else
            media = new ACImage();
    }
    return media;
}

ACMediaData* ACImageOpenCVReaderPlugin::mediaReader(ACMediaType mediaType){
    ACMediaData* media_data(0);
    if(mediaType&MEDIA_TYPE_IMAGE){
        media_data = new ACImageOpenCVData();
    }
    return media_data;
}

std::map<std::string,ACMediaType> ACImageOpenCVReaderPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string,ACMediaType> extensions;
    if(media_type == MEDIA_TYPE_IMAGE || media_type == MEDIA_TYPE_ALL){
#ifdef PARSE_FORMATS_DYNAMICALLY
        std::cerr << "Parsing available formats from OpenCV dynamically is not implemented" << std::endl;
#endif
        // from http://www.openscenegraph.org/projects/Osg/wiki/Support/UserGuides/Plugins
        extensions[".bmp"] = MEDIA_TYPE_IMAGE;
        extensions[".bmpf"] = MEDIA_TYPE_IMAGE;
        extensions[".bw"] = MEDIA_TYPE_IMAGE;
        extensions[".cr2"] = MEDIA_TYPE_IMAGE;
        extensions[".crw"] = MEDIA_TYPE_IMAGE;
        extensions[".cur"] = MEDIA_TYPE_IMAGE;
        extensions[".dcr"] = MEDIA_TYPE_IMAGE;
        extensions[".dds"] = MEDIA_TYPE_IMAGE;
        extensions[".dng"] = MEDIA_TYPE_IMAGE;
        extensions[".epi"] = MEDIA_TYPE_IMAGE;
        extensions[".eps"] = MEDIA_TYPE_IMAGE;
        extensions[".epsf"] = MEDIA_TYPE_IMAGE;
        extensions[".epsi"] = MEDIA_TYPE_IMAGE;
        extensions[".exr"] = MEDIA_TYPE_IMAGE;
        extensions[".fpx"] = MEDIA_TYPE_IMAGE;
        extensions[".fpxi"] = MEDIA_TYPE_IMAGE;
        extensions[".gif"] = MEDIA_TYPE_IMAGE;
        extensions[".hdr"] = MEDIA_TYPE_IMAGE;
        extensions[".icns"] = MEDIA_TYPE_IMAGE;
        extensions[".ico"] = MEDIA_TYPE_IMAGE;
        extensions[".int"] = MEDIA_TYPE_IMAGE;
        extensions[".inta"] = MEDIA_TYPE_IMAGE;
        extensions[".jp2"] = MEDIA_TYPE_IMAGE;
        extensions[".jpc"] = MEDIA_TYPE_IMAGE;
        extensions[".jpe"] = MEDIA_TYPE_IMAGE;
        extensions[".jpeg"] = MEDIA_TYPE_IMAGE;
        extensions[".jpg"] = MEDIA_TYPE_IMAGE;
        extensions[".jps"] = MEDIA_TYPE_IMAGE;
        extensions[".mac"] = MEDIA_TYPE_IMAGE;
        extensions[".mrw"] = MEDIA_TYPE_IMAGE;
        extensions[".nef"] = MEDIA_TYPE_IMAGE;
        extensions[".orf"] = MEDIA_TYPE_IMAGE;
        extensions[".pct"] = MEDIA_TYPE_IMAGE;
        extensions[".pic"] = MEDIA_TYPE_IMAGE;
        extensions[".pict"] = MEDIA_TYPE_IMAGE;
        extensions[".pbm"] = MEDIA_TYPE_IMAGE;
        extensions[".pgm"] = MEDIA_TYPE_IMAGE;
        extensions[".png"] = MEDIA_TYPE_IMAGE;
        extensions[".pnm"] = MEDIA_TYPE_IMAGE;
        extensions[".pnt"] = MEDIA_TYPE_IMAGE;
        extensions[".ppm"] = MEDIA_TYPE_IMAGE;
        extensions[".psd"] = MEDIA_TYPE_IMAGE;
        extensions[".ptng"] = MEDIA_TYPE_IMAGE;
        extensions[".qti"] = MEDIA_TYPE_IMAGE;
        extensions[".qtif"] = MEDIA_TYPE_IMAGE;
        extensions[".raf"] = MEDIA_TYPE_IMAGE;
        extensions[".raw"] = MEDIA_TYPE_IMAGE;
        extensions[".rgb"] = MEDIA_TYPE_IMAGE;
        extensions[".rgba"] = MEDIA_TYPE_IMAGE;
        extensions[".sgi"] = MEDIA_TYPE_IMAGE;
        extensions[".srf"] = MEDIA_TYPE_IMAGE;
        extensions[".targa"] = MEDIA_TYPE_IMAGE;
        extensions[".tga"] = MEDIA_TYPE_IMAGE;
        extensions[".tif"] = MEDIA_TYPE_IMAGE;
        extensions[".tiff"] = MEDIA_TYPE_IMAGE;
        extensions[".xbm"] = MEDIA_TYPE_IMAGE;
    }
    return extensions;
}
