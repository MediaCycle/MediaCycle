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

#ifndef ACAudioWaveformThumbnailerPlugin_H
#define	ACAudioWaveformThumbnailerPlugin_H

#include <ACPlugin.h>

//#include "ACOsgRendererPlugin.h"

#include <simple_svg.hpp>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

struct ACAudioWaveformThumbnailSpecs{
    std::string name, filename;
    int hop_samples;
    float width,height,length;
    bool circular;
    svg::Dimensions dimensions;
    svg::Polygon top_p,down_p;
    float top_v, down_v, scale_x, scale_y, offset_x, offset_y;
    int index;
    typedef boost::function<void(ACAudioWaveformThumbnailSpecs&)> ACAudioWaveformBufferCallback;
    ACAudioWaveformBufferCallback callback;
    ACAudioWaveformThumbnailSpecs(std::string _name="", std::string _media_filename="", float _width=0.0f, float _height=0.0f, float _length=0.0f, int _hop_samples=0, bool _circular=false, ACAudioWaveformBufferCallback _callback = 0):
        name(_name), filename(""),
        hop_samples(_hop_samples),width(_width),height(_height),length(_length),
        circular(_circular),
        top_v(0.0f),down_v(0.0f),
        scale_x(_width/2.0f), scale_y(_height/2.0f),offset_x(_width/2.0f), offset_y(_height/2.0f),
        dimensions(_width,_height),
        top_p(svg::Fill(svg::Color::Silver), svg::Stroke(.1, svg::Color::Silver)),
        down_p(svg::Fill(svg::Color::Silver), svg::Stroke(.1, svg::Color::Silver)),
        callback(_callback),
        index(0)
    {
        std::stringstream thumbnail_path;
        boost::filesystem::path media_path(_media_filename.c_str());
        std::string thumbnail_suffix(_name);
        boost::to_lower(thumbnail_suffix);
        boost::replace_all(thumbnail_suffix," ","_");
        //std::cout << name << " converted to " << thumbnail_suffix << std::endl;
        std::string slash = "/";
#ifdef WIN32
        slash = "\\";
#endif
#ifdef __APPLE__
        thumbnail_path << media_path.parent_path().string() << slash << media_path.stem().string() << "_" << thumbnail_suffix << ".svg";
#else // this seems required on ubuntu to compile...
        thumbnail_path << media_path.parent_path() << slash << media_path.stem() << "_" << thumbnail_suffix << ".svg";
#endif
        filename = thumbnail_path.str();
        dimensions = svg::Dimensions(_width,_height);
    }
    ~ACAudioWaveformThumbnailSpecs(){callback = 0;}
};

class ACAudioWaveformThumbnailerPlugin : public ACThumbnailerPlugin
{
public:
    ACAudioWaveformThumbnailerPlugin();
    ~ACAudioWaveformThumbnailerPlugin();
    virtual std::vector<std::string> getThumbnailNames();
    virtual std::map<std::string,ACMediaType> getThumbnailTypes();
    virtual std::map<std::string,std::string> getThumbnailDescriptions();
    virtual std::map<std::string,std::string> getThumbnailExtensions();
    virtual std::string requiresMediaReaderPlugin();
    virtual std::vector<std::string> requiresFeaturesPlugins(); // list of plugin names (not paths)
    virtual std::vector<std::string> requiresSegmentationPlugins(); // list of plugin names (not paths)
    virtual std::vector<ACMediaThumbnail*> summarize(ACMedia* theMedia);
protected:
    std::map<std::string,ACAudioWaveformThumbnailSpecs> thumbnails_specs;
};

#endif
