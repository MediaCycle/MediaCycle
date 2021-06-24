/**
 * @brief Plugin for gridding nodes in a proximity grid
 * @author Christian Frisson
 * @date 23/02/2014
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

//@inproceedings{
//  author = {March, W.B., Ram, P., and Gray, A.G.},
//  title = {{Fast Euclidean Minimum Spanning Tree: Algorithm, Analysis,
//     Applications.}},
//  booktitle = {Proceedings of the 16th ACM SIGKDD International Conference
//     on Knowledge Discovery and Data Mining}
//  series = {KDD 2010},
//  year = {2010}
//}

#include<armadillo>
#include "ACFilterPlugProximityGrid.h"
#include "emst.h"

#include "mlpack/core.hpp"
#include "mlpack/methods/emst/dtb.hpp"
#include "mlpack/methods/neighbor_search/neighbor_search.hpp"
#include <mlpack/core/metrics/lmetric.hpp>
#include <mlpack/core/tree/binary_space_tree.hpp>

#include <mlpack/core/util/version.hpp>

#if(MLPACK_VERSION_MAJOR >= 3)
#include "mlpack/methods/neighbor_search/ns_model.hpp"
#endif

#include<limits>

#include <iostream>
#include <sstream>

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

using namespace arma;
/*using namespace mlpack;
using namespace mlpack::emst;
using namespace mlpack::tree;
using namespace std;*/
using namespace mlpack::neighbor;
#if(MLPACK_VERSION_MAJOR >= 2 || __MLPACK_VERSION_MAJOR >= 2)
using namespace mlpack::tree;
using namespace mlpack::metric;
using namespace mlpack::util;
// Convenience typedef.
typedef NSModel<NearestNeighborSort> KNNModel;
#endif
using namespace mediacycle;

ACFilterPlugProximityGrid::ACFilterPlugProximityGrid() : ACFilteringPlugin(),
    min_x(1),min_y(1),max_x(-1),max_y(-1){
    this->mMediaType = MEDIA_TYPE_ALL;
    // this->mPluginType = PLUGIN_TYPE_NONE;
    this->mName = "Proximity Grid";
    this->mDescription = "Plugin for Positioning items on a grid";
    this->mId = "";

    this->addCallback("Update","Update view",boost::bind(&ACFilterPlugProximityGrid::setProximityGrid,this));
    this->addCallback("Cost","Eval neighborhoodness",boost::bind(&ACFilterPlugProximityGrid::evalNeighborhoodness,this));

    methods.push_back("Greedy Empty");
    methods.push_back("Greedy Swap");
    methods.push_back("Greedy Bump");
    methods.push_back("Horizontal");
    this->addStringParameter("Method",methods.front(),methods,"Grid allocation method"/*,boost::bind(&ACFilterPlugProximityGrid::setProximityGrid,this)*/);
    this->addNumberParameter("Grid side",0,0,FLT_MAX,1,"Grid side"); // don't use DBL_MAX, other classes parsing plugin parameters (such as qwt widgets) might work in float resolution
    distances.push_back("Features");
    distances.push_back("Coordinates");
    this->addStringParameter("Distance",distances.front(),distances,"Dimensions to compute minimum spanning tree");
    sortings.push_back("Closest Neighbor");
    sortings.push_back("Minimum Spanning Tree");
    this->addStringParameter("Sorting",sortings.back(),sortings,"Sorting");
    this->addNumberParameter("Compact",1,0,1,1,"Compact the grid by removing empty rows and colums. Useful for square views, less progressive for other views.");

#ifdef USE_DEBUG
    this->addNumberParameter("Save Benchmark",1,0,1,1,"Save benchmark automatically");
#endif
}

ACFilterPlugProximityGrid::~ACFilterPlugProximityGrid() {
}

