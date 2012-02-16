/*
 *  ACEuclideanDistance.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 2/06/09
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

#include "ACEuclideanDistance.h"
#include <math.h>
#include <iostream>
using namespace std;

ACEuclideanDistance::ACEuclideanDistance(ACMediaFeatures* F1, ACMediaFeatures* F2) : ACDistance( F1, F2) {
// TODO XS add tests !
	V1 = F1->getFeaturesVector();
	V2 = F2->getFeaturesVector();
}

ACEuclideanDistance::ACEuclideanDistance(FeaturesVector *F1, FeaturesVector *F2) : ACDistance( F1, F2) {
	V1 = F1;
	V2 = F2;
}

double ACEuclideanDistance::distance(){
	if (V1->size() != V2->size()){
		cerr << "<ACEuclideanDistance::distance> : incomparable features" << endl;
		return 0.0;
	}
	int s = V1->size();
	if (s==0) {
		//cerr << "<ACEuclideanDistance::distance> : empty features" << endl;
		return 0.0;
	}
	double d = 0.0 ;
	for (int i=0; i < s; i++){
		d+= pow(((*V1)[i]-(*V2)[i]),2);
	}	
	return sqrt(d)/s;
}
