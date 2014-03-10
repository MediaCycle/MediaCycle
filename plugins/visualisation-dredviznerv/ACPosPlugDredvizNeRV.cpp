/**
 * @brief Plugin for computing node positions using the Dredviz NeRV algorithm
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

#include "nervlib.hh"

#include<armadillo>
#include "ACPosPlugDredvizNeRV.h"

using namespace arma;

ACPosPlugDredvizNeRV::ACPosPlugDredvizNeRV() : ACClusterPositionsPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "Dredviz NeRV";
    this->mDescription = "Plugin for positioning items using the dredviz Neighbor Retrieval Visualizer (NeRV) dimensionality reduction";
    this->mId = "";

    this->addNumberParameter("Lambda",0.1,0,1,0.01,"Control the trustworthiness-continuity tradeoff. A value of zero emphasizes trustworthiness only; increasing L shifts the emphasis towards continuity. L must be a floating point number between 0 and 1, but in practice values higher than 0.5 are not recommended.");
    this->addNumberParameter("Neighbors",20,1,100,1,"Set the number of neighbors that each point should have: each point will consider the K points closest to it its neighbors. K should be a positive integer.");
    this->addNumberParameter("Iterations",10,1,100,1,"The number of iterations to perform.");
    this->addNumberParameter("Conjugate Gradient Steps",2,1,100,1,"The number of conjugate gradient steps to perform in NeRV's optimization scheme.");
    this->addNumberParameter("Final Gradient Steps",40,1,100,1,"The number of conjugate gradient steps to perform on the final iteration in NeRV's optimization scheme.");
}

ACPosPlugDredvizNeRV::~ACPosPlugDredvizNeRV() {
}

void ACPosPlugDredvizNeRV::updateNextPositions(ACMediaBrowser* mediaBrowser){

    if(!media_cycle) return;
    if(media_cycle->getLibrarySize()==0) return;

    int libSize = media_cycle->getLibrarySize();

    std::vector<long> ids = media_cycle->getLibrary()->getAllMediaIds();

    if(libSize != ids.size())
        std::cout << "ACPosPlugDredvizNeRV::updateNextPositions: warning, lib size and number of accessible media ids don't match"<< std::endl;

    arma::mat desc_m;
    vector<string> featureNames;
    this->extractDescMatrix(this->media_cycle->getBrowser(), desc_m, featureNames);
    //desc_m = desc_m.t();//arma::inplace_trans(desc_m); // required but contradictory with the example provided by mlpack

    double lambda = this->getNumberParameterValue("Lambda");
    size_t lastNeighbor = this->getNumberParameterValue("Neighbors");
    size_t iterations = this->getNumberParameterValue("Iterations");
    size_t stepsPerRound = this->getNumberParameterValue("Conjugate Gradient Steps");
    size_t stepsOnLastRound = this->getNumberParameterValue("Final Gradient Steps");
    arma::mat pos = nerv(desc_m, lambda, lastNeighbor, iterations, stepsPerRound, stepsOnLastRound);

    float min_x = arma::min(pos.col(0));
    float min_y = arma::min(pos.col(1));
    float max_x = arma::max(pos.col(0));
    float max_y = arma::max(pos.col(1));

    float osg = 0.33f;

    if(pos.n_rows == ids.size() && pos.n_cols == 2){
        ACPoint p;
        for (int i=0; i<ids.size(); i++){
            //mediaBrowser->setMediaNodeDisplayed(ids[i], true);
            p.x = -osg +2*osg*(pos(i,0)-min_x)/(max_x-min_x);
            p.y = -osg +2*osg*(pos(i,1)-min_y)/(max_y-min_y);
            p.z = 0;
            mediaBrowser->setNodeNextPosition(ids[i], p);
            std::cout << "ACPosPlugImportGrid::updateNextPositions: media " << ids[i] << " x " << p.x << " y " << p.y << std::endl;
        }
    }
}


void ACPosPlugDredvizNeRV::extractDescMatrix(ACMediaBrowser* mediaBrowser, arma::mat& desc_m, vector<string> &featureNames){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;
    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfFeaturesVectors();
    std::vector<float> weight=mediaBrowser->getWeightVector();
    if (nbFeature!=weight.size())
        std::cerr<<"ACPosPlugDredvizNeRV::extractDescMatrix weight vector size incompatibility"<<endl;
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
