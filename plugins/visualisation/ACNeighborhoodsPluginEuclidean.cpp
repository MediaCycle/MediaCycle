/**
 * @brief ACNeighborhoodsPluginEuclidean.cpp
 * @author Thierry Ravet
 * @date 18/09/2012
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

#include "ACNeighborhoodsPluginEuclidean.h"

using namespace arma;
using namespace std;

ACNeighborhoodsPluginEuclidean::ACNeighborhoodsPluginEuclidean() : ACNeighborMethodPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle Euclidean";
    this->mDescription = "Plugin for the computation of Euclidean neighborhoods";
    this->mId = "";
    //local vars
    lastClickedNodeId = -1;
    nNeighbors = 10;
    //CF changing neighbor number requires removing media nodes
    //this->addNumberParameter("neighbors",nNeighbors,1,15,1,"number of desired neighbors",boost::bind(&ACNeighborhoodsPluginEuclidean::neighborsNumberChanged,this));
}

ACNeighborhoodsPluginEuclidean::~ACNeighborhoodsPluginEuclidean() {
}

void ACNeighborhoodsPluginEuclidean::neighborsNumberChanged(){
    if(!this->media_cycle) return;
    media_cycle->updateDisplay(true);
    media_cycle->setNeedsDisplay(true);
}

void ACNeighborhoodsPluginEuclidean::updateNeighborhoods(ACMediaBrowser* mediaBrowser) {
    std::cout << "ACNeighborhoodsPluginEuclidean::updateNeighborhoods" << std::endl;

    if(mediaBrowser->getLibrary()->getSize()==0) return;

    //CF changing neighbor number requires removing media nodes
    //nNeighbors=this->getNumberParameterValue("neighbors");

   
    //CF if the user log has just been (re)created
    if(mediaBrowser->getClickedNode() == -1)
        lastClickedNodeId = -1;

    //CF if the user clicked twice on the same node OR if updateNeighborhoods is called again without newly-clicked node (changing parameters)
    if (mediaBrowser->getClickedNode() >=0) {
        int newClickedNodeId=mediaBrowser->getClickedNode();
        lastClickedNodeId = newClickedNodeId;
        std::cout << "ACNeighborhoodsPluginEuclidean::updateNeighborhoods adding neighbors to node " << lastClickedNodeId << std::endl;
        ACMedia* mediaF = mediaBrowser->getLibrary()->getFirstMedia();

        long libSize = mediaBrowser->getLibrary()->getSize();
        int nbFeature = mediaF->getNumberOfPreProcFeaturesVectors();
        mat desc_m;
        rowvec weight_v;
        mat tg_v;
        colvec dist_v(libSize);

        std::vector<long> ids;// = mediaBrowser->getLibrary()->getAllMediaIds();
        const ACMediaNodes	mediaNodes=mediaBrowser->getMediaNodes();
        int lcn=0;
        int i=0;
//        for(ACMediaNodes::const_iterator media = mediaNodes.begin(); media != mediaNodes.end(); media++) 
//            if (media->second->getNavigationLevel() >= mediaBrowser->getNavigationLevel()){
//                ids.push_back(media->first);
        
        for(ACMediaNodes::const_iterator media = mediaNodes.begin(); media != mediaNodes.end(); media++)
            if ((mediaBrowser->getLibrary()->getMedia(media->first)->getMediaType() == mediaBrowser->getLibrary()->getMediaType()) &&(media->second->getNavigationLevel() >= mediaBrowser->getNavigationLevel())){
                //if (media->second->getNavigationLevel() >= mediaBrowser->getNavigationLevel()&&){
                ids.push_back(media->first);
                if (media->first==lastClickedNodeId)
                    lcn=i;
                i++;
            }
        extractDescMatrix(mediaBrowser,ids, desc_m, weight_v);
        
        //        tg_v = desc_m.row(lastClickedNodeId);
        tg_v = desc_m.row(lcn);
        dist_v= sqrt(sum(square(desc_m - repmat(tg_v, desc_m.n_rows, 1)) % repmat(weight_v, desc_m.n_rows, 1), 1));

        //	std::cout << "1_v = " << std::endl <<  square(desc_m - repmat(tg_v, desc_m.n_rows, 1)) << std::endl;
        //	std::cout << "2_v = " << std::endl <<  repmat(weight_v, desc_m.n_rows, 1) << std::endl;
        //	std::cout << "3_v = " << std::endl <<  sum(square(desc_m - repmat(tg_v, desc_m.n_rows, 1)) % repmat(weight_v, desc_m.n_rows, 1),1) << std::endl;

        int test=ids.size();
        ucolvec sortRank_v = sort_index(dist_v);

        int k=0,m=1; // m=1 to avoid returning the request itself (k=1)
        while(k<nNeighbors && m<ids.size() && m < sortRank_v.n_elem){
            if( mediaBrowser->addNeighborNode(lastClickedNodeId, ids[sortRank_v(m)], 0))
                k++;
            m++;
        }
    }
    std::cout << "---" << std::endl;
    mediaBrowser->dumpNeighborNodes();
    std::cout << "ACNeighborhoodsPluginPareto::updateNeighborhoods done" << std::endl;
}

void ACNeighborhoodsPluginEuclidean::extractDescMatrix(ACMediaBrowser* mediaBrowser, std::vector<long> ids, mat &desc_m, rowvec &weight_v){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
   // int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;

    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfPreProcFeaturesVectors();
    for(int f=0; f< nbFeature; f++){
        featDim = mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getSize();
        std::cout << "feature weight " << f << " = " << mediaBrowser->getWeight(f) << std::endl;
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
    for (std::vector<long>::iterator it=ids.begin();it!=ids.end();it++){
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
