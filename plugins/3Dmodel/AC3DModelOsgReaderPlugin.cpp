/*
 *  AC3DModelOsgReaderPlugin.cpp
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

#include "AC3DModelOsgReaderPlugin.h"
#include "AC3DModel.h"
#include <iostream>

#include <OsgDB/Registry>
#include <OsgDB/ReaderWriter>
#include <OsgDB/FileNameUtils>
#include <OsgDB/ReaderWriter>
#include <OsgDB/PluginQuery>

using namespace std;

// uncomment this to parse formats dynamically
//#define PARSE_FORMATS_DYNAMICALLY

// uncomment this to parse formats in verbose mode
//#define PARSE_FORMATS_VERBOSE

AC3DModelOsgReaderPlugin::AC3DModelOsgReaderPlugin() : ACMediaReaderPlugin(){
    this->mName = "3DModel Reader (OSG)";
    this->mDescription ="Plugin for reading 3D model files with OpenSceneGraph";
    this->mMediaType = MEDIA_TYPE_3DMODEL;
}

AC3DModelOsgReaderPlugin::~AC3DModelOsgReaderPlugin(){
}

ACMedia* AC3DModelOsgReaderPlugin::mediaFactory(ACMediaType mediaType, const ACMedia* copy){
    ACMedia* media(0);
    if(mediaType&MEDIA_TYPE_3DMODEL){
        if(copy)
            media = new AC3DModel((AC3DModel&)(*copy));
        else
            media = new AC3DModel();
    }
    return media;
}

std::map<std::string, ACMediaType> AC3DModelOsgReaderPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string, ACMediaType> extensions;
    if(media_type == MEDIA_TYPE_3DMODEL || media_type == MEDIA_TYPE_ALL){
#ifdef PARSE_FORMATS_DYNAMICALLY
#ifdef PARSE_FORMATS_VERBOSE
        std::cout << "Gathering media file extensions from Osg plugins..." << std::endl;
#endif//def PARSE_FORMATS_VERBOSE

        // The Osgdb_ffmpeg plugin uses thread locking since 3.0.1
        // To avoid crashes, Osg plugins should be queried through the OsgBD registry thread, so that its instance initiates the lock for Osgdb_ffmpeg

        // Open all Osg plugins
        OsgDB::FileNameList plugins = OsgDB::listAllAvailablePlugins();
        for(OsgDB::FileNameList::iterator itr = plugins.begin();itr != plugins.end();++itr)
        {
            bool library_loaded = OsgDB::Registry::instance()->loadLibrary(*itr);
#ifdef PARSE_FORMATS_VERBOSE
            if(library_loaded)
                std::cout<<"-- opening plugin "<<*itr<<std::endl;
#endif//def PARSE_FORMATS_VERBOSE;
        }

        // Get all registered readers/writers from the loaded Osg plugins and check the extensions they provide
        for(OsgDB::Registry::ReaderWriterList::iterator rw_itr = OsgDB::Registry::instance()->getReaderWriterList().begin();
            rw_itr != OsgDB::Registry::instance()->getReaderWriterList().end();
            ++rw_itr)
        {
            OsgDB::ReaderWriter::FormatDescriptionMap rwfdm = (*rw_itr)->supportedExtensions();
            OsgDB::ReaderWriter::FormatDescriptionMap::iterator fdm_itr;
            for(fdm_itr = rwfdm.begin();fdm_itr != rwfdm.end();++fdm_itr)
            {
#ifdef PARSE_FORMATS_VERBOSE
                std::cout<<"-- adding extension: ."<<fdm_itr->first<<" ("<<fdm_itr->second<<")"<<std::endl;
#endif//def PARSE_FORMATS_VERBOSE
                std::string ext = std::string(".") + fdm_itr->first;
                //extensions[ext] = MEDIA_TYPE_3DMODEL;
            }
        }

        // Close all Osg plugins
        OsgDB::Registry::instance()->closeAllLibraries();

        // Verify which readers/writers haven't been closed:
#ifdef PARSE_FORMATS_VERBOSE
        for(OsgDB::Registry::ReaderWriterList::iterator rw_itr = OsgDB::Registry::instance()->getReaderWriterList().begin();
            rw_itr != OsgDB::Registry::instance()->getReaderWriterList().end();
            ++rw_itr)
            std::cout<<"-- plugin still open: "<<(*rw_itr)->className()<<std::endl;
#endif//def PARSE_FORMATS_VERBOSE;

#ifdef PARSE_FORMATS_VERBOSE
        std::cout << "Gathering media file extensions from Osg plugins... done" << std::endl;
#endif//def PARSE_FORMATS_VERBOSE
#else // PARSE_FORMATS_DYNAMICALLY
        // from http://www.openscenegraph.org/projects/Osg/wiki/Support/UserGuides/Plugins
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
        extensions[".osgt"] = MEDIA_TYPE_3DMODEL;
        extensions[".shp"] = MEDIA_TYPE_3DMODEL;
        extensions[".sta"] = MEDIA_TYPE_3DMODEL;
        extensions[".stl"] = MEDIA_TYPE_3DMODEL;
        extensions[".wrl"] = MEDIA_TYPE_3DMODEL;
        extensions[".x"] = MEDIA_TYPE_3DMODEL;
#endif
    }
    return extensions;
}

