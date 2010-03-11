/**
 * @brief dct.cpp
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

#include "dct.h"

mat dct(mat a, int n){
	
	mat dct_m;
	mat aa;
	mat y;
	cx_mat yy;
	cx_mat b;

	if (a.n_elem == 0){
		return dct_m;
	}

	// If input is a vector, make it a column:
	bool do_trans = (size(a,1) == 1);
	if (do_trans) 
		a = trans(a);
	
	int m = size(a,2);

	// Pad or truncate input if necessary
	if (size(a,1)<n){
		aa.zeros(n,m);
		aa.rows(0,size(a,1)-1) = a;
	}
	else
		aa = a.rows(0,n-1);

	// Compute weights to multiply DFT coefficients
	mat tmp0_v = "0;";
	mat tmp1_v = "1;";
	cx_mat Ic(tmp0_v,tmp1_v);
	cx_mat ww = trans(exp(-Ic*linspace<rowvec>(0,n-1,n)*math::pi()/(2*n))/sqrt(2*n));
	ww(0) = ww(0) / sqrt(2);
	
	y.set_size(aa.n_rows,aa.n_cols);
  // Re-order the elements of the columns of x
  for (int i=0; i < n; i=i+2){
		y.row(i/2) = aa.row(i);
		y.row(n-i/2-1) = aa.row(i+1);
	}
	//	y.save("y.txt", arma_ascii);
	//	y = [ aa(1:2:n,:); aa(n:-2:2,:) ];
  yy = fft(y, y.n_rows);  
  ww = 2*ww;  // Double the weights for even-length case  
	//	ww.save("ww.txt", arma_ascii);
	//yy.save("yy.txt", arma_ascii);
	// Multiply FFT by weights:
	b = ww.cols(0,m-1) % yy;
	
	dct_m = real(b);
	if (do_trans) 
		dct_m = trans(dct_m);
	return dct_m;
}
