/**
 * @brief Plugin for displaying Delaunay triangulation edges from the current node positions
 * @author Christian Frisson
 * @date 28/02/2014
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

#include<armadillo>
#include "ACFilterPlugDelaunayTriangulationLinking.h"

// Waiting for Delaunay triangulation in boost::polygon
#include <s_hull.h>

using namespace arma;

ACFilterPlugDelaunayTriangulationLinking::ACFilterPlugDelaunayTriangulationLinking() : QObject(), ACPluginQt(), ACFilteringPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle Delaunay Triangulation Edges";
    this->mDescription = "Plugin for displaying Delaunay triangulation edges from the current node positions";
    this->mId = "";

    this->addCallback("Clear","Clear links",boost::bind(&ACFilterPlugDelaunayTriangulationLinking::clearLinks,this));
}


ACFilterPlugDelaunayTriangulationLinking::~ACFilterPlugDelaunayTriangulationLinking() {
}

void ACFilterPlugDelaunayTriangulationLinking::clearLinks() {
    if(this->browser_renderer){
        this->browser_renderer->removeLinks();
    }
}

void ACFilterPlugDelaunayTriangulationLinking::filter() {
    preFilterPositions.clear();

    if(!media_cycle) return;
    if(media_cycle->getLibrarySize()==0) return;

    int libSize = media_cycle->getLibrarySize();

    std::vector<long> ids = media_cycle->getLibrary()->getAllMediaIds();

    //if(ids.size() != preFilterPositions.size()){ // TODO a better test against insertions/deletions
    preFilterPositions.clear();

    for (int i=0; i<ids.size(); i++){
        ACMediaNode* node = media_cycle->getMediaNode(ids[i]);
        if(node){
            preFilterPositions[ids[i]] = node->getCurrentPosition();
        }
    }
    //}

    arma::mat pos;
    pos.set_size(libSize,2);//(2,libSize);

    for (int i=0; i<ids.size(); i++){
        ACPoint p = preFilterPositions[ids[i]] ;
        pos(i,0) = p.x;
        pos(i,1) = p.y;
    }

    arma::umat id_x = sort_index(sort_index( pos.col(0)));
    arma::umat id_y = sort_index(sort_index( pos.col(1)));

    arma::umat pre_uniform;
    pre_uniform.set_size(libSize,2);
    pre_uniform = arma::join_rows(id_x,id_y);

    std::vector<Shx> pts, hull;
    Shx pt;

    for(int v=0; v<ids.size(); v++){
        pt.id = ids[v];
        pt.r = pre_uniform(v,0);
        pt.c = pre_uniform(v,1);
        pts.push_back(pt);
    }

    std::vector<Triad> triads;
    s_hull_del_ray2( pts, triads);

    std::map<int,std::set<int> > pairs;
    //arma::mat pairs;
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

    int edges = 0;

    if(this->browser_renderer){
        media_cycle->getBrowser()->setLayout(AC_LAYOUT_TYPE_NODELINK);
        this->browser_renderer->removeLinks();

        for(std::map<int,std::set<int> >::iterator pair = pairs.begin(); pair != pairs.end(); pair++){
            int in = pair->first;
            std::set<int> outs = pair->second;
            for(std::set<int>::iterator out = outs.begin(); out != outs.end(); out++){
                //std::cout << "Edge " << edges++ << " " << in << " " << *out << std::endl;
                this->browser_renderer->addLink( ids[in], ids[*out], 1.0f);

            }
        }
    }
    media_cycle->setNeedsDisplay(true);
}
