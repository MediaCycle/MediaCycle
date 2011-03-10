/**
 * @brief pdist.cpp
 * @author Xavier Siebert
 * @date 31/01/2011
 * @copyright (c) 2011 – UMONS - Numediart
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

#include "Armadillo-utils.h"
using namespace arma;


mat pdist(mat data){
	//mode : distance type    // 0 = euclidian distance
    int n = data.n_rows;
    int p = data.n_cols;

    mat similarityM = zeros<mat>(n,n);

    for (int i = 0; i < n-1; i++) {
        colvec dsq = zeros<colvec>(n-1-i);
        colvec tmpResult = zeros<colvec>(n-1-i);

        for (int q = 0; q < p; q++) {
            colvec tmp2 = data.col(q);
            colvec tmp = tmp2.rows(i+1,n-1);
            dsq = dsq + square(data(i,q) - tmp);
        }
        tmpResult = sqrt(dsq);
        similarityM.submat(i+1,i,n-1,i) = tmpResult;
        similarityM.submat(i,i+1,i,n-1) = trans(tmpResult);
    }
    return similarityM;
}

