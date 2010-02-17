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
#include <algorithm>
using namespace std;

ACMediaFeatures::ACMediaFeatures(){
  _type = FT_BASE; 
  _computed = false; 
  needs_normalization = 1;
  name = "none";
}

float ACMediaFeatures::getFeature(int i) {
	if (i >= features_vector.size() || i < 0){
		cerr << " <ACMediaFeatures::getFeature(int i) error> Invalid Feature Index" << endl;
		return -1;
	}
	return features_vector[i];
}

int ACMediaFeatures::getDiscretizedFeature(){
  if (this->size()>1){
    std::cout << "Warning : Multidimensionnal feature, cannot be discretized" << std::endl;
    return 0;
  }
  else{
    int res = 0;
    if (this->getFeature(0) > .5)
      res=3;
    else 
      if (this->getFeature(0) < -.5)
	res = 1;
      else
	res = 2;
    return res;
  }
}


void ACMediaFeatures::setFeature(int i, float f) {
	if (i >= features_vector.size() || i < 0){
		cerr << " <ACMediaFeatures::setFeature(int i) error> Invalid Feature Index" << endl;
		return;
	}
	features_vector[i] = f;
}

void ACMediaFeatures::addFeature(float f){
	features_vector.push_back(f);
}

int ACMediaFeatures::size() {
	return features_vector.size();
}

void ACMediaFeatures::dump(){ // output in terminal
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
