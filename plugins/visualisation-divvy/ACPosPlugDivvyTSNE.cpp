/**
 * @brief Plugin for computing node positions using Divvy's t-SNE
 * @author Christian Frisson
 * @date 27/02/2014
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

extern "C" {
#include "tsne.h"
}

#include<armadillo>
#include "ACPosPlugDivvyTSNE.h"

using namespace arma;

ACPosPlugDivvyTSNE::ACPosPlugDivvyTSNE() : ACClusterPositionsPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "Divvy TSNE";
    this->mDescription = "Plugin for positioning items using Divvy's tSNE dimensionality reduction";
    this->mId = "";

    this->addNumberParameter("Perplexity",10,1,10000,10,"Value of desired perplexity");
    this->addNumberParameter("Maximum iterations",2000,10,10000,50,"Number of maximum iterations");
}

ACPosPlugDivvyTSNE::~ACPosPlugDivvyTSNE() {
}

void ACPosPlugDivvyTSNE::updateNextPositions(ACMediaBrowser* mediaBrowser){

    if(!media_cycle) return;
    if(media_cycle->getLibrarySize()==0) return;

    int libSize = media_cycle->getLibrarySize();

    std::vector<long> ids = media_cycle->getLibrary()->getAllMediaIds();

    if(libSize != ids.size())
        std::cout << "ACPosPlugDivvyTSNE::updateNextPositions: warning, lib size and number of accessible media ids don't match"<< std::endl;

    arma::mat desc_m;
    vector<string> featureNames;
    this->extractDescMatrix(this->media_cycle->getBrowser(), desc_m, featureNames);
    //desc_m = desc_m.t();//arma::inplace_trans(desc_m); // required but contradictory with the example provided by mlpack

    float perplexity = this->getNumberParameterValue("Perplexity");
    float max_iter = this->getNumberParameterValue("Maximum iterations");

    float* pos = (float*) malloc(desc_m.n_rows * 2 * sizeof(float));;
    float* desc= (float*) malloc(desc_m.n_rows * desc_m.n_cols * sizeof(float));;
    for(int n = 0; n < desc_m.n_rows; n++) {
        for(int d = 0; d < desc_m.n_cols; d++) {
            desc[n * desc_m.n_cols + d] = desc_m(n,d);
        }
    }

    perform_tsne(desc, desc_m.n_cols, desc_m.n_rows, pos, 2, perplexity, (int)max_iter,1);

    float osg = 0.33f;
    if(pos){//pos.n_rows == ids.size() && pos.n_cols == 2){
        ACPoint p;
        for (int i=0; i<ids.size(); i++){
            //mediaBrowser->setMediaNodeDisplayed(ids[i], true);
            p.x = -osg + 2*osg*pos[i*2+0];//pos(i,0);
            p.y = -osg + 2*osg*pos[i*2+1];//pos(i,1);
            p.z = 0;
            mediaBrowser->setNodeNextPosition(ids[i], p);
            //std::cout << "ACPosPlugDivvyTSNE::updateNextPositions: media " << ids[i] << " x " << p.x << " y " << p.y << std::endl;
        }
    }
    free(pos);
    free(desc);
}


void ACPosPlugDivvyTSNE::extractDescMatrix(ACMediaBrowser* mediaBrowser, arma::mat& desc_m, vector<string> &featureNames){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;
    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfFeaturesVectors();
    std::vector<float> weight=mediaBrowser->getWeightVector();
    if (nbFeature!=weight.size())
        std::cerr<<"ACPosPlugDivvyTSNE::extractDescMatrix weight vector size incompatibility"<<endl;
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
