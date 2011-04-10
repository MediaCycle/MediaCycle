/**
 * @brief randperm.h
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


#ifndef RANDPERM_H 
#define RANDPERM_H

#include "Armadillo-utils.h"

#include <time.h>

inline arma::ucolvec randperm(int n){
	int t0 = clock();
	int seed = t0 - (int)(t0/1000)*1000; 
	srand ( seed * 3 ); //not from arma
	
	#ifdef ARMADILLO_HAVE_RANDU
		arma::colvec q_v = arma::randu<arma::colvec>(n+1);
	#else
		arma::colvec q_v = arma::rand<arma::colvec>(n+1);
	#endif
	arma::ucolvec perm_v = arma::sort_index(q_v.rows(1,n));
	return perm_v;
}

#endif