void ACFilterPlugProximityGrid::evalNeighborhoodness() {

    if(!media_cycle) return;
    if(media_cycle->getLibrarySize()==0) return;

    int libSize = media_cycle->getLibrarySize();

    int currentSize = this->getNumberParameterValue("Grid side");

    int minSize = ceil(sqrt(libSize));

    ACMedias medias = media_cycle->getLibrary()->getAllMedia();
    std::vector<long> ids = media_cycle->getLibrary()->getAllMediaIds();
    int cpt=0;
    for (long i=0; i<ids.size(); i++){
        if (media_cycle->getMediaNode(ids[i]) && media_cycle->getMediaNode(ids[i])->isDisplayed()){
            cpt++;
        }
    }
    arma::mat pos;
    pos.set_size(2,cpt);
    int tmpIdy=0;
    for(int i=0; i<ids.size(); i++) {
        if (media_cycle->getMediaNode(ids[i]) && !(media_cycle->getMediaNode(ids[i])->isDisplayed()))
            continue;
        ACPoint p=media_cycle->getMediaNode(ids[i])->getNextPosition();
        pos(0,tmpIdy)=p.x;
        pos(1,tmpIdy)=p.y;
        //pos(tmpIdy,2)=p.z;
        //cout<<"ACArmaVisPlugin::catchCurrentPosition node "<<ids[i]<<" : "<<p.x<<" "<<p.y<<" "<<p.z<<endl;
        tmpIdy++;
    }


    int k = 8; // 2 horizontal, 2 vertical, 4 diagonal
    if(libSize <= k || libSize <= 1){
        std::cerr << "Library too small to evaluate neighborhoodness on " << k << " neighbors" << std::endl;
        return;
    }

    arma::mat desc_m;
    vector<string> featureNames;
    this->extractDescMatrix(this->media_cycle->getBrowser(), desc_m, featureNames);

    if(!desc_m.is_finite()){
        std::cerr << "ACFilterPlugProximityGrid::evalNeighborhoodness: features contain NaN values, aborting neighborhoodness evaluation" << std::endl;
        return;
    }

    desc_m = desc_m.t();//arma::inplace_trans(desc_m); // required but contradictory with the example provided by mlpack

    arma::Mat<size_t> resultingFeatureNeighbors,resultingPosNeighbors;
    arma::mat resultingFeatureDistances,resultingPosDistances;
    #if(MLPACK_VERSION_MAJOR >= 3)
    int leaf_size = 20;
    KNNModel featKNN,posKNN;
    featKNN.TreeType() = KNNModel::KD_TREE;
    featKNN.LeafSize() = size_t(leaf_size);
    featKNN.BuildModel(std::move(desc_m), size_t(leaf_size), DUAL_TREE_MODE,
        0.0);
    posKNN.TreeType() = KNNModel::KD_TREE;
    posKNN.LeafSize() = size_t(leaf_size);
    posKNN.BuildModel(std::move(pos), size_t(leaf_size), DUAL_TREE_MODE,
        0.0);
    #else
    AllkNN featKNN(desc_m),posKNN(pos);
    #endif
    ////mlpack::data::Save("./desc_m.csv", desc_m, true);
    featKNN.Search(1, resultingFeatureNeighbors, resultingFeatureDistances);
    posKNN.Search(k, resultingPosNeighbors, resultingPosDistances);

    //mlpack::data::Save("./resultingFeatureDistances.csv", resultingFeatureDistances, true);
    //mlpack::data::Save("./resultingPosNeighbors.csv", resultingPosNeighbors, true);
    //mlpack::data::Save("./resultingPosDistances.csv", resultingPosDistances, true);
    //mlpack::data::Save("./pos.csv", pos, true);

    double epsilon = std::numeric_limits < double >::epsilon ();

    double min_2nn_2d = arma::min(resultingPosDistances.row(0)); // the minimal 2d distance to a nearest neighbor in hd
    int num_min_2nn_2d = 0; // number of times the minimal 2d distance to a first hd nearest neighbor is repeated

    double cumdist = 0; // cumulated 2d distances to each first hd nearest neighbor

    int same_nn_2dvsnd = 0; // number of times the first hd nearest neighbor is the same as the first 2d nearest neighbor
    int same_nn_2dvsnd_plus = 0; // number of times the first hd nearest neighbor is the among the 4 2d nearest neighbor if they have the same distance (grid)

    for(int a=0; a< resultingFeatureNeighbors.n_cols; a++){
        int nn_nd = resultingFeatureNeighbors(0,a);
        int n = 0;

        double d2d = sqrt( pow( pos(0,nn_nd) - pos(0,a) , 2) + pow( pos(1,nn_nd) - pos(1,a) , 2) );

        cumdist += d2d;

        if(d2d <= min_2nn_2d+epsilon) num_min_2nn_2d++;

        int nn_2d = resultingPosNeighbors(n,a);
        //std::cout << "Edge " << a << " " << " id " << ids[a] << " nn_nd " << ids[nn_nd] << "/"<< nn_nd  << " nn_2d " << nn_2d << " d nd " << resultingFeatureDistances(n,a) << " d2d " << d2d << std::endl;

        if(nn_nd == nn_2d){
            same_nn_2dvsnd++;
            same_nn_2dvsnd_plus++;
        }
        else{
            n++;
            double _d2d = resultingPosDistances(n,a); //sqrt( pow( pos(0,nn_2d) - pos(0,a) , 2) + pow( pos(1,nn_2d) - pos(1,a) , 2) );
            while(n<=4 && n < resultingPosNeighbors.n_rows && nn_2d != nn_nd && abs(_d2d - d2d) < epsilon){
                nn_2d = resultingPosNeighbors(n,a);
                //std::cout << "Edge " << a << " " << " id " << ids[a] << " nn_nd " << ids[nn_nd] << "/"<< nn_nd  << " nn_2d " << nn_2d << " d2d " << d2d << " vs "  << _d2d << std::endl;
                if(nn_nd == nn_2d /*&& _d2d == d2d*/ )
                    same_nn_2dvsnd_plus++;
                n++;
                _d2d = sqrt( pow( pos(0,nn_2d) - pos(0,a) , 2) + pow( pos(1,nn_2d) - pos(1,a) , 2) );
            }

        }
    }

    double osg = 0.33f;
    double mincelldist = 2*osg/(double)(minSize-1);
    double currentcelldist = 2*osg/(double)(currentSize-1);

    double mincost = cumdist/(mincelldist*libSize); // average cell distance factor to reach nearest neighbors, considering the smallest grid
    double currentcost = cumdist/(currentcelldist*libSize); // average cell distance factor to reach nearest neighbors, considering the current resolution

    std::cout << " min_2nn_2d " << min_2nn_2d << std::endl;
    std::cout << " num_min_2nn_2d " << num_min_2nn_2d << std::endl;
    std::cout << " same_nn_2dvsnd " << same_nn_2dvsnd << std::endl;
    std::cout << " same_nn_2dvsnd_plus " << same_nn_2dvsnd_plus << std::endl;

    std::cout << " mincost " << mincost << std::endl;
    std::cout << " currentcost " << currentcost << std::endl;

#ifdef USE_DEBUG
    int save = this->getNumberParameterValue("Save Benchmark");
    if(save==1 && !media_cycle->isImporting()){
        std::ofstream file;
        std::string library_name = this->media_cycle->getLibrary()->getTitle();
        if(library_name == ""){
            library_name = "Test";
        }
        /*std::vector<std::string> visualisation_plugins = this->media_cycle->getActivePluginNames(PLUGIN_TYPE_CLUSTERS_POSITIONS,this->media_cycle->getMediaType());// ->getBrowser()->getClustersPositionsPlugin()

        std::string visualisation_plugin ("UnknownViz");
        if(visualisation_plugins.size() == 1)
            visualisation_plugin = visualisation_plugins.front();*/

        std::string filepath = "./" + library_name + "-Neighbors.csv";
        std::cout << "Trying to save to " << filepath << std::endl;

        int compact = this->getNumberParameterValue("Compact");

        file.open(filepath.c_str(), ios_base::out | ios_base::in | ios_base::app | std::ios_base::ate);
        if(file.is_open()){

            long pos =  file.tellp();

            if(pos == 0){
                file << "\"Library size\"" <<",";
                file << "\"Grid side\"" <<",";
                file << "\"Compact\"" <<",";
                file << "\"Min 2D dist HD 1nn\"" <<",";
                file << "\"Min 2D dist HD 1nn repeats\"" <<",";
                file << "\"Same 2D/HD 1nn\"" <<",";
                file << "\"Same 2D/HD 1nn/4\"" <<",";
                file << "\"Nearest cell dist factor for smallest grid\"" <<",";
                file << "\"Nearest cell dist factor for current grid\"" ;//<<","; // no comma
                file << std::endl;
            }

            file << libSize << ",";
            file << gridSize <<",";
            file << compact << ",";
            file << min_2nn_2d << ",";
            file << num_min_2nn_2d << ",";
            file << same_nn_2dvsnd << ",";
            file << same_nn_2dvsnd_plus << ",";
            file << mincost << ",";
            file << currentcost;//<< ","; // no comma
            file << std::endl;

            file.close();
        }
        else{
            std::cerr << "Couldn't open file " << filepath<< std::endl;
        }
    }
#endif
}

