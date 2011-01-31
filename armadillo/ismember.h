/**
 * @brief ismember.h
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


#ifndef ISMEMBER_H
#define ISMEMBER_H

#include "Armadillo-utils.h"

template<typename eT>
arma::uvec ismember(const arma::Col<eT>& A_v, const arma::Col<eT>& X_v){
	arma::uvec res_v;
	bool cont;
	int idx;
	res_v.zeros(A_v.n_rows);
	for (int i=0; i<A_v.n_rows; i++){
		idx=0;
		cont = true;
		while (cont & (idx < X_v.n_rows)) {
			if (A_v(i) == X_v(idx)){
				res_v(i) = 1;
				cont = false;
			}	
			idx++;	
		}
	}
	return res_v;
}

#endif
