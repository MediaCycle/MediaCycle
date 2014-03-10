/**
 * @brief ACArmaVisPlugin.cpp
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
#include "ACPlugin.h"
#include "ACArmaVisPlugin.h"

using namespace arma;
using namespace std;
#include "Batch.h"

#ifdef Knn_Validation
#include "ClassificatorErrorMeasure.h"
#include "dimensionReductionQuality.h"
#include "Knn.h"

#include "CrossSpaceClassificatorErrorMeasure.h"
#endif


//TR: I modified this class to take into account the feature that are selected by the user (with te weights).
ACArmaVisPlugin::ACArmaVisPlugin() : ACClusterPositionsPlugin()
{
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle visualisation";
    this->mDescription = "";
    this->mId = "";
    
    
#ifdef Knn_Validation
    knn_K=5;
    this->addNumberParameter("KNN Validation parameter",knn_K,1,20,1,"Number processed neighboor in KNN algorithm",boost::bind(&ACArmaVisPlugin::knnValueChanged,this));
    batchSize=1.00;
    this->addNumberParameter("knn batch size",batchSize,0.01,1,0.01,"batch size in procent",boost::bind(&ACArmaVisPlugin::batchSizeValueChanged,this));
    batchNb=100;
    this->addNumberParameter("Number of batches",batchNb,10,1000,10,"number of batches",boost::bind(&ACArmaVisPlugin::batchNbValueChanged,this));
    tw_K=1000;
    this->addNumberParameter("evaluation neighbor size",tw_K,10,1000,10,"number of elements considered in neighborhood for Trustworthiness and continuity computation",boost::bind(&ACArmaVisPlugin::twkValueChanged,this));
#endif

    //local vars
}

ACArmaVisPlugin::~ACArmaVisPlugin(){
}


void ACArmaVisPlugin::updateNextPositions(ACMediaBrowser* mediaBrowser){
    int itemClicked, labelClicked, action;
    vector<string> featureNames;
    int libSize = mediaBrowser->getLibrary()->getSize();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();

    itemClicked = mediaBrowser->getClickedNode();
    labelClicked = mediaBrowser->getClickedLabel();
    int nbActiveFeatures;
    mat desc_m, descD_m;
    mat posDisp_m;
    urowvec _tag;
    nbActiveFeatures = 9;
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
//    mat descN_m = zscore(desc_m);
//    mat coeff;
//    mat score;
    //    princomp(coeff, posDisp_m, descN_m);
    string fileName=this->mName;
#ifndef Knn_Validation
    //umat tag(_tag.n_cols,2);
    //tag.col(0)=linspace<ucolvec>(0, _tag.n_cols-1,_tag.n_cols);
    //tag.col(1)=_tag.t();
    
#ifdef USE_DEBUG
    desc_m.save(fileName+string("_desc.txt"),arma_ascii);
#endif
    //float batchSize=0.2;
    //Batch batchAlgo;
    //batchAlgo.setTagVector((ucolvec)(_tag.t()));
    //umat tag=batchAlgo.select(batchSize*1);
    dimensionReduction(posDisp_m,desc_m,_tag.t());
#ifdef USE_DEBUG
    _tag.save(fileName+string("_tag.txt"),arma_ascii);
    cout<<"LD max: "<<(max(posDisp_m))<<", min: "<<(min(posDisp_m))<<endl;
    
    cout<<"LD mean: "<<(mean(posDisp_m))<<", std: "<<(stddev(posDisp_m))<<endl;
#endif
   // for (int i=0; i<featureNames.size(); i++)
   //     std::cout << "featureNames : " << featureNames[i] << std::endl;
#else
    
//#ifdef Knn_Validation
    /*//for (int j=3;j<=10;j++)
    {int j=1;
        cout<<"Rate:"<<batchSize*j<<endl;
    Batch batchAlgo;
    batchAlgo.setTagVector((ucolvec)(_tag.t()));
    umat tag=batchAlgo.select(batchSize*j);
    dimensionReduction(posDisp_m,desc_m,tag);
    knn algoKnn;
    algoKnn.setFeatureMatrix(posDisp_m, tag);
    ucolvec res=algoKnn.compute(knn_K);
    cout<<"LD knn error with tagged data rate"<<endl;
    cout<<batchSize*j<<"\t"<<(float)sum(res!=_tag.t())/res.n_elem<<endl;
*/    
    umat tag(_tag.n_cols,2);
    
    tag.col(0)=linspace<ucolvec>(0, _tag.n_cols-1,_tag.n_cols);
    tag.col(1)=_tag.t();
    dimensionReduction(posDisp_m,desc_m,tag);
    posDisp_m.save(fileName+string("_posDisp.txt"),arma_ascii);
    desc_m.save(fileName+string("_desc.txt"),arma_ascii);
    tag.save(fileName+string("_tag.txt"),arma_ascii);
    
    ClassificatorErrorMeasure algo;
//    CrossSpaceClassificatorErrorMeasure algo4;
    std::vector<double> HDKnnErrorMean(10),LDKnnErrorMean(10),HDLDKnnErrorMean(10);
    std::vector<double> HDKnnErrorStd(10),LDKnnErrorStd(10),HDLDKnnErrorStd(10);
    for (int i=1;i<=10;i++)
    {
        //int i=1;
        cout<<"ClassificatorErrorMeasure sizebatch:"<< (double)i*batchSize/10.0<<endl;
        algo.setTagVector(_tag.t(), batchNb, (double)i*batchSize/10.0, knn_K);
  //      algo4.setTagVector(_tag.t(), batchNb, (double)i*batchSize/10.0, knn_K);
        cout<<"HD:"<<algo.errorKnnMeasure(descD_m,HDKnnErrorMean[i-1],HDKnnErrorStd[i-1])<<endl;
        cout<<"LD:"<<algo.errorKnnMeasure(posDisp_m,LDKnnErrorMean[i-1],LDKnnErrorStd[i-1])<<endl;
  //      cout<<"LD&HD:"<<algo4.errorKnnMeasure(desc_m,posDisp_m,HDLDKnnErrorMean[i-1],HDLDKnnErrorStd[i-1])<<endl;
        
    }
    cout<<"knn classification error with high dimensionnal data with "<<this->mName<<" (sizebatch errorMean errorStd)"<<endl;
    for (int i=1;i<=10;i++){
        cout<<(double)i*batchSize/10.0<<"\t"<<HDKnnErrorMean[i-1]<<"\t"<<HDKnnErrorStd[i-1]<<endl;
    }
    cout<<"knn classification error with  dimensionnaly reduced data with "<<this->mName<<" (sizebatch error)"<<endl;
    for (int i=1;i<=10;i++){
        cout<<(double)i*batchSize/10.0<<"\t"<<LDKnnErrorMean[i-1]<<"\t"<<LDKnnErrorStd[i-1]<<endl;
    }
//    cout<<"knn classification error HD&LD with  dimensionnaly reduced data with "<<this->mName<<" (sizebatch error)"<<endl;
  //  for (int i=1;i<=10;i++){
 //       cout<<(double)i*batchSize/10.0<<"\t"<<HDLDKnnErrorMean[i-1]<<"\t"<<HDLDKnnErrorStd[i-1]<<endl;
   // }
    
    dimensionReductionQuality algo2;
    algo2.setFeatureMatrixHighDim(desc_m);
    algo2.setFeatureMatrixLowDim(posDisp_m);
    std::vector<double> trustCont,continuityCont;
    int k=(int)((double)tw_K/100);
    for (int kInc=1;kInc<=20;kInc++){
        cout<<"dimensionReductionQuality it:"<<k<<"/"<<tw_K<<endl;
        double trust,continuity;
        algo2.compute(k,trust,continuity);
        trustCont.push_back(trust);
        continuityCont.push_back(continuity);
         k=(int)((double)kInc*(double)tw_K/20);
    }
    int icpt=0;
    cout<<"Trustworthiness of dimension reduction \t"<<this->mName<<endl;
     k=(int)((double)tw_K/100);
    for (int kInc=1;kInc<=20;kInc++){
        cout<<k<<"\t"<<trustCont[icpt]<<endl;
        k=(int)((double)kInc*(double)tw_K/20);
        icpt++;
    }
    icpt=0;
    cout<<"Continuity of dimension reduction \t\t"<<this->mName<<endl;
     k=(int)((double)tw_K/100);
    for (int kInc=1;kInc<=20;kInc++){
        cout<<k<<"\t"<<continuityCont[icpt]<<endl;
        k=(int)((double)kInc*(double)tw_K/20);
        icpt++;
    }
    

#endif
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
    
    int cpt=0;
    int locRefNode=-1;
    
    for (int i=0; i<ids.size(); i++){
        if (mediaBrowser->getMediaNode(ids[i]) && mediaBrowser->getMediaNode(ids[i])->isDisplayed()&&cpt<desc_m.n_rows){
            if (mediaBrowser->getReferenceNode()==ids[i])
                locRefNode=cpt;
            cpt++;
        }
    }
    if (locRefNode>-1&&locRefNode<posDisp_m.n_rows){
        posDisp_m=posDisp_m-ones(posDisp_m.n_rows,1)*posDisp_m.row(locRefNode);
    }
    ACPoint p;
    cpt=0;
    float mx1=abs(min(posDisp_m.col(0)));
    float mx2=max(posDisp_m.col(0));
    float my1=abs(min(posDisp_m.col(1)));
    float my2=max(posDisp_m.col(1));
    float mTot=2*max(max(mx1,my1),max(mx2,my2));
    float mx3 = min(posDisp_m.col(0));
    float my3 = min(posDisp_m.col(1));
    float osg = 0.33f;
    if (mTot==0)
        mTot=1;
   // mTot=1;
    for (int i=0; i<ids.size(); i++){
        if (mediaBrowser->getMediaNode(ids[i]) && mediaBrowser->getMediaNode(ids[i])->isDisplayed()&&cpt<desc_m.n_rows){
            //mediaBrowser->setMediaNodeDisplayed(ids[i], true);
            // TODO: make sure you meant next
            //p.x = posDisp_m(cpt,0)/mTot;
            //p.y = posDisp_m(cpt,1)/mTot;
            p.x = -osg + 2*osg*(posDisp_m(cpt,0)-mx3)/(mx2-mx3);
            p.y = -osg + 2*osg*(posDisp_m(cpt,1)-my3)/(my2-my3);
            p.z = 0;
            mediaBrowser->setNodeNextPosition(ids[i], p);
            cpt++;
        }
    }
    if (cpt!=desc_m.n_rows)
        cout << "ACArmaVisPlugin::updateNextPositions, problem with desc matrix dimensions "<<endl;
    ////////////////////////////////////////////////////////////////
}


