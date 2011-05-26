/*
 *  ACKMeansPlugin.cpp
 *  MediaCycle
 *
 *  @author Ravet Thierry
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

#include "ACKMeansPlugin.h"
#include "Armadillo-utils.h"
static int statCpt=0;
ACKMeansPlugin::ACKMeansPlugin() {
    //vars herited from ACPlugin
    this->mMediaType = MEDIA_TYPE_ALL;
    //this->mPluginType = PLUGIN_TYPE_CLUSTERS_METHOD;
    this->mName = "ACKMeans";
    this->mDescription = "Clustering";
    this->mId = "";
}

ACKMeansPlugin::~ACKMeansPlugin() {
}


void ACKMeansPlugin::updateClusters(ACMediaBrowser* mediaBrowser){
	ACMediaLibrary *library=mediaBrowser->getLibrary();
	int i,j,d,f;
	
	if(library == NULL) {
		cerr << "<ACKMeansPlugin::updateClustersKMeans> : Media Library NULL" << endl;
		return;
	}
	else if(library->isEmpty()) {
		cerr << "<ACKMeansPlugin::updateClustersKMeans> : empty Media Library " << endl;
		return;
	}
	int object_count = library->getSize();
	
	// XS note: problem if all media don't have the same number of features
	//          but we suppose it is not going to happen
	int feature_count = library->getMedia(0)->getNumberOfFeaturesVectors();
	
	vector< int > 			cluster_counts;
	vector<vector<vector <float> > >cluster_accumulators; // cluster, feature, desc
	vector< float > 		cluster_distances; // for computation
	int clusterCount=mediaBrowser->getClusterCount();
	vector<vector<vector <float> > > clusterCenters; // cluster index, feature index, descriptor index
	vector<float>			featureWeights=mediaBrowser->getFeatureWeights(); // each value must be in [0,1], important for euclidian distance.
	
	int navigationLevel=mediaBrowser->getNavigationLevel();
	
	// picking random object as initial cluster center
	srand(15);
	clusterCenters.resize(clusterCount);
	cluster_counts.resize(clusterCount);
	cluster_accumulators.resize(clusterCount);
	cluster_distances.resize(clusterCount);
	statCpt++;
	srand(clusterCount*statCpt);
	for(i=0; i<clusterCount; i++)
	{
		clusterCenters[i].resize(feature_count);
		cluster_accumulators[i].resize(feature_count);
		
		// initialize cluster center with a randomly chosen object
		int r = rand() % object_count;
		int l = 100;
		
		// TODO SD - Avoid selecting the same twice
		while(l--)
		{
			if(mediaBrowser->getMediaNode(r).getNavigationLevel() >= navigationLevel) break;
			else r = rand() % object_count;
		}
		
		// couldn't find center in this nav level...
		if(l <= 0) return;
		
		for(f=0; f<feature_count; f++)
		{
			// XS again, what if all media don't have the same number of features ?
			int desc_count = library->getMedia(0)->getPreProcFeaturesVector(f)->getSize();
			
			clusterCenters[i][f].resize(desc_count);
			cluster_accumulators[i][f].resize(desc_count);
			
			for(d=0; d<desc_count; d++)
			{
				clusterCenters[i][f][d] = library->getMedia(r)->getPreProcFeaturesVector(f)->getFeatureElement(d);
				
				//printf("cluster  %d center: %f\n", i, clusterCenters[i][f][d]);
			}
		}
	}
	
	int n_iterations = 20, it;
	
	printf("<ACKMeansPlugin>feature weights:");
	for (unsigned int fw=0; fw < featureWeights.size(); fw++)
		printf("%f ", featureWeights[fw]);
	printf("\n");
	
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
				int desc_count = library->getMedia(0)->getPreProcFeaturesVector(f)->getSize();
				
				for(d=0; d<desc_count; d++)
				{
					cluster_accumulators[i][f][d] = 0.0;
				}
			}
		}
		for(i=0; i<object_count; i++)
		{
			// check if we should include this object
			// note: the following "if" skips to next i if true.
			if(mediaBrowser->getMediaNode(i).getNavigationLevel() < navigationLevel) continue;
			
			// compute distance between this object and every cluster
			for(j=0; j<clusterCount; j++)
			{
				cluster_distances[j] = 0;
				cluster_distances[j] = compute_distance(library->getMedia(i)->getAllPreProcFeaturesVectors(), clusterCenters[j], featureWeights, false);
				
				//printf("distance cluster %d to object %d = %f\n", j, i,  cluster_distances[j]);
			}
			
			
			// pick the one with smallest distance
			int jmin;
			
			jmin = min_element(cluster_distances.begin(), cluster_distances.end()) - cluster_distances.begin();
			
			// update accumulator and counts
			
			cluster_counts[jmin]++;
			mediaBrowser->getMediaNode(i).setClusterId (jmin);
			for(f=0; f<feature_count; f++)
			{
				// XS again, what if all media don't have the same number of features ?
				int desc_count = library->getMedia(0)->getPreProcFeaturesVector(f)->getSize();
				
				for(d=0; d<desc_count; d++)
				{
					cluster_accumulators[jmin][f][d] += library->getMedia(i)->getPreProcFeaturesVector(f)->getFeatureElement(d);
				}
			}
		}
		
		printf("K-means it: %d\n", it);
		// get new centers from accumulators
		for(j=0; j<clusterCount; j++)
		{
			if(cluster_counts[j] > 0)
			{
				for(f=0; f<feature_count; f++)
				{
					// XS again, what if all media don't have the same number of features ?
					int desc_count = library->getMedia(0)->getPreProcFeaturesVector(f)->getSize();
					
					for(d=0; d<desc_count; d++)
					{
						clusterCenters[j][f][d] = cluster_accumulators[j][f][d] / (float)cluster_counts[j];
					}
				}
			}
			
			printf("\tcluster %d count = %d\n", j, cluster_counts[j]); 
		}
	}
	mediaBrowser->initClusterCenters();
	
	for(j=0; j<clusterCount; j++)
	{
		mediaBrowser->setClusterCenter(j, clusterCenters[j]);
	}
	printf("<ACKMeansPlugin>clustering finished\n");
	
}


void ACKMeansPlugin::updateNextPositions(ACMediaBrowser* mediaBrowser){
}