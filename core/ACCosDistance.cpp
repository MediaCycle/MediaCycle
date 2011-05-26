/*
 *  ACCosDistance.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 20/10/10
 *  @copyright (c) 2010 – UMONS - Numediart
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



#include "ACCosDistance.h"
#include <math.h>
#include <iostream>
using namespace std;

ACCosDistance::ACCosDistance(ACMediaFeatures* F1, ACMediaFeatures* F2) : ACDistance( F1, F2) {
	// TODO XS add tests !
	V1 = F1->getFeaturesVector();
	V2 = F2->getFeaturesVector();
}

ACCosDistance::ACCosDistance(FeaturesVector *F1, FeaturesVector *F2) : ACDistance( F1, F2) {
	V1 = F1;
	V2 = F2;
}

double ACCosDistance::distance(){
	if (V1->size() != V2->size()){
		cerr << "<ACCosDistance::distance> : incomparable features" << endl;
		return 0.0;
	}
	int s = V1->size();
	if (s==0) {
		cerr << "<ACCosDistance::distance> : empty features" << endl;
		return 0.0;
	}
	double a = 0.0, b= 0.0, ab= 0.0 ;
	for (int i=0; i < s; i++){
		float temp1=(*V1)[i],temp2=(*V2)[i];
		ab+= temp1*temp2;
		a+= temp1*temp1;
		b+= temp2*temp2;
		
	}
	if (a==0||b==0)
		return 0;
	return 1-(ab/sqrt(a*b));
}
