/**
 * @brief A plugin that provides an audio waveform thumbnailer with SVG as output.
 * @author Christian Frisson
 * @date 4/11/2012
 * @copyright (c) 2012 – UMONS - Numediart
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

#include "ACAudioWaveformThumbnailerPlugin.h"
#include <ACAudioData.h>
#include <ACMedia.h>
#include <iostream>

#ifndef ACPi
#define	ACPi		3.14159265358979323846  /* pi */
#endif

using namespace std;
using namespace svg;

ACAudioWaveformThumbnailerPlugin::ACAudioWaveformThumbnailerPlugin() : ACThumbnailerPlugin(){
    this->mName = "Audio Waveform Thumbnailer";
    this->mDescription ="Plugin for summarizing audio files in waveforms";
    this->mMediaType = MEDIA_TYPE_AUDIO;
}

ACAudioWaveformThumbnailerPlugin::~ACAudioWaveformThumbnailerPlugin(){
    thumbnails_specs.clear();
}

std::string ACAudioWaveformThumbnailerPlugin::requiresMediaReaderPlugin()
{
    return "ACAudioSndfileReaderPlugin"; //for now
}

std::vector<std::string> ACAudioWaveformThumbnailerPlugin::requiresFeaturesPlugins(){
    std::vector<std::string> features_plugins;
    // No features plugin is required
    return features_plugins;
}

std::vector<std::string> ACAudioWaveformThumbnailerPlugin::requiresSegmentationPlugins(){
    std::vector<std::string> segmentation_plugins;
    // No segmentation plugin is required
    return segmentation_plugins;
}

void classic_waveform(ACAudioWaveformThumbnailSpecs& _specs)
{
      _specs.top_p << Point(_specs.index,_specs.offset_y+_specs.scale_y*_specs.top_v);
      _specs.down_p << Point(_specs.index,_specs.offset_y+_specs.scale_y*_specs.down_v);
}

void circular_waveform(ACAudioWaveformThumbnailSpecs& _specs)
{
    float top = max(_specs.top_v,abs(_specs.down_v));
    float angle = -1.0f*(float)(_specs.index)/(float)(_specs.length)*2*ACPi + 0.5f*ACPi;
    _specs.top_p << Point(
                        _specs.offset_x + (_specs.width/2.0f * top)*cos(angle),
                        _specs.offset_y + (_specs.height/2.0f * top)*sin(angle)
                        );
}

