/*
 *  ACKnnRestrictedKMeansPlugin.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 23/04/2013
 *  @copyright (c) 2013 – UMONS - Numediart
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

#include "ACKnnRestrictedKMeansPlugin.h"
#include "Armadillo-utils.h"
static int statCpt=0;
using namespace arma;
using namespace std;

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
        //std::cerr << "ACKnnRestrictedKMeansPlugin::compute_distance: obj1.size() " << obj1.size() << " obj2.size() " << obj2.size() << " weights.size() " << weights.size() << std::endl;
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

ACKnnRestrictedKMeansPlugin::ACKnnRestrictedKMeansPlugin()
: ACClusterMethodPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle KMeans (restricted to Knn neighbors)";
    this->mDescription = "Plugin for KMeans Clustering of only child nodes";
    this->mId = "";
    this->addNumberParameter("clusters",8,1,10,1,"number of desired clusters",boost::bind(&ACKnnRestrictedKMeansPlugin::clusterNumberChanged,this));
    neighborsNumber=20;
    this->addNumberParameter("neighborsNumber",neighborsNumber,10,1000,10,"number of kept neighbors",boost::bind(&ACKnnRestrictedKMeansPlugin::neighborsNumberChanged,this));
}

ACKnnRestrictedKMeansPlugin::~ACKnnRestrictedKMeansPlugin() {
}


void ACKnnRestrictedKMeansPlugin::updateClusters(ACMediaBrowser* mediaBrowser,bool needsCluster){
    ACMediaLibrary *library=mediaBrowser->getLibrary();
    vector<int> currId;
    
    int clusterCount=mediaBrowser->getClusterCount();
    
//    media_cycle->setClusterNumber(  clusterCount);
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
    vector<int> ids;
    int lcn=-1;
    //    for (int i=0;i<currTempId.size();i++){
    int locCpt=0;
    
    for (ACMediaNodes::const_iterator it=mediaBrowser->getMediaNodes().begin(); it !=mediaBrowser->getMediaNodes().end();it++){
        if(it->second){
            it->second->setDisplayed(false);
            it->second->setClusterId(0);
			if ((library->getMedia(it->first)->getMediaType() == library->getMediaType()) &&(it->second->getNavigationLevel() >= mediaBrowser->getNavigationLevel())){
                ids.push_back(it->first);
                if (it->first==mediaBrowser->getReferenceNode()){
                    lcn=locCpt;
                }
                locCpt++;
            }
            
        }
    }
    int libSize=ids.size();
    if (libSize>neighborsNumber&& lcn>-1){
        mat desc_m;
        rowvec weight_v;
        mat tg_v;
        colvec dist_v(libSize);
        extractDescMatrix(mediaBrowser,ids, desc_m, weight_v);
    
        tg_v = desc_m.row(lcn);
        dist_v= sqrt(sum(square(desc_m - repmat(tg_v, desc_m.n_rows, 1)) % repmat(weight_v, desc_m.n_rows, 1), 1));
        ucolvec sortRank_v = sort_index(dist_v);
        for (int i=0;i<neighborsNumber;i++){
            currId.push_back(ids[sortRank_v[i]]);
        }
    
    }
    else{
        currId=ids;
    }
    for (int i=0;i<currId.size();i++){
        ACMediaNode *node=mediaBrowser->getMediaNode(currId[i]);
        if (node){
            node->setDisplayed(true);
            //cout<<"\t"<<currId[i];
            
        }
    }
   // cout<<endl;

    
    
    
    //int object_count = library->getSize();//TR clustering just Parent Nodes
    int object_count = currId.size();
    if (object_count>0){
        
        // XS note: problem if all media don't have the same number of features
        //          but we suppose it is not going to happen
        int feature_count = library->getMedia(currId[0])->getNumberOfPreProcFeaturesVectors();
        
        vector< int > 			cluster_counts;
        vector<vector< FeaturesVector > >cluster_accumulators; // cluster, feature, desc
        vector< float > 		cluster_distances; // for computation
        vector<vector<FeaturesVector> > clusterCenters; // cluster index, feature index, descriptor index
        vector<int> idNodeClusterCenters;
        vector<float> featureWeights=mediaBrowser->getFeatureWeights();
        
        clusterCenters = mediaBrowser->getClusterCenters();
        //clusterCenters.resize(clusterCount);
        
        idNodeClusterCenters.resize(clusterCount);
        cluster_counts.resize(clusterCount);
        cluster_accumulators.resize(clusterCount);
        cluster_distances.resize(clusterCount);
        
        // Estimate Cluster Centers
        if (needsCluster) {
            
            // picking random object as initial cluster center
            srand(15);
            
            // initialize cluster centers
            for(i=0; i<clusterCount; i++) {
                
                clusterCenters[i].resize(feature_count);
                cluster_accumulators[i].resize(feature_count);
                
                // initialize cluster center with a randomly chosen object
                int r = 0;//CF
                //if(library->getMediaType() != MEDIA_TYPE_MIXED)//TR we work just with parent node, then there is no more mediatypecompatibility problem
                {//CF
                    //r = rand() % object_count;
                    // SD OCT 2010 - for gradual appearance of media to be more stable
                    r = i % object_count;
                    int l = 100;
                    
                    // TODO SD - Avoid selecting the same twice
                    //while(l--)
                    {
                        //	if(mediaBrowser->getMediaNode(currId[r]).getNavigationLevel() >= mNavigationLevel) break;
                        //	else r = rand() % object_count;
                    }
                    
                    // couldn't find center in mediaBrowser nav level...
                    if(l <= 0) return;
                }
                
                for(f=0; f<feature_count; f++)
                {
                    // XS again, what if all media don't have the same number of features ?
                    int desc_count = library->getMedia(currId[0])->getPreProcFeaturesVector(f)->getSize();
                    
                    /*mClusterCenters[i][f].setDistanceType(library->getMedia(currId[0])->getPreProcFeaturesVector(f)->getFeaturesVector()->getDistanceType());
                     mClusterCenters[i][f].setIsSparse(library->getMedia(currId[0])->getPreProcFeaturesVector(f)->getFeaturesVector()->getIsSparse());
                     mClusterCenters[i][f].resize(desc_count);*/
                    
                    cluster_accumulators[i][f].setDistanceType(library->getMedia(currId[0])->getPreProcFeaturesVector(f)->getFeaturesVector().getDistanceType()) ;
                    cluster_accumulators[i][f].setIsSparse(library->getMedia(currId[0])->getPreProcFeaturesVector(f)->getFeaturesVector().getIsSparse()) ;
                    cluster_accumulators[i][f].resize(library->getMedia(currId[0])->getPreProcFeaturesVector(f)->getFeaturesVector().size());
                    
                    if(library->getMedia(currId[r]))//CF
                        if(library->getMedia(currId[r])->getType() == library->getMediaType())//CF
                            clusterCenters[i][f]= library->getMedia(currId[r])->getPreProcFeaturesVector(f)->getFeaturesVector();
                    /*for(d=0; d<desc_count; d++)
                     {
                     if(library->getMedia(currId[r])->getType() == library->getMediaType())//CF
                     mClusterCenters[i][f][d] = library->getMedia(currId[r])->getPreProcFeaturesVector(f)->getFeatureElement(d);
                     
                     //printf("cluster  %d center: %f\n", i, mClusterCenters[i][f][d]);
                     }*/
                }
            }
            
            int n_iterations = 20, it;
            
