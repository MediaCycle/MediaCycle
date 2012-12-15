/*
 *  ACKNN.cpp
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

#include "ACKNN.h"
#include "Armadillo-utils.h"
static int statCpt=0;

static double compute_distance(vector<ACMediaFeatures*> &obj1, vector<ACMediaFeatures*> &obj2, const vector<float> &weights, bool inverse_features)
{
    assert(obj1.size() == obj2.size() && obj1.size() == weights.size());
    int feature_count = obj1.size();
    
    double dis = 0.0;
    
    for (int f=0; f<feature_count; f++) {
        //		ACEuclideanDistance* E = new ACEuclideanDistance (obj1[f], obj2[f]);
        //		dis += E->distance() * (inverse_features?(1.0-weights[f]):weights[f]);
        float temp=obj1[f]->getFeaturesVector().distance(obj2[f]->getFeaturesVector());
        dis +=temp*temp*(inverse_features?(1.0-weights[f]):weights[f]);
        //	delete E;
    }
    dis = sqrt(dis);
    
    return dis;
}

// this one is mostly used
// e.g., compute_distance(mLibrary->getMedia(i)->getAllPreProcFeaturesVectors(), mClusterCenters[j], mFeatureWeights, false);

static double compute_distance(vector<ACMediaFeatures*> &obj1, const vector<FeaturesVector> &obj2, const vector<float> &weights, bool inverse_features)
{
    int s1=obj1.size();
    int s2=obj2.size();
    int s3=weights.size();
    
    double dis = 0.0;
    if( (obj1.size() != obj2.size()) || (obj1.size() != weights.size())){
        //std::cerr << "ACKNN::compute_distance: obj1.size() " << obj1.size() << " obj2.size() " << obj2.size() << " weights.size() " << weights.size() << std::endl;
        return dis;
    }
    
    //assert(obj1.size() == obj2.size()) ;
    //assert(obj1.size() == weights.size());
    
    int feature_count = obj1.size();
    
    for (int f=0; f<feature_count; f++) {
        //ACEuclideanDistance* E = new ACEuclideanDistance (&(obj1[f]->getFeaturesVector()), (FeaturesVector *) &obj2[f]);
        //FeaturesVector tmp  = obj1[f]->getFeaturesVector();
        //	ACEuclideanDistance* E = new ACEuclideanDistance (obj1[f]->getFeaturesVector(),  (FeaturesVector *) &obj2[f]);
        //	dis += E->distance() * (inverse_features?(1.0-weights[f]):weights[f]);
        //	delete E;
        float temp=obj1[f]->getFeaturesVector().distance(obj2[f]);
        dis +=temp*temp*(inverse_features?(1.0-weights[f]):weights[f]);
    }
    dis = sqrt(dis);
    
    return dis;
}

ACKNN::ACKNN()
: ACClusterMethodPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle Knn";
    this->mDescription = "Plugin for Knn Classification";
    this->mId = "";
   }

ACKNN::~ACKNN() {
}


void ACKNN::updateClusters(ACMediaBrowser* mediaBrowser,bool needsCluster){
    ACMediaLibrary *library=mediaBrowser->getLibrary();
    
    int clusterCount=this->getNumberParameterValue("clusters");
    
    int i,j,d,f;
    
    if(library == 0) {
        cerr << "<ACMediaBrowser::updateClustersKMeans> : Media Library 0" << endl;
        return;
    }
    else if(library->isEmpty()) {
        cerr << "<ACMediaBrowser::updateClustersKMeans> : empty Media Library " << endl;
        return;
    }
    
    //    vector<int> currTempId=library->getParentIds();
    vector<int> currId;
    
    vector<int> taggedNodeId;
    //    for (int i=0;i<currTempId.size();i++){
    for (ACMediaNodes::const_iterator it=mediaBrowser->getMediaNodes().begin(); it !=mediaBrowser->getMediaNodes().end();it++){
        if(it->second){
			cout << "new media" << endl;
			cout << library->getMedia(it->first)->getMediaType() << endl;
			cout << library->getMedia(it->first)->getParentId() << endl;
			cout << it->second->getNavigationLevel() << endl;
            if ((library->getMedia(it->first)->getMediaType() == library->getMediaType()) &&(it->second->getNavigationLevel() >= mediaBrowser->getNavigationLevel()))
            {
                currId.push_back(it->first);
                int temp=library->getMedia(it->first)->getTaggedClassId();
                if (temp>-1){
                    cout<<"ACKNN::updateClusters media Id "<<it->first<<" tagged: "<<temp<<endl;
                    taggedNodeId.push_back(it->first);
                }
            }
        }
    }
    
    
    //int object_count = library->getSize();//TR clustering just Parent Nodes
    int object_count = currId.size();
    if (taggedNodeId.size()>0){
        
        // XS note: problem if all media don't have the same number of features
        //          but we suppose it is not going to happen
        int feature_count = library->getMedia(currId[0])->getNumberOfPreProcFeaturesVectors();
        
    
        vector<float> featureWeights=mediaBrowser->getFeatureWeights();
        
        
        // Estimate Cluster Centers
        
        // Assign Samples to Clusters
        for(i=0; i<object_count; i++) {
            if(mediaBrowser->getMediaNode(currId[i])->getNavigationLevel() < mediaBrowser->getNavigationLevel()) continue;
            if (library->getMedia(currId[i])->getTaggedClassId()>-1){
                mediaBrowser->getMediaNode(currId[i])->setClusterId(library->getMedia(currId[i])->getTaggedClassId());
                continue;
            }

            // check if we should include this object
            vector< float > 		knn_distances; // for computation
            knn_distances.resize(taggedNodeId.size());
            // compute distance between this object and every cluster
            for(j=0; j<taggedNodeId.size(); j++) {
                knn_distances[j] = 0;
                if(library->getMedia(currId[i])->getType() == library->getMediaType())//CF multimedia compatibility
                    knn_distances[j] = compute_distance(library->getMedia(currId[i])->getAllPreProcFeaturesVectors(), library->getMedia(taggedNodeId[j])->getAllPreProcFeaturesVectors(), featureWeights, false);
            }
            // pick the one with smallest distance
            int jmin;
            jmin = min_element(knn_distances.begin(), knn_distances.end()) - knn_distances.begin();
            
            // assign cluster
            mediaBrowser->getMediaNode(currId[i])->setClusterId(library->getMedia(taggedNodeId[jmin])->getTaggedClassId());
            
        }
        
    }
    else{
        for (ACMediaNodes::const_iterator it=mediaBrowser->getMediaNodes().begin(); it !=mediaBrowser->getMediaNodes().end();it++){
            if(it->second){
                if ((library->getMedia(it->first)->getMediaType() == library->getMediaType()) &&(it->second->getNavigationLevel() >= mediaBrowser->getNavigationLevel()))
                {
                    it->second->setDisplayed(true);
                    it->second->setClusterId(0);
                }
            }
        }
        
    }
	currId.clear();
	
	
}
