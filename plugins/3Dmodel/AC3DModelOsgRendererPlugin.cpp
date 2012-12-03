/*
 *  AC3DModelOsgRendererPlugin.cpp
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

#include "AC3DModelOsgRendererPlugin.h"
#include "ACOsg3DModelRenderer.h"

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

AC3DModelOsgRendererPlugin::AC3DModelOsgRendererPlugin() : ACOsgRendererPlugin(){
    this->mName = "3Dmodel Renderer (OSG)";
    this->mDescription ="Plugin for rendering 3DModel files with OpenSceneGraph";
    this->mMediaType = MEDIA_TYPE_3DMODEL;
}

AC3DModelOsgRendererPlugin::~AC3DModelOsgRendererPlugin(){
}

std::map<std::string,ACMediaType> AC3DModelOsgRendererPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string, ACMediaType> extensions;
    if(media_type == MEDIA_TYPE_3DMODEL || media_type == MEDIA_TYPE_ALL){
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
                //extensions[ext] = MEDIA_TYPE_3DMODEL;
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
        extensions[".3dc"] = MEDIA_TYPE_3DMODEL;
        extensions[".3ds"] = MEDIA_TYPE_3DMODEL;
        extensions[".ac"] = MEDIA_TYPE_3DMODEL;
        extensions[".asc"] = MEDIA_TYPE_3DMODEL;
        extensions[".bsp"] = MEDIA_TYPE_3DMODEL;
        extensions[".dae"] = MEDIA_TYPE_3DMODEL;
        extensions[".dw"] = MEDIA_TYPE_3DMODEL;
        extensions[".dxf"] = MEDIA_TYPE_3DMODEL;
        extensions[".fbx"] = MEDIA_TYPE_3DMODEL;
        extensions[".flt"] = MEDIA_TYPE_3DMODEL;
        extensions[".geo"] = MEDIA_TYPE_3DMODEL;
        extensions[".gem"] = MEDIA_TYPE_3DMODEL;
        extensions[".iv"] = MEDIA_TYPE_3DMODEL;
        extensions[".ive"] = MEDIA_TYPE_3DMODEL;
        extensions[".logo"] = MEDIA_TYPE_3DMODEL;
        extensions[".lw"] = MEDIA_TYPE_3DMODEL;
        extensions[".lwo"] = MEDIA_TYPE_3DMODEL;
        extensions[".lws"] = MEDIA_TYPE_3DMODEL;
        extensions[".md2"] = MEDIA_TYPE_3DMODEL;
        extensions[".obj"] = MEDIA_TYPE_3DMODEL;
        extensions[".ogr"] = MEDIA_TYPE_3DMODEL;
        extensions[".osg"] = MEDIA_TYPE_3DMODEL;
        extensions[".shp"] = MEDIA_TYPE_3DMODEL;
        extensions[".sta"] = MEDIA_TYPE_3DMODEL;
        extensions[".stl"] = MEDIA_TYPE_3DMODEL;
        extensions[".wrl"] = MEDIA_TYPE_3DMODEL;
        extensions[".x"] = MEDIA_TYPE_3DMODEL;
#endif
    }
    return extensions;
}

std::vector<ACMediaType> AC3DModelOsgRendererPlugin::getSupportedMediaTypes(){
    std::vector<ACMediaType> media_types;
    media_types.push_back(MEDIA_TYPE_3DMODEL);
    return media_types;
}

ACOsgMediaRenderer* AC3DModelOsgRendererPlugin::createMediaRenderer(ACMediaType media_type){
    ACOsgMediaRenderer* renderer = 0;
    if(media_type == MEDIA_TYPE_3DMODEL)
        renderer = new ACOsg3DModelRenderer();
    return renderer;
}