void ACFilterPlugProximityGrid::filter() {
    this->setProximityGrid();
}

void ACFilterPlugProximityGrid::librarySizeChanged(){

    preFilterPositions.clear();
    gridSize = 0;

    if(!media_cycle) return;
    if(media_cycle->getLibrarySize()==0) return;

    int libSize = media_cycle->getLibrarySize();

    /*int*/ gridSize = ceil(sqrt(libSize));

    this->updateNumberParameter("Grid side",libSize,gridSize,libSize,1);
}


void ACFilterPlugProximityGrid::setProximityGrid() {

    if(!media_cycle) return;
    if(media_cycle->getLibrarySize()==0) return;

    if(media_cycle->isImporting())
        return;

    int libSize = media_cycle->getLibrarySize();

    std::vector<long> ids = media_cycle->getLibrary()->getAllMediaIds();

    if(libSize != ids.size()){
        std::cout << "ACFilterPlugProximityGrid::updateNextPositions: warning, lib size and number of accessible media ids don't match"<< std::endl;
        return;
    }

    int minSize = ceil(sqrt(libSize));
    int currentSize = this->getNumberParameterValue("Grid side");
    gridSize = currentSize;

    if(gridSize <= 1)
        return;

    std::cout << "gridSize " << gridSize << " for " << libSize << " elements " << std::endl;

    if(ids.size() != preFilterPositions.size()){ // TODO a better test against insertions/deletions
        preFilterPositions.clear();

        for (int i=0; i<ids.size(); i++){
            ACMediaNode* node = media_cycle->getMediaNode(ids[i]);
            if(node){
                preFilterPositions[ids[i]] = node->getNextPosition();
            }
        }
    }

    std::string sorting = this->getStringParameterValue("Sorting");

    bool visual_mst = true;
    std::string distance = this->getStringParameterValue("Distance");
    if (distance == "Features")
        visual_mst = false;

    std::string method = this->getStringParameterValue("Method");

    arma::mat desc_m;
    arma::mat pos;

    vector<string> featureNames;
    if(!visual_mst)
        this->extractDescMatrix(this->media_cycle->getBrowser(), desc_m, featureNames);

    // Force visual neighbors if features contain NaN values
    if(!desc_m.is_finite()){
        std::cerr << "ACFilterPlugProximityGrid::setProximityGrid: features contain NaN values, computing minimum spanning tree from positions" << std::endl;
        visual_mst = true;
    }

    if(visual_mst)
        desc_m.set_size(libSize,2);//(2,libSize);
    pos.set_size(2,libSize);

    //double min_x(1),min_y(1),max_x(-1),max_y(-1);
    min_x = 1;
    min_y = 1;
    max_x = -1;
    max_y = -1;
    for (int i=0; i<ids.size(); i++){
        ACPoint p = preFilterPositions[ids[i]] ;
        if(min_x > p.x) min_x = p.x;
        if(min_y > p.y) min_y = p.y;
        if(max_x < p.x) max_x = p.x;
        if(max_y < p.y) max_y = p.y;

        if(visual_mst){
            desc_m(i,0) = p.x;
            desc_m(i,1) = p.y;
        }
        pos(0,i) = p.x;
        pos(1,i) = p.y;
    }

    double t_x(0.0f),t_y(0.0f),z_x(1.0f),z_y(1.0f);
    t_x = -(max_x+min_x)/2.0f;
    t_y = -(max_y+min_y)/2.0f;
    z_x = 2.0f/(max_x-min_x);
    z_y = 2.0f/(max_y-min_y);

    desc_m = desc_m.t();//arma::inplace_trans(desc_m); // required but contradictory with the example provided by mlpack

    arma::mat mst;
    arma::Mat<size_t> resultingFeatureNeighbors,resultingPosNeighbors;
    arma::mat resultingFeatureDistances,resultingPosDistances;
    int k = 2;
    arma::umat sortedDistances;

    // featKNN is required for stats at the end / or for the "Closest Neighbors" mode
    if(libSize <= k)
        return;

    // Our dataset matrices, which are column-major.
    #if(MLPACK_VERSION_MAJOR >= 3)
    int leaf_size = 20;
    KNNModel featKNN,posKNN;
    featKNN.TreeType() = KNNModel::KD_TREE;
    featKNN.LeafSize() = size_t(leaf_size);
    featKNN.BuildModel(std::move(desc_m), size_t(leaf_size), DUAL_TREE_MODE,
        0.0);
    posKNN.TreeType() = KNNModel::KD_TREE;
    posKNN.LeafSize() = size_t(leaf_size);
    posKNN.BuildModel(std::move(pos), size_t(leaf_size), DUAL_TREE_MODE,
        0.0);
    #else
    AllkNN featKNN(desc_m),posKNN(pos);
    #endif

    featKNN.Search(k, resultingFeatureNeighbors, resultingFeatureDistances);

    sortedDistances = sort_index(resultingFeatureDistances.row(0));
    std::cout << "sortedDistances rows " << sortedDistances.n_rows << " cols " << sortedDistances.n_cols << std::endl;
    //mlpack::data::Save("./sortedDistances.csv", sortedDistances, true);

    if(sorting == "Minimum Spanning Tree"){

        // Euclidean Minimum Spanning Tree from methods/emst/emst_main.cpp
        // using the dual-tree Boruvka algorithm.
        // The output is saved in a three-column matrix, where each row indicates an
        // edge.  The first column corresponds to the lesser index of the edge; the
        // second column corresponds to the greater index of the edge; and the third
        // column corresponds to the distance between the two points.
        // can be using O(n^2) naive algorithm
        // leaf_size: leaf size in the kd-tree.  One-element leaves give the "
        // empirically best performance, but at the cost of greater memory requirements. default 1);
        bool naive = false;
        const size_t leafSize = 1;
        mst = emst(desc_m,naive,leafSize);
        //mlpack::data::Save("./mst.csv", mst, true);
    }
    else if(sorting == "Closest Neighbor"){


        //mlpack::data::Save("./desc_m.csv", desc_m, true);

        posKNN.Search(k, resultingPosNeighbors, resultingPosDistances);
        //mlpack::data::Save("./resultingFeatureNeighbors.csv", resultingFeatureNeighbors, true);
        //mlpack::data::Save("./resultingFeatureDistances.csv", resultingFeatureDistances, true);
        //mlpack::data::Save("./resultingPosNeighbors.csv", resultingPosNeighbors, true);
        //mlpack::data::Save("./resultingPosDistances.csv", resultingPosDistances, true);

        //mst = arma::join_rows(resultingFeatureNeighbors,resultingFeatureDistances);
        //mst = resultingFeatureNeighbors(sort_index(resultingFeatureDistances));

        std::cout << "resultingFeatureNeighbors rows " << resultingFeatureNeighbors.n_rows << " cols " << resultingFeatureNeighbors.n_cols << std::endl;
        std::cout << "resultingFeatureDistances rows " << resultingFeatureDistances.n_rows << " cols " << resultingFeatureDistances.n_cols << std::endl;
        std::cout << "resultingPosNeighbors rows " << resultingPosNeighbors.n_rows << " cols " << resultingPosNeighbors.n_cols << std::endl;
        std::cout << "resultingPosDistances rows " << resultingPosDistances.n_rows << " cols " << resultingPosDistances.n_cols << std::endl;

        //        for(int a=0; a< resultingFeatureNeighbors.n_cols; a++)
        //            std::cout << "Edge " << a << " " << ids[a] << " " << ids[resultingFeatureNeighbors(0,a)] << " d nd " << resultingFeatureDistances(0,a) << std::endl;
        //        for(int a=0; a< resultingPosNeighbors.n_cols; a++)
        //            std::cout << "Edge " << a << " " << ids[a] << " " << ids[resultingPosNeighbors(0,a)] << " d 2d " << resultingPosDistances(0,a) << std::endl;

        double min_nn_2d = min(resultingPosDistances.row(0));
        double max_nn_2d = max(resultingPosDistances.row(0));
        double min_nn_nd = min(resultingFeatureDistances.row(0));
        double max_nn_nd = max(resultingFeatureDistances.row(0));

        std::cout << " min_nn_2d " << min_nn_2d << " max_nn_2d " << max_nn_2d << std::endl;
        std::cout << " min_nn_nd " << min_nn_nd << " max_nn_nd " << max_nn_nd << std::endl;

        arma::mat count_0, count_1, count_2;
        count_0 = zeros(ids.size(),1);
        count_1 = zeros(ids.size(),1);
        count_2 = zeros(ids.size(),1);
        for(int a=0; a< sortedDistances.n_cols; a++){
            count_0( ids[sortedDistances(a)] ) += 1;
            count_1( ids[resultingFeatureNeighbors(0,sortedDistances(a))] ) += 1;
            if(k>1)
                count_2( ids[resultingFeatureNeighbors(1,sortedDistances(a))] ) += 1;
        }

        for(int a=0; a< sortedDistances.n_cols; a++){
            int n_0 = ids[sortedDistances(a)];
            int n_1 = ids[resultingFeatureNeighbors(0,sortedDistances(a))];
            //std::cout << "Edge " << a << " " << ids[sortedDistances(a)] << "<->" << ids[resultingFeatureNeighbors(0,sortedDistances(a))];
            double dnd = resultingFeatureDistances(0,sortedDistances(a));
            //std::cout << " dnd " << dnd;
            double d2d = sqrt( pow( pos(0,resultingFeatureNeighbors(0,sortedDistances(a))) - pos(0,sortedDistances(a)) , 2) + pow( pos(1,resultingFeatureNeighbors(0,sortedDistances(a))) - pos(1,sortedDistances(a)) , 2) );
            //std::cout << " d2d " << d2d;

            // We first adjust the distance between the pair along their high dimensional distance
            /*double c_x = 0.5f*(pos(0,n_0) + pos(0,n_1));
            double c_y = 0.5f*(pos(1,n_0) + pos(1,n_1));
            double nd = dnd/min_nn_nd * min_nn_2d;

            pos(0,n_0) = c_x + 0.5f*(pos(0,n_0)-c_x)*nd/d2d;
            pos(0,n_1) = c_x + 0.5f*(pos(0,n_1)-c_x)*nd/d2d;
            pos(1,n_0) = c_y + 0.5f*(pos(1,n_0)-c_y)*nd/d2d;
            pos(1,n_1) = c_y + 0.5f*(pos(1,n_1)-c_y)*nd/d2d;*/


            if(k>1){
                int n_2 = ids[resultingFeatureNeighbors(1,sortedDistances(a))];
                std::cout << ids[sortedDistances(a)] << "<->" << ids[resultingFeatureNeighbors(1,sortedDistances(a))];
                double dnd2 = resultingFeatureDistances(1,sortedDistances(a));
                //dnd2 = (dnd2-min_nn_nd)/(max_nn_nd-min_nn_nd);
                std::cout << " dnd " << dnd2;
                double d2d2 = sqrt( pow( pos(0,resultingFeatureNeighbors(1,sortedDistances(a))) - pos(0,sortedDistances(a)) , 2) + pow( pos(1,resultingFeatureNeighbors(1,sortedDistances(a))) - pos(1,sortedDistances(a)) , 2) );
                //d2d2 = (d2d2-min_nn_2d)/(max_nn_2d-min_nn_2d);
                std::cout << " d2d2 " << d2d2;

                int cn_0 = count_0( ids[sortedDistances(a)]);
                int cn_1 = count_1( ids[sortedDistances(a)]);
                int cn_2 = count_2( ids[sortedDistances(a)]);

                //                if(this->browser){
                //                    this->browser->changeNodeSize( n_0, 0.05f+0.1*(cn_0+cn_1)+0.05f*cn_2);
                //                }

                if(a>0){

                    int p_0 = ids[sortedDistances(a-1)];
                    int p_1 = ids[resultingFeatureNeighbors(0,sortedDistances(a-1))];
                    int p_2 = ids[resultingFeatureNeighbors(1,sortedDistances(a-1))];

                    int cp_0 = count_0( ids[sortedDistances(a-1)]);
                    int cp_1 = count_1( ids[sortedDistances(a-1)]);
                    int cp_2 = count_2( ids[sortedDistances(a-1)]);

                    // If the previous edge is the same as the current (with permuted in/out nodes)
                    if( (p_0 == n_1) && (p_1 == n_0) && (n_2 == p_2) && (cn_0 == 1)  && (cn_1 == 1) && (cp_0 == 1) && (cp_1 == 1) ){

                        // We first adjust the distance between the pair along their high dimensional distance
                        double c_x = 0.5f*(pos(0,n_0) + pos(0,n_1));
                        double c_y = 0.5f*(pos(1,n_0) + pos(1,n_1));
                        double nd = dnd/min_nn_nd * min_nn_2d;

                        pos(0,n_0) = c_x + 0.5f*(pos(0,n_0)-c_x)*nd/d2d;
                        pos(0,n_1) = c_x + 0.5f*(pos(0,n_1)-c_x)*nd/d2d;
                        pos(1,n_0) = c_y + 0.5f*(pos(1,n_0)-c_y)*nd/d2d;
                        pos(1,n_1) = c_y + 0.5f*(pos(1,n_1)-c_y)*nd/d2d;

                        // if the pair forms a single triplet with their common next neighbor
                        if( ((cn_2 == 0) || (cp_2 == 0)) ){

                            // We then move the pair closer to the common next neighbor along their high dimensional distance
                            double nd2 = dnd2/min_nn_nd * min_nn_2d;
                            pos(0,n_0) = pos(0,n_2) + (pos(0,n_0)-pos(0,n_2))*nd2/d2d2;
                            pos(1,n_0) = pos(1,n_2) + (pos(1,n_0)-pos(1,n_2))*nd2/d2d2;

                            double dnd2_2 = resultingFeatureDistances(1,sortedDistances(a-1));
                            double nd2_2 = dnd2_2/min_nn_nd * min_nn_2d;
                            double d2d2_2 = sqrt( pow( pos(0,resultingFeatureNeighbors(1,sortedDistances(a-1))) - pos(0,sortedDistances(a-1)) , 2) + pow( pos(1,resultingFeatureNeighbors(1,sortedDistances(a-1))) - pos(1,sortedDistances(a-1)) , 2) );
                            pos(0,n_1) = pos(0,n_2) + (pos(0,n_1)-pos(0,n_2))*nd2_2/d2d2_2;
                            pos(1,n_1) = pos(1,n_2) + (pos(1,n_1)-pos(1,n_2))*nd2_2/d2d2_2;
                        }
                    }
                }
            }
            std::cout << std::endl;
        }

        std::cout << " min_nn_2d " << min_nn_2d << " max_nn_2d " << max_nn_2d << std::endl;
        std::cout << " min_nn_nd " << min_nn_nd << " max_nn_nd " << max_nn_nd << std::endl;
        //arma::umat si = sort_index(resultingFeatureDistances);
        //arma::mat sortedNeighbors = resultingFeatureNeighbors(si);
    }

    //mlpack::data::Save("./pos.csv", desc_m, true);
    //mlpack::data::Save("./mst.csv", mst, true);

    std::cout << "ACFilterPlugProximityGrid desc rows " << desc_m.n_rows << " cols " << desc_m.n_cols << std::endl;
    std::cout << "ACFilterPlugProximityGrid mst rows " << mst.n_rows << " cols " << mst.n_cols << std::endl;

    cell_ids.clear();
    //cell_ids = -1*arma::ones(gridSize,gridSize);
    cell_ids.ones(gridSize,gridSize);
    cell_ids *= -1;
    //std::map<long,bool> id_celled;

    id_celled.clear();
    for(std::vector<long>::iterator id=ids.begin(); id!=ids.end();id++){
        id_celled[*id] = false;
    }

    int _size = 0;
    if(sorting == "Minimum Spanning Tree"){
        _size = mst.n_cols;
        if(method == "Horizontal"){
            method = "Greedy Empty";
        }
    }
    else if(sorting == "Closest Neighbor"){
        _size = sortedDistances.n_cols;
    }

    // Considering closest MST edges first for empty strategy, farthest otherwise
    int increment = 1;
    int init = 0;
    if(method == "Greedy Swap" || method == "Greedy Bump"){
        increment = -1;
        init = _size -1;
    }
    /*else if(method != "Greedy Empty"){
        return;
    }*/

    for(int e = init; e >= 0 && e < _size;e+=increment){

        std::vector<int> id;
        double dist = 0;

        if(sorting == "Minimum Spanning Tree"){
            id.push_back( mst(0,e) );
            id.push_back( mst(1,e) );
            dist = mst(2,e);
        }
        else if(sorting == "Closest Neighbor"){
            id.push_back( sortedDistances(e) );
            id.push_back( resultingFeatureNeighbors(0,sortedDistances(e)) );
            dist = resultingFeatureDistances(0,sortedDistances(e));
        }

        //std::cout << "Edge " << e << " " << id[0] << " " << id[1] << " d " << dist << std::endl;

        for(int m = 0;m<2;m++){
            int _id = id[m];

            std::map<long,bool>::iterator is_celled = id_celled.find( id[m] );
            if( is_celled != id_celled.end() && is_celled->second == false){
                std::map<long,ACPoint>::iterator preFilterPosition = preFilterPositions.find( id[m] );
                if(preFilterPosition != preFilterPositions.end()){
                    ACPoint p = preFilterPosition->second;

                    if(sorting == "Closest Neighbor"){

                        p.x = pos(0,id[m]);
                        p.y = pos(1,id[m]);

                        if(method == "Horizontal"){

                            if(!id_celled[_id]){

                                int cell_id = -2;

                                double f_x;
                                double f_y;
                                int i_x;
                                int i_y;

                                if(m==1){
                                    ACPoint q;
                                    q.x = pos(0,id[0]);
                                    q.y = pos(1,id[0]);

                                    f_x = (q.x - min_x)/(max_x-min_x)*(double)(gridSize-1);
                                    f_y = (q.y - min_y)/(max_y-min_y)*(double)(gridSize-1);
                                    i_x = round(f_x);
                                    i_y = round(f_y);

                                    int _x = i_x;
                                    int _y = i_y;

                                    cell_id = -2;
                                    int segcell = 0;//index of current segment cell

                                    if(_x >= 0 && _x < gridSize && _y >= 0 && _y < gridSize)
                                        cell_id = cell_ids(_x,_y);

                                    int inc = p.x > q.x ? 1 : -1;

                                    if(cell_id != -1){
                                        _x = i_x + inc;
                                        if(_x >= 0 && _x < gridSize && _y >= 0 && _y < gridSize)
                                            cell_id = cell_ids(_x,_y);
                                    }
                                    if(cell_id != -1){
                                        _x = i_x + -1*inc;
                                        if(_x >= 0 && _x < gridSize && _y >= 0 && _y < gridSize)
                                            cell_id = cell_ids(_x,_y);
                                    }

                                    //if(cell_id != -1)
                                    //    spiralSearch(_id, q, "Greedy Empty");

                                }

                                // Determine ideal cell
                                f_x = (p.x - min_x)/(max_x-min_x)*(double)(gridSize-1);
                                f_y = (p.y - min_y)/(max_y-min_y)*(double)(gridSize-1);
                                i_x = round(f_x);
                                i_y = round(f_y);

                                int _x = i_x;
                                int _y = i_y;


                                int segcell = 0;//index of current segment cell

                                if(cell_id == -2)
                                    if(_x >= 0 && _x < gridSize && _y >= 0 && _y < gridSize)
                                        cell_id = cell_ids(_x,_y);

                                int inc = f_x > i_x ? 1 : -1;

                                if(cell_id != -1){
                                    _x = i_x + inc;
                                    if(_x >= 0 && _x < gridSize && _y >= 0 && _y < gridSize)
                                        cell_id = cell_ids(_x,_y);
                                }
                                if(cell_id != -1){
                                    _x = i_x + -1*inc;
                                    if(_x >= 0 && _x < gridSize && _y >= 0 && _y < gridSize)
                                        cell_id = cell_ids(_x,_y);
                                }

                                if(cell_id == -1){
                                    if(_x >= 0 && _x < gridSize && _y >= 0 && _y < gridSize){
                                        cell_ids(_x,_y) = _id;
                                        id_celled[_id] = true;
                                    }

                                }
                                else
                                    spiralSearch(_id, p, "Greedy Empty");
                            }
                        }
                        else
                            spiralSearch(_id, p, method);
                    }
                    else{
                        spiralSearch(_id, p, method);
                    }
                }
                else{
                    std::cerr << "Cell outside dimensions" << std::endl;
                }

            }
        }
    }

    // Compact the grid by removing empty rows and colums. Useful for square views, less progressive for other views.
    int compact = this->getNumberParameterValue("Compact");
    if(compact==1){
        // Remove empty columns and rows
        int r_rows(0),r_cols(0);

        if(cell_ids.n_rows>1){
            for(int i=0;i<cell_ids.n_rows;i++){
                double r_max = arma::max(cell_ids.row(i));
                double r_min = arma::min(cell_ids.row(i));
                if(r_max == -1 && r_min == -1){
                    cell_ids.shed_row(i);
                    i--; // revisit the current row replaced by the next
                    r_rows++;
                }
            }
        }

        if(cell_ids.n_rows>1){
            for(int j=0;j<cell_ids.n_cols;j++){
                double c_max = arma::max(cell_ids.col(j));
                double c_min = arma::min(cell_ids.col(j));
                if(c_max == -1 && c_min == -1){
                    cell_ids.shed_col(j);
                    j--; // revisit the current row replaced by the next
                    r_cols++;
                }
            }
        }

        if(r_rows != 0 || r_cols != 0)
            std::cout << "Shed " << r_rows << " row(s) and " << r_cols << " col(s)" << std::endl;

        double resolution = (double)currentSize/(double)(ceil(sqrt(libSize-1)));
        double compact_resolution = (double)(ceil(sqrt(cell_ids.n_cols*cell_ids.n_rows)))/(double)(ceil(sqrt(libSize-1)));

        std::cout << "Resolution " << resolution << " vs compact resolution " << compact_resolution << std::endl;
    }

    ACPoint p;
    double osg = 0.33f;

    arma::mat id_cell;
    id_cell.set_size(ids.size(),2);

    for(int g=0;g<cell_ids.n_rows;g++){
        for(int h=0;h<cell_ids.n_cols;h++){
            long id = cell_ids(g,h);
            if(id>-1){
                id_cell(id,0)=g;
                id_cell(id,1)=h;
                p.x = -osg + 2*osg*(double)g/(double)(cell_ids.n_rows-1);
                p.y = -osg + 2*osg*(double)h/(double)(cell_ids.n_cols-1);
                p.z = 0;
                this->media_cycle->getBrowser()->setNodeNextPosition(id, p);

            }

        }
    }

    int v_n = 0; //vertical neighbors
    int h_n = 0; //horizontal neighbors
    int dv_n = 0; //direct vertical neighbors (next cell)
    int dh_n = 0; //direct horizontal neighbors (next cell)
    int dd_n = 0;//direct diagonal neighbors (one of the next diag cell)

    for(int a=0; a< sortedDistances.n_cols; a++){
        int n_0 = ids[sortedDistances(a)];
        int n_1 = ids[resultingFeatureNeighbors(0,sortedDistances(a))];

        if(abs(id_cell(n_0,0) - id_cell(n_1,0)) == 0 ){ // 0
            v_n++;
            if(abs(id_cell(n_0,1) - id_cell(n_1,1)) == 1) // 1
                dv_n++;
        }
        if(abs(id_cell(n_0,1) - id_cell(n_1,1)) == 0 ){
            h_n++;
            if(abs(id_cell(n_0,0) - id_cell(n_1,0)) == 1)
                dh_n++;
        }

        if(abs(id_cell(n_0,0) - id_cell(n_1,0)) == 1 && abs(id_cell(n_0,1) - id_cell(n_1,1)) == 1)
            dd_n++;
    }

    std::cout << "Horizontal neighbors " << h_n << std::endl;
    std::cout << "Vertical neighbors " << v_n << std::endl;
    std::cout << "Direct horizontal neighbors " << dh_n << std::endl;
    std::cout << "Direct vertical neighbors " << dv_n << std::endl;
    std::cout << "Direct plus neighbors " << dh_n+dv_n << std::endl;
    std::cout << "Direct diagonal neighbors " << dd_n << std::endl;
    std::cout << "Direct star neighbors " << dh_n+dv_n+dd_n << std::endl;

#ifdef USE_DEBUG
    int save = this->getNumberParameterValue("Save Benchmark");
    if(save==1 && !media_cycle->isImporting()){
        std::ofstream file;
        std::string library_name = this->media_cycle->getLibrary()->getTitle();
        if(library_name == ""){
            library_name = "Test";
        }
        /*std::vector<std::string> visualisation_plugins = this->media_cycle->getActivePluginNames(PLUGIN_TYPE_CLUSTERS_POSITIONS,this->media_cycle->getMediaType());// ->getBrowser()->getClustersPositionsPlugin()

        std::string visualisation_plugin ("UnknownViz");
        if(visualisation_plugins.size() == 1)
            visualisation_plugin = visualisation_plugins.front();*/

        std::string filepath = "./" + library_name + "-Grid.csv";
        std::cout << "Trying to save to " << filepath << std::endl;

        file.open(filepath.c_str(), ios_base::out | ios_base::in | ios_base::app | std::ios_base::ate);
        if(file.is_open()){
            /*file << "\"Library size\",\"" << libSize << "\"" << std::endl;
            file << "\"Grid side\",\"" << gridSize << "\"" << std::endl;
            file << "\"Compact\",\"" << compact << "\"" << std::endl;
            file << "\"Horizontal neighbors\",\"" << h_n << "\"" << std::endl;
            file << "\"Vertical neighbors\",\"" << v_n << "\"" << std::endl;
            file << "\"Direct horizontal neighbors\",\"" << dh_n << "\"" << std::endl;
            file << "\"Direct vertical neighbors\",\"" << dv_n << "\"" << std::endl;
            file << "\"Direct plus neighbors\",\"" << dh_n+dv_n << "\"" << std::endl;
            file << "\"Direct diagonal neighbors\",\"" << dd_n << "\"" << std::endl;
            file << "\"Direct star neighbors\",\"" << dh_n+dv_n+dd_n << "\"" << std::endl;*/

            long pos =  file.tellp();

            if(pos == 0){

                file << "\"Library size\"" <<",";
                file << "\"Grid side\"" <<",";
                file << "\"Compact\"" <<",";
                file << "\"Horizontal neighbors\"" <<",";
                file << "\"Vertical neighbors\"" <<",";
                file << "\"Direct horizontal neighbors\"" <<",";
                file << "\"Direct vertical neighbors\"" <<",";
                file << "\"Direct plus neighbors\"" <<",";
                file << "\"Direct diagonal neighbors\"" <<",";
                file << "\"Direct star neighbors\"" ;//<<","; // no comma
                file << std::endl;
            }

            file << libSize << ",";
            file << gridSize << ",";
            file << compact << ",";
            file << h_n << ",";
            file << v_n << ",";
            file << dh_n << ",";
            file << dv_n << ",";
            file << dh_n+dv_n << ",";
            file << dd_n << ",";
            file << dh_n+dv_n+dd_n ;//<< ","; // no comma
            file << std::endl;

            file.close();
        }
        else{
            std::cerr << "Couldn't open file " << filepath<< std::endl;
        }



        this->evalNeighborhoodness();

    }
#endif
}


