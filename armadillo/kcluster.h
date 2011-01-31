/**
 * @brief kcluster.h
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


#ifndef KCLUSTER_H
#define KCLUSTER_H

#include "Armadillo-utils.h"

template<typename eT>
void kcluster(const arma::Mat<eT>& A_m, int nbClusters, arma::Col<eT>& clusterid_m, arma::Mat<eT>& center_m){
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

#endif
