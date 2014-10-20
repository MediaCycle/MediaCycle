/*
 *  ACMlPackTaggedGMMPlugin.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 11/10/2010
 *  @copyright (c) 2010 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 */

#include <float.h>
#include <armadillo>
#include "ACMlPackTaggedGMMPlugin.h"
#include "Armadillo-utils.h"
//#ifdef DEBUG
//#undef DEBUG
#include "mlpack/core.hpp"
#include "mlpack/methods/gmm/gmm.hpp"
//#define DEBUG
//#else
//#include "mlpack/core.hpp"
//#include "mlpack/methods/kmeans/kmeans.hpp"
//#endif
static int statCpt=0;
using namespace arma;


ACMlPackTaggedGMMPlugin::ACMlPackTaggedGMMPlugin()
: ACClusterMethodPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle MlPack GMM tagged";
    this->mDescription = "Plugin for GMM Clustering on tagged samples by using MLPACK";
    this->mId = "";
    //this->addNumberParameter("clusters",5,1,10,1,"number of desired clusters",boost::bind(&ACMlPackTaggedGMMPlugin::clusterNumberChanged,this));
}

ACMlPackTaggedGMMPlugin::~ACMlPackTaggedGMMPlugin() {
}


void ACMlPackTaggedGMMPlugin::updateClusters(ACMediaBrowser* mediaBrowser,bool needsCluster){
    ACMediaLibrary *library=mediaBrowser->getLibrary();
    
    int clusterCount=0;
    
    if(library == 0) {
        cerr << "<ACMediaBrowser::updateClustersKMeans> : Media Library 0" << endl;
        return;
    }
    else if(library->isEmpty()) {
        cerr << "<ACMediaBrowser::updateClustersKMeans> : empty Media Library " << endl;
        return;
    }
    
    
	for (ACMediaNodes::const_iterator it=mediaBrowser->getMediaNodes().begin(); it !=mediaBrowser->getMediaNodes().end();it++){
        if(it->second){
            if ((library->getMedia(it->first)->getMediaType() == library->getMediaType()) &&(it->second->getNavigationLevel() >= mediaBrowser->getNavigationLevel()))
            {
                
				it->second->setDisplayed(true);
			}
        }
    }
    mat desc_m, descD_m;
    mat posDisp_m;
    irowvec _tag;
    vector<string> featureNames;
    extractDescMatrix(mediaBrowser, desc_m, featureNames,_tag);
    arma::ucolvec taggedIndex=find(_tag!=-1);
    cout<<taggedIndex<<endl;
    if (taggedIndex.n_elem>0){
        clusterCount=max(_tag+1);
        media_cycle->setClusterNumber(clusterCount);
       // mlpack::gmm::GMM<> algo(clusterCount,desc_m.n_cols);
        
        
        urowvec v = linspace<urowvec>(0, desc_m.n_cols-1, desc_m.n_cols);
        std::vector<arma::vec> _means;
        std::vector<arma::mat> _covariances;
        arma::vec _weights(clusterCount);
        
        for (int clusterId=0;clusterId<clusterCount;clusterId++){
            
            arma::ucolvec locTaggedIndex=find(_tag==clusterId);
            if (locTaggedIndex.n_elem>0){
                descD_m=desc_m.submat(locTaggedIndex,v).t();
            }
            else {
                descD_m=zeros<colvec>(desc_m.n_cols);
            }
            vector<double> evalLike;
            for (int i=1;i<=10;i++){
                mlpack::gmm::GMM<> locAlgo(i,desc_m.n_cols);
                evalLike.push_back(locAlgo.Estimate(descD_m));
                cout<<(locAlgo.Means())[0]<<endl;
            }
            for (int i=0;i<evalLike.size();i++){
                cout<<evalLike[i]<<" ";
            }
            cout<<endl;
            mlpack::gmm::GMM<> locAlgo(1,desc_m.n_cols);
            cout<<locAlgo.Estimate(descD_m)<<endl;
            //cout<<"preprocessed"<<endl;
            //cout<<clusterId<<endl;
            //cout<<(locAlgo.Means())[0]<<endl;
            //cout<<(locAlgo.Covariances())[0]<<endl;
            _means.push_back((locAlgo.Means())[0]);
            _covariances.push_back((locAlgo.Covariances())[0]);
                _weights[clusterId]=(float)locTaggedIndex.n_elem/taggedIndex.n_elem;
        }
        mlpack::gmm::GMM<> algo(_means,_covariances,_weights);
        
        descD_m=desc_m.t();
        arma::Col<size_t> assignments;
        cout<<assignments<<endl;
        algo.Classify(descD_m,assignments);
      
        
        
        ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
        std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
        int nbMedia = medias.size();
        
        int tmpIdy=0;
        for(int i=0; i<nbMedia; i++) {
            if (!(mediaBrowser->getMediaNode(ids[i])->isDisplayed()))
                continue;
            if (tmpIdy<assignments.n_elem)
                mediaBrowser->getMediaNode(ids[i])->setClusterId(assignments[tmpIdy]);
            tmpIdy++;
            
        }
        //Set ClusterCenters
        std::vector<arma::vec> gmmMeans=algo.Means();
        std::vector<arma::mat> gmmCov=algo.Covariances();
        arma::mat centers(desc_m.n_cols,gmmMeans.size());
        for (int i=0;i<centers.n_cols;i++){
            centers.col(i)=gmmMeans[i];
            //cout<<gmmMeans[i]<<endl;
            //cout<<gmmCov[i]<<endl;
        }
        cout<<"weight"<<endl;
        cout<<algo.Weights()<<endl;
        this->transferToClusterCenter(mediaBrowser,centers);
        
    }
    else{
        ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
        std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
        int nbMedia = medias.size();
        
        int tmpIdy=0;
        for(int i=0; i<nbMedia; i++) {
            if (!(mediaBrowser->getMediaNode(ids[i])->isDisplayed()))
                continue;
            mediaBrowser->getMediaNode(ids[i])->setClusterId(0);
            tmpIdy++;
        }
    }
    
	
}

