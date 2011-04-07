/**
 * @brief ACNeighborhoodsPluginEuclidean.cpp
 * @author Thierry Ravet
 * @date 07/04/2011
 * @copyright (c) 2011 – UMONS - Numediart
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

#include "ACNeighborhoodsPluginEuclidean.h"
#include "Armadillo-utils.h"

using namespace arma;
using namespace std;

ACNeighborhoodsPluginEuclidean::ACNeighborhoodsPluginEuclidean() {
    this->mMediaType = MEDIA_TYPE_MIXED; // ALL
    //this->mPluginType = PLUGIN_TYPE_NEIGHBORS_METHOD;
    this->mName = "EuclideanNeighborhoods";
    this->mDescription = "Plugin for the computation of Euclidean neighborhoods";
    this->mId = "";
    //local vars
		lastClickedNodeId = -1;
}

ACNeighborhoodsPluginEuclidean::~ACNeighborhoodsPluginEuclidean() {
}

void ACNeighborhoodsPluginEuclidean::updateNeighborhoods(ACMediaBrowser* mediaBrowser) {
	//int _clickedloop = mediaBrowser->getClickedLoop();
	std::cout << "ACNeighborhoodsPluginEuclidean::updateNeighborhoods" << std::endl;
	if (mediaBrowser->getUserLog()->getLastClickedNodeId() == -1) {	 
		mediaBrowser->getUserLog()->dump();
		mediaBrowser->getUserLog()->addRootNode(mediaBrowser->getReferenceNode(), 0); // 0
		mediaBrowser->getUserLog()->clickNode(mediaBrowser->getReferenceNode(), 0);
		lastClickedNodeId = 0;
	}
	else if ( (mediaBrowser->getUserLog()->getLastClickedNodeId() !=0) && (mediaBrowser->getUserLog()->getLastClickedNodeId() == lastClickedNodeId) ) {
			//CF define properly what to do if the user clicked twice on the same node: 
			//CF previous case: add 8 more children node
			//CF possible case: hide the children, or replace the nodes
	}	
	else{
		lastClickedNodeId = mediaBrowser->getUserLog()->getLastClickedNodeId();
		long targetMediaId = mediaBrowser->getUserLog()->getMediaIdFromNodeId(lastClickedNodeId);
		ACMedia* loop = mediaBrowser->getLibrary()->getMedia(0);
		
		long libSize = mediaBrowser->getLibrary()->getSize();
		int nbFeature = loop->getNumberOfFeaturesVectors();
		mat desc_m;
		rowvec weight_v; 
		mat tg_v;
		colvec dist_v(libSize);
		
		extractDescMatrix(mediaBrowser, desc_m, weight_v);
		tg_v = desc_m.row(targetMediaId);
		dist_v= sqrt(sum(square(desc_m - repmat(tg_v, desc_m.n_rows, 1)) % repmat(weight_v, desc_m.n_rows, 1), 1));

	//	std::cout << "1_v = " << std::endl <<  square(desc_m - repmat(tg_v, desc_m.n_rows, 1)) << std::endl;
	//	std::cout << "2_v = " << std::endl <<  repmat(weight_v, desc_m.n_rows, 1) << std::endl;
	//	std::cout << "3_v = " << std::endl <<  sum(square(desc_m - repmat(tg_v, desc_m.n_rows, 1)) % repmat(weight_v, desc_m.n_rows, 1),1) << std::endl;

		ucolvec sortRank_v = sort_index(dist_v);
		std::cout << "sortRank_v = " << sortRank_v(0) << " " << sortRank_v(1) << " " << sortRank_v(2) << std::endl;
		for (int k=1; k<10; k++){ // to avoid returning the request itself (k=1)
			mediaBrowser->getUserLog()->addNode(lastClickedNodeId, sortRank_v(k), 0);
		}
		mediaBrowser->getUserLog()->dump();
	}
	
	
}


void ACNeighborhoodsPluginEuclidean::extractDescMatrix(ACMediaBrowser* mediaBrowser, mat &desc_m, rowvec &weight_v){
  vector<ACMedia*> loops = mediaBrowser->getLibrary()->getAllMedia();
  int nbMedia = loops.size(); 
	int featDim;
	int totalDim = 0;
	
	// Count nb of feature
	int nbFeature = loops.back()->getNumberOfFeaturesVectors();
	for(int f=0; f< nbFeature; f++){
		featDim = loops.back()->getFeaturesVector(f)->getSize();
		std::cout << "feature weight " << f << " = " << mediaBrowser->getWeight(f) << std::endl;
		for(int d=0; d < featDim; d++){
			totalDim++;
		}
	}
	
  desc_m.set_size(nbMedia,totalDim);
  weight_v.set_size(totalDim);
  mat pos_m(nbMedia,2);
  
  for(int i=0; i<nbMedia; i++) {    
    int tmpIdx = 0;
    for(int f=0; f< nbFeature; f++){
			//std::cout << f << std::endl;
      featDim = loops.back()->getFeaturesVector(f)->getSize();
			for(int d=0; d < featDim; d++){
				desc_m(i,tmpIdx) = loops[i]->getFeaturesVector(f)->getFeatureElement(d);
				weight_v(tmpIdx) = mediaBrowser->getWeight(f);
				tmpIdx++;
      }
    }
  }
}
