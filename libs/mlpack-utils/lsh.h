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

#include <mlpack/core.hpp>
#include <mlpack/core/metrics/lmetric.hpp>
#include <mlpack/methods/lsh/lsh_search.hpp>

#ifndef _mlpackUtilsLSH_
#define _mlpackUtilsLSH_

/**
 * All K-Approximate-Nearest-Neighbor Search with LSH
 * This method will calculate the k approximate-nearest-neighbors of a set 
 * of points using locality-sensitive hashing. You may specify a separate set
 *  of reference points and query points, or just a reference set which will 
 * be used as both the reference and query set. 
 * \n\n
 * The output files are organized such that row i and column j in the 
 * neighbors output file corresponds to the index of the point in the 
 * reference set which is the i'th nearest neighbor from the point in the 
 * query set with index j.  Row i and column j in the distances output file 
 * corresponds to the distance between those two points.
 * \n\n
 * Because this is approximate-nearest-neighbors search, results may be 
 * different from run to run.  Thus, the --seed option can be specified to 
 * set the random seed.
 */

bool lsh(arma::mat referenceData, 
         arma::Mat<size_t> &neighbors, 
         arma::mat& distances, 
         size_t k, /*Number of nearest neighbors to find*/
         arma::mat queryData = 0
         );

#endif // _mlpackUtilsLSH_
