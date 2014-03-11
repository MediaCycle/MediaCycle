/**
 * @brief Plugin for gridding nodes using the unispring algorithm
 * @author Christian Frisson
 * @date 27/02/2014
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

// Inspired partially from
//@INPROCEEDINGS{Lallemand2011,
//  author = {Ianis Lallemand and Diemo Schwarz},
//  title = {Interaction-optimized sound database representation},
//  booktitle = {Proceedings of the 14th International Conference on Digital Audio
//	Effects (DAFx-11)},
//  year = {2011}
//}
// but computed by decreasing high dimensional distance

#include<armadillo>
#include "ACFilterPlugUnispring.h"

// Waiting for Delaunay triangulation in boost::polygon
#include <s_hull.h>

using namespace arma;

ACFilterPlugUnispring::ACFilterPlugUnispring() : QObject(), ACPluginQt(), ACFilteringPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle Unispring";
    this->mDescription = "Plugin for positioning items on a grid using the unispring algorithm";
    this->mId = "";

    this->addCallback("Update","Update view",boost::bind(&ACFilterPlugUnispring::setProximityGrid,this));

    this->addNumberParameter("Iterations",100,1,1000,1,"Iterations");
    methods.push_back("Expand 2D");
    methods.push_back("Fit 2D");
    this->addStringParameter("Method",methods.front(),methods,"Morph methods"/*,boost::bind(&ACFilterPlugUnispring::setProximityGrid,this)*/);
}


ACFilterPlugUnispring::~ACFilterPlugUnispring() {
}

void ACFilterPlugUnispring::filter() {
    //this->setProximityGrid();
    preFilterPositions.clear();
    gridSize = 0;

    int libSize = media_cycle->getLibrarySize();

    /*int*/ gridSize = ceil(sqrt(libSize));

    // this->updateNumberParameter("Grid side",libSize,gridSize,libSize,1);
}

