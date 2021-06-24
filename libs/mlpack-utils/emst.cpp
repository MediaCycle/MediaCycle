/**
 * @brief Convenience function to use the emst method from mlpack  
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

#include "emst.h"

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
#endif

namespace mediacycle {

arma::mat emst(arma::mat desc_m, bool naive, const size_t leafSize){

    arma::mat mst;

    if(naive){
        mlpack::emst::DualTreeBoruvka<> naive(desc_m, true);
        naive.ComputeMST(mst);
    }
    else{
        std::vector<size_t> oldFromNew;
        #if(MLPACK_VERSION_MAJOR >= 2 || __MLPACK_VERSION_MAJOR >= 2)
        const size_t leafSize = 20;
        mlpack::tree::KDTree<EuclideanDistance, mlpack::emst::DTBStat, arma::mat> tree(desc_m,oldFromNew,leafSize);
        mlpack::metric::LMetric<2, true> metric;
        mlpack::emst::DualTreeBoruvka<> dtb(&tree, metric);
        #else
        mlpack::tree::BinarySpaceTree <mlpack::bound::HRectBound<2>, mlpack::emst::DTBStat> tree(desc_m,oldFromNew, leafSize);
        mlpack::metric::LMetric<2, true> metric;
        mlpack::emst::DualTreeBoruvka<> dtb(&tree, desc_m, metric);
        #endif

        arma::mat results;
        dtb.ComputeMST(results);

        // Unmap the results.
        arma::mat unmappedResults(results.n_rows, results.n_cols);
        for (size_t i = 0; i < results.n_cols; ++i)
        {
            const size_t indexA = oldFromNew[size_t(results(0, i))];
            const size_t indexB = oldFromNew[size_t(results(1, i))];

            if (indexA < indexB)
            {
                unmappedResults(0, i) = indexA;
                unmappedResults(1, i) = indexB;
            }
            else
            {
                unmappedResults(0, i) = indexB;
                unmappedResults(1, i) = indexA;
            }

            unmappedResults(2, i) = results(2, i);
        }

        mst = unmappedResults;

    }
    return mst;
}

}
