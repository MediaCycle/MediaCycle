/**
 * @brief fft.cpp
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

#include "fft.h"
using namespace arma;

cx_mat fft(mat x_m, int n){
	//  std::cout << "Calling fft for mat" << std::endl;
  cx_mat y_m;
  if (x_m.n_rows > 1){
    y_m.set_size(n, x_m.n_cols);
    for (unsigned int i = 0; i < x_m.n_cols; i++)
      y_m.col(i) = fft_helper(x_m.col(i),n);
  }
  // only one line several columns
  else if (x_m.n_cols > 1){
    y_m.set_size(1, n);
    y_m.row(0) = trans(fft_helper(trans(x_m.row(0)),n));
  }
  // one line one col
  else{
    y_m = ones<cx_mat>(1,n)*x_m(0,0);
  }
  return y_m;
}

