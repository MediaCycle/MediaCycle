/**
 * @brief Plugin for computing node positions using the linear time NeRV algorithm
 * @author Christian Frisson
 * @date 25/02/2014
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

#include "linearnervlib.hh"

#include<armadillo>
#include "ACPosPlugLinearTimeNeRV.h"

using namespace arma;

ACPosPlugLinearTimeNeRV::ACPosPlugLinearTimeNeRV() : ACClusterPositionsPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "LinearTimeNeRV";
    this->mDescription = "Plugin for positioning items using the linear time Neighbor Retrieval Visualizer (NeRV) dimensionality reduction";
    this->mId = "";
}

ACPosPlugLinearTimeNeRV::~ACPosPlugLinearTimeNeRV() {
}

void ACPosPlugLinearTimeNeRV::updateNextPositions(ACMediaBrowser* mediaBrowser){

    if(!media_cycle) return;
    if(media_cycle->getLibrarySize()==0) return;

    int libSize = media_cycle->getLibrarySize();

    std::vector<long> ids = media_cycle->getLibrary()->getAllMediaIds();

    if(libSize != ids.size())
        std::cout << "ACPosPlugLinearTimeNeRV::updateNextPositions: warning, lib size and number of accessible media ids don't match"<< std::endl;

    arma::mat desc_m;
    vector<string> featureNames;
    this->extractDescMatrix(this->media_cycle->getBrowser(), desc_m, featureNames);
    //desc_m = desc_m.t();//arma::inplace_trans(desc_m); // required but contradictory with the example provided by mlpack

    arma::mat pos = linearnerv(desc_m);

    if(pos.n_rows == ids.size() && pos.n_cols == 2){
        ACPoint p;
        for (int i=0; i<ids.size(); i++){
            //mediaBrowser->setMediaNodeDisplayed(ids[i], true);
            p.x = pos(i,0);
            p.y = pos(i,1);
            p.z = 0;
            mediaBrowser->setNodeNextPosition(ids[i], p);
            std::cout << "ACPosPlugImportGrid::updateNextPositions: media " << ids[i] << " x " << p.x << " y " << p.y << std::endl;
        }
    }
}


void ACPosPlugLinearTimeNeRV::extractDescMatrix(ACMediaBrowser* mediaBrowser, arma::mat& desc_m, vector<string> &featureNames){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;
    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfFeaturesVectors();
    std::vector<float> weight=mediaBrowser->getWeightVector();
    if (nbFeature!=weight.size())
        std::cerr<<"ACPosPlugLinearTimeNeRV::extractDescMatrix weight vector size incompatibility"<<endl;
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