#ifdef VERBOSE
            printf("feature weights:");
            for (unsigned int fw=0; fw < featureWeights.size(); fw++)
                printf("%f ", featureWeights[fw]);
            printf("\n");
#endif // VERBOSE
            
            // applying a few K-means iterations
            for(it = 0; it < n_iterations; it++)
            {
                // reset accumulators and counts
                for(i=0; i<clusterCount; i++)
                {
                    cluster_counts[i] = 0;
                    for(f=0; f<feature_count; f++)
                    {
                        // XS again, what if all media don't have the same number of features ?
                        //int desc_count = library->getMedia(currId[0])->getPreProcFeaturesVector(f)->getSize();
                        cluster_accumulators[i][f].init();
                    }
                }
                
                for(i=0; i<object_count; i++)
                {
                    // check if we should include mediaBrowser object
                    // note: the following "if" skips to next i if true.
                    if(mediaBrowser->getMediaNode(currId[i])) continue;
                    if(mediaBrowser->getMediaNode(currId[i])->getNavigationLevel() < mediaBrowser->getNavigationLevel()) continue;
                    
                    // compute distance between this object and every cluster
                    for(j=0; j<clusterCount; j++)
                    {
                        cluster_distances[j] = 0;
                        if(library->getMedia(currId[i])->getType() == library->getMediaType())//CF multimedia compatibility
                            cluster_distances[j] = compute_distance(library->getMedia(currId[i])->getAllPreProcFeaturesVectors(), clusterCenters[j], featureWeights, false);
                        //printf("distance cluster %d to object %d = %f\n", j, i,  cluster_distances[j]);
                    }
                    // pick the one with smallest distance
                    int jmin;
                    
                    jmin = min_element(cluster_distances.begin(), cluster_distances.end()) - cluster_distances.begin();
                    
                    // update accumulator and counts
                    
                    cluster_counts[jmin]++;
                    
                    // SD 2010 OCT - see below
                    //mediaBrowser->getMediaNode(i).setClusterId (jmin);
                    
                    for(f=0; f<feature_count; f++)
                    {
                        // XS again, what if all media don't have the same number of features ?
                        int desc_count = library->getMedia(currId[0])->getPreProcFeaturesVector(f)->getSize();
                        if(library->getMedia(currId[i])->getType() == library->getMediaType())//CF
                            cluster_accumulators[jmin][f].meanAdd( library->getMedia(currId[i])->getPreProcFeaturesVector(f)->getFeaturesVector());
                    }
                }
                
#ifdef VERBOSE
                printf("K-means it: %d\n", it);
#endif // VERBOSE
                // get new centers from accumulators
                float stopDist=0.f;
                for(j=0; j<clusterCount; j++)
                {
                    if(cluster_counts[j] > 0)
                    {
                        for(f=0; f<feature_count; f++)
                        {
                            // XS again, what if all media don't have the same number of features ?
                            int desc_count = library->getMedia(currId[0])->getPreProcFeaturesVector(f)->getSize();
                            cluster_accumulators[j][f]/=(float)cluster_counts[j];
                            stopDist+=clusterCenters[j][f].distance(cluster_accumulators[j][f]);
                            clusterCenters[j][f] = cluster_accumulators[j][f] ;
                        }
                    }
#ifdef VERBOSE
                    printf("\tcluster %d count = %d\n", j, cluster_counts[j]);
#endif //VERBOSE
                }
                if (stopDist==0.f)
                    break;
            }
            mediaBrowser->setClusterCenters(clusterCenters);
        }
        
        vector<float> centerDistMin;
        centerDistMin.resize(clusterCount);
        for (int j=0;j<clusterCount;j++){
            centerDistMin[j]=FLT_MAX;
        }
        // Assign Samples to Clusters
        for(i=0; i<object_count; i++) {
            // check if we should include this object
            if(mediaBrowser->getMediaNode(currId[i])->getNavigationLevel() < mediaBrowser->getNavigationLevel()) continue;
            
            // compute distance between this object and every cluster
            for(j=0; j<clusterCount; j++) {
                cluster_distances[j] = 0;
                if(library->getMedia(currId[i])->getType() == library->getMediaType())//CF multimedia compatibility
                    cluster_distances[j] = compute_distance(library->getMedia(currId[i])->getAllPreProcFeaturesVectors(), clusterCenters[j], featureWeights, false);
            }
            // pick the one with smallest distance
            int jmin;
            jmin = min_element(cluster_distances.begin(), cluster_distances.end()) - cluster_distances.begin();
            
            // assign cluster
            mediaBrowser->getMediaNode(currId[i])->setClusterId(jmin);
            if (cluster_distances[jmin]<centerDistMin[jmin]){
                idNodeClusterCenters[jmin]=currId[i];
                centerDistMin[jmin]=cluster_distances[jmin];
            }
        }
        
        for (int i=0;i<clusterCount;i++)
            mediaBrowser->setClusterCenter(i, clusterCenters[i] );
        mediaBrowser->setIdNodeClusterCenter(idNodeClusterCenters);
    }
    
	
	
}

