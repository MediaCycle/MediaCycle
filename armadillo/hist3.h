/**
 * @brief hist3.h
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


#ifndef HIST3_H 
#define HIST3_H

#include "Armadillo-utils.h"

template<typename eT>
arma::umat hist3(const arma::Mat<eT>& A_m, int nbrBin0, int nbrBin1, float minE0, float maxE0, float minE1, float maxE1){
  if (A_m.n_cols != 2){
    std::cerr<<"Error using hist3 : A_m must be a matrix with two columns." << std::endl;
    exit(1);
  }
  arma::umat resultHist = arma::zeros<arma::umat>(nbrBin0,nbrBin1);   //final result
  arma::Col<eT> binWidth(2);                                 //one bin width for every column of the data file
  arma::Col<eT> binEdge0 = arma::Col<eT>(nbrBin0+1);           //edge values of the bins, one column vector for each data column
  arma::Col<eT> binEdge1 = arma::Col<eT>(nbrBin1+1);           //edge values of the bins, one column vector for each data column
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

#endif
