/**
 * @brief A plugin that provides an audio features thumbnailer with SVG as output.
 * @author Christian Frisson
 * @date 28/10/2013
 * @copyright (c) 2013 – UMONS - Numediart
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

#include "ACAudioSvgFeaturesThumbnailerPlugin.h"
#include <ACAudioData.h>
#include <ACMedia.h>
#include <iostream>

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

ACAudioSvgFeaturesThumbnailerPlugin::ACAudioSvgFeaturesThumbnailerPlugin() : ACThumbnailerPlugin(){
    this->mName = "Audio Features Thumbnailer (SVG)";
    this->mDescription ="Plugin for summarizing audio files from features for SVG browsers";
    this->mMediaType = MEDIA_TYPE_AUDIO;
}

ACAudioSvgFeaturesThumbnailerPlugin::~ACAudioSvgFeaturesThumbnailerPlugin(){
    thumbnails_specs.clear();
}

std::string ACAudioSvgFeaturesThumbnailerPlugin::requiresMediaReaderPlugin()
{
    return "ACAudioSndfileReaderPlugin"; //for now
}

std::vector<std::string> ACAudioSvgFeaturesThumbnailerPlugin::requiresFeaturesPlugins(){
    std::vector<std::string> features_plugins;
    features_plugins.push_back("audio features yaafe");
    // No features plugin is required
    return features_plugins;
}

std::vector<std::string> ACAudioSvgFeaturesThumbnailerPlugin::requiresSegmentationPlugins(){
    std::vector<std::string> segmentation_plugins;
    // No segmentation plugin is required
    return segmentation_plugins;
}

void classic_features(ACAudioFeaturesThumbnailSpecs& _specs)
{
    _specs.top_p << Point(_specs.index,_specs.offset_y+_specs.scale_y*_specs.top_v);
    _specs.down_p << Point(_specs.index,_specs.offset_y+_specs.scale_y*_specs.down_v);
}

void circular_features(ACAudioFeaturesThumbnailSpecs& _specs)
{
    float top = max(_specs.top_v,abs(_specs.down_v));
    float angle = -1.0f*(float)(_specs.index)/(float)(_specs.length)*2*ACPi + 0.5f*ACPi;
    _specs.top_p << Point(
                        _specs.offset_x + (_specs.width/2.0f * top)*cos(angle),
                        _specs.offset_y + (_specs.height/2.0f * top)*sin(angle)
                        );
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

std::vector<ACMediaThumbnail*> ACAudioSvgFeaturesThumbnailerPlugin::summarize(ACMedia* media){

    std::vector<ACMediaThumbnail*> thumbnails;

    float start_time = getTime();

    if(!media){
        std::cerr << "ACAudioSvgFeaturesThumbnailerPlugin::summarize: no media to summarize" << std::endl;
        return thumbnails;
    }

    std::string filename = media->getFileName();

    //if(feature_names.size() == 0){
    std::vector<std::string> features_plugins = media_cycle->getPluginManager()->getAvailablePluginsNames(PLUGIN_TYPE_FEATURES, media_cycle->getMediaType());
    for(std::vector<std::string>::iterator features_plugin = features_plugins.begin();features_plugin!=features_plugins.end();features_plugin++){
        ACTimedFeaturesPlugin* plugin = dynamic_cast<ACTimedFeaturesPlugin*>(media_cycle->getPluginManager()->getPlugin(*features_plugin));
        if(plugin){
            ACFeatureDimensions featdims = plugin->getTimedFeaturesDimensions();
            for(ACFeatureDimensions::iterator featdim = featdims.begin(); featdim != featdims.end();featdim++){
                std::cout << "ACAudioSvgFeaturesThumbnailerPlugin: feature: " << featdim->first << " of dim " << featdim->second << std::endl;
                if(featdim->second == 1){
                    //feature_names.push_back(featdim->first);
                    std::string _featname = featdim->first;
                    std::string _thumbname = _featname + " Ring";
                    feature_names.push_back(_thumbname);

                    ACMediaTimedFeature* tf = media->getTimedFeature(_featname);
                    if(tf){

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
                        std::string _filename = generateThumbnailName(media->getFileName(),_thumbname, ".svg");

                        bool thumbnails_exist = true;
                        fs::path p( _filename.c_str());// , fs::native );
                        if ( fs::exists( p ) )
                        {
                            //std::cout << "ACAudioSvgFeaturesThumbnailerPlugin::summarize: the expected thumbnail already exists as file: " << _filename << std::endl;
                            if ( fs::is_regular( p ) )
                            {
                                //std::cout << "ACAudioSvgFeaturesThumbnailerPlugin::summarize: file is regular: " << _filename << std::endl;
                                if(fs::file_size( p ) > 0 ){
                                    //std::cout << "ACAudioSvgFeaturesThumbnailerPlugin::summarize: size of " << _filename << " is non-zero, not recomputing "<< std::endl;
                                }
                                else
                                    thumbnails_exist = false;
                            }
                            else
                                thumbnails_exist = false;
                        }
                        else
                            thumbnails_exist = false;

                        if(thumbnails_exist){
                            // Consistency checks for the provided media instance (media data, start/end)
                            ACMediaThumbnail* thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
                            thumbnail->setFileName(thumbnails_specs[_thumbname].filename);
                            thumbnail->setName(thumbnails_specs[_thumbname].name);
                            thumbnail->setWidth(thumbnails_specs[_thumbname].width);
                            thumbnail->setHeight(thumbnails_specs[_thumbname].height);
                            thumbnail->setLength(thumbnails_specs[_thumbname].length);
                            thumbnail->setCircular(thumbnails_specs[_thumbname].circular);
                            thumbnails.push_back(thumbnail);
                            std::cout << "ACAudioSvgFeaturesThumbnailerPlugin::summarize: reusing thumbnails took " << getTime() - start_time << std::endl;
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

                            Document doc(thumbnails_specs[_thumbname].filename, Layout(thumbnails_specs[_thumbname].dimensions, Layout::BottomLeft));
                            thumbnails_specs[_thumbname].top_p.addPoints( thumbnails_specs[_thumbname].down_p.getPoints() );
                            doc << thumbnails_specs[_thumbname].top_p;

                            bool saved = doc.save();
                            if(!saved)
                                std::cerr << "ACAudioSvgFeaturesThumbnailerPlugin::summarize: couldn't save thumbnail " << thumbnails_specs[_thumbname].filename << std::endl;

                            ACMediaThumbnail* thumbnail = new ACMediaThumbnail(MEDIA_TYPE_IMAGE);
                            thumbnail->setFileName(thumbnails_specs[_thumbname].filename);
                            thumbnail->setName(thumbnails_specs[_thumbname].name);
                            thumbnail->setWidth(thumbnails_specs[_thumbname].width);
                            thumbnail->setHeight(thumbnails_specs[_thumbname].height);
                            thumbnail->setLength(thumbnails_specs[_thumbname].length);
                            thumbnail->setCircular(thumbnails_specs[_thumbname].circular);
                            thumbnails.push_back(thumbnail);

                        }
                    }
                    delete tf;
                }
            }
        }
    }

    //}

    //std::cout << "ACAudioSvgFeaturesThumbnailerPlugin::summarize: done computing Features(s) for " << filename << " in " << getTime()-Features_in << " sec." << std::endl;
    //progress = 1.0f;

    thumbnails_specs.clear();
    std::cout << "ACAudioSvgFeaturesThumbnailerPlugin::summarize: creating thumbnails took " << getTime() - start_time << std::endl;
    return thumbnails;
}

std::vector<std::string> ACAudioSvgFeaturesThumbnailerPlugin::getThumbnailNames(){
    return feature_names;
}

std::map<std::string,std::string> ACAudioSvgFeaturesThumbnailerPlugin::getThumbnailExtensions(){
    std::map<std::string,std::string> extensions;
    for(std::vector<std::string>::iterator feature_name = feature_names.begin();feature_name!=feature_names.end();feature_name++)
        extensions[*feature_name] = ".svg";
    return extensions;
}

std::map<std::string,ACMediaType> ACAudioSvgFeaturesThumbnailerPlugin::getThumbnailTypes(){
    std::map<std::string,ACMediaType> thumbnail_types;
    for(std::vector<std::string>::iterator feature_name = feature_names.begin();feature_name!=feature_names.end();feature_name++)
        thumbnail_types[*feature_name] = MEDIA_TYPE_IMAGE;
    return thumbnail_types;
}

std::map<std::string,std::string> ACAudioSvgFeaturesThumbnailerPlugin::getThumbnailDescriptions(){
    std::map<std::string,std::string> thumbnail_descriptions;
    for(std::vector<std::string>::iterator feature_name = feature_names.begin();feature_name!=feature_names.end();feature_name++)
        thumbnail_descriptions[*feature_name] = *feature_name;
    return thumbnail_descriptions;
}
