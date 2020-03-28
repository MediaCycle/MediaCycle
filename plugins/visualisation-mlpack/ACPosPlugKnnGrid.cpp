/**
 * @brief Plugin for binning k nearest neighbors from a node in a grid 
 * @author Christian Frisson
 * @date 09/09/2015
 * @copyright (c) 2015 – UMONS - Numediart
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
#include "ACPosPlugKnnGrid.h"

#include "mlpack/core.hpp"

#include "mlpack/methods/neighbor_search/neighbor_search.hpp"

//#include "mlpack/core/tree/binary_space_tree.hpp"
#include "mlpack/core/tree/cover_tree.hpp"

#include "mlpack/methods/rann/ra_search.hpp"
#include <mlpack/methods/neighbor_search/unmap.hpp>

#include <mlpack/core/metrics/lmetric.hpp>
#include "mlpack/methods/lsh/lsh_search.hpp"

#include <mlpack/core/util/version.hpp>

#if(MLPACK_VERSION_MAJOR >= 3)
#include "mlpack/methods/neighbor_search/ns_model.hpp"
#endif

#include <boost/chrono/thread_clock.hpp>

using namespace std;
//using namespace arma;
using namespace mlpack;
using namespace mlpack::neighbor;
using namespace mlpack::tree;
using namespace mlpack::metric;
#if(MLPACK_VERSION_MAJOR >= 2 || __MLPACK_VERSION_MAJOR >= 2)
using namespace mlpack::util;
// Convenience typedef.
typedef NSModel<NearestNeighborSort> KNNModel;
#endif

using namespace boost::chrono;

ACPosPlugKnnGrid::ACPosPlugKnnGrid() : ACClusterPositionsPlugin() {
    this->mMediaType = MEDIA_TYPE_ALL;
    this->mName = "MediaCycle KNN Grid";
    this->mDescription = "Plugin for positioning k nearest neighbors on a grid";
    this->mId = "";
    
    this->addNumberParameter("Landmower mode",0,0,1,1,"Landmower mode",boost::bind(&ACPosPlugKnnGrid::updateGrid,this));
    //this->addCallback("Update","Update view",boost::bind(&ACPosPlugKnnGrid::setProximityGrid,this));
}

ACPosPlugKnnGrid::~ACPosPlugKnnGrid() {
}

void ACPosPlugKnnGrid::updateGrid()
{
    if(media_cycle)
        this->updateNextPositions(media_cycle->getBrowser());
}


void ACPosPlugKnnGrid::updateNextPositions(ACMediaBrowser* mediaBrowser){
    
    thread_clock::time_point start = thread_clock::now();
    
    std::cout << "ACPosPlugKnnGrid::updateNextPositions"<< std::endl;
    
    if(!media_cycle) return;
    if(media_cycle->getLibrarySize()==0) return;
    
    int libSize = media_cycle->getLibrarySize();
    
    std::vector<long> ids = media_cycle->getLibrary()->getAllMediaIds();
    
    if(libSize != ids.size())
        std::cout << "ACPosPlugKnnGrid::updateNextPositions: warning, lib size and number of accessible media ids don't match"<< std::endl;
    
    /*int*/ gridSize = ceil(sqrt(libSize));
    
    if(gridSize == 0)
        return;
    
    std::cout << "gridSize " << gridSize << " for " << libSize << " elements " << std::endl;
    
    thread_clock::time_point desc_start = thread_clock::now();  
    arma::mat desc_m; // rows: media items, cols: feature dimensions
    vector<string> featureNames;
    this->extractDescMatrix(this->media_cycle->getBrowser(), desc_m, featureNames);
    /*desc_m = desc_m.t();*/arma::inplace_trans(desc_m); // required but contradictory with the example provided by mlpack
    
    thread_clock::time_point desc_stop = thread_clock::now();  
    
    int k = libSize;//(libSize>20?20:libSize);
    int n = 0;
    
    arma::mat query = desc_m.col(n);
    mlpack::data::Save("./query.csv", query, true);
    mlpack::data::Save("./desc_m.csv", desc_m, true);
    
    arma::Mat<size_t> resultingFeatureNeighbors;
    arma::mat resultingFeatureDistances;
    
    bool using_query = true;
    thread_clock::time_point knn_start = thread_clock::now();
    #if(MLPACK_VERSION_MAJOR >= 3)
    int leaf_size = 20;
    KNNModel featKNN;
    featKNN.TreeType() = KNNModel::KD_TREE;
    featKNN.LeafSize() = size_t(leaf_size);
    featKNN.BuildModel(std::move(desc_m), size_t(leaf_size), DUAL_TREE_MODE,
        0.0);
    featKNN.Search(std::move(query), k, resultingFeatureNeighbors, resultingFeatureDistances);
    #else
    AllkNN featKNN(desc_m,query);
    featKNN.Search(k, resultingFeatureNeighbors, resultingFeatureDistances);
    #endif
    thread_clock::time_point knn_stop = thread_clock::now();
    
    std::cout << " resultingFeatureNeighbors rows " << resultingFeatureNeighbors.n_rows << " cols " << resultingFeatureNeighbors.n_cols << std::endl;
    
    mlpack::data::Save("./resultingFeatureNeighbors.csv", resultingFeatureNeighbors, true);
    mlpack::data::Save("./resultingFeatureDistances.csv", resultingFeatureDistances, true);
    
    /*arma::Mat<size_t> resultingFeatureNeighbors20k;
    arma::mat resultingFeatureDistances20k;
    thread_clock::time_point twentynn_start = thread_clock::now();
    AllkNN feat20NN(desc_m,query);
    feat20NN.Search((libSize>20?20:libSize), resultingFeatureNeighbors20k, resultingFeatureDistances20k);
    thread_clock::time_point twentynn_stop = thread_clock::now();
    */ 
    
    ACPoint p;
    int row = 0;
    float osg = 0.33f;
    
    bool landmower = this->getNumberParameterValue("Landmower mode");
    
    
    thread_clock::time_point pos_start = thread_clock::now();  
    for(int i = 0; i<k/*libSize*/; i++){
        
        int _id = n;
        if(using_query){
            _id = resultingFeatureNeighbors(i,0);
        }
        else if(i > 0){
            _id = resultingFeatureNeighbors(i-1,n);
        }
        
        if(i%gridSize==0){
            row++;
        }
        if(landmower)
            p.x = (-osg + 2*osg*(float)(i%gridSize)/(float)(gridSize-1))*pow(-1.0f,row+1);
        else
            p.x = -osg + 2*osg*(float)(i%gridSize)/(float)(gridSize-1);
        p.y = -osg +2*osg*(float)(row-1)/(float)(gridSize-1);
        
        mediaBrowser->setNodeNextPosition(_id, p);
        //std::cout << "ACPosPlugImportGrid::updateNextPositions: media " << _id << " i%gridSize " << i%gridSize << " x " << p.x << " y " << p.y << " filename " << media_cycle->getMediaFileName(_id) << std::endl; 
        
    }
    
    thread_clock::time_point pos_stop = thread_clock::now();  
    
    /*thread_clock::time_point rann_start = thread_clock::now();
      
    double alpha = 0.80;//0.95; //The desired success probability.
    size_t leafSize = 20; // Leaf size for tree building.
    bool singleMode = false; // If true, single-tree search is used, as opposed to dual-tree search.
    double tau = 5; // The allowed rank-error in terms of the percentile of the data
    bool sampleAtLeaves = false; // The flag to trigger sampling at leaves
    bool firstLeafExact = false; // The flag to trigger sampling only after exactly exploring the first leaf.
    size_t singleSampleLimit = 20; // The limit on the maximum number of samples (and hence the largest node you can approximate).
    k = 20;
    // Sanity check on the value of 'tau' with respect to 'k' so that
    // 'k' neighbors are not requested from the top-'rank_error' neighbors
    // where 'rank_error' <= 'k'.
    size_t rank_error = (size_t) ceil(tau * (double) desc_m.n_cols / 100.0);
    std::cout << " rank error " << rank_error << " >? k " << k << " with tau " << tau << std::endl; 
    tau = 100.0*(k+1)/(double) desc_m.n_cols;
    tau *=100;
    if (rank_error <= k){
        //Log::Fatal << "Invalid 'tau' (" << tau << ") - k (" << k << ") " <<
        //              "combination. Increase 'tau' or decrease 'k'." << endl;
        
    }
    rank_error = (size_t) ceil(tau * (double) desc_m.n_cols / 100.0);
    std::cout << " rank error corrected to " << rank_error << " >? k " << k << " with tau " << tau << std::endl; 
    
    arma::Mat<size_t> neighbors;
    arma::mat distances;
    // The results output by the AllkRANN class are
    // shuffled if the tree construction shuffles the point sets.
    arma::Mat<size_t> neighborsOut;
    arma::mat distancesOut;
    
    // Mappings for when we build the tree.
    std::vector<size_t> oldFromNewRefs;
    std::vector<size_t> oldFromNewQueries;
    
    // Build trees by hand, so we can save memory: if we pass a tree to
    // NeighborSearch, it does not copy the matrix.
    //std::cout << "Building reference tree..." << endl;
    Timer::Start("tree_building");
    
    //mlpack 1.0.12+
    //typedef KDTree<EuclideanDistance, RAQueryStat<NearestNeighborSort>, arma::mat> TreeType;
    //mlpack 1.0.12
    typedef BinarySpaceTree<bound::HRectBound<2, false>,
            RAQueryStat<NearestNeighborSort> > TreeType;
            
    TreeType refTree(desc_m, oldFromNewRefs, leafSize);
    Timer::Stop("tree_building");
    
    std::cout << "Building query tree..." << endl;
    Timer::Start("tree_building");
    TreeType queryTree(query, oldFromNewQueries, leafSize);
    Timer::Stop("tree_building");
    std::cout << "Tree built." << endl;
    
    // Because we may construct it differently, we need a pointer.
    AllkRANN* allkrann = 0;
    //mlpack 1.0.12+
    //allkrann = new AllkRANN(&refTree, singleMode, tau, alpha, sampleAtLeaves,
    //                  firstLeafExact, singleSampleLimit);
    //mlpack 1.0.12
    allkrann = new AllkRANN(&refTree, &queryTree, desc_m, query,
                            singleMode);
                            
                            
    std::cout << "Computing " << k << " nearest neighbors " << "with " <<
                 tau << "% rank approximation..." << endl;
    //mlpack 1.0.12+
    //allkrann->Search(&queryTree, k, neighborsOut, distancesOut);
    //mlpack 1.0.12
    allkrann->Search(k, neighborsOut, distancesOut,
                     tau, alpha, sampleAtLeaves,
                     firstLeafExact, singleSampleLimit);
                     
                     
    // Map the results back to the correct places.
    //Unmap(neighborsOut, distancesOut, oldFromNewRefs, oldFromNewQueries,
    //      neighbors, distances);
    
    thread_clock::time_point rann_stop = thread_clock::now();
    
    mlpack::data::Save("./rann_distances.csv", distances, true);
    mlpack::data::Save("./rann_neighbors.csv", neighbors, true);*/
    
    
