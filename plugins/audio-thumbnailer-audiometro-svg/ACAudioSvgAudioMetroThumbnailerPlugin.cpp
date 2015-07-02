/**
 * @brief A plugin to compute AudioMetro thumbnails as PNG files.
 * @author Christian Frisson
 * @date 20/05/2015
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

#include "ACAudioSvgAudioMetroThumbnailerPlugin.h"
#include <ACAudioData.h>
#include <ACMedia.h>
#include <iostream>
#include <rsvg-convert.h>

#ifndef ACPi
#define	ACPi		3.14159265358979323846  /* pi */
#endif

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

using namespace std;
using namespace svg;

std::string generateThumbnailName(std::string filename, std::string thumbnail_name, std::string extension){
    boost::filesystem::path thumbnail_path;
    boost::filesystem::path media_path(filename.c_str());
    std::string thumbnail_suffix(thumbnail_name);
    boost::to_lower(thumbnail_suffix);
    boost::replace_all(thumbnail_suffix," ","_");
    //std::cout << name << " converted to " << thumbnail_suffix << std::endl;
    std::string slash = "/";
#ifdef WIN32
    slash = "\\";
#endif
    thumbnail_path = boost::filesystem::path( media_path.parent_path().string() + slash + media_path.stem().string() + "_" + thumbnail_suffix + extension);
    return thumbnail_path.string();
}

ACAudioSvgAudioMetroThumbnailerPlugin::ACAudioSvgAudioMetroThumbnailerPlugin() : ACThumbnailerPlugin(){
    this->mName = "AudioMetro Thumbnailer (PNG)";
    this->mDescription ="Plugin for computing AudioMetro thumbnails as PNG files";
    this->mMediaType = MEDIA_TYPE_AUDIO;
}

ACAudioSvgAudioMetroThumbnailerPlugin::~ACAudioSvgAudioMetroThumbnailerPlugin(){
    thumbnails_specs.clear();
}

std::string ACAudioSvgAudioMetroThumbnailerPlugin::requiresMediaReaderPlugin()
{
    return "ACAudioSndfileReaderPlugin"; //for now
}

std::vector<std::string> ACAudioSvgAudioMetroThumbnailerPlugin::requiresFeaturesPlugins(){
    std::vector<std::string> features_plugins;
    features_plugins.push_back("Audio Features Yaafe");
    return features_plugins;
}

std::vector<std::string> ACAudioSvgAudioMetroThumbnailerPlugin::requiresSegmentationPlugins(){
    std::vector<std::string> segmentation_plugins;
    // No segmentation plugin is required
    return segmentation_plugins;
}

void ring_features(ACAudioFeaturesThumbnailSpecs& _specs)
{
    float top = max(_specs.top_v,abs(_specs.down_v));
    float angle = -1.0f*(float)(_specs.index)/(float)(_specs.length)*2*ACPi + 0.5f*ACPi;
    _specs.top_p << Point(
                        _specs.offset_x + (0.5f*_specs.width * (0.125+0.125f*top))*cos(angle),
                        _specs.offset_y + (0.5f*_specs.height * (0.125+0.125f*top))*sin(angle)
                        );
}

