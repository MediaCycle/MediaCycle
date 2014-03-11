/**
 * @brief Plugin for binning nodes in a grid using the Minimum Spanning Tree
 * @author Christian Frisson
 * @date 02/02/2014
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

//@inproceedings{
//  author = {March, W.B., Ram, P., and Gray, A.G.},
//  title = {{Fast Euclidean Minimum Spanning Tree: Algorithm, Analysis,
//     Applications.}},
//  booktitle = {Proceedings of the 16th ACM SIGKDD International Conference
//     on Knowledge Discovery and Data Mining}
//  series = {KDD 2010},
//  year = {2010}
//}

#include<armadillo>
#include "ACPosPlugMinSpanGrid.h"

#include "mlpack/core.hpp"
//Wrapper to prevent mlpack::emst duplicate symbols when compiling
#include "ACFilterPlugProximityGrid.h"

using namespace arma;
/*using namespace mlpack;
using namespace mlpack::emst;
using namespace mlpack::tree;
using namespace std;*/

ACPosPlugMinSpanGrid::ACPosPlugMinSpanGrid() : ACClusterPositionsPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle Minimum Spanning Grid";
    this->mDescription = "Plugin for positioning items on a minimal spanning grid";
    this->mId = "";

    this->addNumberParameter("Landmower mode",0,0,1,1,"Landmower mode",boost::bind(&ACPosPlugMinSpanGrid::updateGrid,this));
    //this->addCallback("Update","Update view",boost::bind(&ACPosPlugMinSpanGrid::setProximityGrid,this));
}

ACPosPlugMinSpanGrid::~ACPosPlugMinSpanGrid() {
}

void ACPosPlugMinSpanGrid::updateGrid()
{
    if(media_cycle)
        this->updateNextPositions(media_cycle->getBrowser());
}


void ACPosPlugMinSpanGrid::updateNextPositions(ACMediaBrowser* mediaBrowser){

    if(!media_cycle) return;
    if(media_cycle->getLibrarySize()==0) return;

    int libSize = media_cycle->getLibrarySize();

    std::vector<long> ids = media_cycle->getLibrary()->getAllMediaIds();

    if(libSize != ids.size())
        std::cout << "ACPosPlugMinSpanGrid::updateNextPositions: warning, lib size and number of accessible media ids don't match"<< std::endl;

    /*int*/ gridSize = ceil(sqrt(libSize));

    if(gridSize == 0)
        return;

    std::cout << "gridSize " << gridSize << " for " << libSize << " elements " << std::endl;

    arma::mat desc_m;
    vector<string> featureNames;
    this->extractDescMatrix(this->media_cycle->getBrowser(), desc_m, featureNames);
    desc_m = desc_m.t();//arma::inplace_trans(desc_m); // required but contradictory with the example provided by mlpack

    // Euclidean Minimum Spanning Tree from methodss/emst/emst_main.cpp
    // using the dual-tree Boruvka algorithm.
    // The output is saved in a three-column matrix, where each row indicates an
    // edge.  The first column corresponds to the lesser index of the edge; the
    // second column corresponds to the greater index of the edge; and the third
    // column corresponds to the distance between the two points.
    // can be using O(n^2) naive algorithm
    // leaf_size: leaf size in the kd-tree.  One-element leaves give the
    // empirically best performance, but at the cost of greater memory requirements. default 1);

    bool naive = false;
    const size_t leafSize = 1;//gridSize;
    arma::mat mst = emst(desc_m,naive,leafSize);

    //mlpack::data::Save("./posg.csv", desc_m, true);
    //mlpack::data::Save("./mstg.csv", mst, true);

    std::cout << "ACPosPlugMinSpanGrid desc rows " << desc_m.n_rows << " cols " << desc_m.n_cols << std::endl;
    std::cout << "ACPosPlugMinSpanGrid mst rows " << mst.n_rows << " cols " << mst.n_cols << std::endl;


    std::map<long,bool> id_celled;
    for(std::vector<long>::iterator id=ids.begin(); id!=ids.end();id++){
        id_celled[*id] = false;
    }

    ACPoint p;
    int row = 0;
    float osg = 0.33f;

    bool landmower = this->getNumberParameterValue("Landmower mode");

    // Considering closest MST edges first for empty strategy, farthest otherwise
    int increment = 1;
    int init = 0;

    int i = 0;
    for(int e = init; e >= 0 && e < mst.n_cols;e+=increment){
        //std::cout << "Edge " << e << " " << mst(0,e) << " " << mst(1,e) << " d " << mst(2,e) << std::endl;
        for(int m = 0;m<2;m++){

            int _id = mst(m,e);
            std::map<long,bool>::iterator is_celled = id_celled.find( mst(m,e) );
            if( is_celled != id_celled.end() && is_celled->second == false){

                if(i%gridSize==0){
                    row++;
                }
                if(landmower)
                    p.x = (-osg + 2*osg*(float)(i%gridSize)/(float)(gridSize-1))*pow(-1,row+1);
                else
                    p.x = -osg + 2*osg*(float)(i%gridSize)/(float)(gridSize-1);
                p.y = osg -2*osg*(float)(row-1)/(float)(gridSize-1);

                mediaBrowser->setNodeNextPosition(_id, p);
                std::cout << "ACPosPlugImportGrid::updateNextPositions: media " << _id << " i%gridSize " << i%gridSize << " x " << p.x << " y " << p.y << std::endl;

                id_celled[_id] = true;
                i++;

            }
        }
    }
}


void ACPosPlugMinSpanGrid::extractDescMatrix(ACMediaBrowser* mediaBrowser, arma::mat& desc_m, vector<string> &featureNames){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;
    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfFeaturesVectors();
    std::vector<float> weight=mediaBrowser->getWeightVector();
    if (nbFeature!=weight.size())
        std::cerr<<"ACPosPlugMinSpanGrid::extractDescMatrix weight vector size incompatibility"<<endl;
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
