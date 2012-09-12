/**
 * @brief ACNeighborhoodsPluginPareto.cpp
 * @author Thierry Ravet
 * @date 12/09/2012
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

#include "ACNeighborhoodsPluginPareto.h"

using namespace arma;
using namespace std;

ACNeighborhoodsPluginPareto::ACNeighborhoodsPluginPareto() : ACNeighborMethodPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle Pareto";
    this->mDescription = "Plugin for the computation of Pareto neighborhoods";
    this->mId = "";
    lastClickedNodeId = -1;
    nNeighbors = 9;
    //CF changing neighbor number requires removing media nodes
    //this->addNumberParameter("neighbors",nNeighbors,1,15,1,"number of desired neighbors",boost::bind(&ACNeighborhoodsPluginPareto::neighborsNumberChanged,this));
}

ACNeighborhoodsPluginPareto::~ACNeighborhoodsPluginPareto() {
}

void ACNeighborhoodsPluginPareto::neighborsNumberChanged(){
    if(!this->media_cycle) return;
    media_cycle->updateDisplay(true);
    media_cycle->setNeedsDisplay(true);
}

void ACNeighborhoodsPluginPareto::updateNeighborhoods(ACMediaBrowser* mediaBrowser) {
    std::cout << "ACNeighborhoodsPluginPareto::updateNeighborhoods" << std::endl;

    if(mediaBrowser->getLibrary()->getSize()==0) return;

    //CF changing neighbor number requires removing media nodes
    //nNeighbors=this->getNumberParameterValue("neighbors");

  
    //CF if the user log has just been (re)created
    if(mediaBrowser->getClickedNode() == -1)
        lastClickedNodeId = -1;

    //CF if the user clicked twice on the same node OR if updateNeighborhoods is called again without newly-clicked node (changing parameters)
    if ( (mediaBrowser->getClickedNode() >=0) ) {
        int newClickedNodeId=mediaBrowser->getClickedNode();
        lastClickedNodeId = newClickedNodeId;
        std::cout << "ACNeighborhoodsPluginPareto::updateNeighborhoods adding neighbors to node " << lastClickedNodeId << std::endl;
        ACMedia* media = mediaBrowser->getLibrary()->getFirstMedia();

        long libSize = mediaBrowser->getLibrary()->getSize();
        int nbFeature = media->getNumberOfPreProcFeaturesVectors();
        mat tmpDesc_m;
        mat tmpTg_v;
        mat dist_m(libSize, nbFeature);

        for (int f=0; f<nbFeature; f++){
            tmpDesc_m = extractDescMatrix(mediaBrowser, f);
            tmpTg_v = tmpDesc_m.row(lastClickedNodeId);
            dist_m.col(f) = sqrt(sum(square(tmpDesc_m - repmat(tmpTg_v, tmpDesc_m.n_rows, 1)), 1));
        }
        ucolvec rank_v = paretorank(dist_m, 2, 6);
        ucolvec selPos_v = find(rank_v > 1);
        colvec distE_v(selPos_v.n_rows);
        for (int k=0; k<selPos_v.n_rows; k++){
            distE_v(k) = sum(dist_m.row(selPos_v(k)));
        }

        std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
        ucolvec selPos2_v = sort_index(distE_v);
        /*if (selPos2_v.n_rows < nNeighbors)
            nNeighbors = selPos2_v.n_rows;*/

        /*for (int k=0; k<nNeighbors; k++){
            mediaBrowser->addNeighborNode(lastClickedNodeId, ids[selPos_v(selPos2_v(k))], 0);
        }*/
        int k=0,m=0;
        while(k<nNeighbors && m<ids.size() && m < selPos2_v.n_rows){
            if( mediaBrowser->addNeighborNode(lastClickedNodeId, ids[selPos_v(selPos2_v(m))], 0))
                k++;
            m++;
        }
    }
    std::cout << "---" << std::endl;
    mediaBrowser->dumpNeighborNodes();
    std::cout << "ACNeighborhoodsPluginPareto::updateNeighborhoods done" << std::endl;
}

mat ACNeighborhoodsPluginPareto::extractDescMatrix(ACMediaBrowser* mediaBrowser, int descId){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    int nbMedia = medias.size();
    int featDim;

    // Count nb of feature
    featDim = mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(descId)->getSize();

    mat desc_m(nbMedia, featDim);
    int i = 0;
    for(ACMedias::iterator media = medias.begin(); media != medias.end(); media++) {
        int tmpIdx = 0;
        for(int d=0; d < featDim; d++){
            desc_m(i,d) = media->second->getPreProcFeaturesVector(descId)->getFeatureElement(d);
        }
        i++;
    }
    return desc_m;
}
