/*
 *  videoTest.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 30/09/09
 *  @copyright (c) 2009 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 */

#include <stdlib.h>
#include <string>
#include <cstring>
#include <iostream>
#include <signal.h>

#include "MediaCycle.h"
#include "ACVideoAnalysis.h"

#include "gnuplot_i.hpp"

using namespace std;

int main(int argc, char** argv) {
	std::string f1= "/Users/xavier/numediart/Project7.3-DancersCycle/Recordings_Raffinerie_0709/FrontShots/H264/Bru_105#2.mov";
	std::string bg = "/Users/xavier/numediart/Project7.3-DancersCycle/Recordings_Raffinerie_0709/FrontShots/average_bg_0148.jpg";
	ACVideoAnalysis* V = new ACVideoAnalysis(f1);
	V->initialize();
	IplImage *imgp_bg = cvLoadImage(bg.c_str(), CV_LOAD_IMAGE_COLOR);
	V->computeBlobsInteractively(imgp_bg,true); // , int small_blob)
	
	//V->computeBlobs(imgp_bg);
	V->computeContractionIndices();
	
	cvReleaseImage(&imgp_bg);
	delete V;
	
	
	std::vector<float> ci = V->getContractionIndices();
	std::vector<double> dci;
	for (int i=0; i< ci.size(); i++) {
		dci[i] = (double) ci[i];
	}
	
	Gnuplot g1 = Gnuplot("lines");
	g1.reset_plot();
	g1.plot_x(dci,"dci");

	return (EXIT_SUCCESS);
}

