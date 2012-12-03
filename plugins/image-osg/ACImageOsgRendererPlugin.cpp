/*
 *  ACImageOsgRendererPlugin.cpp
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

#include "ACImageOsgRendererPlugin.h"
#include "ACOsgImageRenderer.h"

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

ACImageOsgRendererPlugin::ACImageOsgRendererPlugin() : ACOsgRendererPlugin(){
    this->mName = "Image Renderer (OSG)";
    this->mDescription ="Plugin for rendering Image files with OpenSceneGraph";
    this->mMediaType = MEDIA_TYPE_IMAGE;
}

ACImageOsgRendererPlugin::~ACImageOsgRendererPlugin(){
}

std::map<std::string,ACMediaType> ACImageOsgRendererPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string,ACMediaType> extensions;
    if(media_type == MEDIA_TYPE_IMAGE || media_type == MEDIA_TYPE_ALL){
#ifdef PARSE_FORMATS_DYNAMICALLY
#ifdef PARSE_FORMATS_VERBOSE
        std::cout << "Gathering media file extensions from OSG plugins..." << std::endl;
#endif//def PARSE_FORMATS_VERBOSE

        // The osgdb_ffmpeg plugin uses thread locking since 3.0.1
        // To avoid crashes, OSG plugins should be queried through the osgBD registry thread, so that its instance initiates the lock for osgdb_ffmpeg

        // Open all OSG plugins
        osgDB::FileNameList plugins = osgDB::listAllAvailablePlugins();
        for(osgDB::FileNameList::iterator itr = plugins.begin();itr != plugins.end();++itr)
        {
            bool library_loaded = osgDB::Registry::instance()->loadLibrary(*itr);
#ifdef PARSE_FORMATS_VERBOSE
            if(library_loaded)
                std::cout<<"-- opening plugin "<<*itr<<std::endl;
#endif//def PARSE_FORMATS_VERBOSE;
        }

        // Get all registered readers/writers from the loaded OSG plugins and check the extensions they provide
        for(osgDB::Registry::ReaderWriterList::iterator rw_itr = osgDB::Registry::instance()->getReaderWriterList().begin();
            rw_itr != osgDB::Registry::instance()->getReaderWriterList().end();
            ++rw_itr)
        {
            osgDB::ReaderWriter::FormatDescriptionMap rwfdm = (*rw_itr)->supportedExtensions();
            osgDB::ReaderWriter::FormatDescriptionMap::iterator fdm_itr;
            for(fdm_itr = rwfdm.begin();fdm_itr != rwfdm.end();++fdm_itr)
            {
#ifdef PARSE_FORMATS_VERBOSE
                std::cout<<"-- adding extension: ."<<fdm_itr->first<<" ("<<fdm_itr->second<<")"<<std::endl;
#endif//def PARSE_FORMATS_VERBOSE
                std::string ext = std::string(".") + fdm_itr->first;
                //extensions[ext] = MEDIA_TYPE_IMAGE;
            }
        }

        // Close all OSG plugins
        osgDB::Registry::instance()->closeAllLibraries();

        // Verify which readers/writers haven't been closed:
#ifdef PARSE_FORMATS_VERBOSE
        for(osgDB::Registry::ReaderWriterList::iterator rw_itr = osgDB::Registry::instance()->getReaderWriterList().begin();
            rw_itr != osgDB::Registry::instance()->getReaderWriterList().end();
            ++rw_itr)
            std::cout<<"-- plugin still open: "<<(*rw_itr)->className()<<std::endl;
#endif//def PARSE_FORMATS_VERBOSE;

#ifdef PARSE_FORMATS_VERBOSE
        std::cout << "Gathering media file extensions from OSG plugins... done" << std::endl;
#endif//def PARSE_FORMATS_VERBOSE
#else // PARSE_FORMATS_DYNAMICALLY

        // from http://www.openscenegraph.org/projects/osg/wiki/Support/UserGuides/Plugins
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
#endif
    }
    return extensions;
}

std::vector<ACMediaType> ACImageOsgRendererPlugin::getSupportedMediaTypes(){
    std::vector<ACMediaType> media_types;
    media_types.push_back(MEDIA_TYPE_IMAGE);
    return media_types;
}

ACOsgMediaRenderer* ACImageOsgRendererPlugin::createMediaRenderer(ACMediaType media_type){
    ACOsgMediaRenderer* renderer = 0;
    if(media_type == MEDIA_TYPE_IMAGE)
        renderer = new ACOsgImageRenderer();
    return renderer;
}
