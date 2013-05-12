/**
 * @brief ACVisPluginFeatureSelection.cpp
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
#include "ACVisPluginFeatureSelection.h"
#include "ClassificatorErrorMeasure.h"
#include "dimensionReductionQuality.h"
#include "Knn.h"

using namespace arma;
using namespace std;
//TR: I modified this class to take into account the feature that are selected by the user (with te weights).
ACVisPluginFeatureSelection::ACVisPluginFeatureSelection() : ACArmaVisPlugin()
{
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle Feature Selection";
    this->mDescription = "Feature weight selection with PCA visualisation";
    this->mId = "";

    //local vars
}

ACVisPluginFeatureSelection::~ACVisPluginFeatureSelection(){
}

void ACVisPluginFeatureSelection::updateNextPositions(ACMediaBrowser* mediaBrowser){
    int itemClicked, labelClicked, action;
    vector<string> featureNames;
    int libSize = mediaBrowser->getLibrary()->getSize();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    std::vector<float> savedWeight=mediaBrowser->getWeightVector();
    std::vector<float> tempWeight;
    int nbFeature=savedWeight.size();
    tempWeight.resize(nbFeature);
    
    itemClicked = mediaBrowser->getClickedNode();
    labelClicked = mediaBrowser->getClickedLabel();
   
    mat desc_m, descD_m;
    mat posDisp_m;
    urowvec _tag;
    umat tag(_tag.n_cols,2);
    
    tag.col(0)=linspace<ucolvec>(0, _tag.n_cols-1,_tag.n_cols);
    tag.col(1)=_tag.t();
    bool flagStop=true;
    while (flagStop)
    {
        arma::colvec HDKnnErrorMean(10);
        arma::colvec HDKnnErrorStd(10);
        
        for (int k=0;k<nbFeature;k++){
        extractDescMatrix(mediaBrowser, desc_m, featureNames,_tag);
        descD_m=desc_m;
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
        unsigned int  knn_K=5,batchNb=100;
        float  batchSize=0.2;
        string fileName=this->mName;
    
        ClassificatorErrorMeasure algo;
    //    CrossSpaceClassificatorErrorMeasure algo4;
        
        
        //int i=1;
            cout<<"ClassificatorErrorMeasure sizebatch:"<< (double)batchSize/10.0<<endl;
            algo.setTagVector(_tag.t(), batchNb, (double)batchSize/10.0, knn_K);
        //      algo4.setTagVector(_tag.t(), batchNb, (double)i*batchSize/10.0, knn_K);
            double tempMean,tempStd;
            algo.errorKnnMeasure(descD_m,tempMean,tempStd);
        
        }
    }
    mediaBrowser->setWeightVector(savedWeight);
    extractDescMatrix(mediaBrowser, desc_m, featureNames,_tag);
    descD_m=desc_m;
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
    dimensionReduction(posDisp_m,desc_m,tag);
    
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
    float mTot=2*max(max(mx1,my1),max(mx2,my2));
    if (mTot==0)
        mTot=1;
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
        cout << "ACArmaVisPlugin::updateNextPositions, problem with desc matrix dimensions "<<endl;
    ////////////////////////////////////////////////////////////////
}
void  ACVisPluginFeatureSelection::dimensionReduction(mat &posDisp_m,arma::mat desc_m,arma::umat tag){
    mat tempPosDisp;
    mat descN_m = zscore(desc_m);
    mat coeff;
    mat score;
    princomp(coeff, tempPosDisp, descN_m);
    posDisp_m.set_size(tempPosDisp.n_rows,2);
    posDisp_m.col(0)=tempPosDisp.col(0);
    posDisp_m.col(1)=tempPosDisp.col(1);

}

