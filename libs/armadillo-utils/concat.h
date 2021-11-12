/**
 * @brief concat.h
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


#ifndef CONCAT_H 
#define CONCAT_H

#include "Armadillo-utils.h"

template<typename eT>
const arma::Mat<eT> concat(int dim, const arma::Mat<eT>& A_m, const arma::Mat<eT>& B_m){
	arma::Mat<eT> C_m;
  if (dim == 1){
		if (A_m.n_cols != B_m.n_cols){
			std::cerr << "Matrix dimensions must agree" << std::endl;
			exit(1);
		}
    C_m.set_size(A_m.n_rows + B_m.n_rows, A_m.n_cols);
			for (int i = 0; i < B_m.n_rows; i++) 
				for (int j = 0; j < B_m.n_cols; j++)
					C_m(i+A_m.n_rows,j) = B_m(i,j);
  }
  else{
    if (dim == 2){
			if (A_m.n_rows != B_m.n_rows){
				std::cerr << "Dimensions must agree" << std::endl;	
				exit(1);
			}
			C_m.set_size(A_m.n_rows, A_m.n_cols + B_m.n_cols);
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

#endif
