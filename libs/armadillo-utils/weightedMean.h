/**
 * @brief weightedMean.h
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


#ifndef WEIGHTEDMEAN_H
#define WEIGHTEDMEAN_H

#include "Armadillo-utils.h"

template<typename eT>
const arma::Row<eT> weightedMean(const arma::Col<eT>& in_v, const arma::Col<eT>& weight_v){
#ifdef __clang__
    float sumWeight = (sum(weight_v));
#else   //__clang__
    float sumWeight = arma::as_scalar(sum(weight_v));
#endif  //__clang__
	arma::Col<eT> weightN_v = weight_v / sumWeight;
        //arma::Row<eT> tmp_m = trans(weightN_v) * in_v;
        //return tmp_m;
        return trans(weightN_v) * in_v;
}

#endif