//    thread_clock::time_point lsh_start = thread_clock::now();
//    arma::Mat<size_t> neighbors;
//    arma::mat distances;
//    this->lsh(desc_m,query,neighbors,distances,/*k*/2);
//    thread_clock::time_point lsh_stop = thread_clock::now();

    thread_clock::time_point stop = thread_clock::now();  
    std::cout << "ACPosPlugKnnGrid::updateNextPositions took: " << duration_cast<milliseconds>(stop - start).count() << " ms ";
    std::cout << "(";
    std::cout << "extractDescMatrix: " << duration_cast<milliseconds>(desc_stop - desc_start).count() <<"ms ";
    std::cout << "- knn: " << duration_cast<milliseconds>(knn_stop - knn_start).count() <<"ms ";
    //std::cout << "- lsh: " << duration_cast<milliseconds>(lsh_stop - lsh_start).count() <<"ms ";
    //std::cout << "- twentynn: " << duration_cast<milliseconds>(twentynn_stop - twentynn_start).count() <<"ms ";
    //std::cout << "- rann: " << duration_cast<milliseconds>(rann_stop - rann_start).count() <<"ms ";
    std::cout << "- pos: " << duration_cast<milliseconds>(pos_stop - pos_start).count() <<"ms ";
    std::cout << ") " << std::endl; 
    
}

