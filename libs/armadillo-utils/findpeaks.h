/**
 * @brief findpeaks.h
 * @author Christian Frisson
 * @date 01/09/2019
 * @copyright (c) 2019 – UMONS - Numediart
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


#ifndef FINDPEAKS_H
#define FINDPEAKS_H

#include "Armadillo-utils.h"

#include <algorithm>
#include <iostream>
#include <string>

template<typename eT>
arma::icolvec findpeaks(const arma::Col<eT>& Data, long Pd)
{
	long m = 0;                  // counter for peaks found
	long long L = Data.n_rows;

	long long j = 0;
	arma::Col<eT> pks  = arma:: zeros< arma::Col<eT> >(Data.n_rows);
	arma::icolvec locs = arma:: zeros<arma::icolvec>(Data.n_rows);

	// First, the "Pd" neighbors, on either side, of the current data point are
	// found. Then the current data point is compared with these "Pd" neighbors
	// to determine whether it is a peak.
	long endL, endR;
	while (j < L){
		// leftmost neighbor index
		endL = std::max((long long) 0,j-Pd);

		// Update the leftmost neighbor index if there is a peak within
		// "Pd" neighbors of leftmost index
		if(m >= 0){
			if (j < std::min((long long)locs(m)+Pd, L-1)){
				j = std::min((long long)locs(m)+Pd, L-1);
				endL = j-Pd;
			}
		}

		// rightmost neighbor index
		endR = std::min(L-1,j+ Pd);

		// create neighbor data set
		arma::Col<eT> temp = Data.rows(endL,endR);

		// set current data point to -Inf in the neighbor data set
		temp(j-endL) = min(temp)-1;
		
		// Compare current data point with all neighbors-
#ifdef __clang__
		if( ((sum(Data(j) > temp)) == temp.n_rows) && (j != 0) && j!=L-1 ){
#else //__clang__
		if( (arma::as_scalar(sum(Data(j) > temp)) == temp.n_rows) && (j != 0) && j!=L-1 ){
#endif //__clang__
			pks(m)  = Data(j);
			locs(m) = j;
			m = m+1;
		}
		j =j+1;
	}

	// return all peaks found
	if (m != 0){
		pks = pks.rows(0,m-1);
		locs = locs.rows(0,m-1);
	}
	else{
		pks.reset();
		locs.reset();
	}
	return locs;
}

#endif
