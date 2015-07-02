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

#ifndef ACAudioSvgAudioMetroThumbnailerPlugin_H
#define	ACAudioSvgAudioMetroThumbnailerPlugin_H

#include <MediaCycle.h>
#include <ACPlugin.h>

#include <simple_svg.hpp>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

static std::string ext(".png");

static std::string generateThumbnailName(std::string filename, std::string thumbnail_name, std::string extension);

struct ACAudioFeaturesThumbnailSpecs{
    std::string name, filename;
    int hop_samples;
    float width,height,length;
    bool circular;
    svg::Dimensions dimensions;
    svg::Polygon top_p,down_p;
    float top_v, down_v, scale_x, scale_y, offset_x, offset_y;
    int index;
    typedef boost::function<void(ACAudioFeaturesThumbnailSpecs&)> ACAudioFeaturesBufferCallback;
    ACAudioFeaturesBufferCallback callback;
    ACAudioFeaturesThumbnailSpecs(std::string _name="", std::string _media_filename="", float _width=0.0f, float _height=0.0f, float _length=0.0f, int _hop_samples=0, bool _circular=false, ACAudioFeaturesBufferCallback _callback = 0):
        name(_name), filename(""),
        width(_width),height(_height),length(_length),
        circular(_circular),
        top_v(0.0f),down_v(0.0f),
        scale_x(_width/2.0f), scale_y(_height/2.0f),offset_x(_width/2.0f), offset_y(_height/2.0f),
        dimensions(_width,_height),
        top_p(svg::Fill(svg::Color::Silver), svg::Stroke(.1, svg::Color::Silver)),
        down_p(svg::Fill(svg::Color::Silver), svg::Stroke(.1, svg::Color::Silver)),
        callback(_callback),
        index(0)
    {
        if((int)(_hop_samples)<1)
            hop_samples = 1;
        else
            hop_samples= _hop_samples;
        filename = generateThumbnailName(_media_filename,_name,"");
        dimensions = svg::Dimensions(_width,_height);
    }
    ~ACAudioFeaturesThumbnailSpecs(){callback = 0;}
};

class ACAudioSvgAudioMetroThumbnailerPlugin : public ACThumbnailerPlugin
{
public:
    ACAudioSvgAudioMetroThumbnailerPlugin();
    virtual ~ACAudioSvgAudioMetroThumbnailerPlugin();
    virtual std::vector<std::string> getThumbnailNames();
    virtual std::map<std::string,ACMediaType> getThumbnailTypes();
    virtual std::map<std::string,std::string> getThumbnailDescriptions();
    virtual std::map<std::string,std::string> getThumbnailExtensions();
    virtual std::string requiresMediaReaderPlugin();
    virtual std::vector<std::string> requiresFeaturesPlugins(); // list of plugin names (not paths)
    virtual std::vector<std::string> requiresSegmentationPlugins(); // list of plugin names (not paths)
    virtual std::vector<ACMediaThumbnail*> summarize(ACMedia* theMedia);
protected:
    std::map<std::string,ACAudioFeaturesThumbnailSpecs> thumbnails_specs;
    std::vector<std::string> feature_names;
};

#endif
