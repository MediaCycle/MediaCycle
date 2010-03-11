/**
 * @brief diagxcorr2.cpp
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

#include "diagxcorr2.h"

colvec diagxcorr2(mat A_m, mat B_m){
  if (A_m.n_rows != A_m.n_cols | B_m.n_rows != B_m.n_cols){
    std::cout << "Error : Matrices must be square" << endl;
    exit(1);
  }
  mat Tmp_m(1,1);
  mat tmp2_m,tmp3_m;
  colvec C_v(A_m.n_rows+B_m.n_rows-1); 
  for (unsigned int i=0; i < C_v.n_rows; i++){
    if (i < B_m.n_rows-1){
      Tmp_m = conv2(A_m.submat(0,0,i,i), rot90(B_m.submat(B_m.n_rows-i-1, B_m.n_rows-i-1, B_m.n_rows-1, B_m.n_rows-1),2), "valid");
      C_v(i)=Tmp_m(0,0);
    }
    else if (i > A_m.n_rows-1){
      Tmp_m = conv2(A_m.submat(i-B_m.n_rows+1,i-B_m.n_cols+1,A_m.n_rows-1,A_m.n_cols-1), rot90(B_m.submat(0, 0, C_v.n_rows-i-1, C_v.n_rows-i-1),2), "valid");
      C_v(i)=Tmp_m(0,0);
    }
    else{
      Tmp_m = conv2(A_m.submat(i-B_m.n_rows+1, i-B_m.n_rows+1,i,i), rot90(B_m,2), "valid");
      C_v(i) = Tmp_m(0,0); 
    }
  }
  return C_v;
}