std::vector<ACMediaThumbnail*> ACAudioWaveformThumbnailerPlugin::summarize(ACMedia* media){
    std::vector<ACMediaThumbnail*> thumbnails;

    // Consistency checks for the provided media instance (media data, start/end)
    if(!media->getMediaData()){
        std::cerr << "ACAudioWaveformThumbnailerPlugin::summarize: no media data set" << std::endl;
        return thumbnails;
    }
    ACAudioData* audio_data = dynamic_cast<ACAudioData*>(media->getMediaData());
    if(!audio_data)
    {
        std::cerr << "ACAudioWaveformThumbnailerPlugin::summarize: no audio data set" << std::endl;
        return thumbnails;
    }

    float n_frames = audio_data->getNumberOfFrames();
    float sample_rate = audio_data->getSampleRate();
    int channels = audio_data->getNumberOfChannels();

    int last_frame = n_frames-1;
    int current_frame = 0;
    if(media->getStart()<0){
        std::cerr << "ACAudioWaveformThumbnailerPlugin::summarize: start not set" << std::endl;
        return thumbnails;
    }
    else if(media->getStart() >0){
        current_frame = (int)(media->getStart()*sample_rate);//sf_seek(infile, (int)(media->getStart()*sample_rate), SEEK_SET) ;
        std::cout << "ACAudioWaveformThumbnailerPlugin::summarize: seeked at " << current_frame << " while " << (int)(media->getStart()*sample_rate) << " was asked." << std::endl;
    }
    if(media->getEnd()<0){
        std::cerr << "ACAudioWaveformThumbnailerPlugin::summarize: end not set" << std::endl;
        return thumbnails;
    }
    else if(int(media->getEnd() * sample_rate) > n_frames){
        std::cerr << "ACAudioWaveformThumbnailerPlugin::summarize: end beyond sample size" << std::endl;
        return thumbnails;
    }
    else{
        last_frame = (int)((sample_rate)*(media->getEnd()-media->getStart()))-1;
    }

    std::string filename = media->getFileName();

    // Define waveforms to compute

    float hop = 0.02f;
    float progress = 0.0f;
    int bufsize = 1024;

    int browser_waveform_min_length = 200;
    int browser_waveform_length = (float)(last_frame-current_frame) / (float)(sample_rate) / hop;
    browser_waveform_length--;
    if (browser_waveform_length < browser_waveform_min_length){
        browser_waveform_length = browser_waveform_min_length;
        hop =  (float)(media->getEnd()-media->getStart()) / (float)(browser_waveform_length);
    }
    browser_waveform_length *= 2;

    float* buffer = 0;//float buffer [channels * bufsize];
    int k(0), m(0), readcount(0);
    double waveform_in = getTime();
    int s = 0;// source index

    this->thumbnails_specs["Classic browser waveform"] = ACAudioWaveformThumbnailSpecs(
                "Classic browser waveform" /*name*/,
                filename, /*media filename*/
                200 /*width*/,
                200 /*height*/,
                browser_waveform_length /*length*/,
                hop * sample_rate /*hop samples*/,
                false, /*circular*/
                classic_waveform
    );

    this->thumbnails_specs["Classic timeline waveform"] = ACAudioWaveformThumbnailSpecs(
                "Classic timeline waveform" /*name*/,
                filename, /*media filename*/
                5000 /*width*/,
                100 /*height*/,
                5000 /*length*/,
                (float)(media->getEnd()-media->getStart()) / 5000.0f * sample_rate /*hop samples*/,
                false, /*circular*/
                classic_waveform
    );

    this->thumbnails_specs["Circular browser waveform"] = ACAudioWaveformThumbnailSpecs(
                "Circular browser waveform" /*name*/,
                filename, /*media filename*/
                400 /*width*/,
                400 /*height*/,
                360 /*length*/,
                (float)(media->getEnd()-media->getStart()) / 360.0f * sample_rate /*hop samples*/,
                true, /*circular*/
                circular_waveform
    );

    // Loop on the audio frames using a buffer of bufsize
    while ( current_frame < last_frame && s<last_frame )
    {
        // Access to the buffer with consistency checks
        ACMediaDataContainer* data_container = audio_data->getBuffer(current_frame, bufsize, 0);
        if(!data_container){
            std::cerr << "ACAudioWaveformThumbnailerPlugin::summarize: couldn't get buffer at frame " << current_frame << " with buf size " << bufsize << std::endl;
            break;//return 0;
        }
        ACAudioFloatPtrDataContainer* float_data = dynamic_cast<ACAudioFloatPtrDataContainer*>(data_container);
        if(!float_data){
            std::cerr << "ACAudioWaveformThumbnailerPlugin::summarize: couldn't convert buffer at frame " << current_frame << " with buf size " << bufsize << std::endl;
            break;//return 0;
        }
        buffer = float_data->getData();
        readcount = float_data->getNumberOfFrames();
        if(!buffer)
            std::cerr << "ACAudioWaveformThumbnailerPlugin::summarize: couldn't access data at current frame " << current_frame << std::endl;
        current_frame += bufsize;

        // Draw an initial origin point
        for(std::map<std::string,ACAudioWaveformThumbnailSpecs>::iterator thumbnail_specs = thumbnails_specs.begin(); thumbnail_specs != thumbnails_specs.end(); ++ thumbnail_specs){
            if(thumbnail_specs->second.callback){
                thumbnail_specs->second.callback(thumbnail_specs->second);
            }
        }

        // Loop on all the buffer samples to match waveform points depending on their hop sizes
        for (k = 0 ; k < readcount ; k++)
        {
            int m = 0; // channel id
            for(std::map<std::string,ACAudioWaveformThumbnailSpecs>::iterator thumbnail_specs = thumbnails_specs.begin(); thumbnail_specs != thumbnails_specs.end(); ++ thumbnail_specs){
                // Produce coordinates of a reached waveform point, using the waveform specs callback
                if(s % thumbnail_specs->second.hop_samples == 0){
                    if(thumbnail_specs->second.callback){
                        thumbnail_specs->second.callback(thumbnail_specs->second);
                    }
                    ++(thumbnail_specs->second.index);

                    thumbnail_specs->second.down_v = buffer[k * channels + m];
                    thumbnail_specs->second.top_v = buffer[k * channels + m];
                }
                // Compute and store min/max values along the buffer
                else{
                    if ( buffer[k * channels + m] < thumbnail_specs->second.down_v ) {
                        thumbnail_specs->second.down_v = buffer[k * channels + m];
                    }
                    if ((buffer[k * channels + m])> thumbnail_specs->second.top_v ) {
                        thumbnail_specs->second.top_v = buffer[k * channels + m];
                    }
                }
            }
            s++;
        }

        delete buffer;
        buffer = 0;
    }

    // Draw a terminating point
    for(std::map<std::string,ACAudioWaveformThumbnailSpecs>::iterator thumbnail_specs = thumbnails_specs.begin(); thumbnail_specs != thumbnails_specs.end(); ++ thumbnail_specs){
        thumbnail_specs->second.top_v = 0;
        thumbnail_specs->second.down_v = 0;
        if(thumbnail_specs->second.callback){
            thumbnail_specs->second.callback(thumbnail_specs->second);
        }
    }

    // Save the waveforms as svg files and add them as media thumbnails
    for(std::map<std::string,ACAudioWaveformThumbnailSpecs>::iterator thumbnail_specs = thumbnails_specs.begin(); thumbnail_specs != thumbnails_specs.end(); ++ thumbnail_specs){
        //thumbnail_specs->second.document.save();
        Document doc(thumbnail_specs->second.filename, Layout(thumbnail_specs->second.dimensions, Layout::BottomLeft));

        /*if(thumbnail_specs->second.circular){
            Circle circle (Point( thumbnail_specs->second.offset_x, thumbnail_specs->second.offset_y),
                          thumbnail_specs->second.width/2.0f,
                          Fill(Color::Transparent),
                          Stroke(.1, Color::Silver)
                    );
            doc << circle;
        }*/

        doc << thumbnail_specs->second.top_p;
        doc << thumbnail_specs->second.down_p;
        doc.save();

        ACMediaThumbnail* thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
        thumbnail->setFileName(thumbnail_specs->second.filename);
        thumbnail->setName(thumbnail_specs->second.name);
        thumbnail->setWidth(thumbnail_specs->second.width);
        thumbnail->setHeight(thumbnail_specs->second.height);
        thumbnail->setLength(thumbnail_specs->second.length);
        thumbnail->setCircular(thumbnail_specs->second.circular);
        thumbnails.push_back(thumbnail);
    }

    std::cout << "ACAudioWaveformThumbnailerPlugin::summarize: done computing waveform(s) for " << filename << " in " << getTime()-waveform_in << " sec." << std::endl;
    progress = 1.0f;

    thumbnails_specs.clear();
    return thumbnails;
}