void ACMlPackTaggedGMMPlugin::extractDescMatrix(ACMediaBrowser* mediaBrowser, mat& desc_m, vector<string> &featureNames,irowvec &tag){
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
        if (mediaBrowser->getMediaNode(ids[i])->isDisplayed()){
            cpt++;
        }
    }
    
    desc_m.set_size(cpt,totalDim);
    tag.set_size(cpt);
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
        tag(tmpIdy)= mediaBrowser->getLibrary()->getMedia(ids[i])->getTaggedClassId();
        tmpIdy++;
    }
    // normalizing features between 0 and 1 ///////////////////////////////////////
    //   rowvec maxDesc_v = max(desc_m);
    //   rowvec minDesc_v = min(desc_m);
    //   desc_m = desc_m - repmat(minDesc_v, desc_m.n_rows, 1);
    //   desc_m = desc_m/repmat(maxDesc_v-minDesc_v, desc_m.n_rows, 1);
    /////////////////////////////////////////////////////////////////////////////////
}

void ACMlPackTaggedGMMPlugin::transferToClusterCenter(ACMediaBrowser* mediaBrowser,  const arma::mat& center){
    ACMediaLibrary *library=mediaBrowser->getLibrary();
    
    vector<vector<FeaturesVector> > clusterCenters;
    vector<float> featureWeights=mediaBrowser->getFeatureWeights();
    int clusterCount=mediaBrowser->getClusterCount();
    
    //clusterCenters.resize(clusterCount);
    clusterCenters = mediaBrowser->getClusterCenters();
    int feature_count = library->getFirstMedia()->getNumberOfPreProcFeaturesVectors();
    if (center.n_cols!=clusterCount){
        cerr<<"ACMlPackGMMPlugin::transferToClusterCenter bad dimensionnality for number of clusters"<<endl;
        return;
    }
    if ( clusterCenters.size()!=clusterCount)
        clusterCenters.resize(clusterCount);
    std::vector<float> weight=mediaBrowser->getWeightVector();
    for (int i=0;i<clusterCount;i++){
        int idf=0;
        clusterCenters[i].resize(feature_count);
        for (int j=0;j<feature_count;j++){
            int featDim = library->getFirstMedia()->getPreProcFeaturesVector(j)->getSize();
            clusterCenters[i][j].setDistanceType(library->getFirstMedia()->getPreProcFeaturesVector(j)->getFeaturesVector().getDistanceType()) ;
            clusterCenters[i][j].setIsSparse(library->getFirstMedia()->getPreProcFeaturesVector(j)->getFeaturesVector().getIsSparse()) ;
            clusterCenters[i][j].resize(library->getFirstMedia()->getPreProcFeaturesVector(j)->getFeaturesVector().size());
            
            if (weight[j]>0.f){
                for (int f=0;f<featDim;f++){
                    if (idf>=center.n_rows){
                        cerr<<"ACMlPackGMMPlugin::transferToClusterCenter bad dimensionnality for clusters centers dimension"<<endl;
                        return;
                    }
                    clusterCenters[i][j].set(f,center(idf,i));
                }
                idf++;
            }
        }
        mediaBrowser->setClusterCenter(i, clusterCenters[i]);
    }
}

void ACMlPackTaggedGMMPlugin::clusterNumberChanged(){
    if(!this->media_cycle) return;
    media_cycle->setClusterNumber( this->getNumberParameterValue("clusters") );
    media_cycle->updateDisplay(true);
    media_cycle->setNeedsDisplay(true);
}
