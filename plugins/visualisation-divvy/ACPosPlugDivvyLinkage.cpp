/**
 * @brief Plugin for computing node positions using Divvy's linkage
 * @author Christian Frisson
 * @date 9/03/2014
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
#include "linkage.h"
}

#include<armadillo>
#include "ACPosPlugDivvyLinkage.h"

#include<math.h>

using namespace arma;

ACPosPlugDivvyLinkage::ACPosPlugDivvyLinkage() : ACClusterPositionsPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "Divvy Linkage";
    this->mDescription = "Plugin for positioning items using Divvy's Linkage";
    this->mId = "";

    this->addNumberParameter("Complete",1,0,1,1,"Complete (1: longer, 0: shorter distances)");
}

ACPosPlugDivvyLinkage::~ACPosPlugDivvyLinkage() {
}

void ACPosPlugDivvyLinkage::updateNextPositions(ACMediaBrowser* mediaBrowser){

    if(!media_cycle) return;
    if(media_cycle->getLibrarySize()==0) return;

    int libSize = media_cycle->getLibrarySize();

    std::vector<long> ids = media_cycle->getLibrary()->getAllMediaIds();

    if(libSize != ids.size())
        std::cout << "ACPosPlugDivvyLinkage::updateNextPositions: warning, lib size and number of accessible media ids don't match"<< std::endl;

    arma::mat desc_m;
    vector<string> featureNames;
    this->extractDescMatrix(this->media_cycle->getBrowser(), desc_m, featureNames);
    //desc_m = desc_m.t();//arma::inplace_trans(desc_m); // required but contradictory with the example provided by mlpack

    float* pos = (float*) malloc(desc_m.n_rows * 2 * sizeof(float));;
    float* desc= (float*) malloc(desc_m.n_rows * desc_m.n_cols * sizeof(float));;
    for(int n = 0; n < desc_m.n_rows; n++) {
        for(int d = 0; d < desc_m.n_cols; d++) {
            desc[n * desc_m.n_cols + d] = desc_m(n,d);
        }
    }

    //linkage(desc,desc_m.n_cols,desc_m.n_rows,15,1);

    int n = desc_m.n_rows;
    int d = desc_m.n_cols;
    int k = 15;
    int complete = this->getNumberParameterValue("Complete");

    float *distance_out = (float *)malloc(sizeof(float) * n * (n - 1) / 2);
    dendrite *dendrogram_out = (dendrite *)malloc(sizeof(dendrite) * (n - 1));
    distance(n, d, desc, distance_out);
    dendrogram(n, complete, distance_out, dendrogram_out);

    std::vector<int> _ids;
    int c = 0;
    for(int t=0;t<n-1;t++){
        int i = dendrogram_out[t].i;
        int j = dendrogram_out[t].j;

        if( i < n)
            _ids.push_back(i);

        if(j < n)
            _ids.push_back(j);

        /*if(i >= n && i <= 2 * (n - 1) - (k - 1))
            i -= n;
        if(j >= n && j <= 2 * (n - 1) - (k - 1))
            j -= n;*/

        std::cout << "Dendrite " << c++ <<"/"<< t << ": " << i << "<->" << j << " " << dendrogram_out[t].distance << std::endl;

        if( i < n && j < n){
            //std::cout << "Dendrite " << c++ <<"/"<< t << ": " << i << "<->" << j << " " << dendrogram_out[t].distance << std::endl;
            //_ids.push_back(i);
            //_ids.push_back(j);
        }
    }

    ACPoint p;
    float osg = 0.33f;

    int init = 0;//_ids.size()-1;
    int increment = 1;//-1;


    for(int e = init; e < _ids.size() && e >=0; e+=increment){
        float angle = 2*M_PI*(float)e/(float)(_ids.size()-1);
        float radius = 1;
        p.x = osg*radius*cos(angle);
        p.y = osg*radius*sin(angle);
        int _id = _ids[e];

        mediaBrowser->setNodeNextPosition(_id, p);
    }


    free(distance_out);
    free(dendrogram_out);

    free(pos);
    free(desc);
}


void ACPosPlugDivvyLinkage::extractDescMatrix(ACMediaBrowser* mediaBrowser, arma::mat& desc_m, vector<string> &featureNames){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;
    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfFeaturesVectors();
    std::vector<float> weight=mediaBrowser->getWeightVector();
    if (nbFeature!=weight.size())
        std::cerr<<"ACPosPlugDivvyLinkage::extractDescMatrix weight vector size incompatibility"<<endl;
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
