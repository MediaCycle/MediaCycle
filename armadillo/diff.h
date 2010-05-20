/**
 * @brief diff.h
 * @author Damien Tardieu
 * @date 05/05/2010
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

#ifndef DIFF_H
#define DIFF_H

template<typename eT>
const Mat<eT> diff(const Mat<eT>& A_m, int n=1, int dim=0){
  Mat<eT> R_m;
	if (dim == 0)
		R_m = A_m.rows(1,A_m.n_rows-1) - A_m.rows(0,A_m.n_rows-2);
	else
    if (dim == 1)
			R_m = A_m.cols(1,A_m.n_cols-1) - A_m.cols(0,A_m.n_cols-2);
    else {
      std::cout << "Wrong dimension" << std::endl;
      exit(1);
    } 
	return R_m;
}

#endif
