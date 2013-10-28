/*
 *  ACVideoOsgRendererPlugin.cpp
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

#include "ACVideoOsgRendererPlugin.h"
#include "ACOsgVideoThumbnail.h"
#include "ACOsgVideoRenderer.h"
#include "ACOsgVideoTrackRenderer.h"

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

ACVideoOsgRendererPlugin::ACVideoOsgRendererPlugin() : ACOsgRendererPlugin(){
    this->mName = "Video Renderer (OSG)";
    this->mDescription ="Plugin for rendering Video files with OpenSceneGraph";
    this->mMediaType = MEDIA_TYPE_VIDEO;

    this->addCallback("Timeline playback","Timeline playback",boost::bind(&ACOsgRendererPlugin::changeTimelinePlaybackThumbnail,this));

    browser_node_thumbnails.push_back("None");
    browser_node_thumbnails.push_back("Slit-scan");
    browser_node_thumbnails.push_back("Circular slit-scan");
    this->addStringParameter("Browser node", browser_node_thumbnails.front(), browser_node_thumbnails, "Browser node",boost::bind(&ACOsgRendererPlugin::changeBrowserNodeThumbnail,this));

    timeline_selection_thumbnails.push_back("None");
    timeline_selection_thumbnails.push_back("Keyframes");
    this->addStringParameter("Timeline selection",timeline_selection_thumbnails.back(),timeline_selection_thumbnails,"Timeline selection",boost::bind(&ACOsgRendererPlugin::changeTimelineSelectionThumbnail,this));

    timeline_summary_thumbnails.push_back("None");
    timeline_summary_thumbnails.push_back("Keyframes");
    this->addStringParameter("Timeline summary",timeline_summary_thumbnails.back(),timeline_summary_thumbnails,"Timeline summary",boost::bind(&ACOsgRendererPlugin::changeTimelineSummaryThumbnail,this));
}

ACVideoOsgRendererPlugin::~ACVideoOsgRendererPlugin(){
}

std::map<std::string,ACMediaType> ACVideoOsgRendererPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string,ACMediaType> extensions;
    if(media_type == MEDIA_TYPE_VIDEO || media_type == MEDIA_TYPE_ALL){
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
                //extensions[ext] = MEDIA_TYPE_VIDEO;
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
        extensions[".3gp"] = MEDIA_TYPE_VIDEO;
        extensions[".avi"] = MEDIA_TYPE_VIDEO;
        extensions[".ffmpeg"] = MEDIA_TYPE_VIDEO;
        extensions[".flv"] = MEDIA_TYPE_VIDEO;
        extensions[".m4v"] = MEDIA_TYPE_VIDEO;
        extensions[".mjpeg"] = MEDIA_TYPE_VIDEO;
        extensions[".mkv"] = MEDIA_TYPE_VIDEO;
        extensions[".mov"] = MEDIA_TYPE_VIDEO;
        extensions[".mp4"] = MEDIA_TYPE_VIDEO;
        extensions[".mpeg"] = MEDIA_TYPE_VIDEO;
        extensions[".mpg"] = MEDIA_TYPE_VIDEO;
        extensions[".mpv"] = MEDIA_TYPE_VIDEO;
        extensions[".ogg"] = MEDIA_TYPE_VIDEO;
        extensions[".sav"] = MEDIA_TYPE_VIDEO;
        extensions[".sdp"] = MEDIA_TYPE_VIDEO;
        extensions[".swf"] = MEDIA_TYPE_VIDEO;
        extensions[".wmv"] = MEDIA_TYPE_VIDEO;
        extensions[".xine"] = MEDIA_TYPE_VIDEO;
        extensions[".xvid"] = MEDIA_TYPE_VIDEO;
#endif
    }
    return extensions;
}

std::vector<ACMediaType> ACVideoOsgRendererPlugin::getSupportedMediaTypes(){
    std::vector<ACMediaType> media_types;
    media_types.push_back(MEDIA_TYPE_VIDEO);
    return media_types;
}

ACMediaThumbnail* ACVideoOsgRendererPlugin::createSharedThumbnail(ACMedia* media){
    ACMediaThumbnail* thumbnail = 0;
    if(media->getType()== MEDIA_TYPE_VIDEO){
        thumbnail = new ACOsgVideoThumbnail(media->getFileName(), media->getWidth(),media->getHeight());
    }
    return thumbnail;
}

std::string ACVideoOsgRendererPlugin::sharedThumbnailName(ACMediaType media_type){
    if(media_type == MEDIA_TYPE_VIDEO){
        return "Video image stream (OSG)";
    }
    else
        return "";
}

ACOsgMediaRenderer* ACVideoOsgRendererPlugin::createMediaRenderer(ACMediaType media_type){
    ACOsgMediaRenderer* renderer = 0;
    if(media_type == MEDIA_TYPE_VIDEO)
        renderer = new ACOsgVideoRenderer();
    return renderer;
}

ACOsgTrackRenderer* ACVideoOsgRendererPlugin::createTrackRenderer(ACMediaType media_type){
    ACOsgTrackRenderer* renderer = 0;
    if(media_type == MEDIA_TYPE_VIDEO)
        renderer = new ACOsgVideoTrackRenderer();
    return renderer;
}