/**
 * @brief Adapted from mlpack/methods/lsh/lsh_main.cpp
 * @author Parikshit Ram
 *
 * This function computes the approximate nearest-neighbors using 2-stable
 * Locality-sensitive Hashing.
 *
 * This function is adapted from mlpack 1.0.12.
 *
 * mlpack is free software; you may redstribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
bool ACPosPlugKnnGrid::lsh(arma::mat referenceData, arma::mat queryData, arma::Mat<size_t> &neighbors, arma::mat& distances, size_t k){
    
    // Define our input parameters that this program will take.
    size_t numProj = 10; //The number of hash functions for each table
    size_t numTables = 30; //The number of hash tables to be used.
    double hashWidth = 0.0; //The hash width for the first-level hashing in the LSH preprocessing. By default, the LSH class automatically estimates a hash width for its use.
    size_t secondHashSize = 99901; //The size of the second level hash table.
    size_t bucketSize = 500; //The size of a bucket in the second level hash.
    int seed = 0; //Random seed. If 0, 'std::time(NULL)' is used.
            
    if (seed!= 0)
        math::RandomSeed((size_t) seed);
    else
        math::RandomSeed((size_t) time(NULL));
    
    // Sanity check on k value: must be greater than 0, must be less than the
    // number of reference points.
    if (k > referenceData.n_cols)
    {
        std::cerr << "Invalid k: " << k << "; must be greater than 0 and less ";
        std::cerr << "than or equal to the number of reference points (";
        std::cerr << referenceData.n_cols << ")." << endl;
        return false;
    }
    
    if (hashWidth == 0.0)
        std::cout << "Using LSH with " << numProj << " projections (K) and " <<
                     numTables << " tables (L) with default hash width." << endl;
    else
        std::cout << "Using LSH with " << numProj << " projections (K) and " <<
                     numTables << " tables (L) with hash width(r): " << hashWidth << endl;
    
    //Timer::Start("hash_building");
    
    LSHSearch<>* allkann;

    #if(MLPACK_VERSION_MAJOR >= 2 || __MLPACK_VERSION_MAJOR >= 2)
    int numProbes = 0;
    allkann = new LSHSearch<>();
    allkann->Train(std::move(referenceData), numProj, numTables, hashWidth, secondHashSize, bucketSize);
    if (queryData.n_cols != 0 && queryData.n_rows != 0)
        allkann->Search(std::move(queryData), k, neighbors, distances, 0, numProbes);
    else
        allkann->Search(k, neighbors, distances, 0, numProbes);

    #else
    if (queryData.n_cols != 0 && queryData.n_rows != 0)
        allkann = new LSHSearch<>(referenceData, queryData, numProj, numTables,
                                  hashWidth, secondHashSize, bucketSize);
    else
        allkann = new LSHSearch<>(referenceData, numProj, numTables, hashWidth,
                                  secondHashSize, bucketSize);
    //Timer::Stop("hash_building");
    
    //std::cout << "Computing " << k << " distance approximate nearest neighbors " << endl;
    allkann->Search(k, neighbors, distances);
    #endif
    
    //std::cout << "Neighbors computed." << endl;
    
    delete allkann;
    return true;
}

void ACPosPlugKnnGrid::extractDescMatrix(ACMediaBrowser* mediaBrowser, arma::mat& desc_m, vector<string> &featureNames){
    ACMedias medias = mediaBrowser->getLibrary()->getAllMedia();
    std::vector<long> ids = mediaBrowser->getLibrary()->getAllMediaIds();
    int nbMedia = medias.size();
    int featDim;
    std::string featName;
    int totalDim = 0;
    // Count nb of feature
    int nbFeature = mediaBrowser->getLibrary()->getFirstMedia()->getNumberOfFeaturesVectors();
    std::vector<float> weight=mediaBrowser->getWeightVector();
    if (nbFeature!=weight.size())
        std::cerr<<"ACPosPlugKnnGrid::extractDescMatrix weight vector size incompatibility"<<endl;
    for(int f=0; f< nbFeature; f++){
        if (weight[f]>0.f){
            featureNames.push_back(mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getName());
            featDim = mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getSize();
            //std::cout << "Feature " << mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getName() << " dim " << featDim << std::endl;
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
                featName = mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getName();
                featDim = mediaBrowser->getLibrary()->getFirstMedia()->getPreProcFeaturesVector(f)->getSize();
                for(int d=0; d < featDim; d++){
                    desc_m(tmpIdy,tmpIdx) = medias[ids[i]]->getPreProcFeaturesVector(f)->getFeatureElement(d);
                    arma::mat value(1,1);
                    value(0,0) = desc_m(tmpIdy,tmpIdx);
                    if(value.has_nan())
                        std::cout << "Media " << media_cycle->getMediaFileName(ids[i]) << " feature " << featName << " dimd " << featDim << " dim " << d << " .has_nan() " << value.has_nan() << std::endl;
                    tmpIdx++;
                }
            }
        }
        if(mediaBrowser->getMediaNode(ids[i])){
            tmpIdy++;
        }
    }
}
