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
//#include <exception>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <armadillo>

#include "boost/filesystem.hpp"

using namespace arma;

using std::cerr;
using std::cout;
using std::endl;
using std::ofstream;
using std::vector;
using std::string;

using namespace boost;

ACMediaFeatures::ACMediaFeatures(void){
 	//XS TODO: is this flag still necessary ?
  _computed = false;
  needs_normalization = 1;
  name = "none";
}

ACMediaFeatures::ACMediaFeatures(FeaturesVector V, string myname){
	// XS TODO:  add checks
	features_vector = V;

	//XS 151110
	needs_normalization = 1;

	name = myname;
	if (V.size() > 0) _computed = true;
	else cerr << "<ACMediaFeatures Constructor> trying to construct empty media features" << endl;
}

//ACMediaFeatures::ACMediaFeatures(FeaturesVectorAsDouble V, string myname){
//	// XS TODO:  add checks
//	// XS TODO: clumsy conversion double -> float. keep double ? template ?
//	vector<float> tmp(V.begin(), V.end());
//	features_vector = tmp;
//	name = myname;
//	if (V.size() > 0) _computed = true;
//	else cerr << "<ACMediaFeatures Constructor> trying to construct empty media features" << endl;
//}


float ACMediaFeatures::getFeatureElement(int i) {
	if (i >= int(features_vector.size()) || i < 0){
		cerr << " <ACMediaFeatures::getFeatureElement(int i) error> Invalid Feature Index" << endl;
		return -1;
	}
	return features_vector[i];
}



//XS TODO: is often multidimensional...
int ACMediaFeatures::getDiscretizedFeature(){
  if (this->getSize()>1){
    cout << "Warning : Multidimensionnal feature, cannot be discretized" << endl;
    return 0;
  }
  else{
    int res = 0;
    if (this->getFeatureElement(0) > .5)
      res=3;
    else 
      if (this->getFeatureElement(0) < -.5)
	res = 1;
      else
	res = 2;
    return res;
  }
}

void ACMediaFeatures::setFeatureElement(int i, float f) {
	if (i >= int(features_vector.size()) || i < 0){
		cerr << " <ACMediaFeatures::setFeatureElement(int i) error> Invalid Feature Index" << endl;
		return;
	}
	features_vector.set(i,f);
}

//TR Modification. What's the use of push back if we know the acurate number of elements

void ACMediaFeatures::addFeatureElement(float f){
	features_vector.push_back(f);
}


int ACMediaFeatures::getSize() { // XS TODO:  getSize
	return features_vector.size();
}

void ACMediaFeatures::dump(){ // output in terminal
	for (int i=0; i< int(features_vector.size()); i++){
		cout << i << " : " << features_vector[i] << endl;
	}
}

void ACMediaFeatures::write(string file_name){ // output in file
	ofstream out(file_name.c_str()); 
	out << "features name: " << getName() << endl;
	for (int i=0; i<int(features_vector.size()); i++){
		out << i << " : " << features_vector[i] << endl;
	}
	
	out.close();
}

bool ACMediaFeatures::saveInFile(string _fname, bool _binary){ 
	bool save_ok = false;
	
	vec data(features_vector.size(),fill::zeros);
	for (int i=0; i<int(features_vector.size()); i++){
		data(i) = features_vector[i];
	}
	
	if (_binary){
		#ifdef ARMADILLO_HAVE_RANDU // randu and .save returns were both introduced in version 0.9.50
			save_ok = data.save(_fname,arma_binary); // default format = arma_binary
		#else
			data.save(_fname, arma_binary); // default format = arma_binary
			if ( boost::filesystem::exists( _fname ) )
				save_ok = true;
		#endif
	}
	else {
		#ifdef ARMADILLO_HAVE_RANDU // randu and .save returns were both introduced in version 0.9.50
			save_ok = data.save(_fname, arma_ascii);
		#else
			data.save(_fname, arma_ascii);
			if ( boost::filesystem::exists( _fname ) )
				save_ok = true;
		#endif	
	}
	return save_ok;
}

bool ACMediaFeatures::loadFromFile(string _fname, bool _binary){
	mat tmp_m;
	bool load_ok = false;
    
    boost::filesystem::path p( _fname.c_str());// , boost::filesystem::native );
    if ( !boost::filesystem::exists( p ) ){
        std::cout << "ACMediaTimedFeature::loadFromFile: file " << _fname << " doesn't exist " << std::endl;
        return false;
    }
    /*if(!boost::filesystem::is_regular( p ) ){
        std::cout << "ACMediaTimedFeature::loadFromFile: file " << _fname << " isn't regular " << std::endl;
        return false;
    }
    if(boost::filesystem::file_size( p ) == 0 ){
        std::cout << "ACMediaTimedFeature::loadFromFile: file " << _fname << " has null size " << std::endl;
        return false;
    }*/
    
	if (_binary){
		#ifdef ARMADILLO_HAVE_RANDU // randu and .save returns were both introduced in version 0.9.50
			load_ok = tmp_m.load(_fname, arma_binary); // default format = arma_binary
		#else
			tmp_m.load(_fname, arma_binary); // default format = arma_binary
			if ( tmp_m.n_rows > 0 && tmp_m.n_cols > 0 )
				load_ok = true;
		#endif
	}
	else {
		#ifdef ARMADILLO_HAVE_RANDU // randu and .save returns were both introduced in version 0.9.50
			load_ok = tmp_m.load(_fname, arma_ascii);
		#else
			tmp_m.load(_fname, arma_ascii);
			if ( tmp_m.n_rows > 0 && tmp_m.n_cols > 0 )
				load_ok = true;
		#endif
	}
	
	if (load_ok) { 
        for (int i=0; i<int(tmp_m.n_rows); i++){
            features_vector.push_back(tmp_m(i,0));
        }
	}
	return load_ok;
}

