/**
 * @brief Convenience function to use the knn method from mlpack  
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

#include "knn.h"

#include <mlpack/core/util/version.hpp>

#if(MLPACK_VERSION_MAJOR >= 3)
#include "mlpack/methods/neighbor_search/ns_model.hpp"
#endif

using namespace std;
using namespace mlpack;
using namespace mlpack::neighbor;
using namespace mlpack::tree;
#if(MLPACK_VERSION_MAJOR >= 2 || __MLPACK_VERSION_MAJOR >= 2)
using namespace mlpack::metric;
using namespace mlpack::util;
// Convenience typedef.
typedef NSModel<NearestNeighborSort> KNNModel;
#endif

/**
 * @brief Adapted from mlpack/methods/neighbor_search/allknn_main.cpp
 * @author Ryan Curtin
 *
 * Implementation of the AllkNN executable.  Allows some number of standard
 * options.
 *
 * This function is adapted from mlpack 1.0.12.
 *
 * mlpack is free software; you may redstribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */

bool knn(arma::mat referenceData, 
         arma::Mat<size_t> &neighbors, 
         arma::mat& distances, 
         size_t k,
         arma::mat queryData)
{
    #if(MLPACK_VERSION_MAJOR >= 3)
    std::cerr << "knn implementation incomplete" << std::endl;
    #else

    // Define our input parameters that this program will take.
    size_t leafSize = 20; //Leaf size for tree building.
    bool naive = false; // If true, O(n^2) naive mode is used for computation.
    bool singleMode = false; //If true, single-tree search is used (as opposed to dual-tree search).
    const bool randomBasis = false; // Before tree-building, project the data onto a random orthogonal basis.
    bool cover_tree = false; // If true, use cover trees to perform the search (experimental, may be slow).
    int seed = 0; //Random seed. If 0, 'std::time(NULL)' is used.
    
    if (seed!= 0)
        math::RandomSeed((size_t) seed);
    else
        math::RandomSeed((size_t) std::time(NULL));
    
    // Sanity check on k value: must be greater than 0, must be less than the
    // number of reference points.  Since it is unsigned, we only test the upper bound.
    if (k > referenceData.n_cols)
    {
        std::cerr << "Invalid k: " << k << "; must be greater than 0 and less ";
        std::cerr << "than or equal to the number of reference points (";
        std::cerr << referenceData.n_cols << ")." << endl;
        return false;
    }
    
    // Sanity check on leaf size.
    if (leafSize < 1)
    {
        std::cerr << "Invalid leaf size: " << leafSize << ".  Must be greater "
                                                          "than 0." << endl;
        return false;
    }
    
    // Naive mode overrides single mode.
    if (singleMode && naive)
    {
        std::cerr << "--single_mode ignored because --naive is present." << endl;
    }
    
    if (naive)
        leafSize = referenceData.n_cols;
    
    // See if we want to project onto a random basis.
    if (randomBasis)
    {
        // Generate the random basis.
        while (true)
        {
            // [Q, R] = qr(randn(d, d));
            // Q = Q * diag(sign(diag(R)));
            arma::mat q, r;
            if (arma::qr(q, r, arma::randn<arma::mat>(referenceData.n_rows,
                                                      referenceData.n_rows)))
            {
                arma::vec rDiag(r.n_rows);
                for (size_t i = 0; i < rDiag.n_elem; ++i)
                {
                    if (r(i, i) < 0)
                        rDiag(i) = -1;
                    else if (r(i, i) > 0)
                        rDiag(i) = 1;
                    else
                        rDiag(i) = 0;
                }
                
                q *= arma::diagmat(rDiag);
                
                // Check if the determinant is positive.
                if (arma::det(q) >= 0)
                {
                    referenceData = q * referenceData;
                    std::cerr << "knn implementation incomplete" << std::endl;
                    if (queryFile != "")
                        queryData = q * queryData;
                    break;
                }
            }
        }
    }
    
    //arma::Mat<size_t> neighbors;
    //arma::mat distances;
    
    if (!cover_tree)
    {
        // Because we may construct it differently, we need a pointer.
        #if(MLPACK_VERSION_MAJOR >= 3)
        KNNModel allknn = NULL;
        #else
        AllkNN* allknn = NULL;
        #endif
        
        // Mappings for when we build the tree.
        std::vector<size_t> oldFromNewRefs;
        
        // Build trees by hand, so we can save memory: if we pass a tree to
        // NeighborSearch, it does not copy the matrix.
        std::cout << "Building reference tree..." << endl;
        //Timer::Start("tree_building");
        
        #if(MLPACK_VERSION_MAJOR >= 3)
        std::cerr << "knn implementation incomplete" << std::endl;
        #else
        BinarySpaceTree<bound::HRectBound<2>,
                NeighborSearchStat<NearestNeighborSort> >
                refTree(referenceData, oldFromNewRefs, leafSize);
        BinarySpaceTree<bound::HRectBound<2>,
                NeighborSearchStat<NearestNeighborSort> >*
                queryTree = NULL; // Empty for now.
        #endif
        
        //Timer::Stop("tree_building");
        
        std::vector<size_t> oldFromNewQueries;
        
        if (queryData.n_cols != 0 && queryData.n_rows != 0)
        {
            if (naive && leafSize < queryData.n_cols)
                leafSize = queryData.n_cols;
            
            std::cout << "Loaded query data from '" << queryFile << "' ("
                      << queryData.n_rows << " x " << queryData.n_cols << ")." << endl;
            
            std::cout << "Building query tree..." << endl;
            
            // Build trees by hand, so we can save memory: if we pass a tree to
            // NeighborSearch, it does not copy the matrix.
            if (!singleMode)
            {
                //Timer::Start("tree_building");
                
                queryTree = new BinarySpaceTree<bound::HRectBound<2>,
                        NeighborSearchStat<NearestNeighborSort> >(queryData,
                                                                  oldFromNewQueries, leafSize);
                
                //Timer::Stop("tree_building");
            }
            
            #if(MLPACK_VERSION_MAJOR >= 3)
            std::cerr << "knn implementation incomplete" << std::endl;
            #else
            allknn = new AllkNN(&refTree, queryTree, referenceData, queryData,
                                singleMode);
            #endif
            
            std::cout << "Tree built." << endl;
        }
        else
        {
            #if(MLPACK_VERSION_MAJOR >= 3)
            std::cerr << "knn implementation incomplete" << std::endl;
            #else
            allknn = new AllkNN(&refTree, referenceData, singleMode);
            #endif
            
            std::cout << "Trees built." << endl;
        }
        
        arma::mat distancesOut;
        arma::Mat<size_t> neighborsOut;
        
        std::cout << "Computing " << k << " nearest neighbors..." << endl;
        allknn->Search(k, neighborsOut, distancesOut);
        
        std::cout << "Neighbors computed." << endl;
        
        // We have to map back to the original indices from before the tree
        // construction.
        std::cout << "Re-mapping indices..." << endl;
        
        // Map the results back to the correct places.
        if ((queryData.n_cols != 0 && queryData.n_rows != 0) && !singleMode)
            Unmap(neighborsOut, distancesOut, oldFromNewRefs, oldFromNewQueries,
                  neighbors, distances);
        else if ((queryData.n_cols != 0 && queryData.n_rows != 0) && singleMode)
            Unmap(neighborsOut, distancesOut, oldFromNewRefs, neighbors, distances);
        else
            Unmap(neighborsOut, distancesOut, oldFromNewRefs, oldFromNewRefs,
                  neighbors, distances);
        
        // Clean up.
        if (queryTree)
            delete queryTree;
        
        delete allknn;
    }
    else // Cover trees.
    {
        // Make sure to notify the user that they are using cover trees.
        std::cout << "Using cover trees for nearest-neighbor calculation." << endl;
        
        // Build our reference tree.
        std::cout << "Building reference tree..." << endl;
        //Timer::Start("tree_building");
        CoverTree<metric::LMetric<2, true>, tree::FirstPointIsRoot,
                NeighborSearchStat<NearestNeighborSort> > referenceTree(referenceData,
                                                                        1.3);
        CoverTree<metric::LMetric<2, true>, tree::FirstPointIsRoot,
                NeighborSearchStat<NearestNeighborSort> >* queryTree = NULL;
        //Timer::Stop("tree_building");
        
        NeighborSearch<NearestNeighborSort, metric::LMetric<2, true>,
                CoverTree<metric::LMetric<2, true>, tree::FirstPointIsRoot,
                NeighborSearchStat<NearestNeighborSort> > >* allknn = NULL;
        
        // See if we have query data.
        if (queryData.n_cols != 0 && queryData.n_rows != 0)
        {
            // Build query tree.
            if (!singleMode)
            {
                std::cout << "Building query tree..." << endl;
                //Timer::Start("tree_building");
                queryTree = new CoverTree<metric::LMetric<2, true>,
                        tree::FirstPointIsRoot, NeighborSearchStat<NearestNeighborSort> >(
                            queryData, 1.3);
                //Timer::Stop("tree_building");
            }
            
            allknn = new NeighborSearch<NearestNeighborSort, metric::LMetric<2, true>,
                    CoverTree<metric::LMetric<2, true>, tree::FirstPointIsRoot,
                    NeighborSearchStat<NearestNeighborSort> > >(&referenceTree, queryTree,
                                                                referenceData, queryData, singleMode);
        }
        else
        {
            allknn = new NeighborSearch<NearestNeighborSort, metric::LMetric<2, true>,
                    CoverTree<metric::LMetric<2, true>, tree::FirstPointIsRoot,
                    NeighborSearchStat<NearestNeighborSort> > >(&referenceTree,
                                                                referenceData, singleMode);
        }
        
        std::cout << "Computing " << k << " nearest neighbors..." << endl;
        allknn->Search(k, neighbors, distances);
        
        std::cout << "Neighbors computed." << endl;
        
        delete allknn;
        
        if (queryTree)
            delete queryTree;
    }
    #endif
    return true;
}
