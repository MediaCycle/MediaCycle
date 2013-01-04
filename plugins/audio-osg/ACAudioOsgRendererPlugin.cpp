/*
 *  ACAudioOsgRendererPlugin.cpp
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

#include "ACAudioOsgRendererPlugin.h"
#include "ACOsgAudioRenderer.h"
#include "ACOsgAudioTrackRenderer.h"

#include <iostream>

#include <osgDB/Registry>
#include <osgDB/ReaderWriter>
#include <osgDB/FileNameUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/PluginQuery>

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sndfile.h>

// uncomment this to parse formats dynamically
//#define PARSE_FORMATS_DYNAMICALLY

// uncomment this to parse formats in verbose mode
//#define PARSE_FORMATS_VERBOSE

ACAudioOsgRendererPlugin::ACAudioOsgRendererPlugin() : ACOsgRendererPlugin(){
    this->mName = "Audio Renderer (OSG and libsndfile)";
    this->mDescription ="Plugin for rendering audio files with OpenSceneGraph";
    this->mMediaType = MEDIA_TYPE_AUDIO;

    browser_node_thumbnails.push_back("None");
    browser_node_thumbnails.push_back("Circular browser waveform");
    browser_node_thumbnails.push_back("Classic browser waveform");
    this->addStringParameter("Browser node", browser_node_thumbnails.front(), browser_node_thumbnails, "Browser node",boost::bind(&ACOsgRendererPlugin::changeBrowserThumbnail,this));

    this->updateStringParameterCallback("Timeline playback",boost::bind(&ACAudioOsgRendererPlugin::changeTimelinePlaybackThumbnail,this));
    this->updateStringParameterCallback("Timeline summary",boost::bind(&ACAudioOsgRendererPlugin::changeTimelineSummaryThumbnail,this));

    timeline_playback_thumbnails.push_back("None");
    timeline_playback_thumbnails.push_back("Classic timeline waveform");
    this->addStringParameter("Timeline playback",timeline_playback_thumbnails.front(),timeline_playback_thumbnails,"Timeline playback",boost::bind(&ACOsgRendererPlugin::changeTimelinePlaybackThumbnail,this));

    timeline_summary_thumbnails.push_back("None");
    timeline_summary_thumbnails.push_back("Classic timeline waveform");
    this->addStringParameter("Timeline summary",timeline_summary_thumbnails.front(),timeline_summary_thumbnails,"Timeline summary",boost::bind(&ACOsgRendererPlugin::changeTimelineSummaryThumbnail,this));
}

ACAudioOsgRendererPlugin::~ACAudioOsgRendererPlugin(){
}

std::map<std::string,ACMediaType> ACAudioOsgRendererPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string,ACMediaType> extensions;
    if(media_type == MEDIA_TYPE_AUDIO || media_type == MEDIA_TYPE_ALL){
#ifdef PARSE_FORMATS_DYNAMICALLY
#ifdef PARSE_FORMATS_VERBOSE
        std::cout << "Gathering audio file extensions from sndfile..." << std::endl;
#endif//def PARSE_OSG_PLUGINS_VERBOSE
        //CF ripped from sndfile's examples/list_formats.c
        SF_FORMAT_INFO	info ;
        SF_INFO 		sfinfo ;
        char buffer [128] ;
        int format, major_count, subtype_count, m, s;

        memset (&sfinfo, 0, sizeof (sfinfo));
        buffer [0] = 0 ;
        sf_command (0, SFC_GET_LIB_VERSION, buffer, sizeof (buffer));
        if (strlen (buffer) < 1)
        {	printf ("sndfile line %d: could not retrieve lib version.\n", __LINE__);
            //exit (1);
        } ;
        //printf ("Adding libsnfile supported extension, version : %s\n\n", buffer);// CF enable for debug mode

        sf_command (0, SFC_GET_FORMAT_MAJOR_COUNT, &major_count, sizeof (int)) ;
        sf_command (0, SFC_GET_FORMAT_SUBTYPE_COUNT, &subtype_count, sizeof (int)) ;

        sfinfo.channels = 1 ;
        for (m = 0 ; m < major_count ; m++)
        {
            info.format = m ;
            sf_command (0, SFC_GET_FORMAT_MAJOR, &info, sizeof (info)) ;

            std::string ext =  std::string(".") + std::string(info.extension);

            if (ext == ".aif" || ext == ".aiff"){
                extensions[".aif"];
                extensions[".aiff"];
#ifdef PARSE_FORMATS_VERBOSE
                std::cout << "ACMediaFactory: adding sndfile extensions .aiff and .aif" << std::endl;
#endif//def PARSE_FORMATS_VERBOSE
            }
            else if (ext == ".oga"){
                extensions[".oga"];
                extensions[".ogg"];
#ifdef PARSE_FORMATS_VERBOSE
                std::cout << "ACMediaFactory: adding sndfile extensions .oga and .ogg" << std::endl;
#endif//def PARSE_FORMATS_VERBOSE
            }
            else{
                extensions[ext];
#ifdef PARSE_FORMATS_VERBOSE
                std::cout << "ACMediaFactory: adding sndfile extension " << ext << std::endl;
#endif//def PARSE_FORMATS_VERBOSE
            }
            //CF the following could be used to add format metadata
            /*
    printf ("%s  (extension \"%s\")\n", info.name, info.extension) ;
    format = info.format ;
    for (s = 0 ; s < subtype_count ; s++)
    {	info.format = s ;
        sf_command (0, SFC_GET_FORMAT_SUBTYPE, &info, sizeof (info)) ;

        format = (format & SF_FORMAT_TYPEMASK) | info.format ;

        sfinfo.format = format ;
        if (sf_format_check (&sfinfo))
            printf ("   %s\n", info.name) ;
    } ;
    */
        }
