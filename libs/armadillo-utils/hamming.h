/**
 * @brief hamming.h
 * @author Xavier Siebert
 * @date 18/07/2011
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


#ifndef HAMMING_H
#define HAMMING_H

#include "Armadillo-utils.h"

inline arma::colvec hamming(int L){
	arma::colvec window_v(L);
	int half;
	if(L%2==0){		//nombre de points pair
		half=L/2;

		arma::colvec half_window_v(half);	
		arma::colvec rev_half_window_v(half);
		arma::colvec n_v = arma::linspace<arma::colvec>(0, half-1, half);

		half_window_v = 0.54 - 0.46 * cos(2*arma::math::pi()*n_v/(L-1)); 
		rev_half_window_v = flipud(half_window_v);
		
		window_v = arma::join_cols(half_window_v,rev_half_window_v);
	
	}
	else{
		half=(L+1)/2;
		arma::colvec half_window_v(half);
		arma::colvec rev_half_window_v(half);
		arma::colvec n_v = arma::linspace<arma::colvec>(0, half-1, half);

		half_window_v = 0.54 - 0.46 * cos(2*arma::math::pi()*n_v/(L-1)); 
		rev_half_window_v = flipud(half_window_v);
		window_v = arma::join_cols(half_window_v,rev_half_window_v.rows(1,half-1));

	}
	return window_v;
}

#endif
