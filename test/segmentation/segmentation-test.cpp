/**
 * @brief segmentation-test.cpp
 * @author Xavier Siebert
 * @date 09/12/2010
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
#include "MediaCycle.h"

#include "ACBicSegmentationPlugin.h"
#include "gnuplot_i.hpp"
#include "Armadillo-utils.h" 

const string videodir = "/Users/xavier/numediart/Project7.3-DancersCycle/VideosSmall/TestSmallSize/";

void test_single_bic(){
	arma::fmat M;
	M.set_size(1,100);
	for (int i=0; i<30; i++){
		M(0,i)= -20+double(rand())/RAND_MAX;
	}
	for (int i=30; i<80; i++){
		M(0,i)= -10+double(rand())/RAND_MAX;
	}
	for (int i=80; i<100; i++){
		M(0,i)= -5+double(rand())/RAND_MAX;
	}
	M.print();
	
	//plotting in gnuplot
	vector<double> m;
	for (unsigned int i=0; i< M.n_cols; i++) {
		m.push_back( (double) M(0,i) );
	}
	
	Gnuplot g1 = Gnuplot("lines");
	g1.reset_plot();
	g1.plot_x(m,"M");
    sleep(1);
	
	ACBicSegmentationPlugin* P = new ACBicSegmentationPlugin();
	P->segment(M);
	delete P;	
}

int main(){
	cout << "testing BIC segmentation" << endl;
	test_single_bic();
			
	return 0;
}

