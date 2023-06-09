/**
 * @brief ACVisPlugin2Desc.cpp
 * @author Christian Frisson
 * @date 10/03/2014
 * @copyright (c) 2014 – UMONS - Numediart
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

#include <armadillo>
#include "Armadillo-utils.h"
#include "ACVisPlugin2Desc.h"

using namespace arma;
using namespace std;

ACVisPlugin2Desc::ACVisPlugin2Desc() : ACClusterPositionsPlugin()
{
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle ScatterPlot";
    this->mDescription = "Visualization plugin with 1 feature per cartesian axis";
    this->mId = "";
    //local vars
}

void ACVisPlugin2Desc::mediaCycleSet()
{
    // CF this will work only since/if we load feature plugins before this plugin...
    this->updateAvailableFeatures();
}

void ACVisPlugin2Desc::pluginLoaded(std::string plugin_name){
    if(!media_cycle)
        return;
    ACPlugin* plugin = media_cycle->getPlugin(plugin_name);
    if(!plugin)
        return;
    if(plugin->implementsPluginType(PLUGIN_TYPE_FEATURES))
        this->updateAvailableFeatures();
}

void ACVisPlugin2Desc::assignedFeaturesChanged(){
    if(!this->media_cycle) return;
    //media_cycle->updateDisplay(true);
    media_cycle->setNeedsDisplay(true);
}

bool ACVisPlugin2Desc::updateAvailableFeatures(){
    if(this->media_cycle){
        if(this->media_cycle->getPluginManager()){
            std::vector<std::string> feature_names;
            /*std::vector<std::string> names = this->media_cycle->getListOfActivePlugins();
            for (std::vector<std::string>::iterator name = names.begin();name != names.end();name++){
                feature_names.push_back(*name + " (dim 0)");
                //std::cout << "ACVisPlugin2Desc: feature: " << *feature << std::endl;
            }*/
            std::vector<std::string> plugin_names = media_cycle->getPluginManager()->getAvailableFeaturesPluginsNames(media_cycle->getMediaType());
            for(std::vector<std::string>::iterator plugin_name = plugin_names.begin();plugin_name!=plugin_names.end();plugin_name++){
                ACFeaturesPlugin* plugin = dynamic_cast <ACFeaturesPlugin*> (media_cycle->getPlugin(*plugin_name));
                if(plugin){
                    ACFeatureDimensions featdims = plugin->getFeaturesDimensions();
                    for(ACFeatureDimensions::iterator featdim = featdims.begin(); featdim != featdims.end();featdim++){
                        //std::cout << "ACVisPlugin2Desc: feature: " << featdim->first << " of dim " << featdim->second << std::endl;
                        if(featdim->second == 1)
                            feature_names.push_back(featdim->first);
                        // Disabling from the drop-down list all features with dimensions different than 1 for now (waiting for dimension reduction solutions)`
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

            if(feature_names.size()>=1){
                if(this->hasStringParameterNamed("x"))
                    this->updateStringParameter("x",feature_names.front(),feature_names);
                else
                    this->addStringParameter("x",feature_names.front(),feature_names,"feature assigned to x axis",boost::bind(&ACVisPlugin2Desc::assignedFeaturesChanged,this));
                if(this->hasStringParameterNamed("y"))
                    this->updateStringParameter("y",feature_names.front(),feature_names);
                else
                    this->addStringParameter("y",feature_names.front(),feature_names,"feature assigned to y axis",boost::bind(&ACVisPlugin2Desc::assignedFeaturesChanged,this));
                return true;
            }
        }
    }
    return false;
}

ACVisPlugin2Desc::~ACVisPlugin2Desc()
{
}

void ACVisPlugin2Desc::updateNextPositions(ACMediaBrowser* mediaBrowser){
    if(!media_cycle) return;
    if(media_cycle->getLibrarySize()==0) return;

    if(!(this->hasStringParameterNamed("x") && this->hasStringParameterNamed("y"))){
        if(!(this->updateAvailableFeatures())){
            std::cout << "ACVisPlugin2Desc::updateNextPositions: available features not set" << std::endl;
            media_cycle->setNeedsDisplay(true);
            return;
        }
    }

    int itemClicked, labelClicked, action;
    vector<string> featureNames;
    int libSize = mediaBrowser->getLibrary()->getSize();
    itemClicked = mediaBrowser->getClickedNode();
    labelClicked = mediaBrowser->getClickedLabel();

    mat desc_m, descD_m;
    mat posDisp_m;

    extractDescMatrix(mediaBrowser, desc_m, featureNames);
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    if (desc_m.n_cols < 2){
        std::cout << "Not enough features for this display" << std::endl;
        ACPoint p;
        for (int i=0; i<ids.size(); i++){
            //mediaBrowser->setMediaNodeDisplayed(ids[i], true);
            // TODO: make sure you meant next
            p.x = 0.f;//posDisp_m(i,0);
            p.y = 0.f;//posDisp_m(i,1);
            p.z = 0.f;
            mediaBrowser->setNodeNextPosition(ids[i], p);
        }

        return;
    }

    for (int i=0; i< featureNames.size(); i++)
        std::cout << "featureNames : " << featureNames[i] << std::endl;

    mediaBrowser->setNumberOfDisplayedNodes(desc_m.n_rows);

    ACPoint p;
    for (int i=0; i<ids.size(); i++){
        //mediaBrowser->setMediaNodeDisplayed(ids[i], true);
        // TODO: make sure you meant next
        p.x = desc_m(i,0)*.1;
        p.y = desc_m(i,1)*.1;
        p.z = 0;
        mediaBrowser->setNodeNextPosition(ids[i], p);
    }
    ////////////////////////////////////////////////////////////////
}


