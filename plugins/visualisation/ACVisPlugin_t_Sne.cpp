/**
 * @brief ACVisPlugin_t_Sne.cpp
 * @author Thierry Ravet
 * @date 16/12/2012
 * @copyright (c) 2012 – UMONS - Numediart
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
#include "ACVisPlugin_t_Sne.h"
#include "t_Sne.h"

using namespace arma;
using namespace std;
//TR: I modified this class to take into account the feature that are selected by the user (with te weights).
ACVisPlugin_t_Sne::ACVisPlugin_t_Sne() : ACArmaVisPlugin(){
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle t_Sne";
    this->mDescription = "dimensionnality reduction resulting from t_Sne";
    this->mId = "";
    perplexity=20;
    this->addNumberParameter("perplexity",perplexity,10,100,10,"value of desired perplexity",boost::bind(&ACVisPlugin_t_Sne::perplexityValueChanged,this));
    itMax=100;
    this->addNumberParameter("Iteration Number",itMax,10,2000,50,"Number of t-Sne iteration",boost::bind(&ACVisPlugin_t_Sne::itMaxValueChanged,this));
    vector<string> initializeValue;
    initializeValue.push_back("Yes");
    initializeValue.push_back("No");
    this->addStringParameter("Initialize","Yes",initializeValue,"Must we catch the current positions to initialize the t-Sne algorithm",boost::bind(&ACVisPlugin_t_Sne::initValueChanged,this));
    initFlag=true;
    
    //local vars
}

ACVisPlugin_t_Sne::~ACVisPlugin_t_Sne(){
}

/*
void ACVisPlugin_t_Sne::updateNextPositions(ACMediaBrowser* mediaBrowser){
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
    t_Sne t_Sne_algo;
    t_Sne_algo.setPerplexity(perplexity);
    t_Sne_algo.setFeatureMatrix(desc_m);
    posDisp_m=t_Sne_algo.compute(2);

    
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
        cout << "ACVisPlugin_t_Sne::updateNextPositions, problem with desc matrix dimensions "<<endl;
    ////////////////////////////////////////////////////////////////
}


void ACVisPlugin_t_Sne::extractDescMatrix(ACMediaBrowser* mediaBrowser, mat& desc_m, vector<string> &featureNames){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;

    std::vector<float> weight=mediaBrowser->getWeightVector();
    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfPreProcFeaturesVectors();
    if (nbFeature!=weight.size())
        std::cerr<<"ACVisPlugin_t_Sne::extractDescMatrix weight vector size incompatibility"<<endl;
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

void ACVisPlugin_t_Sne::perplexityValueChanged(void){
    if(!this->media_cycle) return;
    
    perplexity=this->getNumberParameterValue("perplexity");
    
}
void ACVisPlugin_t_Sne::itMaxValueChanged(void){
    if(!this->media_cycle) return;
    
    itMax=this->getNumberParameterValue("Iteration Number");
    
}
void ACVisPlugin_t_Sne::initValueChanged(void){
    if(!this->media_cycle) return;
    string initializeValue=this->getStringParameterValue("Initialize");
    if (initializeValue==string("Yes"))
        initFlag=true;
    else
        initFlag=false;
}

void  ACVisPlugin_t_Sne::dimensionReduction(mat &posDisp_m,arma::mat desc_m,urowvec tag){
    t_Sne t_Sne_algo;
    t_Sne_algo.setPerplexity(perplexity);
    t_Sne_algo.setIterMax(itMax);
    t_Sne_algo.setFeatureMatrix(desc_m);
    if (initFlag){
        arma::mat pos_m;
        this->catchCurrentPosition(media_cycle->getBrowser(), pos_m);
        posDisp_m=t_Sne_algo.compute(2,pos_m);
    }
    else
        posDisp_m=t_Sne_algo.compute(2);
 
    
}


