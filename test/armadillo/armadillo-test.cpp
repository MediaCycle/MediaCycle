/**
 * @brief armadillo-test.cpp
 * @author Christian Frisson
 * @date 08/12/2010
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

#include <string>
#include <armadillo>
#include "Armadillo-utils.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "ACMediaTimedFeature.h"
#include "MediaCycle.h"
using namespace arma;


int main(){

	int a;
	int L = 10000;
	int D = 1;
	int t0 = clock();
	std::cout << t0  << std::endl;
	int seed = t0 - (int)(t0/1000)*1000;
	std::cout << seed + 3 << std::endl;
	srand ( seed * 3 );
	for (int i =0; i<3; i++){
		//colvec q = randu<rowvec>(10);
		std::cout << trans(randperm(10)) << std::endl;
	}
	//	std::cout << ACRandom() << std::endl;
	std::cout << rand() << std::endl;
// 	// 	float* mem;
// 	// 	mem = new float* [D];
// 	// 	for(int i=0; i<n; i++)
// 	// 		mem[i] = new double [L];
// 	float* mem = new float[L];

// 	for (int d=0; d<D; d++){
// 		for (int i=0; i<L; i++){
// 			mem[d] = d+i*.1;
// 		}
// 	}
	
// 	t0=clock();
// 	fmat test4(L, D);
// 	for (int d=0; d<D; d++){
// 		for (int i=0; i<L; i++){
// 			test4(d,i) = mem[i];
// 		}
// 	}
// 	std::cout << "Methode 2 : " << clock()-t0 << std::endl;	

// 	t0=clock();
// 	fmat test3(mem, L, D, true);
// 	std::cout << "Methode 1 : " << clock()-t0 << std::endl;
	
// 	t0=clock();
// 	fmat test(mem, L, D, true);
// 	std::cout << "Methode 1 : " << clock()-t0 << std::endl;
	
// 	t0=clock();
// 	fmat test2(L, D);
// 	for (int d=0; d<D; d++){
// 		for (int i=0; i<L; i++){
// 			test2(d,i) = mem[i];
// 		}
// 	}
// 	std::cout << "Methode 2 : " << clock()-t0 << std::endl;	


// 	test.save("test.txt", arma_ascii);
// 	test.save("test3.txt", arma_ascii);
	return 0;
}

