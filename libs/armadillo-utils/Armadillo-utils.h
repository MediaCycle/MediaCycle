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


#ifndef ARMADILLO_UTILS_H 
#define ARMADILLO_UTILS_H

#include <armadillo>
#include "cluster.h"

#include "hanning.h"
#include "hamming.h"

#include "max_index.h"
#include "min_index.h"
#include "nextpow2.h"
#include "rot90.h"
#include "size.h"
#include "findpeaks.h"

// diff has been introduced in armadillo v7.8
// https://gitlab.com/conradsnicta/armadillo-code/commits/9.700.x/include/armadillo_bits/fn_diff.hpp
//#include "diff.h"

#include "cart2pol.h"
#include "concat.h"
#include "ismember.h"

#include "weightedMean.h"
#include "weightedStdDeviation.h"

#include "cluster.h"
#include "kcluster.h"

#include "blackman.h"
#include "tukeywin.h"
#include "fft-helper.h"
#include "fft.h"
#include "fft2.h"
#include "ifft2.h"
#include "conv2.h"
#include "xcorr2.h"
#include "diagxcorr2.h"
#include "dct.h"

#include "melfilters.h"

#include "hist.h"
#include "hist3.h"
#include "zscore.h"

#include "linefit.h"

#include "modulation.h"
#include "paretofront.h"
#include "paretorank.h"
#include "pdist.h"
#include "euclideanDistance.h"
#include "randperm.h"


#endif
/* mat conv2(mat A_m, mat B_m); */
/* mat conv2(mat in, mat kernel, std::string); */

/* cx_mat fft_helper(mat, int); */
/* cx_mat fft(mat x_m, int n); */

/* cx_mat fft2(mat ); */
/* cx_mat fft2(mat, int, int); */
/* mat ifft2(cx_mat ); */
/* mat ifft2(cx_mat, int, int); */
/* template<typename eT> */
/* int size(const Mat<eT>& m, int dim); */
/* template<typename eT> */
/* const Mat<eT>& concat(int dim, const Mat<eT>& A_m, const Mat<eT>& B_m); */
/* int nextpow2(int n); */
/* mat rot90(mat A_m, int k); */
/* mat xcorr2(mat A_m, mat B_m); */
/* colvec diagxcorr2(mat A_m, mat B_m); */
/* void kcluster(mat A_m, int nbClusters, colvec& clusterid_m, mat& center_m); */
/* void princomp(mat x, mat &coeff, mat &score); */
/* umat hist(mat A_m, int nbrBin, float minE=0, float maxE=0); */
/* umat hist3(mat A_m, int nbrBin0, int nbrBin1, float minE0=0, float maxE0=0, float minE1=0, float maxE1=0); */
/* ucolvec randperm(int); */
/* colvec find(umat A_v); */
/* mat diff(mat A_m, int n=1, int dim=1); */
/* ucolvec paretofront(mat M); */
/* ucolvec paretorank(mat X, int maxRank, int minNbItems); */
/* uvec ismember(colvec A_v, colvec X_v); */
/* mat pdist(mat data); */
/* mat zscore(mat x, int dim=0); */
/* int cart2pol(colvec x_v, colvec y_v, colvec &th_v, colvec &r_v); */
/* colvec blackman(int length); */
/* rowvec weightedMean(colvec in_v, colvec weight_v); */
/* rowvec weightedStdDeviation(mat in_m, colvec weight_v); */
/* icolvec findpeaks(colvec Data, long Pd); */
/* rowvec linefit(colvec x_v, colvec y_v); */
/* void modulation(colvec x_v, int fs, mat &modFr_m, mat &modAmp_m); */


/* template<typename eT> */
/* unsigned long max_index(const Col<eT>& A_v){ */
/* 	ucolvec Asort_v; */
/* 	Asort_v = sort_index(A_v, 1); */
/* 	return Asort_v(0); */
/* } */

/* template<typename eT> */
/* unsigned long max_index(const Row<eT>& A_v){ */
/* 	urowvec Asort_v; */
/* 	Asort_v = sort_index(A_v, 1); */
/* 	return Asort_v(0); */
/* } */

/* template<typename eT> */
/* unsigned long min_index(const Col<eT>& A_v){ */
/* 	ucolvec Asort_v; */
/* 	Asort_v = sort_index(A_v, 0); */
/* 	return Asort_v(0); */
/* } */

/* template<typename eT> */
/* unsigned long min_index(const Row<eT>& A_v){ */
/* 	urowvec Asort_v; */
/* 	Asort_v = sort_index(A_v, 0); */
/* 	return Asort_v(0); */
/* } */
