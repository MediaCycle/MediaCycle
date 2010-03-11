/**
 * @brief zscore.h
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

#ifndef ZSCORE_H
#define ZSCORE_H

template<typename eT>
const Mat<eT> zscore(const Mat<eT>& x, int dim=0){
	Mat<eT> z(x.n_rows, x.n_cols);
	if (x.n_elem == 0)
		return z;
	Mat<eT> mu = mean(x,dim);
	Mat<eT> sigma = stddev(x, 0, dim);
	for (int i=0; i<sigma.n_rows; i++)
		for (int j=0; j<sigma.n_cols; j++)
			if (sigma(i,j)==0)
				sigma(i,j) = 1;
	if (dim==0){
		z = x - repmat(mu, x.n_rows, 1);
		z = z / repmat(sigma, x.n_rows, 1);
	}
	else
		if (dim == 1){
			z = x - repmat(mu, 1, x.n_cols);
			z = z / repmat(sigma, 1, x.n_cols);
		}
	return z;
}

#endif