void ACArmaVisPlugin::extractDescMatrix(ACMediaBrowser* mediaBrowser, mat& desc_m, vector<string> &featureNames,urowvec &tag){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;

    std::vector<float> weight=mediaBrowser->getWeightVector();
    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfPreProcFeaturesVectors();
    if (nbFeature!=weight.size())
        std::cerr<<"ACArmaVisPlugin::extractDescMatrix weight vector size incompatibility"<<endl;
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
    tag.set_size(cpt);
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
            tag(tmpIdy)= mediaBrowser->getMediaNode(ids[i])->getClusterId();
            tmpIdy++;
        }
    }
    // normalizing features between 0 and 1 ///////////////////////////////////////
    //   rowvec maxDesc_v = max(desc_m);
    //   rowvec minDesc_v = min(desc_m);
    //   desc_m = desc_m - repmat(minDesc_v, desc_m.n_rows, 1);
    //   desc_m = desc_m/repmat(maxDesc_v-minDesc_v, desc_m.n_rows, 1);
    /////////////////////////////////////////////////////////////////////////////////
}

void ACArmaVisPlugin::catchCurrentPosition(ACMediaBrowser* mediaBrowser, mat& pos_m){
    
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    int nbMedia = medias.size();
    int cpt=0;
    for (long i=0; i<ids.size(); i++){
        if (mediaBrowser->getMediaNode(ids[i]) && mediaBrowser->getMediaNode(ids[i])->isDisplayed()){
            cpt++;
        }
    }
    pos_m.set_size(cpt,2);
    int tmpIdy=0;
    for(int i=0; i<ids.size(); i++) {
        if (mediaBrowser->getMediaNode(ids[i]) && !(mediaBrowser->getMediaNode(ids[i])->isDisplayed()))
            continue;
        ACPoint p=mediaBrowser->getMediaNode(ids[i])->getNextPosition();
        pos_m(tmpIdy,0)=p.x;
        pos_m(tmpIdy,1)=p.y;
        //pos_m(tmpIdy,2)=p.z;
        //cout<<"ACArmaVisPlugin::catchCurrentPosition node "<<ids[i]<<" : "<<p.x<<" "<<p.y<<" "<<p.z<<endl;
        tmpIdy++;
        
    }
}
#ifdef Knn_Validation

void ACArmaVisPlugin::knnValueChanged(void){
    if(!this->media_cycle) return;
    
    knn_K=this->getNumberParameterValue("KNN Validation parameter");
    
}
void ACArmaVisPlugin::batchSizeValueChanged(void){
    if(!this->media_cycle) return;
    
    batchSize=this->getNumberParameterValue("knn batch size");
    
}
void ACArmaVisPlugin::twkValueChanged(void){
    if(!this->media_cycle) return;
    
    tw_K=this->getNumberParameterValue("evaluation neighbor size");
    

}
void ACArmaVisPlugin::batchNbValueChanged(void){
    if(!this->media_cycle) return;

    batchNb=this->getNumberParameterValue("Number of batches");


}
#endif

