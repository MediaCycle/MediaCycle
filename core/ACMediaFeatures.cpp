/*
 *  ACMediaFeatures.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 13/05/09
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

#include "ACMediaFeatures.h"
#include <exception>
#include <iostream>
#include <fstream>

using namespace std;

ACMediaFeatures::ACMediaFeatures(){
	_type = FT_BASE; 
	_computed = false; 
}

float ACMediaFeatures::getFeature(int i) {
	if (i >= features_vector.size() || i < 0){
		cerr << " <ACMediaFeatures::GetFeature(int i) error> Invalid Feature Index" << endl;
		return -1;
	}
	return features_vector[i];
}

void ACMediaFeatures::setFeature(int i, float f) {
	if (i >= features_vector.size() || i < 0){
		cerr << " <ACMediaFeatures::GetFeature(int i) error> Invalid Feature Index" << endl;
		return;
	}
	features_vector[i] = f;
}

int ACMediaFeatures::size() {
	return features_vector.size();
}

void ACMediaFeatures::dump(){ // output in terminal
// XS TODO : make this better
	cout << "features of type: " << _type << endl;
	for (int i=0; i<features_vector.size(); i++){
		cout << i << " : " << features_vector[i] << endl;
	}
}

void ACMediaFeatures::write(string file_name){ // output in file
	ofstream out(file_name.c_str()); 
	out << "features of type: " << _type << endl;
	for (int i=0; i<features_vector.size(); i++){
		out << i << " : " << features_vector[i] << endl;
	}
	
	out.close();
}