#else // PARSE_FORMATS_DYNAMICALLY
        // from libsndfile 1.0.21
        extensions[".aif"] = MEDIA_TYPE_AUDIO;
        extensions[".aiff"] = MEDIA_TYPE_AUDIO;
        extensions[".au"] = MEDIA_TYPE_AUDIO;
        extensions[".avr"] = MEDIA_TYPE_AUDIO;
        extensions[".caf"] = MEDIA_TYPE_AUDIO;
        extensions[".flac"] = MEDIA_TYPE_AUDIO;
        extensions[".htk"] = MEDIA_TYPE_AUDIO;
        extensions[".iff"] = MEDIA_TYPE_AUDIO;
        extensions[".mat"] = MEDIA_TYPE_AUDIO;
        extensions[".mpc"] = MEDIA_TYPE_AUDIO;
        extensions[".oga"] = MEDIA_TYPE_AUDIO;
        extensions[".ogg"] = MEDIA_TYPE_AUDIO;
        extensions[".paf"] = MEDIA_TYPE_AUDIO;
        extensions[".pvf"] = MEDIA_TYPE_AUDIO;
        extensions[".raw"] = MEDIA_TYPE_AUDIO;
        extensions[".rf64"] = MEDIA_TYPE_AUDIO;
        extensions[".sd2"] = MEDIA_TYPE_AUDIO;
        extensions[".sds"] = MEDIA_TYPE_AUDIO;
        extensions[".sf"] = MEDIA_TYPE_AUDIO;
        extensions[".voc"] = MEDIA_TYPE_AUDIO;
        extensions[".w64"] = MEDIA_TYPE_AUDIO;
        extensions[".wav"] = MEDIA_TYPE_AUDIO;
        extensions[".wve"] = MEDIA_TYPE_AUDIO;
        extensions[".xi"] = MEDIA_TYPE_AUDIO;
#endif // PARSE_FORMATS_DYNAMICALLY
    }
    return extensions;
}

std::vector<ACMediaType> ACAudioOsgRendererPlugin::getSupportedMediaTypes(){
    std::vector<ACMediaType> media_types;
    media_types.push_back(MEDIA_TYPE_AUDIO);
    return media_types;
}

ACOsgMediaRenderer* ACAudioOsgRendererPlugin::createMediaRenderer(ACMediaType media_type){
    ACOsgMediaRenderer* renderer = 0;
    if(media_type == MEDIA_TYPE_AUDIO)
        renderer = new ACOsgAudioRenderer();
    return renderer;
}

ACOsgTrackRenderer* ACAudioOsgRendererPlugin::createTrackRenderer(ACMediaType media_type){
    ACOsgTrackRenderer* renderer = 0;
    if(media_type == MEDIA_TYPE_AUDIO)
        renderer = new ACOsgAudioTrackRenderer();
    return renderer;
}
