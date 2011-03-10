/**
 * @brief rot90.cpp
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

#include "Armadillo-utils.h"

using namespace arma;

mat rot90(mat A_m, int k){
  mat B_m;
  k = fmod((float)k,(float)4);
  if (k < 0)
    k = k + 4;
  if (k == 1){
    B_m.set_size(A_m.n_cols, A_m.n_rows);
    A_m = trans(A_m);
    for (unsigned int i=0; i < A_m.n_rows; i++)
      for (unsigned int j=0; j < A_m.n_cols; j++)
	B_m(i,j) = A_m(A_m.n_rows-i-1,j);
  }
  else if (k == 2){
    B_m.set_size(A_m.n_rows, A_m.n_cols);
    for (unsigned int i=0; i < A_m.n_rows; i++)
      for (unsigned int j=0; j < A_m.n_cols; j++)
	B_m(i,j) = A_m(A_m.n_rows-i-1,A_m.n_cols-j-1);
  }
  else if (k == 3){
    B_m.set_size(A_m.n_rows, A_m.n_cols);
    for (unsigned int i=0; i < A_m.n_rows; i++)
      for (unsigned int j=0; j < A_m.n_cols; j++)
	B_m(i,j) = A_m(A_m.n_rows-i-1,j);
    B_m = trans(B_m);
  }
  else
    B_m = A_m;
  return B_m;
}

