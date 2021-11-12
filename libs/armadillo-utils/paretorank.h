/**
 * @brief paretorank.h
 * @author Christian Frisson
 * @date 10/04/2011
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


#ifndef PARETORANK_H
#define PARETORANK_H

#include "Armadillo-utils.h"

template<typename eT>
arma::ucolvec paretorank(arma::Mat<eT> X, int maxRank, int minNbItems){
	// careful : the case were to lines are the same is not handled 
	int row = X.n_rows;
	arma::ucolvec front_v(row);
	arma::ucolvec rank_v;
	int currentRank = 1;
	arma::ucolvec posFront_v;
	#ifdef ARMADILLO_HAVE_RANDU // the linspace prototype changed at the same time randu was introduced, for version 0.9.50
		arma::imat pos_v = arma::linspace<arma::imat>(0, row-1, row);
	#else
		arma::imat pos_v = arma::linspace<arma::imat>(0, row-1, row, 0);
	#endif
	arma::Mat<eT> oldX;
	int tmpIdx = 0;
	arma::imat oldPos_v;
	int itemRanked = 0;
	
	rank_v.zeros(row);

	if (minNbItems == 0)
		minNbItems = row+1;

	while (X.n_elem > 0 & itemRanked < minNbItems){
		front_v = paretofront(X);
		for (int iFront=0; iFront < front_v.n_rows; iFront++){
			if (front_v(iFront) == 1) 
				rank_v(pos_v(iFront,0)) = currentRank;
		}
		currentRank = currentRank+1;
		oldX = X;
		oldPos_v = pos_v;
		X.set_size(oldX.n_rows - arma::as_scalar(arma::sum(front_v)), X.n_cols);
		pos_v.set_size(oldX.n_rows - sum(front_v), 1);
		tmpIdx = 0;
		for (int iFront=0; iFront < front_v.n_rows; iFront++){
			if (!front_v(iFront)){
				X.row(tmpIdx) = oldX.row(iFront);
				pos_v(tmpIdx,0) = oldPos_v(iFront,0);
				tmpIdx++;
			}
		}
		itemRanked = row - X.n_rows;
	}
	return rank_v;
}

#endif
