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

#include <ACMediaFeatureMapperRendererPlugin.h>

#include "mlpack/core.hpp"
#include "emst.h"

#include "mlpack/methods/neighbor_search/neighbor_search.hpp"

#include <mlpack/core/util/version.hpp>

#if(MLPACK_VERSION_MAJOR >= 3)
#include "mlpack/methods/neighbor_search/ns_model.hpp"
#endif

using namespace mlpack;
using namespace mlpack::neighbor;
#if(MLPACK_VERSION_MAJOR >= 3)
using namespace mlpack::tree;
using namespace mlpack::metric;
using namespace mlpack::util;
// Convenience typedef.
typedef NSModel<NearestNeighborSort> KNNModel;
#endif

using namespace arma;

ACMediaFeatureMapperRendererPlugin::ACMediaFeatureMapperRendererPlugin()
    :ACAbstractRendererPlugin(),ACMediaRendererPlugin()
{
    //this->mName = "Feature Mapper Thumbnailer";
    this->mName = "Feature Mapper";
    this->mDescription ="Plugin for creating media node thumbnails based on features";
    this->mMediaType = MEDIA_TYPE_ALL;

    //visual_variables.push_back("Color Hue");
    //visual_variables.push_back("Color Saturation");
    visual_variables.push_back("Color Brightness");
    //visual_variables.push_back("Contour Clockwise");
    //visual_variables.push_back("Size");
    visual_variables.push_back("Link Brightness");

    distances.push_back("Features");
    distances.push_back("Coordinates");
    this->addStringParameter("Link Weight",distances.front(),distances,"Dimensions to compute minimum spanning tree");
    sortings.push_back("Closest Neighbor");
    sortings.push_back("Minimum Spanning Tree");
    this->addStringParameter("Link Method",sortings.front(),sortings,"Sorting");
    this->addNumberParameter("Link Neighbors",1,1,5,1,"Number of closest neighbors to consider when linking");

    this->addCallback("Link Update","Update links",boost::bind(&ACMediaFeatureMapperRendererPlugin::updateLinks,this));
    this->addCallback("Link Clear","Clear links",boost::bind(&ACMediaFeatureMapperRendererPlugin::clearLinks,this));

}

void ACMediaFeatureMapperRendererPlugin::clearLinks() {
    if(this->browser){
        this->browser->removeLinks();
    }
}

/*std::string ACMediaFeatureMapperRendererPlugin::requiresMediaReaderPlugin()
{
    return "";
}

std::vector<std::string> ACMediaFeatureMapperRendererPlugin::requiresFeaturesPlugins(){
    std::vector<std::string> features_plugins;
    features_plugins.push_back("File Features"); // to require at least some features
    return features_plugins;
}

std::vector<std::string> ACMediaFeatureMapperRendererPlugin::requiresSegmentationPlugins(){
    std::vector<std::string> segmentation_plugins;
    // No segmentation plugin is required
    return segmentation_plugins;
}

std::vector<ACMediaThumbnail*> ACMediaFeatureMapperRendererPlugin::summarize(ACMedia* media){
    std::vector<ACMediaThumbnail*> thumbnails;
    return thumbnails;
}*/

