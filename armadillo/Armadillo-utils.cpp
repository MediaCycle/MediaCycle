/*
 *  Armadillo-utils.cpp
 *  MediaCycle
 *
 *  @author Damien Tardieu
 *  @date 28/08/09
 *  @copyright (c) 2009 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 */

#include "Armadillo-utils.h"
//#include "fftsg_h.c"
#include "fftsg.c"
#include "fftsg2d.c"
#include <iostream>
#include <algorithm>
using namespace std;

mat conv2(mat A_m, mat B_m){
  return conv2(A_m, B_m, "full");
}

mat conv2(mat A_m, mat B_m, string mode){

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


cx_mat fft(mat x_m, int n){
  std::cout << "Calling fft for mat" << std::endl;
  cx_mat y_m;
  if (x_m.n_rows > 1){
    y_m.set_size(n, x_m.n_cols);
    for (int i = 0; i < x_m.n_cols; i++)
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

// fft 1D for column vector
cx_mat fft_helper(mat x_m, int n){
  std::cout << "Calling fft helper" << std::endl;
  // n : fft size
  double* x_db = NULL;
  cx_mat y_m;
  mat real_m(n,1);
  mat imag_m(n,1);
  int xSize, sigSize;
  
  if (n==1)
    y_m = ones<cx_mat>(1,1)*x_m(0,0);
  else{
    int* ip = new int[ 2 + (int)(sqrt(n)+.5)];
    double* w = new double[n/2];

    xSize = x_m.n_rows;

    sigSize = min(n, xSize);

    x_db = new double(n);
 
    for (int i=0; i<sigSize; i++){
      x_db[i] = x_m(i,0);
    }

    for (int i=sigSize; i < n; i++){
      x_db[i] = 0;
    }
    rdft(n, 1, x_db, ip, w);
  
    // fill arma matrices
    for (int i=0; i<n; i=i+2){
      real_m((i)/2,0) = x_db[i];
      imag_m((i)/2,0) = x_db[i+1];
    }
    imag_m(0,0) = 0;
    real_m(n/2,0) = x_db[1];
    // rearrange data to have matlab like output
    imag_m = -imag_m;
    for (int i=0; i<real_m.n_rows/2-1; i++){
      real_m(real_m.n_rows-i-1,0) = real_m(i+1,0);
      imag_m(imag_m.n_rows-i-1,0) = -imag_m(i+1,0);
    }  
    //imag_m.print("imag");
    y_m = cx_mat(real_m, imag_m);
    delete(x_db);
    delete(ip);
    delete(w);
  }
  return y_m;
}

cx_mat fft2(mat x_m){
  return fft2(x_m, x_m.n_rows, x_m.n_cols);
}

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
  
  n = max(nrows, ncols / 2);
  int* ip = new int[ 2 + (int)(sqrt(n)+.5)];
  ip[0] = 0;

  n = max(nrows, ncols)*3/2;
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

/////////////////////////////////////
// IFFT2
/////////////////////////////////////
mat ifft2(cx_mat x_m){
  return ifft2(x_m, x_m.n_rows, x_m.n_cols);
}

mat ifft2(cx_mat x_m, int nrows, int ncols){
  // n : fft size  
  int n1 = x_m.n_rows;
  int n2 = x_m.n_cols;
  int n;
  double** x_db;
  mat y_m(nrows, ncols);
  
  n = max(nrows, ncols / 2);
  int* ip = new int[ 2 + (int)(sqrt(n)+.5)];
  ip[0] = 0;

  n = max(nrows, ncols)*3/2;
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

  return y_m;
}


template<typename eT>
int size(const Mat<eT>& m, int dim){
  if (dim == 1)
    return m.n_rows;
  else
    if (dim == 2)
      return m.n_cols;
    else {
      std::cout << "Wrong dimension" << std::endl;
      exit(1);
    } 
}

template<typename eT>
const Mat<eT>& concat(int dim, const Mat<eT>& A_m, const Mat<eT>& B_m){
  const Mat<eT>& C_m = mat(A_m);
  if (dim == 1){
    for (int i = 0; i < B_m.n_rows; i++) 
      for (int j = 0; j < B_m.n_cols; j++)
	C_m(i+A_m.n_rows,j) = B_m(i,j);
  }
  else{
    if (dim == 2){
      for (int i = 0; i < B_m.n_rows; i++) 
	for (int j = 0; j < B_m.n_cols; j++)
	  C_m(i,j+A_m.n_cols) = B_m(i,j);
    }
    else{
      std::cout<<"Wrong dimension" << std::endl;
      exit(1);
    }      
  }  
  return C_m;
}


int nextpow2(int n){
  double significand;
  int exponent;
  significand = frexp (n , &exponent);
  // Check if n is an exact power of 2.
  if (significand==.5)
    exponent = exponent-1;
  return exponent;
}


// template<typename eT>
// const Mat<eT>& rot90(const Mat<eT>& A_m, int k){
mat rot90(mat A_m, int k){ 
  mat B_m;
  k = fmod((float)k,(float)4);
  if (k < 0)
    k = k + 4;
  if (k == 1){
    B_m.set_size(A_m.n_cols, A_m.n_rows);
    A_m = trans(A_m);
    for (int i=0; i < A_m.n_rows; i++)
      for (int j=0; j < A_m.n_cols; j++)
	B_m(i,j) = A_m(A_m.n_rows-i-1,j);
  }
  else if (k == 2){
    B_m.set_size(A_m.n_rows, A_m.n_cols);
    for (int i=0; i < A_m.n_rows; i++)
      for (int j=0; j < A_m.n_cols; j++)
	B_m(i,j) = A_m(A_m.n_rows-i-1,A_m.n_cols-j-1);
  }
  else if (k == 3){
    B_m.set_size(A_m.n_rows, A_m.n_cols);
    for (int i=0; i < A_m.n_rows; i++)
      for (int j=0; j < A_m.n_cols; j++)
	B_m(i,j) = A_m(A_m.n_rows-i-1,j);
    B_m = trans(B_m);
  }
  else
    B_m = A_m;
  return B_m;
}


mat xcorr2(mat A_m, mat B_m){
  mat C_m(A_m.n_rows+B_m.n_rows-1, A_m.n_cols+B_m.n_cols-1); 
  return conv2(A_m, rot90(B_m,2));
}
  
colvec diagxcorr2(mat A_m, mat B_m){
  if (A_m.n_rows != A_m.n_cols | B_m.n_rows != B_m.n_cols){
    std::cout << "Error : Matrices must be square" << endl;
    exit(1);
  }
  mat Tmp_m(1,1);
  mat tmp2_m,tmp3_m;
  colvec C_v(A_m.n_rows+B_m.n_rows-1); 
  for (int i=0; i < C_v.n_rows; i++){
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
