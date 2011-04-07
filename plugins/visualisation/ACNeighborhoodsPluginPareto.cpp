/**
 * @brief ACNeighborhoodsPluginPareto.cpp
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

#include "ACNeighborhoodsPluginPareto.h"

using namespace arma;
using namespace std;

ACNeighborhoodsPluginPareto::ACNeighborhoodsPluginPareto() {
    this->mMediaType = MEDIA_TYPE_MIXED; // ALL
    //this->mPluginType = PLUGIN_TYPE_NEIGHBORS_METHOD;
    this->mName = "ParetoNeighborhoods";
    this->mDescription = "Plugin for the computation of Pareto neighborhoods";
    this->mId = "";
		lastClickedNodeId = -1;	
    //local vars
}

ACNeighborhoodsPluginPareto::~ACNeighborhoodsPluginPareto() {
}

void ACNeighborhoodsPluginPareto::updateNeighborhoods(ACMediaBrowser* mediaBrowser) {
	//int _clickedloop = mediaBrowser->getClickedLoop();
	std::cout << "ACNeighborhoodsPluginPareto::updateNeighborhoods" << std::endl;
	if (mediaBrowser->getUserLog()->getLastClickedNodeId() == -1) {	 
		mediaBrowser->getUserLog()->addRootNode(mediaBrowser->getReferenceNode(), 0); // 0
		mediaBrowser->getUserLog()->clickNode(mediaBrowser->getReferenceNode(), 0);
		lastClickedNodeId = 0;
	}
	else{
		lastClickedNodeId = mediaBrowser->getUserLog()->getLastClickedNodeId();
		long targetMediaId = mediaBrowser->getUserLog()->getMediaIdFromNodeId(lastClickedNodeId);
		ACMedia* loop = mediaBrowser->getLibrary()->getMedia(0);
		
		long libSize = mediaBrowser->getLibrary()->getSize();
		int nbFeature = loop->getNumberOfFeaturesVectors();
		mat tmpDesc_m;
		mat tmpTg_v;
		mat dist_m(libSize, nbFeature);

		for (int f=0; f<nbFeature; f++){
			tmpDesc_m = extractDescMatrix(mediaBrowser, f);
			tmpTg_v = tmpDesc_m.row(targetMediaId);
			dist_m.col(f) = sqrt(sum(square(tmpDesc_m - repmat(tmpTg_v, tmpDesc_m.n_rows, 1)), 1));
		}
		ucolvec rank_v = paretorank(dist_m, 2, 6);
		ucolvec selPos_v = find(rank_v > 1);
		colvec distE_v(selPos_v.n_rows);
		for (int k=0; k<selPos_v.n_rows; k++){
			distE_v(k) = sum(dist_m.row(selPos_v(k)));
		}
		
		int nbItems = 9;
		ucolvec selPos2_v = sort_index(distE_v);
		if (selPos2_v.n_rows < nbItems)
			nbItems = selPos2_v.n_rows;
			
		for (int k=0; k<nbItems; k++){
			mediaBrowser->getUserLog()->addNode(lastClickedNodeId, selPos_v(selPos2_v(k)), 0);
		}
		mediaBrowser->getUserLog()->dump();
	}	
}


mat ACNeighborhoodsPluginPareto::extractDescMatrix(ACMediaBrowser* mediaBrowser, int descId){
  vector<ACMedia*> loops = mediaBrowser->getLibrary()->getAllMedia();
  int nbMedia = loops.size(); 
	int featDim;

	// Count nb of feature
	featDim = loops.back()->getFeaturesVector(descId)->getSize();
	
  mat desc_m(nbMedia, featDim);
  for(int i=0; i<nbMedia; i++) {    
    int tmpIdx = 0;
		for(int d=0; d < featDim; d++){
			desc_m(i,d) = loops[i]->getFeaturesVector(descId)->getFeatureElement(d);
		}
  }
	return desc_m;
}
