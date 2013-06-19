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

#include <ACMediaNodeFeatureMapperThumbnailerPlugin.h>

ACMediaNodeFeatureMapperThumbnailerPlugin::ACMediaNodeFeatureMapperThumbnailerPlugin()
    :ACOsgRendererPlugin()
{
    this->mName = "Feature Mapper Thumbnailer";
    this->mDescription ="Plugin for creating media node thumbnails in SVG based on features";
    this->mMediaType = MEDIA_TYPE_ALL;

    //visual_variables.push_back("Color Hue");
    //visual_variables.push_back("Color Saturation");
    visual_variables.push_back("Color Brightness");
    //visual_variables.push_back("Contour Clockwise");
    //visual_variables.push_back("Size");
}

/*std::string ACMediaNodeFeatureMapperThumbnailerPlugin::requiresMediaReaderPlugin()
{
    return "";
}

std::vector<std::string> ACMediaNodeFeatureMapperThumbnailerPlugin::requiresFeaturesPlugins(){
    std::vector<std::string> features_plugins;
    features_plugins.push_back("File Features"); // to require at least some features
    return features_plugins;
}

std::vector<std::string> ACMediaNodeFeatureMapperThumbnailerPlugin::requiresSegmentationPlugins(){
    std::vector<std::string> segmentation_plugins;
    // No segmentation plugin is required
    return segmentation_plugins;
}

std::vector<ACMediaThumbnail*> ACMediaNodeFeatureMapperThumbnailerPlugin::summarize(ACMedia* media){
    std::vector<ACMediaThumbnail*> thumbnails;
    return thumbnails;
}*/

void ACMediaNodeFeatureMapperThumbnailerPlugin::mediaCycleSet(){
    ACMediaType _media_type = media_cycle->getMediaType();

    int size = feature_names.size();

    std::vector<std::string> features_plugins = media_cycle->getPluginManager()->getAvailablePluginsNames(PLUGIN_TYPE_FEATURES, media_cycle->getMediaType());
    for(std::vector<std::string>::iterator features_plugin = features_plugins.begin();features_plugin!=features_plugins.end();features_plugin++){
        ACFeaturesPlugin* plugin = dynamic_cast<ACFeaturesPlugin*>(media_cycle->getPluginManager()->getPlugin(*features_plugin));
        if(plugin){
            ACFeatureDimensions featdims = plugin->getFeaturesDimensions();
            for(ACFeatureDimensions::iterator featdim = featdims.begin(); featdim != featdims.end();featdim++){
                //std::cout << "ACVisPlugin2Desc: feature: " << featdim->first << " of dim " << featdim->second << std::endl;
                if(featdim->second == 1)
                    feature_names.push_back(featdim->first);
                // Disabling from the drop-down list all features with dimensions different than 1 for now (waiting for dimension reduction solutions)
                /*else if(featdim->second > 1){
                    for(int f=0;f<featdim->second;f++){
                        std::stringstream name;
                        name << featdim->first << " (dim " << f << ")";
                        feature_names.push_back(name.str());
                    }
                }*/
            }
        }
    }
    if(size != feature_names.size())
        this->updateAvailableFeatures();
}

void ACMediaNodeFeatureMapperThumbnailerPlugin::pluginLoaded(std::string plugin_name){
    if(!media_cycle)
        return;

    ACPlugin* _plugin = media_cycle->getPlugin(plugin_name);
    if(!_plugin)
        return;

    ACMediaType _media_type = media_cycle->getMediaType();

    int size = feature_names.size();

    if(_plugin->implementsPluginType(PLUGIN_TYPE_FEATURES)){
        ACFeaturesPlugin * plugin = dynamic_cast<ACFeaturesPlugin*>(_plugin);
        if(plugin){
            ACFeatureDimensions featdims = plugin->getFeaturesDimensions();
            for(ACFeatureDimensions::iterator featdim = featdims.begin(); featdim != featdims.end();featdim++){
                //std::cout << "ACVisPlugin2Desc: feature: " << featdim->first << " of dim " << featdim->second << std::endl;
                if(featdim->second == 1)
                    feature_names.push_back(featdim->first);
                else if(featdim->second > 1){
                    for(int f=0;f<featdim->second;f++){
                        std::stringstream name;
                        name << featdim->first << " (dim " << f << ")";
                        feature_names.push_back(name.str());
                    }
                }
            }
            if(size != feature_names.size())
                this->updateAvailableFeatures();
        }
    }
}

