/**
 * @brief weightedStdDeviation.h
 * @author Christian Frisson
 * @date 11/04/2013
 * @copyright (c) 2013 – UMONS - Numediart
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


#ifndef WEIGHTEDSTDDEVIATION_H
#define WEIGHTEDSTDDEVIATION_H

#include "Armadillo-utils.h"

template<typename eT>
const arma::Row<eT> weightedStdDeviation(const arma::Mat<eT>& x_m, const arma::Col<eT>& weight_v){
	//	long L = x_m.n_rows;
#ifdef __clang__
	arma::Col<eT> weightN_v	= weight_v/(sum(weight_v));
#else //__clang__
    arma::Col<eT> weightN_v	= weight_v/arma::as_scalar(sum(weight_v));
#endif //__clang__
	arma::Mat<eT> m	= trans(x_m) * weightN_v;
	arma::Mat<eT> xc_m	= trans(x_m) - repmat(m, 1, x_m.n_rows);
	
	arma::Row<eT> m2	= square(xc_m) * weightN_v;
	arma::Row<eT> sm2	= sqrt( m2 );
	
	return sm2;
}

#endif
