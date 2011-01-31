/**
 * @brief conv2.cpp
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

#include "conv2.h"
using namespace arma;

mat conv2(mat A_m, mat B_m, std::string mode){

  // special case when A is smaller than B and mode is "valid"
  if (B_m.n_rows > A_m.n_rows & B_m.n_cols > A_m.n_cols & !strcmp(mode.c_str(), "valid"))
    return mat(0,0);
  else{

    int fftnrows = (int)pow((float)2, nextpow2(A_m.n_rows+B_m.n_rows-1));
    int fftncols = (int)pow((float)2, nextpow2(A_m.n_cols+B_m.n_cols-1));

//     cx_mat Afft_m(fftnrows, fftncols);
//     cx_mat Bfft_m(fftnrows, fftncols);
//     cx_mat Cfft_m(fftnrows, fftncols);
//     mat C_m(fftnrows, fftncols);

    cx_mat Afft_m = fft2(A_m, fftnrows, fftncols);
    cx_mat Bfft_m = fft2(B_m, fftnrows, fftncols);
    cx_mat Cfft_m = Afft_m % Bfft_m;
    mat C_m = ifft2(Cfft_m, fftnrows, fftncols);
    
    if (!strcmp(mode.c_str(), "full"))
      return C_m.submat(0,0,A_m.n_rows+B_m.n_rows-2,A_m.n_cols+B_m.n_cols-2);
    else if (!strcmp(mode.c_str(), "same")){
      int px=((B_m.n_rows-1)+((B_m.n_rows-1)%2))/2;
      int py=((B_m.n_cols-1)+((B_m.n_cols-1)%2))/2;
      return C_m.submat(px, py, px+A_m.n_rows-1, px+A_m.n_cols-1);
    }
    else if (!strcmp(mode.c_str(), "valid")){
      int px=B_m.n_rows;
      int py=B_m.n_cols; 
      return C_m.submat(px-1, py-1, px+A_m.n_rows-B_m.n_rows-1, py+A_m.n_cols-B_m.n_cols-1);
    }
    else{
      std::cout<<"Wrong mode" << std::endl;
      exit(1);
    }  
  }
}

mat conv2(mat A_m, mat B_m){
  return conv2(A_m, B_m, "full");
}