bool ACMediaNodeFeatureMapperThumbnailerPlugin::updateAvailableFeatures(){
    if(feature_names.size()>=1){
        std::vector<std::string> _feature_names(feature_names);
        _feature_names.push_back("None");
        for(std::vector<std::string>::iterator visual_variable = visual_variables.begin(); visual_variable != visual_variables.end(); visual_variable++){
            if(this->hasStringParameterNamed(*visual_variable))
                this->updateStringParameter(*visual_variable,_feature_names.back(),_feature_names);
            else
                this->addStringParameter(*visual_variable,_feature_names.back(),_feature_names,*visual_variable,boost::bind(&ACMediaNodeFeatureMapperThumbnailerPlugin::assignedFeaturesChanged,this));
        }
        return true;
    }
    return false;
}

// http://en.wikipedia.org/wiki/HSL_and_HSV
typedef struct {
    double r;       // percent
    double g;       // percent
    double b;       // percent
} rgb;

typedef struct {
    double h;       // angle in degrees [0°;360°]
    double s;       // percent [0;1]
    double v;       // percent [0;1]
} hsv;

hsv rgb2hsv(rgb in)
{
    hsv out;
    double cmax = max(max(in.r,in.g),in.b);
    double cmin = min(min(in.r,in.g),in.b);
    double c = cmax - cmin;
    double h = 0.0f;
    double s = c/cmax;
    if(c==0){
        std::cerr << "rgb2hsv: hue undefined"<< std::endl;
        s = 0.0f;
    }
    else if (cmax == in.r){
        h = ((int)floor((in.g-in.b)/c))%6;
        //h = (in.g-in.b)/c;
        //h = h - floor(h/6.0f)*6; // %6
    }
    else if (cmax == in.g){
        h = ((in.b-in.r)/c)+2;
    }
    else if (cmax == in.b){
        h = ((in.r-in.g)/c)+4;
    }
    out.h = h*60.0f;
    out.s = s;
    out.v = cmax;
    return out;
}

rgb hsv2rgb(hsv in)
{
    rgb out;
    double c = in.v * in.s; // chroma
    double h = in.h/60.0f;
    //double x = c*(1-abs(h%2-1));
    double x = c*(1-abs(((int)floor(h))%2-1));
    double r1(0.0f), g1(0.0f), b1(0.0f);
    if(h<=0 && h<1){
        r1 = c;
        g1 = x;
    }
    else if(h<=1 && h<2){
        r1 = x;
        g1 = c;
    }
    else if(h<=2 && h<3){
        g1 = c;
        b1 = x;
    }
    else if(h<=3 && h<4){
        g1 = x;
        b1 = c;
    }
    else if(h<=4 && h<5){
        r1 = x;
        b1 = c;
    }
    else if(h<=5 && h<6){
        r1 = c;
        b1 = x;
    }
    double m = in.v - c;
    out.r = r1 + m;
    out.g = g1 + m;
    out.b = b1 + m;
    return out;
}

