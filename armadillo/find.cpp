/**
 * @brief find.cpp
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

colvec find(umat A_v){
  colvec pos_v;
  double tmp;
  if (A_v.n_cols > 1){
    std::cerr << "Error : Find only works on column vectors" << std::endl;
    exit(-1);
  }
  tmp = conv_to<double>::from(sum(A_v==1));
  pos_v.set_size(tmp);
  int index = 0;
  for (int i=0; i<A_v.n_rows; i++){
    if (A_v(i,0) != 0){
      pos_v(index) = i;
      index++;
    }
  }
  return pos_v;
}

