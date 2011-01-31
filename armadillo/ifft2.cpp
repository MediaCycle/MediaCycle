/**
 * @brief ifft2.cpp
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

#include "ifft2.h"
using namespace arma;
/////////////////////////////////////
// IFFT2
/////////////////////////////////////

mat ifft2(cx_mat x_m, int nrows, int ncols){
  // n : fft size  
  int n1 = x_m.n_rows;
  int n2 = x_m.n_cols;
  int n;
  double** x_db;
  mat y_m(nrows, ncols);
  
  n = std::max(nrows, ncols / 2);
  int* ip = new int[ 2 + (int)(sqrt(n)+.5)];
  ip[0] = 0;

  n = std::max(nrows, ncols)*3/2;
  double* w = new double[n];
  
  cx_mat x2_m(nrows, ncols);
  
  for (int i=0; i<nrows; i++){
    for (int j=0; j<ncols; j++){
      if (i >= n1 | j >= n2)
				x2_m(i,j) = 0;
      else
				x2_m(i,j) = x_m(i,j);
    }
  }
  
  mat real_m = real(x2_m);
  mat imag_m = -imag(x2_m);

  x_db = new double*[nrows];
  for (int i = 0; i < nrows; i++)
    x_db[i] = new double[ncols];
  
  /*  input data
      a[j1][2*j2] = R[j1][j2] = R[n1-j1][n2-j2], 
      a[j1][2*j2+1] = I[j1][j2] = -I[n1-j1][n2-j2], 
      0<j1<n1, 0<j2<n2/2, 
      a[0][2*j2] = R[0][j2] = R[0][n2-j2], 
      a[0][2*j2+1] = I[0][j2] = -I[0][n2-j2], 
      0<j2<n2/2, 
      a[j1][0] = R[j1][0] = R[n1-j1][0], 
      a[j1][1] = I[j1][0] = -I[n1-j1][0], 
      a[n1-j1][1] = R[j1][n2/2] = R[n1-j1][n2/2], 
      a[n1-j1][0] = -I[j1][n2/2] = I[n1-j1][n2/2], 
      0<j1<n1/2, 
      a[0][0] = R[0][0], 
      a[0][1] = R[0][n2/2], 
      a[n1/2][0] = R[n1/2][0], 
      a[n1/2][1] = R[n1/2][n2/2]
  */
  
  for (int j1=1; j1<nrows; j1++){
    for (int j2=1; j2<ncols/2; j2++){
      x_db[j1][2*j2] = real_m(j1,j2);
      x_db[j1][2*j2+1] = imag_m(j1,j2);
    }
  }
  for (int j2=1; j2<ncols/2; j2++){
    x_db[0][2*j2] = real_m(0,j2);
    x_db[0][2*j2+1] = imag_m(0,j2);
  }
  for (int j1=1; j1<nrows/2; j1++){
    x_db[j1][0] = real_m(j1,0);;
    x_db[j1][1] = imag_m(j1,0);
    x_db[nrows-j1][1] = real_m(j1,ncols/2);
    x_db[nrows-j1][0] = -imag_m(j1,ncols/2);
  }
  x_db[0][0] = real_m(0,0);
  
  if (ncols > 1)
    x_db[0][1] = real_m(0,ncols/2); 
  if (nrows > 1)
    x_db[nrows/2][0] = real_m(nrows/2,0); 
  if (nrows > 1 & ncols > 1)
    x_db[nrows/2][1] = real_m(nrows/2,ncols/2);

  


  rdft2d(nrows, ncols, -1, x_db, NULL, ip, w);
  
  for (int i=0; i<nrows; i++)
    for (int j=0; j<ncols; j++)
      y_m(i,j) = 2*x_db[i][j]/(nrows*ncols);

  delete(ip);
  delete(w);
  for (int i = 0; i < nrows; i++)
    delete(x_db[i]);
  delete(x_db);
  return y_m;
}


mat ifft2(cx_mat x_m){
  return ifft2(x_m, x_m.n_rows, x_m.n_cols);
}
