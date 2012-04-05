/**
 * @brief fft-helper.cpp
 * @author Christian Frisson
 * @date 05/04/2012
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

#include "fft-helper.h"

using namespace arma;

#if USE_FFTW

// cx_mat fft_helper(mat x_m, int n){
// 	//  std::cout << "Calling fft helper" << std::endl;
//   // n : fft size
//   fftw_complex *x_db, *y_db;
//   cx_mat y_m;
//   mat real_m(n,1);
//   mat imag_m(n,1);
//   int xSize, sigSize;
  
//   if (n==1)
//     y_m = ones<cx_mat>(1,1)*x_m(0,0);
//   else{
// 		x_db = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

//     for (int i=0; i<sigSize; i++){
//       x_db[i] = x_m(i,0);
//     }

//     for (int i=sigSize; i < n; i++){
//       x_db[i] = 0;
//     }
		
// 	}
// }

#else
// fft 1D for column vector
cx_mat fft_helper(mat x_m, int n){
	//  std::cout << "Calling fft helper" << std::endl;
  // n : fft size
  double* x_db = 0;
  //cx_mat y_m;
  mat real_m(n,1);
  mat imag_m(n,1);
  int xSize, sigSize;
  
  if (n==1)
    return ones<cx_mat>(1,1)*x_m(0,0);
  else{
    int* ip = new int[ 2 + (int)(sqrt(n)+.5)];
    double* w = new double[n/2];
    xSize = x_m.n_rows;

    sigSize = std::min(n, xSize);

    x_db = new double[n];
 
    for (int i=0; i<sigSize; i++){
      x_db[i] = x_m(i,0);
    }

    for (int i=sigSize; i < n; i++){
      x_db[i] = 0;
    }
		ip[0] = 0;
    rdft(n, 1, x_db, ip, w);
  
    // fill arma matrices
    for (int i=0; i<n; i=i+2){
      real_m((i)/2,0) = x_db[i];
      imag_m((i)/2,0) = x_db[i+1];
    }
    imag_m(0,0) = 0;
    real_m(n/2,0) = x_db[1];
    imag_m(n/2,0) = 0;
    // rearrange data to have matlab like output
    imag_m = -imag_m;
    for (unsigned int i=0; i<real_m.n_rows/2-1; i++){
      real_m(real_m.n_rows-i-1,0) = real_m(i+1,0);
      imag_m(imag_m.n_rows-i-1,0) = -imag_m(i+1,0);
    }  
    //imag_m.print("imag");
    //y_m = cx_mat(real_m, imag_m);
    delete[] x_db;
    delete[] ip;
    delete[] w;
    return cx_mat(real_m, imag_m);
  }
  //return y_m;
}

#endif
