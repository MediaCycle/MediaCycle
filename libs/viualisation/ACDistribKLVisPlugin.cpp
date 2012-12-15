/**
 * @brief ACDistribKLVisPlugin.cpp
 * @author Thierry Ravet
 * @date 15/12/2012
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
#include "ACDistribKLVisPlugin.h"

using namespace arma;
using namespace std;
//TR: I modified this class to take into account the feature that are selected by the user (with te weights).
ACDistribKLVisPlugin::ACDistribKLVisPlugin() : ACClusterPositionsPlugin()
{
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->mName = "MediaCycle visualisation based on KL divergence";
    this->mDescription = "";
    this->mId = "";

    //local vars
}

ACDistribKLVisPlugin::~ACDistribKLVisPlugin(){
}


void ACDistribKLVisPlugin::updateNextPositions(ACMediaBrowser* mediaBrowser){
    int itemClicked, labelClicked, action;
    vector<string> featureNames;
    int libSize = mediaBrowser->getLibrary()->getSize();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();

    itemClicked = mediaBrowser->getClickedNode();
    labelClicked = mediaBrowser->getClickedLabel();
    int nbActiveFeatures;
    mat desc_m, descD_m;
    mat posDisp_m;
    urowvec tag;
    nbActiveFeatures = 9;
    extractKLDistMatrix(mediaBrowser, desc_m, featureNames,tag);
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
//    mat descN_m = zscore(desc_m);
//    mat coeff;
//    mat score;
//    princomp(coeff, posDisp_m, descN_m);

    dimensionReduction(posDisp_m,desc_m,tag);
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
    float mTot=2*max(max(mx1,my1),max(mx2,my2));
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
        cout << "ACDistribKLVisPlugin::updateNextPositions, problem with desc matrix dimensions "<<endl;
    ////////////////////////////////////////////////////////////////
}


void ACDistribKLVisPlugin::extractKLDistMatrix(ACMediaBrowser* mediaBrowser, mat& desc_m, vector<string> &featureNames,urowvec &tag){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    std::vector<long> allIds = mediaBrowser->getLibrary()->getAllMediaIds();
    int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;

    std::vector<float> weight=mediaBrowser->getWeightVector();
    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfPreProcFeaturesVectors();
    if (nbFeature!=weight.size())
        std::cerr<<"ACDistribKLVisPlugin::extractDescMatrix weight vector size incompatibility"<<endl;
    for(int f=0; f< nbFeature; f++){
        string tempName=mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getName();
        string subName=tempName.substr(0, tempName.find_first_of(string(":")));
        bool flag=false;
        for (int i=0;i<featureNames.size();i++){
            
            if (subName==featureNames[i]){
                flag=true;
            }
        }
        if (weight[f]>0.f||flag){
            if (flag==false){
                featureNames.push_back(subName);
            }
            weight[f]=1.f;
            featDim = mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getSize();
            for(int d=0; d < featDim; d++){
                totalDim++;
            }
        }
    }
    for (int i=0;i<featureNames.size();i++){
        cout<<featureNames[i]<<endl;
    }
    
    int cpt=0;
    std::vector<long> ids;
    for (long i=0; i<allIds.size(); i++){
        if (mediaBrowser->getMediaNode(allIds[i])->isDisplayed()){
            ids.push_back(allIds[i]);
            cpt++;
        }
    }
    desc_m.set_size(cpt,cpt);
    unsigned int locdim=0;
    
    for(int f=0; f< nbFeature; f++){
        string tempName=mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getName();
        if (tempName.find("Centroid")==(size_t)-1)
            continue;
        string subName=tempName.substr(0, tempName.find_first_of(string(":")));
        for (int i=0;i<featureNames.size();i++){
            if (subName==featureNames[i]){
                    locdim+=mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getSize();
                
            }
        }
    }
    tag.set_size(cpt);
    int tmpIdy=0;
    for(int i=0; i<cpt; i++) {
        cout<<"ACDistribKLVisPlugin::extractKLDistMatrix line:"<<i<<endl;
        tag(i)= mediaBrowser->getMediaNode(ids[i])->getClusterId();
        //compute mu and sigma matrix for ith media
        colvec muI(locdim);
        mat sigmaI(locdim,locdim);
        
        this->selectStat(muI,sigmaI,medias[ids[i]],featureNames);
            
        
        for (int j=0;j<i;j++){
            colvec muJ(locdim);
            mat sigmaJ(locdim,locdim);
            this->selectStat(muJ,sigmaJ,medias[ids[j]],featureNames);
            double temp=log((double)det(sigmaI)/(double)det(sigmaJ));
            if (is_finite(temp)==false)
                temp=0.0;
            double Nij=0.5*(trace(inv(sigmaJ)*sigmaI)+as_scalar(((muJ-muI).t())*inv(sigmaJ)*(muJ-muI))-temp-cpt);
            if (is_finite(Nij)==false)
                {
                    cout<<trace(inv(sigmaJ)*sigmaI)<<endl;
                    cout<<as_scalar(((muJ-muI).t())*inv(sigmaJ)*(muJ-muI))<<endl;
                    cout<<temp<<endl;
                    cout<<det(sigmaI)<<" & "<<det(sigmaJ)<<endl;
                    cout<<"____________"<<endl;
                }
            
            temp=log((double)det(sigmaJ)/(double)det(sigmaI));
            if (is_finite(temp)==false)
                temp=0.0;
            double Nji=0.5*(trace(inv(sigmaI)*sigmaJ)+as_scalar(((muI-muJ).t())*inv(sigmaI)*(muI-muJ))-temp-cpt);
            desc_m(i,j)=0.5*(Nji+Nij);
            
        }
        desc_m(i,i)=0;
    }
    desc_m=symmatl(desc_m);
    cout<<"desc:"<<desc_m<<endl;
    
    // normalizing features between 0 and 1 ///////////////////////////////////////
    //   rowvec maxDesc_v = max(desc_m);
    //   rowvec minDesc_v = min(desc_m);
    //   desc_m = desc_m - repmat(minDesc_v, desc_m.n_rows, 1);
    //   desc_m = desc_m/repmat(maxDesc_v-minDesc_v, desc_m.n_rows, 1);
    /////////////////////////////////////////////////////////////////////////////////
}

void ACDistribKLVisPlugin::selectStat(colvec &mu,mat &sigma,ACMedia *media,vector<string> featureNames){
    unsigned int indCpt=0,indCptR=0,indCptC=0;
    int nbFeature = media->getNumberOfPreProcFeaturesVectors();
    
    for (int g=0;g<featureNames.size();g++){
        int featDimNb;
        for(int f=0; f< nbFeature; f++){
            string tempName=media->getPreProcFeaturesVector(f)->getName();
            string subName=tempName.substr(0, tempName.find_first_of(string(":")));
            if (subName==featureNames[g]){
                if (tempName.find("Centroid")!=(size_t)-1){
                    featDimNb=media->getPreProcFeaturesVector(f)->getSize();
                    for (int h=0;h<featDimNb;h++)
                        mu(indCpt+h)=media->getPreProcFeaturesVector(f)->getFeatureElement(h);
                        indCpt+=featDimNb;
                        }
                else if (tempName.find("Cov")!=(size_t)-1){
                    string subName2=tempName.substr(tempName.find_last_of(string(":"))+1);
                    for (int k=0;k<featureNames.size();k++){
                        if (subName2==featureNames[k]){
                            for (int l=0;l<media->getPreProcFeaturesVector(f)->getSize();l++){
                                sigma(indCptR+l%featDimNb,indCptC+l/featDimNb)=media->getPreProcFeaturesVector(f)->getFeatureElement(l);
                            }
                            indCptC+=media->getPreProcFeaturesVector(f)->getSize()/featDimNb;
                        }
                    }
                }
            }
            
        }
        indCptC=0;
        indCptR+=featDimNb;
    
    }
}
