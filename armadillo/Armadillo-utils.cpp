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
//using namespace std;

mat conv2(mat A_m, mat B_m){
  return conv2(A_m, B_m, "full");
}

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


cx_mat fft(mat x_m, int n){
  std::cout << "Calling fft for mat" << std::endl;
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

    sigSize = std::min(n, xSize);

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
    for (unsigned int i=0; i<real_m.n_rows/2-1; i++){
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

void kcluster(mat A_m, int nbClusters, colvec& clusterid_m, mat& center_m){
  const int nbRows = A_m.n_rows;
  const int nbCols = A_m.n_cols;
  double **data;
  int** mask;
  double* weight;
  int* clusterid = new int[nbRows];
  double error;
  int ifound;
  double** cdata;
  int** cmask;

  clusterid_m.set_size(nbRows);
  center_m.set_size(nbClusters, nbCols); 

  data = new double*[nbRows]; 
  for(int i=0;i < nbRows; i++) { 
    data[i] = new double[nbCols]; 
  } 
  mask = new int*[nbRows]; 
  for(int i=0;i < nbRows; i++) { 
    mask[i] = new int[nbCols]; 
  }   
  weight = new double[nbCols];
  for(int i=0;i < nbCols; i++) { 
    weight[i] = 1; 
  } 
  

  for (unsigned int Irow=0; Irow<A_m.n_rows; Irow++){
    for (unsigned int Icol=0; Icol<A_m.n_cols; Icol++){
      data[Irow][Icol] = A_m(Irow,Icol);
      mask[Irow][Icol] = 1;
    }
  }
  kcluster (nbClusters, nbRows, nbCols, data, mask, weight, 0, 1, 'a', 'e',  clusterid, &error, &ifound);
  // DT:already done in kcluster but freed. Not optimal but simpler

  cdata = (double**)malloc(nbClusters*sizeof(double*));
  cmask = (int**)malloc(nbClusters*sizeof(int*));
  for (int i = 0; i < nbClusters; i++){ 
    cdata[i] = (double*)malloc(nbCols*sizeof(double));
    cmask[i] = (int*)malloc(nbCols*sizeof(int));
  }

  getclustercentroids(nbClusters, nbRows, nbCols, data, mask, clusterid, cdata, cmask,  0, 'a');

  for (unsigned int Irow=0; Irow<A_m.n_rows; Irow++){
    clusterid_m(Irow) = clusterid[Irow];
  }
  for (int Iclus=0; Iclus < nbClusters; Iclus++){
    for (int Icol=0; Icol < nbCols; Icol++)
      center_m(Iclus, Icol) = cdata[Iclus][Icol];
  }

  for(int i=0;i < nbRows; i++) { 
    delete(data[i]);
  } 
  free(data);
  for(int i=0;i < nbRows; i++) { 
    delete(mask[i]);
  }   
  delete(mask);
  delete(weight);
  
  for (int i = 0; i < nbClusters; i++){ 
    delete(cdata[i]);
    delete(cmask[i]);
  }
  delete(cdata);
  delete(cmask);
}


void princomp(mat x, mat &coeff, mat &score){
  // Same as the "econ" mode in matlab

  int n = x.n_rows;
  int p = x.n_cols;

  // Center X by subtracting off column means
  mat x0 = x-repmat(mean(x,0),n,1);
  int r = std::min(n-1,p); // max possible rank of X0

  // The principal component coefficients are the eigenvectors of
  // S = X0'*X0./(n-1), but computed using SVD.
  mat U;
  colvec sigma;
  svd(U, sigma, coeff, x0); 
  //sigma = svd(x0); 
  // Project X0 onto the principal component axes to get the scores.
//   colvec sigmaDiag;
//   if (n == 1) // sigma might have only 1 row
//     sigmaDiag(0) = sigma(0,0);
//   else
//     sigmaDiag = sigma.diag();
  U = U.cols(0,p-1);
  score = U % repmat(trans(sigma), n, 1); // == x0*coeff
  sigma = sigma / sqrt(n-1);
  
  // When X has at least as many variables as observations, eigenvalues
  // n:p of S are exactly zero.
  if (n <= p){
    sigma.submat(n-1,0,p-1,0).zeros();
    score.cols(n-1,p-1).zeros();
  }
  // The variances of the pc's are the eigenvalues of S = X0'*X0./(n-1).
  //  latent = square(sigmaDiag);  
}


umat hist(mat A_m, int nbrBin, float minE, float maxE){
  umat resultHist = zeros<umat>(nbrBin,A_m.n_cols);   //final result
  vec binWidth(A_m.n_cols);                                 //one bin width for every column of the data file
  fmat binEdge = fmat(nbrBin+1,A_m.n_cols);           //edge values of the bins, one column vector for each data column
  double minA, maxA;
  if (minE == 0 && maxE == 0)   //user did not specify min and max values of the histogram (commom case)
    {
      for (int i=0;i<A_m.n_cols;i++)      //for every column of value_m
        {
	  minA = min(min(A_m));
	  maxA = max(max(A_m));
	  binWidth(i) = (maxA - minA)/nbrBin;
	  //cout<<"binWidth : "<<binWidth.back()<<endl;   //ok
	  for (int j=0;j<binEdge.n_rows-1;j++)
            {
	      binEdge(j,i) = minA + j*binWidth(i); //compute bin edges
            }
	  binEdge(binEdge.n_rows-1,i) = maxA;
        }
      //extremaM.print("ExtremaM : ");    //ok
    }
  else    //user specified min and max values of the histogram
    {
      fcolvec binEdgeV = fcolvec(nbrBin+1);        
      float binWidthS = (maxE - minE)/nbrBin;

      for (int j=0;j<binEdgeV.n_rows-1;j++)
	binEdgeV(j) = minE + j*binWidthS;
      binEdgeV(binEdgeV.n_rows-1) = maxE;

      for (int i=0;i<A_m.n_cols;i++)
	binEdge.col(i) = binEdgeV;  //bin edge values are the same for every column of the data file because we use the same min and max values
    }
  //now we have binEdge for both cases (min and max specified or not)
  //binEdge.print("binEdge : ");  //ok

  for (int i=0;i<A_m.n_cols;i++)  //compute the histogram
    {
      for (int j=0;j<A_m.n_rows;j++)   //for each value in the data file
        {
	  for (int k=0;k<nbrBin;k++)
            {
	      if (A_m(j,i) >= binEdge(k,i) && A_m(j,i) < binEdge(k+1,i))
                {
		  resultHist(k,i) += 1;   //value included between the two bin edge values
		  break;
                }
            }
	  if (A_m(j,i)== binEdge(nbrBin,i)) //maximum value exception
	    resultHist(nbrBin-1,i) += 1;
        }
    }
  resultHist.print("Histo : ");       //ok, checked with Matlab
  return resultHist;
}


umat hist3(mat A_m, int nbrBin0, int nbrBin1, float minE0, float maxE0, float minE1, float maxE1){
  if (A_m.n_cols != 2){
    std::cerr<<"Error using hist3 : A_m must be a matrix with two columns." << std::endl;
    exit(1);
  }
  umat resultHist = zeros<umat>(nbrBin0,nbrBin1);   //final result
  vec binWidth(2);                                 //one bin width for every column of the data file
  vec binEdge0 = vec(nbrBin0+1);           //edge values of the bins, one column vector for each data column
  vec binEdge1 = vec(nbrBin1+1);           //edge values of the bins, one column vector for each data column
  double minA, maxA;

  if (minE0 == 0 && maxE0 == 0){   //user did not specify min and max values of the histogram (commom case)    
    // For the first column
    minA = min(A_m.col(0));
    maxA = max(A_m.col(0));
  }
  else{
    minA = minE0;
    maxA = maxE0;
  }

  double binWidth0 = (maxA - minA)/nbrBin0;
  //cout<<"binWidth : "<<binWidth.back()<<endl;   //ok
  for (int j=0;j<binEdge0.n_rows-1;j++){
    binEdge0(j) = minA + j*binWidth0; //compute bin edges
  }
  binEdge0(binEdge0.n_rows-1) = maxA;

  if (minE1 == 0 && maxE1 == 0){   //user did not specify min and max values of the histogram (commom case)
    // For the second column
    minA = min(A_m.col(1));
    maxA = max(A_m.col(1));
  }
  else{
    minA = minE1;
    maxA = maxE1;
  }
  
  double binWidth1 = (maxA - minA)/nbrBin0;
  //cout<<"binWidth : "<<binWidth.back()<<endl;   //ok
  for (int j=0;j<binEdge1.n_rows-1;j++){
    binEdge1(j) = minA + j*binWidth1; //compute bin edges
  }
  binEdge1(binEdge1.n_rows-1) = maxA;
  
  for (int iRows=0;iRows<A_m.n_rows;iRows++){   //for each value in the data file
    for (int iBin0=0;iBin0<nbrBin0;iBin0++){
      if ((A_m(iRows,0) >= binEdge0(iBin0) && A_m(iRows,0) < binEdge0(iBin0+1)) | (iBin0==nbrBin0-1 & A_m(iRows,0) == binEdge0(nbrBin0))){
	for (int iBin1=0;iBin1<nbrBin1;iBin1++){
	  if ( (A_m(iRows,1) >= binEdge1(iBin1) && A_m(iRows,1) < binEdge1(iBin1+1)) | (iBin1==nbrBin1-1 & A_m(iRows,1) == binEdge1(nbrBin1))){
	    resultHist(iBin0,iBin1) += 1;   //value included between the two bin edge values
	    iBin0 = nbrBin0; // break equivalent;
	    iBin1 = nbrBin1; // break equivalent
	  }
	}
      }
    }
  }
  resultHist.print("Histo : ");       //ok, checked with Matlab
  return resultHist;
}

  
ucolvec randperm(int n){
  colvec  q_v       = rand<colvec>(n);
  ucolvec perm_v = sort_index(q_v);
  return perm_v;
}

ucolvec find(umat A_v){
  ucolvec pos_v;
  double tmp;
  if (A_v.n_cols > 1){
    std::cerr << "Error : Find only works on column vectors" << std::endl;
    exit(-1);
  }
  tmp = conv_to<double>::from(sum(A_v==1));
  pos_v.set_size(tmp);
  int index = 0;
  for (int i=0; i<A_v.n_rows; i++){
    if (A_v(i,0) != 0){
      pos_v(index) = i;
      index++;
    }
  }
  return pos_v;
}
