/**
 * @brief Convenience function to use the lsh method from mlpack  
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

#include "lsh.h"

#include <mlpack/core/util/version.hpp>

using namespace std;
//using namespace arma;
using namespace mlpack;
//using namespace mlpack::neighbor;
//using namespace mlpack::tree;
using namespace mlpack::metric;

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
 
bool lsh(arma::mat referenceData, 
         arma::Mat<size_t> &neighbors, 
         arma::mat& distances, 
         size_t k,
         arma::mat queryData)
{
    #if(MLPACK_VERSION_MAJOR >= 3)
    std::cerr << "knn implementation incomplete" << std::endl;
    #else
    
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
    
    if (queryData.n_cols != 0 && queryData.n_rows != 0)
        allkann = new LSHSearch<>(referenceData, queryData, numProj, numTables,
                                  hashWidth, secondHashSize, bucketSize);
    else
        allkann = new LSHSearch<>(referenceData, numProj, numTables, hashWidth,
                                  secondHashSize, bucketSize);
    
    //Timer::Stop("hash_building");
    
    //std::cout << "Computing " << k << " distance approximate nearest neighbors " << endl;
    allkann->Search(k, neighbors, distances);
    
    //std::cout << "Neighbors computed." << endl;
    
    delete allkann;
    #endif
    return true;
}
