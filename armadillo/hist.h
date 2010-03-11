/**
 * @brief hist.h
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

#include "Armadillo-utils.h"

#ifndef HIST_H 
#define HIST_H

template<typename eT>
umat hist(const Mat<eT>& A_m, int nbrBin, float minE, float maxE){
  umat resultHist = zeros<umat>(nbrBin,A_m.n_cols);   //final result
  Col<eT> binWidth(A_m.n_cols);                                 //one bin width for every column of the data file
  Mat<eT> binEdge(nbrBin+1,A_m.n_cols);           //edge values of the bins, one column vector for each data column
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

#endif