void ACVisPlugin2Desc::extractDescMatrix(ACMediaBrowser* mediaBrowser, mat& desc_m, vector<string> &featureNames){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;
    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfFeaturesVectors();

    // CF was meant to activate the 2 first selected features from the check list
    //std::vector<float> weight=mediaBrowser->getWeightVector();
    /*if (nbFeature!=weight.size())
        std::cerr<<"ACVisPluginPCA::extractDescMatrix weight vector size incompatibility"<<endl;
    for(int f=0; f< nbFeature; f++){
        if (weight[f]>0.f){
            featureNames.push_back(mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getName());
            featDim = mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getSize();
            for(int d=0; d < featDim; d++){
                totalDim++;
            }
        }
    }

    desc_m.set_size(nbMedia,totalDim);

    int i = 0;
    for(ACMedias::iterator media = medias.begin();media!=medias.end();media++){
        int tmpIdx = 0;
        for(int f=0; f< nbFeature; f++){
            if (weight[f]>0.f){
                std::cout << f << std::endl;
                featDim = mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getSize();
                for(int d=0; d < featDim; d++){
                    desc_m(i++,tmpIdx) = media->second->getPreProcFeaturesVector(f)->getFeatureElement(d);
                    tmpIdx++;
                }
            }
        }
    }*/

    //CF getting the first dimension of the features assigned to x and y
    std::vector<float> weight ( mediaBrowser->getWeightVector().size() );
    std::string x = this->getStringParameterValue("x");
    int xdim = 0;
    size_t x_pos = x.find(" (dim ");
    if(x_pos != std::string::npos){
        size_t end = x.find_last_of(")");
        std::string dim = x.substr(x_pos+6,end-x_pos-6);
        std::stringstream ndim;
        ndim << dim;
        ndim >> xdim;
        x = x.substr(0,x_pos);
        std::cout << "X: dim '" << dim << "' " << xdim << std::endl;
    }
    /*size_t x_pos = x.find(" (dim");
    if(x_pos != std::string::npos)
        x = x.substr(0,x_pos);*/
    std::string y = this->getStringParameterValue("y");
    int ydim = 0;
    size_t y_pos = y.find(" (dim ");
    if(y_pos != std::string::npos){
        size_t end = y.find_last_of(")");
        std::string dim = y.substr(y_pos+6,end-y_pos-6);
        std::stringstream ndim;
        ndim << dim;
        ndim >> ydim;
        y = y.substr(0,y_pos);
        std::cout << "Y: dim '" << dim << "' " << ydim << std::endl;
    }
    /*size_t y_pos = y.find(" (dim");
    if(y_pos != std::string::npos)
        y = y.substr(0,y_pos);*/

    desc_m.set_size(nbMedia,2);

    ACMediaType library_type = mediaBrowser->getLibrary()->getMediaType();
    int i=0;
    for(ACMedias::iterator media = medias.begin();media!=medias.end();media++){
        //std::cout << "ACVisPlugin2Desc::extractDescMatrix: media " << media->first << " '" << media->second->getFileName() << "'" << std::endl;
        ACMedia* current = 0;
        desc_m(i,0) = 0;
        desc_m(i,1) = 0;

        /*if(library_type == media->second->getMediaType() ) {
            current = media->second;
        }
        else {
            int parentId = media->second->getParentId();
            if (parentId > -1){
                current = mediaBrowser->getLibrary()->getMedia(parentId);
                if(!current){
                     std::cerr << "ACVisPlugin2Desc::extractDescMatrix: media of id " << media->first << " has no parent" << std::endl;
                }
            }
            else{
                std::cerr << "ACVisPlugin2Desc::extractDescMatrix: media of id " << media->first << " has no parent id" << std::endl;
            }
        }*/
        current = media->second; //CF test media documents

        if(current){
            ACMediaFeatures* x_feat = current->getPreProcFeaturesVector(x);
            if(x_feat){
                desc_m(i,0) = x_feat->getFeatureElement(xdim);
            }
            ACMediaFeatures* y_feat = current->getPreProcFeaturesVector(y);
            if(y_feat){
                desc_m(i,1) = y_feat->getFeatureElement(ydim);
            }
        }

        i++;
    }

    // normalizing features between 0 and 1 ///////////////////////////////////////
    //   rowvec maxDesc_v = max(desc_m);
    //   rowvec minDesc_v = min(desc_m);
    //   desc_m = desc_m - repmat(minDesc_v, desc_m.n_rows, 1);
    //   desc_m = desc_m/repmat(maxDesc_v-minDesc_v, desc_m.n_rows, 1);
    /////////////////////////////////////////////////////////////////////////////////
}

