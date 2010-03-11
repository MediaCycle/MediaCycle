/**
 * @brief cart2pol.h
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

#ifndef CART2POL_H 
#define CART2POL_H

template<typename eT>
int cart2pol(const Col<eT>& x_v, const Col<eT>& y_v, Col<eT> &th_v, Col<eT> &r_v){
	// Angle between 0 and 2*pi
	if (x_v.n_elem != y_v.n_elem){
		std::cerr << "Error : x and y must be the same length" << std::endl;
		return 0;
	}

	th_v.set_size(x_v.n_elem);
	r_v.set_size(x_v.n_elem);
		
	for (int i = 0; i < x_v.n_elem; i++){
		if (x_v(i)==0)
			if (y_v(i)==0)
				th_v(i) = 0;
			else if (y_v(i)>0)
				th_v(i) = math::pi()/2;
			else // if (y_v(i)<0)
				th_v(i) = math::pi()/2 *3;
		else
			if (x_v(i) < 0)
				th_v(i) = atan(y_v(i)/x_v(i)) + math::pi();
			else // if (x_v(i) > 0)
				if (y_v(i) >= 0)
					th_v(i) = atan(y_v(i)/x_v(i));
				else //if (y_v(i) < 0)
					th_v(i) = atan(y_v(i)/x_v(i)) + 2* math::pi();
	}
	r_v = sqrt(square(x_v)+square(y_v));
	
	return 1;
}

#endif