void ACMediaFeatureMapperRendererPlugin::mediaCycleSet(){
    ACMediaType _media_type = media_cycle->getMediaType();

    int size = feature_names.size();

    std::vector<std::string> features_plugins = media_cycle->getPluginManager()->getAvailablePluginsNames(PLUGIN_TYPE_FEATURES, media_cycle->getMediaType());
    for(std::vector<std::string>::iterator features_plugin = features_plugins.begin();features_plugin!=features_plugins.end();features_plugin++){
        ACFeaturesPlugin* plugin = dynamic_cast<ACFeaturesPlugin*>(media_cycle->getPluginManager()->getPlugin(*features_plugin));
        if(plugin){
            ACFeatureDimensions featdims = plugin->getFeaturesDimensions();
            for(ACFeatureDimensions::iterator featdim = featdims.begin(); featdim != featdims.end();featdim++){
                std::cout << "ACMediaFeatureMapperRendererPlugin: feature: " << featdim->first << " of dim " << featdim->second << std::endl;
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
    //if(size != feature_names.size())
    this->updateAvailableFeatures();
}

void ACMediaFeatureMapperRendererPlugin::pluginLoaded(std::string plugin_name){
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

void ACMediaFeatureMapperRendererPlugin::mediaLoaded(int n,int nTot,int mId){
    if(n==nTot){
        this->assignedFeaturesChanged();
        std::cout << "ACMediaFeatureMapperRendererPlugin::mediaLoaded" << std::endl;
    }
}

void ACMediaFeatureMapperRendererPlugin::mediasLoaded(int n,int nTot,std::vector<int> mIds){
    //if(n==nTot){
    this->assignedFeaturesChanged();
    std::cout << "ACMediaFeatureMapperRendererPlugin::mediasLoaded" << std::endl;
    //}
}

bool ACMediaFeatureMapperRendererPlugin::updateAvailableFeatures(){
    if(feature_names.size()>=1){
        std::vector<std::string> _feature_names(feature_names);
        _feature_names.push_back("None");
        for(std::vector<std::string>::iterator visual_variable = visual_variables.begin(); visual_variable != visual_variables.end(); visual_variable++){
            if(this->hasStringParameterNamed(*visual_variable))
                this->updateStringParameter(*visual_variable,_feature_names.back(),_feature_names,*visual_variable,boost::bind(&ACMediaFeatureMapperRendererPlugin::assignedFeaturesChanged,this));
            else
                this->addStringParameter(*visual_variable,_feature_names.back(),_feature_names,*visual_variable,boost::bind(&ACMediaFeatureMapperRendererPlugin::assignedFeaturesChanged,this));
        }
        return true;
    }
    return false;
}



hsv ACMediaFeatureMapperRendererPlugin::rgb2hsv(rgb in)
{
    hsv out;
    double cmax = max(max(in.r,in.g),in.b);
    double cmin = min(min(in.r,in.g),in.b);
    double c = cmax - cmin;
    double h = 0.0f;
    double s = c/cmax;
    if(c==0){
        //std::cerr << "rgb2hsv: hue undefined"<< std::endl;
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

rgb ACMediaFeatureMapperRendererPlugin::hsv2rgb(hsv in)
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

void ACMediaFeatureMapperRendererPlugin::assignedFeaturesChanged(){
    if(!this->media_cycle) return;
    if(this->media_cycle->getLibrarySize()==0) return;
    if(!this->browser){
        std::cerr << "ACMediaFeatureMapperRendererPlugin::assignedFeaturesChanged: couldn't access osg browser" << std::endl;
        return;
    }
    std::string color_h_feature_name = this->getStringParameterValue("Color Hue");
    std::string color_s_feature_name = this->getStringParameterValue("Color Saturation");
    std::string color_b_feature_name = this->getStringParameterValue("Color Brightness");

    if(color_b_feature_name == "None"){
        std::cerr << "ACMediaFeatureMapperRendererPlugin::assignedFeaturesChanged: for now just testing color brightness, no feature is assigned" << std::endl;
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
            //std::cout << "Id " << media->second->getId() << " hsV " << hsvc.v << std::endl;
        }

        rgb rgb2 = hsv2rgb(hsvc);

        /*if(color_b_feature){
            std::cout << "Media " << media->first << " feature " << color_b_feature_name << " value " << color_b_feature->getFeatureElement(color_b_feature_dim) << " -> " << hsvc.v << " HSV ("<< hsvc.h << ";" << hsvc.s << ";" << hsvc.v << ") RGB (" << rgb2.r << ";" << rgb2.g << ";" << rgb2.b << ")" << std::endl;
        }*/
        browser->changeNodeColor(media->first,ACColor(rgb2.r,rgb2.g,rgb2.b,1.0f));
    }
    media_cycle->setNeedsDisplay(true);
}


void ACMediaFeatureMapperRendererPlugin::extractDescMatrix(ACMediaBrowser* mediaBrowser, arma::mat& desc_m, vector<string> &featureNames){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;
    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfFeaturesVectors();
    std::vector<float> weight=mediaBrowser->getWeightVector();
    if (nbFeature!=weight.size())
        std::cerr<<"ACFilterPlugProximityGrid::extractDescMatrix weight vector size incompatibility"<<endl;
    for(int f=0; f< nbFeature; f++){
        if (weight[f]>0.f){
            featureNames.push_back(mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getName());
            featDim = mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getSize();
            for(int d=0; d < featDim; d++){
                totalDim++;
            }
        }
    }
    int cpt=0;
    for (long i=0; i<ids.size(); i++){
        if (mediaBrowser->getMediaNode(ids[i]) && mediaBrowser->getMediaNode(ids[i])->isDisplayed()){
            cpt++;
        }
    }

    desc_m.set_size(cpt,totalDim);
    int tmpIdy=0;
    for(int i=0; i<nbMedia; i++) {
        int tmpIdx = 0;
        if (mediaBrowser->getMediaNode(ids[i]) && !(mediaBrowser->getMediaNode(ids[i])->isDisplayed()))
            continue;

        for(int f=0; f< nbFeature; f++){
            if (weight[f]>0.f){

                //std::cout << f << std::endl;
                featDim = mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getSize();
                for(int d=0; d < featDim; d++){
                    desc_m(tmpIdy,tmpIdx) = medias[ids[i]]->getPreProcFeaturesVector(f)->getFeatureElement(d);
                    tmpIdx++;
                }
            }
        }
        if(mediaBrowser->getMediaNode(ids[i])){
            tmpIdy++;
        }
    }
}

void ACMediaFeatureMapperRendererPlugin::updateLinks(){
    if(!this->media_cycle) return;

    int libSize = media_cycle->getLibrarySize();

    if(libSize==0) return;
    if(!this->browser){
        std::cerr << "ACDistanceNodeLinkPlugin::updateLinks: couldn't access osg browser" << std::endl;
        return;
    }

    if(this->browser){
        this->browser->removeLinks();
    }

    std::vector<long> ids = media_cycle->getLibrary()->getAllMediaIds();

    bool visual_mst = false;

    std::string distance = this->getStringParameterValue("Link Weight");
    std::string sorting = this->getStringParameterValue("Link Method");
    if (distance == "Features")
        visual_mst = false;

    arma::Mat<size_t> resultingNeighbors;
    arma::mat resultingDistances;

    arma::mat desc_m;
    arma::mat mst;

    if(visual_mst)
        desc_m.set_size(libSize,2);//(2,libSize);
    vector<string> featureNames;
    if(!visual_mst)
        this->extractDescMatrix(this->media_cycle->getBrowser(), desc_m, featureNames);

    desc_m = desc_m.t();//arma::inplace_trans(desc_m); // required but contradictory with the example provided by mlpack


    int k = this->getNumberParameterValue("Link Neighbors");

    if(sorting == "Minimum Spanning Tree"){

        // Euclidean Minimum Spanning Tree from methodss/emst/emst_main.cpp
        // using the dual-tree Boruvka algorithm.
        // The output is saved in a three-column matrix, where each row indicates an
        // edge.  The first column corresponds to the lesser index of the edge; the
        // second column corresponds to the greater index of the edge; and the third
        // column corresponds to the distance between the two points.
        // can be using O(n^2) naive algorithm
        // leaf_size: leaf size in the kd-tree.  One-element leaves give the "
        // empirically best performance, but at the cost of greater memory requirements. default 1);
        bool naive = false;
        const size_t leafSize = 1;
        mst = emst(desc_m,naive,leafSize);
    }
    else if(sorting == "Closest Neighbor"){

        // Our dataset matrices, which are column-major.
        #if(MLPACK_VERSION_MAJOR >= 3)
        KNNModel a;
        int leaf_size = 20;
        a.TreeType() = KNNModel::KD_TREE;
        a.LeafSize() = size_t(leaf_size);
        a.BuildModel(std::move(desc_m), size_t(leaf_size), DUAL_TREE_MODE,
        0.0);
        #else
        AllkNN a(desc_m);
        #endif
        //mlpack::data::Save("./desc_m.csv", desc_m, true);
        a.Search(k, resultingNeighbors, resultingDistances);

        mlpack::data::Save("./resultingNeighbors.csv", resultingNeighbors, true);
        //mlpack::data::Save("./resultingDistances.csv", resultingDistances, true);
        ////mlpack::data::Save("./mst.csv", mst, true);
        //mst = arma::join_rows(resultingNeighbors,resultingDistances);
    }
    media_cycle->getBrowser()->setLayout(AC_LAYOUT_TYPE_NODELINK);
    std::string color_b_feature_name = this->getStringParameterValue("Link Brightness");

    /*if(color_b_feature_name == "None"){
        std::cerr << "ACMediaFeatureMapperRendererPlugin::assignedFeaturesChanged: for now just testing color brightness, no feature is assigned" << std::endl;
        return;
    }*/

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

    // Considering closest MST edges first for empty strategy, farthest otherwise
    int increment = 1;
    int init = 0;

    //    increment = -1;
    //    init = mst.n_cols-1;

    int _size = 0;
    if(sorting == "Minimum Spanning Tree"){
        _size = mst.n_cols;
    }
    else if(sorting == "Closest Neighbor"){
        _size = resultingNeighbors.n_cols;
    }

    ACColor color(1.0f,1.0f,1.0f,1.0f);

    for(int e = init; e >= 0 && e < _size;e+=increment){

        int in = 0;
        int out = 0;
        float dist = 0;

        if(sorting == "Minimum Spanning Tree"){
            in = mst(0,e);
            out = mst(1,e);
            dist = mst(2,e);
            //std::cout << "Edge " << e << " " << in << " " << out << " d " << dist << std::endl;
            if(this->browser){
                this->browser->addLink( in , out, 10.0f/dist, color );
            }
        }
        else if(sorting == "Closest Neighbor"){
            in = ids[e];
            for(int n=0;n<k;n++){
                out = resultingNeighbors(n,e);
                dist = resultingDistances(n,e);
                //std::cout << "Edge " << e << " " << in << " " << out << " d " << dist << std::endl;
                if(this->browser){
                    color.a = 1/(float)(n+1);
                    this->browser->addLink( in , out, 10.0f/dist, color );
                }
            }
        }
    }
    media_cycle->setNeedsDisplay(true);
}

std::vector<ACMediaType> ACMediaFeatureMapperRendererPlugin::getSupportedMediaTypes(){
    std::vector<ACMediaType> media_types;
    media_types.push_back(MEDIA_TYPE_AUDIO);
    return media_types;
}
