/**
 * @brief ACVisPluginIsoMap.cpp
 * @author Thierry Ravet
 * @date 01/05/2013
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

#include <armadillo>
#include "Armadillo-utils.h"
#include "ACPlugin.h"
#include "ACVisPluginIsoMap.h"
#include "Isomap.h"

using namespace arma;
using namespace std;
//TR: I modified this class to take into account the feature that are selected by the user (with te weights).
ACVisPluginIsoMap::ACVisPluginIsoMap() : ACArmaVisPlugin()
{
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle IsoMap";
    this->mDescription = "dimensionnality reduction resulting from IsoMap";
    this->mId = "";
    Kn=10;

    //local vars
}

ACVisPluginIsoMap::~ACVisPluginIsoMap(){
}


/*void ACVisPluginIsoMap::updateNextPositions(ACMediaBrowser* mediaBrowser){
    int itemClicked, labelClicked, action;
    vector<string> featureNames;
    int libSize = mediaBrowser->getLibrary()->getSize();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();

    itemClicked = mediaBrowser->getClickedNode();
    labelClicked = mediaBrowser->getClickedLabel();
    int nbActiveFeatures;
    mat desc_m, descD_m;
    mat posDisp_m;

    nbActiveFeatures = 9;
    extractDescMatrix(mediaBrowser, desc_m, featureNames);
    if (desc_m.n_cols < 2){
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
   // mat descN_m = zscore(desc_m);
    mat coeff;
    mat score;
    //princomp(coeff, posDisp_m, descN_m);
    Isomap algo;
    while (algo.setFeatureMatrix(desc_m,'k',Kn)==false){
        Kn+=5;
        if (Kn>desc_m.n_rows)
            break;
    }
    posDisp_m=algo.compute(2);

    
    for (int i=0; i<featureNames.size(); i++)
        std::cout << "featureNames : " << featureNames[i] << std::endl;


#ifdef USE_DEBUG
    posDisp_m.save("posDispDef.txt", arma_ascii);
#endif
    // 	///////////////////////////////////////////////////////////////////////////////

    ///for mediacycle osg
    // 	posDisp_m = posDisp_m/10;
    // 	labelPos_m = labelPos_m/10;
    // Set labels in browser ////////////////////////////////////////////////////////

    mediaBrowser->setNumberOfDisplayedNodes(desc_m.n_rows);
    ////////////////////////////////////////////////////////////////////////////////////

    ACPoint p;
    int cpt=0;
    float mx1=abs(min(posDisp_m.row(0)));
    float mx2=max(posDisp_m.row(0));
    float my1=abs(min(posDisp_m.row(1)));
    float my2=max(posDisp_m.row(1));
    float mTot=max(max(mx1,my1),max(mx2,my2));
    if (mTot==0)
        mTot=1;
    for (int i=0; i<ids.size(); i++){
        if (mediaBrowser->getMediaNode(ids[i])->isDisplayed()&&cpt<desc_m.n_rows){
            //mediaBrowser->setMediaNodeDisplayed(ids[i], true);
            // TODO: make sure you meant next
            p.x = posDisp_m(cpt,0)/mTot;
            p.y = posDisp_m(cpt,1)/mTot;
            p.z = 0;
            mediaBrowser->setNodeNextPosition(ids[i], p);
            cpt++;
        }
    }
    if (cpt!=desc_m.n_rows)
        cout << "ACVisPluginIsoMap::updateNextPositions, problem with desc matrix dimensions "<<endl;
    ////////////////////////////////////////////////////////////////
}


void ACVisPluginIsoMap::extractDescMatrix(ACMediaBrowser* mediaBrowser, mat& desc_m, vector<string> &featureNames){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;

    std::vector<float> weight=mediaBrowser->getWeightVector();
    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfPreProcFeaturesVectors();
    if (nbFeature!=weight.size())
        std::cerr<<"ACVisPluginIsoMap::extractDescMatrix weight vector size incompatibility"<<endl;
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
        if (mediaBrowser->getMediaNode(ids[i])->isDisplayed()){
            cpt++;
        }
    }

    desc_m.set_size(cpt,totalDim);
    int tmpIdy=0;
    for(int i=0; i<nbMedia; i++) {
        int tmpIdx = 0;
        if (!(mediaBrowser->getMediaNode(ids[i])->isDisplayed()))
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
        tmpIdy++;
    }
    // normalizing features between 0 and 1 ///////////////////////////////////////
    //   rowvec maxDesc_v = max(desc_m);
    //   rowvec minDesc_v = min(desc_m);
    //   desc_m = desc_m - repmat(minDesc_v, desc_m.n_rows, 1);
    //   desc_m = desc_m/repmat(maxDesc_v-minDesc_v, desc_m.n_rows, 1);
    /////////////////////////////////////////////////////////////////////////////////
}*/

void  ACVisPluginIsoMap::dimensionReduction(mat &posDisp_m,arma::mat desc_m,arma::umat tag){
    Isomap algo;
    while (algo.setFeatureMatrix(desc_m,'k',Kn)==false){
        Kn+=5;
        if (Kn>desc_m.n_rows)
            break;
    }
    posDisp_m=algo.compute(2);
    
}