std::vector<std::string> ACAudioWaveformThumbnailerPlugin::getThumbnailNames(){
    std::vector<std::string> names;
    names.push_back("Classic browser waveform");
    names.push_back("Classic timeline waveform");
    names.push_back("Circular browser waveform");
    return names;
}

std::map<std::string,std::string> ACAudioWaveformThumbnailerPlugin::getThumbnailExtensions(){
    std::map<std::string,std::string> extensions;
    extensions["Classic browser waveform"] = ".svg";
    extensions["Classic timeline waveform"] = ".svg";
    extensions["Circular browser waveform"] = ".svg";
    return extensions;
}

std::map<std::string,ACMediaType> ACAudioWaveformThumbnailerPlugin::getThumbnailTypes(){
    std::map<std::string,ACMediaType> thumbnail_types;
    thumbnail_types["Classic browser waveform"] = MEDIA_TYPE_IMAGE;
    thumbnail_types["Classic timeline waveform"] = MEDIA_TYPE_IMAGE;
    thumbnail_types["Circular browser waveform"] = MEDIA_TYPE_IMAGE;
    return thumbnail_types;
}

std::map<std::string,std::string> ACAudioWaveformThumbnailerPlugin::getThumbnailDescriptions(){
    std::map<std::string,std::string> thumbnail_descriptions;
    thumbnail_descriptions["Classic browser waveform"] = "Classic browser waveform";
    thumbnail_descriptions["Classic timeline waveform"] = "Classic timeline waveform";
    thumbnail_descriptions["Circular browser waveform"] = "Circular browser waveform";
    return thumbnail_descriptions;
}