/**
 * @brief blackman.h
 * @author Alexis Moinet
 * @date 18/11/2010
 * @copyright (c) 2010 – UMONS - Numediart
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

#ifndef BLACKMAN_H
#define BLACKMAN_H

inline colvec blackman(int L){
	colvec window_v(L);
	double alpha = .16;
	double a0 = (1-alpha)/2;
	double a1=1.0/2;
	double a2=alpha/2;
	colvec n_v = linspace<colvec>(0, L-1, L);
	window_v = a0 - a1 * cos(2*arma::math::pi() * n_v / (double)(L-1)) + a2*cos(4*arma::math::pi()*n_v/(double)(L-1));
	return window_v;
}

#endif
