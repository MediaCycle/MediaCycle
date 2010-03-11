/**
 * @brief fft2.cpp
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

#include "fft2.h"
#include "fftsg2d.h"

cx_mat fft2(mat x_m, int nrows, int ncols){

  int n1 = x_m.n_rows;
  int n2 = x_m.n_cols;
  int n;
  double** x_db;
  cx_mat y_m(nrows, ncols);

  if (nrows == 1){
    if (ncols == 1){
      y_m = x_m(0,0);
      return y_m;
    }
    else{
      y_m = fft(x_m.row(0),ncols);
      return y_m;
    }
  }
  else{
    if (ncols == 1){
      y_m = trans(fft(x_m.col(0),nrows));
      y_m.print();
      return y_m;
    }
  }
  
  n = std::max(nrows, ncols / 2);
  int* ip = new int[ 2 + (int)(sqrt(n)+.5)];
  ip[0] = 0;

  n = std::max(nrows, ncols)*3/2;
  double* w = new double[n];
  
  mat x2_m(nrows, ncols);
  
  for (int i=0; i<nrows; i++){
    for (int j=0; j<ncols; j++){
      if (i >= n1 | j >= n2)
	x2_m(i,j) = 0;
      else
	x2_m(i,j) = x_m(i,j);
    }
  }
  
  mat real_m(nrows,ncols);
  mat imag_m(nrows,ncols);

  x_db = new double*[nrows];
  for (int i = 0; i < nrows; i++)
    x_db[i] = new double[ncols];

  for (int i=0; i<nrows; i++){
    for (int j=0; j< ncols; j++){
      x_db[i][j] = x2_m(i,j);
    }
  }
  clock_t start = clock();
  rdft2d(nrows, ncols, 1, x_db, NULL, ip, w);
  std::cout << "Time elapsed: " << ((double)clock() - start) / CLOCKS_PER_SEC << std::endl;
  /*   <case1>
       output data
       a[k1][2*k2] = R[k1][k2] = R[nrows-k1][ncols-k2], 
       a[k1][2*k2+1] = I[k1][k2] = -I[nrows-k1][ncols-k2], 
       0<k1<nrows, 0<k2<ncols/2, 
       a[0][2*k2] = R[0][k2] = R[0][ncols-k2], 
       a[0][2*k2+1] = I[0][k2] = -I[0][ncols-k2], 
       0<k2<ncols/2, 
       a[k1][0] = R[k1][0] = R[nrows-k1][0], 
       a[k1][1] = I[k1][0] = -I[nrows-k1][0], 
       a[nrows-k1][1] = R[k1][ncols/2] = R[nrows-k1][ncols/2], 
       a[nrows-k1][0] = -I[k1][ncols/2] = I[nrows-k1][ncols/2], 
       0<k1<nrows/2, 
       a[0][0] = R[0][0], 
       a[0][1] = R[0][ncols/2], 
       a[nrows/2][0] = R[nrows/2][0], 
       a[nrows/2][1] = R[nrows/2][ncols/2]
  */
  
  for (int k1=1; k1<nrows; k1++){
    for (int k2=1; k2<ncols/2; k2++){
      real_m(k1,k2) = x_db[k1][2*k2];
      real_m(nrows-k1,ncols-k2) = x_db[k1][2*k2];
      imag_m(k1,k2) = x_db[k1][2*k2+1];
      imag_m(nrows-k1,ncols-k2) = -x_db[k1][2*k2+1];
    }
  }
  for (int k2=1; k2<ncols/2; k2++){
    real_m(0,k2) = x_db[0][2*k2];
    real_m(0,ncols-k2) = x_db[0][2*k2];
    imag_m(0,k2) = x_db[0][2*k2+1];
    imag_m(0,ncols-k2) = -x_db[0][2*k2+1];
  }  
  
  for (int k1=1; k1<nrows/2; k1++){
    real_m(k1,0) = x_db[k1][0]; 
    real_m(nrows-k1,0) = x_db[k1][0]; 
    imag_m(k1,0) = x_db[k1][1];
    imag_m(nrows-k1,0) = -x_db[k1][1];
    real_m(k1,ncols/2) = x_db[nrows-k1][1];
    real_m(nrows-k1,ncols/2) = x_db[nrows-k1][1];
    imag_m(k1,ncols/2) = -x_db[nrows-k1][0];
    imag_m(nrows-k1,ncols/2) = x_db[nrows-k1][0]; 
  }

  real_m(0,0) = x_db[0][0]; 
  imag_m(0,0) = 0; 

  real_m(0,ncols/2) = x_db[0][1];
  imag_m(0,ncols/2) = 0;
  real_m(nrows/2,0) = x_db[nrows/2][0];
  real_m(nrows/2,ncols/2) = x_db[nrows/2][1];   
  imag_m(nrows/2,0) = 0;
  imag_m(nrows/2,ncols/2) = 0;
  
  // To have matlab like output
  imag_m = -imag_m;
  
  y_m = cx_mat(real_m, imag_m);
  delete(ip);
  delete(w);
  for (int i = 0; i < nrows; i++)
    delete(x_db[i]);
  delete(x_db);

  return y_m;
}

cx_mat fft2(mat x_m){
  return fft2(x_m, x_m.n_rows, x_m.n_cols);
}
