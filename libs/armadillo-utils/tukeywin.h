/**
 * @brief tukeywin.h
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

#ifndef TUKEYWIN_H
#define TUKEYWIN_H

#include "Armadillo-utils.h"


// TUKEYWIN Tukey window.
//   TUKEYWIN(N) returns an N-point Tukey window in a column vector.
//
//   W = TUKEYWIN(N,R) returns an N-point Tukey window in a column vector.
//   A Tukey window is also known as the cosine-tapered window.  The R 
//   parameter specifies the ratio of taper to constant sections. This ratio 
//   is normalized to 1 (i.e., 0 < R < 1). If omitted, R is set to 0.500.
//
//   NOTE: At extreme values of R, the Tukey window degenerates into other
//   common windows. Thus when R = 1, it is equivalent to a Hanning window.
//   Conversely, for R = 0 the Tukey window is equivalent to a boxcar window.
//
//   EXAMPLE:
//      N = 64; 
//      w = tukeywin(N,0.5); 

inline arma::colvec tukeywin(int n, float r=.5){
	arma::colvec window_v(n);
	arma::colvec t;

	if (r <= 0){
    window_v = arma::ones(n,1);
	}
	else{
		if (r >= 1){
			std::cerr << "Tukeywin : n must be less than 1" << std::endl;
			exit(1);
		}
		else{
			arma::colvec t = arma::linspace<arma::colvec>(0,1,n);
			// Defines period of the taper as 1/2 period of a sine wave.
			float per = r/2.0; 
			int tl = (int)(per*(n-1))+1;
			int th = n-tl+1;
			// Window is defined in three sections: taper, constant, taper
			window_v.rows(0,tl-1) = ((1+cos(arma::math::pi()/per*(t.rows(0,tl-1) - per)))/2);
			window_v.rows(tl, th-2) = arma::ones(th-tl-1,1);
			window_v.rows(th-1,n-1) = ((1+cos(arma::math::pi()/per*(t.rows(th-1, n-1) - 1 + per)))/2);
			//w = [ ((1+cos(pi/per*(t(1:tl) - per)))/2);  arma::ones(th-tl-1,1); ((1+cos(pi/per*(t(th:end) - 1 + per)))/2)];
		}
	}
	return window_v;
}

#endif
