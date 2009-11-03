/*
 *  Armadillo-utils.h
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



#include <armadillo>
#include "cluster.h"

using namespace arma;

mat conv2(mat A_m, mat B_m);
mat conv2(mat in, mat kernel, std::string);

cx_mat fft_helper(mat, int);
cx_mat fft(mat x_m, int n);

cx_mat fft2(mat );
cx_mat fft2(mat, int, int);
mat ifft2(cx_mat );
mat ifft2(cx_mat, int, int);
template<typename eT>
int size(const Mat<eT>& m, int dim);
template<typename eT>
const Mat<eT>& concat(int dim, const Mat<eT>& A_m, const Mat<eT>& B_m);
int nextpow2(int n);
mat rot90(mat A_m, int k);
mat xcorr2(mat A_m, mat B_m);
colvec diagxcorr2(mat A_m, mat B_m);
void kcluster(mat A_m, int nbClusters, colvec& clusterid_m, mat& center_m);
void princomp(mat x, mat &coeff, mat &score);
umat hist(mat A_m, int nbrBin, float minE=0, float maxE=0);
umat hist3(mat A_m, int nbrBin0, int nbrBin1, float minE0=0, float maxE0=0, float minE1=0, float maxE1=0);
ucolvec randperm(int);