void ACFilterPlugUnispring::setProximityGrid() {

    if(!media_cycle) return;
    if(media_cycle->getLibrarySize()==0) return;

    float osg = 0.33f;

    int libSize = media_cycle->getLibrarySize();
    std::vector<long> ids = media_cycle->getLibrary()->getAllMediaIds();

    if(libSize != ids.size())
        std::cout << "ACFilterPlugUnispring::updateNextPositions: warning, lib size and number of accessible media ids don't match"<< std::endl;
    //int currentSize = this->getNumberParameterValue("Grid side");
    //gridSize = currentSize;
    if(gridSize == 0)
        return;
    std::cout << "gridSize " << gridSize << " for " << libSize << " elements " << std::endl;

    //if(ids.size() != preFilterPositions.size()){ // TODO a better test against insertions/deletions
    preFilterPositions.clear();

    for (int i=0; i<ids.size(); i++){
        ACMediaNode* node = media_cycle->getMediaNode(ids[i]);
        if(node){
            preFilterPositions[ids[i]] = node->getNextPosition();
        }
    }
    //}

    arma::mat pos;
    pos.set_size(libSize,2);//(2,libSize);

    float min_x(1),min_y(1),max_x(-1),max_y(-1);
    for (int i=0; i<ids.size(); i++){
        ACPoint p = preFilterPositions[ids[i]] ;
        pos(i,0) = -osg+2*osg*p.x;
        pos(i,1) = -osg+2*osg*p.y;
    }

    // Minimal desired edge distance (2D)
    float mindist = 2*2*osg/(float)(gridSize-1);
    int iter = 0;
    float idealdist = 0; // will be computed after Delaunay triangulation
    float totaldist = 1; // will be computed after Delaunay triangulation
    bool repeateddist = false;

    int maxiter = this->getNumberParameterValue("Iterations");

    // Quick and dirty condition
    while(totaldist > idealdist && iter < maxiter && !repeateddist ) {

        // Sort coordinates in a square grid of side the lib size
        arma::umat id_x = sort_index(sort_index( pos.col(0)));
        arma::umat id_y = sort_index(sort_index( pos.col(1)));

        arma::umat pre_uniform;
        pre_uniform.set_size(libSize,2);
        pre_uniform = arma::join_rows(id_x,id_y);

        std::vector<Shx> pts;
        Shx pt;

        for(int v=0; v<ids.size(); v++){
            pt.id = ids[v];
            pt.r = pre_uniform(v,0);
            pt.c = pre_uniform(v,1);
            pos(v,0) = -osg+2*osg*pre_uniform(v,0)/(float)libSize;
            pos(v,1) = -osg+2*osg*pre_uniform(v,1)/(float)libSize;
            pts.push_back(pt);
        }

        // Compute Delaunay triangulation over the positions
        std::vector<Triad> triads;
        s_hull_del_ray2( pts, triads);

        std::map<int,std::set<int> > pairs;

        // Find unique pairs
        for(int t=0; t<triads.size(); t++){

            arma::vec tri;
            tri.resize(3);
            tri(0) = triads[t].a;
            tri(1) = triads[t].b;
            tri(2) = triads[t].c;
            arma::vec s = sort(tri);

            pairs[ s(0) ].insert(s(1));
            pairs[ s(0) ].insert(s(2));
            pairs[ s(1) ].insert(s(2));
        }

        arma::mat desc_m;
        vector<string> featureNames;
        this->extractDescMatrix(this->media_cycle->getBrowser(), desc_m, featureNames);

        int edges = 0;

        //std::cout << "desc_m n "  << desc_m.n_rows << " cols d " << desc_m.n_cols << std::endl;

        // Compute high-dimensional distances for each edge
        arma::mat dt;
        for(std::map<int,std::set<int> >::iterator pair = pairs.begin(); pair != pairs.end(); pair++){
            int in = pair->first;
            std::set<int> outs = pair->second;

            dt.resize(dt.n_rows+outs.size(),3);

            for(std::set<int>::iterator out = outs.begin(); out != outs.end(); out++){
                float dist = sqrt( sum(arma::pow(desc_m.row(in) - desc_m.row(*out),2)) );
                dt(edges,0) = in;
                dt(edges,1) = *out;
                dt(edges,2) = dist;
                edges++;
            }
        }
        edges = 0;

        // Add visual links for each edge
        if(this->browser_renderer){
            media_cycle->getBrowser()->setLayout(AC_LAYOUT_TYPE_NODELINK);
            this->browser_renderer->removeLinks();

            for(std::map<int,std::set<int> >::iterator pair = pairs.begin(); pair != pairs.end(); pair++){
                int in = pair->first;
                std::set<int> outs = pair->second;
                for(std::set<int>::iterator out = outs.begin(); out != outs.end(); out++){

                    float dist = sqrt( sum(arma::pow(desc_m.row(in) - desc_m.row(*out),2)) );
                    //std::cout << "Edge " << edges++ << " " << in << " " << *out << " " << dist <<  std::endl;
                    this->browser_renderer->addLink( ids[in], ids[*out], 1.0f);

                }
            }
        }
        //media_cycle->setNeedsDisplay(true);

        // Morph the Delaunay grid
        arma::umat sortedDistances = sort_index(dt.col(2));
        float min_nd = min( dt.col(2));
        float max_nd = max( dt.col(2));

        std::string method = this->getStringParameterValue("Method");
        if(method =="Expand 2D" || method == "Fit 2D"){
            // Morph method 1: expand edges whose distance is smaller/different than the minimally required 2D distance, without high dimensional influence
            //for(int a=0; a< sortedDistances.n_rows; a++){
            for(int a=sortedDistances.n_rows-1; a>=0; a--){
                int in = dt( sortedDistances(a) , 0);
                int out = dt( sortedDistances(a) , 1);
                float dist2d = sqrt(  pow(pos(in,0)-pos(out,0),2) + pow(pos(in,1) - pos(out,1),2)  );
                float distnd = dt( sortedDistances(a) , 2);

                if( (method == "Expand 2D" && dist2d < mindist) || (method == "Fit 2D" && dist2d != mindist)){

                    float c_x = 0.5f*(pos(in,0) + pos(out,0));
                    float c_y = 0.5f*(pos(in,1) + pos(out,1));

                    pos(in,0) = c_x + 0.5f*(pos(in,0)-c_x)*mindist/dist2d;
                    pos(in,1) = c_y + 0.5f*(pos(in,1)-c_y)*mindist/dist2d;
                    pos(out,0) = c_x + 0.5f*(pos(out,0)-c_x)*mindist/dist2d;
                    pos(out,1) = c_y + 0.5f*(pos(out,1)-c_y)*mindist/dist2d;

                    // Multiply each by (1-distndn) to draw a diamond

                    // Clip positions
                    // (approximation, should be harmonized between coordinates)
                    /*if(pos(in,0) < -osg) pos(in,0) = -osg;
                    if(pos(in,0) > osg) pos(in,0) = osg;
                    if(pos(in,1) < -osg) pos(in,1) = -osg;
                    if(pos(in,1) > osg) pos(in,1) = osg;
                    if(pos(out,0) < -osg) pos(out,0) = -osg;
                    if(pos(out,0) > osg) pos(out,0) = osg;
                    if(pos(out,1) < -osg) pos(out,1) = -osg;
                    if(pos(out,1) > osg) pos(out,1) = osg;*/
                }

            }
        }

        std::cout << "Iteration " << iter++ << " edges " << edges << " triads " << triads.size() ;// << std::endl;

        idealdist = 0.5*mindist*sortedDistances.n_rows;
        float _totaldist = 0;
        for(int a=sortedDistances.n_rows-1; a>=0; a--){
            int in = dt( sortedDistances(a) , 0);
            int out = dt( sortedDistances(a) , 1);
            float dist2d = sqrt(  pow(pos(in,0)-pos(out,0),2) + pow(pos(in,1) - pos(out,1),2)  );
            _totaldist += dist2d;
        }

        if(_totaldist == totaldist)
            repeateddist = true;

        totaldist = _totaldist;

        std::cout << " total 2D dist " << totaldist <<  " vs total ideal grid dist " << idealdist << std::endl;
    }

    ACPoint p;
    for(int d=0; d<pos.n_rows;d++){
        p.x = pos(d,0);
        p.y = pos(d,1);
        p.z = 0;
        this->media_cycle->getBrowser()->setNodeNextPosition(d, p);

    }
    media_cycle->setNeedsDisplay(true);

}

void ACFilterPlugUnispring::extractDescMatrix(ACMediaBrowser* mediaBrowser, arma::mat& desc_m, vector<string> &featureNames){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;
    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfFeaturesVectors();
    std::vector<float> weight=mediaBrowser->getWeightVector();
    if (nbFeature!=weight.size())
        std::cerr<<"ACFilterPlugUnispring::extractDescMatrix weight vector size incompatibility"<<endl;
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
            tmpIdy++;
        }
    }
}
