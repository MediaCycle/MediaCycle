/**
 * @brief A class that allows to map features to visual variables to render a media node.
 * @author Christian Frisson
 * @date 29/03/2013
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

#ifndef __ACMediaNodeFeatureMapperThumbnailerPlugin_H__
#define __ACMediaNodeFeatureMapperThumbnailerPlugin_H__

#include <ACPlugin.h>
#include <ACOsgRendererPlugin.h>

#include <simple_svg.hpp>

#include <boost/any.hpp>
using boost::any_cast;

enum ACMediaNodeVisualVariable {
    AC_MEDIA_NODE_NONE=0,
    AC_MEDIA_NODE_SHAPE_MACRO, // global contour, for example, circle to square or octagon
    AC_MEDIA_NODE_SHAPE_MICRO, // clockwise variations on the contour of the shape, for timed features
    AC_MEDIA_NODE_COLOR_HUE,
    AC_MEDIA_NODE_COLOR_SATURATION,
    AC_MEDIA_NODE_COLOR_BRIGHTNESS,
    AC_MEDIA_NODE_COLOR_RED,
    AC_MEDIA_NODE_COLOR_GREEN,
    AC_MEDIA_NODE_COLOR_BLUE,
    AC_MEDIA_NODE_SIZE_DIAMETER,
    AC_MEDIA_NODE_SIZE_WIDTH,
    AC_MEDIA_NODE_SIZE_HEIGHT
    //AC_MEDIA_NODE_TEXTURE, // texture inside the area of the shape
};

enum ACMediaNodeShape {
    AC_MEDIA_NODE_SHAPE_CIRCLE,
    AC_MEDIA_NODE_SHAPE_SQUARE,
    AC_MEDIA_NODE_SHAPE_OVAL
};


class ACMediaNodeFeatureMapperThumbnailerPlugin : virtual public ACOsgRendererPlugin
{
public:
    ACMediaNodeFeatureMapperThumbnailerPlugin();
    virtual ~ACMediaNodeFeatureMapperThumbnailerPlugin(){}
    /*virtual std::vector<std::string> getThumbnailNames();
    virtual std::map<std::string,ACMediaType> getThumbnailTypes();
    virtual std::map<std::string,std::string> getThumbnailDescriptions();
    virtual std::map<std::string,std::string> getThumbnailExtensions();
    virtual std::string requiresMediaReaderPlugin();
    virtual std::vector<std::string> requiresFeaturesPlugins(); // list of plugin names (not paths)
    virtual std::vector<std::string> requiresSegmentationPlugins(); // list of plugin names (not paths)
    virtual std::vector<ACMediaThumbnail*> summarize(ACMedia* theMedia);*/
    // bool assignFeatureToVisualVariable(std::string name, ACMediaNodeVisualVariable variable,std::vector<boost::any> arguments);
    // bool assignFeatureToVisualVariable(std::string name, std::string variable,std::vector<boost::any> arguments);
    void assignedFeaturesChanged();
    virtual void mediaCycleSet();
    virtual void pluginLoaded(std::string plugin_name);

    virtual std::map<std::string, ACMediaType> getSupportedExtensions(ACMediaType media_type = MEDIA_TYPE_ALL){return std::map<std::string, ACMediaType>();}

protected:
    bool updateAvailableFeatures();
    std::map<ACMediaNodeVisualVariable,std::string> visual_features;
    std::vector<std::string> visual_variables;
    std::vector<std::string> visual_variable_color;
    std::vector<std::string> visual_variable_shape;
    std::vector<std::string> feature_names;
};

#endif
