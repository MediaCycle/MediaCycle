/**
 * @brief melfilters.h
 * @author Damien Tardieu
 * @date 11/03/2010
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

#ifndef MELFILTERS_H
#define MELFILTERS_H

#include "Armadillo-utils.h"

inline float freq2mel(float F_m){
	float M_m = 2595.0*std::log10(F_m/700.0+1);
	return M_m;
}

inline float mel2freq(float M_m){
	float F_m = 700 * (pow(10, M_m/2595.0) - 1);
	return F_m;
}

inline mat freq2mel(mat F_m){
	mat M_m = 2595.0*log10(F_m/700.0+1);
	return M_m;
}

inline mat mel2freq(mat M_m){
	mat F_m(M_m);
	for (int i=0; i < M_m.n_rows; i++ )
		for (int j=0; j < M_m.n_cols; j++ )
			F_m(i,j) = mel2freq(M_m(i,j));
	return F_m;
}

mat melfilters(int nChannels, int fftSize, int sr_hz);

#endif