void ACFilterPlugProximityGrid::spiralSearch(int id, ACPoint p, std::string method){
    // Determine ideal cell
    double f_x = (p.x - min_x)/(max_x-min_x)*(double)(gridSize-1);
    double f_y = (p.y - min_y)/(max_y-min_y)*(double)(gridSize-1);
    int i_x = round(f_x);
    int i_y = round(f_y);

    if(i_x <0) i_x = 0;
    if(i_x >= gridSize) i_x = gridSize-1;
    if(i_y <0) i_y = 0;
    if(i_y >= gridSize) i_y = gridSize-1;
    /*if(i_x <0 && i_x >= gridSize && i_y <0 && i_y >= gridSize){
        std::cerr << "Error choosing ideal cell for id  " << id << std::endl;
    }*/

    // Check if cell is free by spiral search
    int _x_i = 0; // horizontal increment
    int _y_i = 1; // vertical increment
    bool clockwise = true;

    int _x = i_x;
    int _y = i_y;
    int segcell = 0;//index of current segment cell
    int segcells = 1;//number of cells the current spiral segment measures
    bool seginc = false;//increase segment size
    int cell_id = -2;
    int counter = 0;

    // While there is not free cell and while the number of cells tried are lower than the gridSize
    while( /*_x >=0 && _x < gridSize && _y >=0 && _y < gridSize*/ segcells < gridSize && cell_id != -1 ){

        if(_x >=0 && _x < gridSize && _y >=0 && _y < gridSize)
            cell_id = cell_ids(_x,_y);
        /*else
            std::cerr << "id " << id << " cell outside bounds" << std::endl;*/

        // Example of clockwise spiral starting up from the origin
        // 9...
        // 8 1 2
        // 7 0 3
        // 6 5 4
        //
        // Corresponding increments
        //x 0 1  0  0 -1 -1 0 0 0 1 1 1  0  0  0  0 -1 -1 -1
        //y 1 0 -1 -1  0  0 1 1 1 0 0 0 -1 -1 -1 -1  0  0  0

        if(segcell == segcells - 1){
            segcell = 0;
            if(seginc){
                segcells++;
            }
            seginc = ! seginc;

            if(_x_i == 0 && _y_i == 1) {_x_i = 1; _y_i = 0;}
            else if(_x_i == 1 && _y_i == 0) {_x_i = 0; _y_i = -1;}
            else if(_x_i == 0 && _y_i == -1) {_x_i = -1; _y_i = 0;}
            else if(_x_i == -1 && _y_i == 0) {_x_i = 0; _y_i = 1;}
        }
        else{
            segcell++;
        }

        //std::cout << "id " << id << " x " << _x_i << " y " << _y_i << std::endl;
        _x += _x_i;
        _y += _y_i;
    }
    _x -= _x_i;
    _y -= _y_i;

    if(_x >=0 && _x < gridSize && _y >=0 && _y < gridSize && cell_id == -1){

        id_celled[id] = true;

        if(method == "Greedy Empty"){
            // Assign the id to the free cell
            cell_ids(_x,_y) = id;
        }
        else if(method == "Greedy Swap"){
            // Assign the id using the ideal cell to the free cell
            cell_ids(_x,_y) = cell_ids(i_x,i_y);
            // Assign the id to the ideal cell
            cell_ids(i_x,i_y) = id;
        }
        else if(method == "Greedy Bump"){
            // Check the horizontal and vertical cell increment requirements
            int _x_d = i_x - _x;
            int _y_d = i_y - _y;

            if(_x_d == 0 && _y_d == 0){
                cell_ids(_x,_y) = id;
            }
            else{
                // Check the direction of each increment
                int _x_s = _x_d > 0 ? 1 : -1;
                int _y_s = _y_d > 0 ? 1 : -1;
                int max_inc = max(abs(_x_d),abs(_y_d));

                // Assign free cell coordinates to the current cell coordinates
                int c_x = _x;
                int c_y = _y;

                for(int c=0;c<max_inc;c++){

                    // Assign next cell coordinates to the current cell coordinates
                    int n_x = c_x;
                    int n_y = c_y;

                    // Check if coordinates should be incremented
                    if( (int)(n_x+(double)c*(double)_x_d/(double)max_inc) != (int)(n_x+(double)(c+_x_s)*(double)_x_d/(double)max_inc))
                        n_x += _x_s;
                    if( (int)(n_y+(double)c*(double)_y_d/(double)max_inc) != (int)(n_y+(double)(c+_y_s)*(double)_y_d/(double)max_inc))
                        n_y += _y_s;

                    // Assign the next cell id to the current cell
                    cell_ids(c_x,c_y) = cell_ids(n_x,n_y);

                    // Store the next cell coordinates as current cell coordinates
                    c_x = n_x;
                    c_y = n_y;

                }
                if(c_x != i_x || c_y != i_y)
                    std::cerr << "Didn't bump onto all cells in the way" << std::endl;
                cell_ids(i_x,i_y) = id;
            }

        }
    }
    else{
        std::cerr << "Error choosing cell for id  " << id << std::endl;
    }
}

void ACFilterPlugProximityGrid::extractDescMatrix(ACMediaBrowser* mediaBrowser, arma::mat& desc_m, vector<string> &featureNames){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    int nbMedia = medias.size();
    int featDim;
    int totalDim = 0;
    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfFeaturesVectors();
    std::vector<float> weight=mediaBrowser->getWeightVector();
    if (nbFeature!=weight.size())
        std::cerr<<"ACFilterPlugProximityGrid::extractDescMatrix weight vector size incompatibility"<<endl;
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