void ACMediaNodeFeatureMapperThumbnailerPlugin::assignedFeaturesChanged(){
    if(!this->media_cycle) return;
    if(this->media_cycle->getLibrarySize()==0) return;
    if(!this->browser){
        std::cerr << "ACMediaNodeFeatureMapperThumbnailerPlugin::assignedFeaturesChanged: couldn't access osg browser" << std::endl;
        return;
    }
    std::string color_h_feature_name = this->getStringParameterValue("Color Hue");
    std::string color_s_feature_name = this->getStringParameterValue("Color Saturation");
    std::string color_b_feature_name = this->getStringParameterValue("Color Brightness");

    if(color_b_feature_name == "None"){
        std::cerr << "ACMediaNodeFeatureMapperThumbnailerPlugin::assignedFeaturesChanged: for now just testing color brightness, no feature is assigned" << std::endl;
        return;
    }

    int color_b_feature_dim = 0;
    size_t dim_pos = color_b_feature_name.find(" (dim ");
    if(dim_pos != std::string::npos){
        size_t end = color_b_feature_name.find_last_of(")");
        std::string dim = color_b_feature_name.substr(dim_pos+6,end-dim_pos-6);
        std::stringstream ndim;
        ndim << dim;
        ndim >> color_b_feature_dim;
        color_b_feature_name = color_b_feature_name.substr(0,dim_pos);
        std::cout << "Feat: '" << color_b_feature_name << "' " << color_b_feature_dim << std::endl;
    }

    ACMedias medias = media_cycle->getLibrary()->getAllMedia();

    // Min/max storage should be moved to the core for efficiency
    ACMedias::iterator _media = medias.begin();
    ACMediaFeatures* _feature = _media->second->getFeaturesVector(color_b_feature_name);
    float color_b_feature_min = 0.0f;
    float color_b_feature_max = 0.0f;
    if(_feature){
        color_b_feature_min = _feature->getFeatureElement(color_b_feature_dim);
        color_b_feature_max = _feature->getFeatureElement(color_b_feature_dim);
    }
    for(;_media!=medias.end();_media++){
        _feature = _media->second->getFeaturesVector(color_b_feature_name);
        if(_feature){
            color_b_feature_min = min(color_b_feature_min,_feature->getFeatureElement(color_b_feature_dim));
            color_b_feature_max = max(color_b_feature_max,_feature->getFeatureElement(color_b_feature_dim));
        }
        _feature = 0;
    }

    for(ACMedias::iterator media = medias.begin();media!=medias.end();media++){
        /*ACMediaThumbnail* thumbnail = (*media)->getThumnbnail("");
        std::vector<ACMediaThumbnail*> thumbnails = this->summarize(*media);
        if(thumbnails.size()==1){
            if(!thumbnail){
                thumbnail = thumbnails.front();
                (*media)->addThumbnail(thumbnail);
            }
            else{
                delete thumbnail;
                thumbnail = thumbnails.front();
            }
        }*/

        rgb rgbc;
        rgbc.r = 0.5f;
        rgbc.g = 0.5f;
        rgbc.b = 0.5f;

        hsv hsvc;
        hsvc = rgb2hsv(rgbc);

        ACMediaFeatures* color_b_feature = media->second->getFeaturesVector(color_b_feature_name);
        if(color_b_feature){
            hsvc.v = (color_b_feature->getFeatureElement(color_b_feature_dim)-color_b_feature_min)/(color_b_feature_max-color_b_feature_min);
        }

        rgb rgb2 = hsv2rgb(hsvc);

        if(color_b_feature){
            std::cout << "Media " << media->first << " feature " << color_b_feature_name << " value " << color_b_feature->getFeatureElement(color_b_feature_dim) << " -> " << hsvc.v << " HSV ("<< hsvc.h << ";" << hsvc.s << ";" << hsvc.v << ") RGB (" << rgb2.r << ";" << rgb2.g << ";" << rgb2.b << ")" << std::endl;
        }
        browser->changeNodeColor(media->first,osg::Vec4(rgb2.r,rgb2.g,rgb2.b,1.0f));
    }
    media_cycle->updateDisplay(true);
    media_cycle->setNeedsDisplay(true);
}