std::vector<ACMediaThumbnail*> ACAudioSvgAudioMetroThumbnailerPlugin::summarize(ACMedia* media){

    std::vector<ACMediaThumbnail*> thumbnails;

    float start_time = getTime();

    if(!media){
        std::cerr << "ACAudioSvgAudioMetroThumbnailerPlugin::summarize: no media to summarize" << std::endl;
        return thumbnails;
    }

    std::string filename = media->getFileName();

    std::string _featname = "Perceptual Sharpness";
    std::string _thumbname = _featname + " Ring";

    std::string svg_filename = generateThumbnailName(media->getFileName(),_thumbname, ".svg");
    std::string png_filename = generateThumbnailName(media->getFileName(),_thumbname, ".png");

    ACMediaTimedFeature* tf = media->getTimedFeature(_featname);
    if(tf){

        feature_names.push_back(_thumbname);
        this->thumbnails_specs[_thumbname] = ACAudioFeaturesThumbnailSpecs(
                    _thumbname /*name*/,
                    filename, /*media filename*/
                    400 /*width*/,
                    400 /*height*/,
                    360 /*length*/,
                    tf->getLength(),
                    true, /*circular*/
                    ring_features
                    );

        // Checking if thumbnails already exist:
        bool svg_thumbnail_exists = true;
        fs::path svg_path( svg_filename.c_str());// , fs::native );
        if ( fs::exists( svg_path ) )
        {
            //std::cout << "ACAudioSvgAudioMetroThumbnailerPlugin::summarize: the expected thumbnail already exists as file: " << _filename << std::endl;
            if ( fs::is_regular( svg_path ) )
            {
                //std::cout << "ACAudioSvgAudioMetroThumbnailerPlugin::summarize: file is regular: " << _filename << std::endl;
                if(fs::file_size( svg_path ) > 0 ){
                    //std::cout << "ACAudioSvgAudioMetroThumbnailerPlugin::summarize: size of " << _filename << " is non-zero, not recomputing "<< std::endl;
                }
                else
                    svg_thumbnail_exists = false;
            }
            else
                svg_thumbnail_exists = false;
        }
        else
            svg_thumbnail_exists = false;

        if(svg_thumbnail_exists){
            std::cout << "ACAudioSvgAudioMetroThumbnailerPlugin::summarize: reusing thumbnails took " << getTime() - start_time << std::endl;
            //return thumbnails;
        }
        else{
            std::cout << "Feature: " << _featname << " Length " << tf->getLength() << std::endl;
            std::cout << "Feature: " << _featname << " Dim " << tf->getDim() << std::endl;

            arma::fmat values = tf->getValue();
            for(int i=0;i<tf->getLength();i++){
                this->thumbnails_specs[_thumbname].top_v =values[i];
                this->thumbnails_specs[_thumbname].down_v =values[i];
                this->thumbnails_specs[_thumbname].callback(this->thumbnails_specs[_thumbname]);
                (this->thumbnails_specs[_thumbname].index)++;

            }
            // Draw a terminating point
            this->thumbnails_specs[_thumbname].top_v = 0;
            this->thumbnails_specs[_thumbname].down_v = 0;
            this->thumbnails_specs[_thumbname].callback(this->thumbnails_specs[_thumbname]);

            Document doc(/*thumbnails_specs[_thumbname].*/svg_filename, Layout(thumbnails_specs[_thumbname].dimensions, Layout::BottomLeft));
            thumbnails_specs[_thumbname].top_p.addPoints( thumbnails_specs[_thumbname].down_p.getPoints() );
            doc << thumbnails_specs[_thumbname].top_p;

            bool saved = doc.save();
            if(!saved)
                std::cerr << "ACAudioSvgAudioMetroThumbnailerPlugin::summarize: couldn't save thumbnail " << thumbnails_specs[_thumbname].filename << std::endl;

        }

        //CF hack to force the svg format  (puts the svg filename before the png filename on ACMedia's vector of thumbnail filenames)
        {
            ACMediaThumbnail* thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
            thumbnail->setFileName(svg_filename);
            thumbnail->setName(thumbnails_specs[_thumbname].name);
            thumbnail->setWidth(thumbnails_specs[_thumbname].width);
            thumbnail->setHeight(thumbnails_specs[_thumbname].height);
            thumbnail->setLength(thumbnails_specs[_thumbname].length);
            thumbnail->setCircular(thumbnails_specs[_thumbname].circular);
            thumbnails.push_back(thumbnail);
        }

        rsvg_convert(svg_filename.c_str(),png_filename.c_str(),"png");

        bool png_thumbnail_exists = true;
        fs::path png_path( png_filename.c_str());// , fs::native );
        if ( fs::exists( png_path ) )
        {
            //std::cout << "ACAudioSvgAudioMetroThumbnailerPlugin::summarize: the expected thumbnail already exists as file: " << _filename << std::endl;
            if ( fs::is_regular( png_path ) )
            {
                //std::cout << "ACAudioSvgAudioMetroThumbnailerPlugin::summarize: file is regular: " << _filename << std::endl;
                if(fs::file_size( png_path ) > 0 ){
                    //std::cout << "ACAudioSvgAudioMetroThumbnailerPlugin::summarize: size of " << _filename << " is non-zero, not recomputing "<< std::endl;
                }
                else
                    png_thumbnail_exists = false;
            }
            else
                png_thumbnail_exists = false;
        }
        else
            png_thumbnail_exists = false;

        if(png_thumbnail_exists){

            // Consistency checks for the provided media instance (media data, start/end)
            ACMediaThumbnail* thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
            thumbnail->setFileName(png_filename);
            thumbnail->setName(thumbnails_specs[_thumbname].name);
            thumbnail->setWidth(thumbnails_specs[_thumbname].width);
            thumbnail->setHeight(thumbnails_specs[_thumbname].height);
            thumbnail->setLength(thumbnails_specs[_thumbname].length);
            thumbnail->setCircular(thumbnails_specs[_thumbname].circular);
            thumbnails.push_back(thumbnail);

        }
        else{
            std::cerr << "ACAudioSvgAudioMetroThumbnailerPlugin: couldn't convert thumbnail from svg to png for file "  << png_filename << std::endl;
        }


    }
    delete tf;


    //}

    //std::cout << "ACAudioSvgAudioMetroThumbnailerPlugin::summarize: done computing Features(s) for " << filename << " in " << getTime()-Features_in << " sec." << std::endl;
    //progress = 1.0f;

    thumbnails_specs.clear();
    std::cout << "ACAudioSvgAudioMetroThumbnailerPlugin::summarize: creating thumbnails took " << getTime() - start_time << std::endl;
    return thumbnails;
}

std::vector<std::string> ACAudioSvgAudioMetroThumbnailerPlugin::getThumbnailNames(){
    return feature_names;
}

std::map<std::string,std::string> ACAudioSvgAudioMetroThumbnailerPlugin::getThumbnailExtensions(){
    std::map<std::string,std::string> extensions;
    for(std::vector<std::string>::iterator feature_name = feature_names.begin();feature_name!=feature_names.end();feature_name++)
        extensions[*feature_name] = ext;
    return extensions;
}

std::map<std::string,ACMediaType> ACAudioSvgAudioMetroThumbnailerPlugin::getThumbnailTypes(){
    std::map<std::string,ACMediaType> thumbnail_types;
    for(std::vector<std::string>::iterator feature_name = feature_names.begin();feature_name!=feature_names.end();feature_name++)
        thumbnail_types[*feature_name] = MEDIA_TYPE_IMAGE;
    return thumbnail_types;
}

std::map<std::string,std::string> ACAudioSvgAudioMetroThumbnailerPlugin::getThumbnailDescriptions(){
    std::map<std::string,std::string> thumbnail_descriptions;
    for(std::vector<std::string>::iterator feature_name = feature_names.begin();feature_name!=feature_names.end();feature_name++)
        thumbnail_descriptions[*feature_name] = *feature_name;
    return thumbnail_descriptions;
}
