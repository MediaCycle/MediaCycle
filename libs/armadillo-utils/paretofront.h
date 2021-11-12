/**
 * @brief paretofront.h
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


#ifndef PARETOFRONT_H 
#define PARETOFRONT_H

#include "Armadillo-utils.h"

template<typename eT>
arma::ucolvec paretofront(const arma::Mat<eT>& M){
	unsigned int t,s,i,j,j1,j2;
	bool coldominatedflag;
	int col = M.n_cols;
	int row = M.n_rows;
	arma::ucolvec front;
	arma::ucolvec checklist(row);
	front.zeros(row,1);
	for(t = 0; t<row; t++) checklist[t] = 1;
	for(s = 0; s<row; s++) {
		t=s;
		if (!checklist[t]) continue;
		checklist[t]=0;
		coldominatedflag=1;
		for(i=t+1;i<row;i++) {
			if (!checklist[i]) continue;
			checklist[i]=0;
			for (j=0,j1=i,j2=t;j<col;j++,j1+=row,j2+=row) {
				if (M(j1) < M(j2)) {
					checklist[i]=1;
					break;
				}
			}
			if (!checklist[i]) continue;
			coldominatedflag=0;
			for (j=0,j1=i,j2=t;j<col;j++,j1+=row,j2+=row) {
				if (M(j1) > M(j2)) {
					coldominatedflag=1;
					break;
				}
			}
			if (!coldominatedflag) {     //swap active index continue checking
				front(t)=0;
				checklist[i]=0;
				coldominatedflag=1;
				t=i;
			}
		}
		front(t)=coldominatedflag;
		if (t>s) {
			for (i=s+1; i<t; i++) {
				if (!checklist[i]) continue;
				checklist[i]=0;
				for (j=0,j1=i,j2=t;j<col;j++,j1+=row,j2+=row) {
					if (M(j1) < M(j2)) {
						checklist[i]=1;
						break;
					}
				}
			}
		}
	}
	return front;
}

#endif