void ACKnnRestrictedKMeansPlugin::clusterNumberChanged(){
    if(!this->media_cycle) return;
    media_cycle->setClusterNumber( this->getNumberParameterValue("clusters") );
    // XSCF251003 added this
    media_cycle->updateDisplay(true);
    // XS 310310 removed media_cycle->setNeedsDisplay(true); // now in updateDisplay
    //osg_view->updateTransformsFromBrowser(1.0); //CF 29/06/2012, this is called by ACOsgCompositeViewQt::updateGL if mediacycle needs display
    media_cycle->setNeedsDisplay(true);
}

void ACKnnRestrictedKMeansPlugin::neighborsNumberChanged(){
    if(!this->media_cycle) return;
    neighborsNumber=this->getNumberParameterValue("neighborsNumber");
    // XSCF251003 added this
    media_cycle->updateDisplay(true);
    // XS 310310 removed media_cycle->setNeedsDisplay(true); // now in updateDisplay
    //osg_view->updateTransformsFromBrowser(1.0); //CF 29/06/2012, this is called by ACOsgCompositeViewQt::updateGL if mediacycle needs display
    media_cycle->setNeedsDisplay(true);
}


void ACKnnRestrictedKMeansPlugin::extractDescMatrix(ACMediaBrowser* mediaBrowser, std::vector<int> ids, mat &desc_m, rowvec &weight_v){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    // int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;
    
    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfPreProcFeaturesVectors();
    for(int f=0; f< nbFeature; f++){
        featDim = mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getSize();
        //std::cout << "feature weight " << f << " = " << mediaBrowser->getWeight(f) << std::endl;
        for(int d=0; d < featDim; d++){
            totalDim++;
        }
    }
    int mediaDim=ids.size();
    //   for(ACMedias::iterator media = medias.begin(); media != medias.end(); media++)
    //         if (mediaBrowser->getMediaNode(media->first)->getNavigationLevel() >= media_cycle->getNavigationLevel())
    //           mediaDim++;
    
    //    desc_m.set_size(nbMedia,totalDim);
    desc_m.set_size(mediaDim,totalDim);
    
    weight_v.set_size(totalDim);
    //mat pos_m(nbMedia,2);
    mat pos_m(mediaDim,2);
    
    int i = 0;
    //for(ACMedias::iterator media = medias.begin(); media != medias.end(); media++) {
    for (std::vector<int>::iterator it=ids.begin();it!=ids.end();it++){
        //if (mediaBrowser->getMediaNode(*it)->getNavigationLevel() >= media_cycle->getNavigationLevel()){
        
        int tmpIdx = 0;
        for(int f=0; f< nbFeature; f++){
            //std::cout << f << std::endl;
            featDim = mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getSize();
            for(int d=0; d < featDim; d++){
                desc_m(i,tmpIdx) = medias[*it]->getPreProcFeaturesVector(f)->getFeatureElement(d);
                weight_v(tmpIdx) = mediaBrowser->getWeight(f);
                tmpIdx++;
            }
        }
        i++;
        
    }
}

