/**
 * @brief blackman.h
 * @author Thierry Ravet
 * @date 07/09/2012
 * @copyright (c) 2012 – UMONS - Numediart
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


#ifndef BLACKMAN_H
#define BLACKMAN_H

#include "Armadillo-utils.h"

inline arma::colvec blackman(int L){
	arma::colvec window_v(L);
	double alpha = .16;
	double a0 = (1-alpha)/2;
	double a1=1.0/2;
	double a2=alpha/2;
	arma::colvec n_v = arma::linspace<arma::colvec>(0, L-1, L);
#ifdef __clang__
    window_v =cos(n_v*4*arma::math::pi()/(double)(L-1))*a2-cos(n_v*2*arma::math::pi() / (double)(L-1))*a1 +a0;
#else //__clang__
    window_v = a0 - a1 * cos(2*arma::math::pi() * n_v / (double)(L-1)) + a2*cos(4*arma::math::pi()*n_v/(double)(L-1));
#endif //__clang__
            
	return window_v;
}

#endif
