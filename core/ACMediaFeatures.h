/*
 *  ACMediaFeatures.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 13/05/09
 *
 *  - XS 26/06/09 : 
 *    changed double to float (see ACFeaturesType)
 *    added setFeature
 *
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

#ifndef _ACMEDIAFEATURES_H
#define _ACMEDIAFEATURES_H

//#include <iostream>
#include <string>
#include <vector>

#include "ACFeaturesTypes.h"

class ACMediaFeatures  {
	// ACMediaFeatures is essentially a vector of floats with some extra methods.
	// . each plugin should return one ACMediaFeatures.
	// . each element of ACMediaFeatures could be called "feature", but we call it featureElement to avoid confusion between the vector and one of its elements
protected:
	FeaturesVector features_vector; // vector<float>
	bool _computed ;
	std::string name;
	int needs_normalization;
public:
	ACMediaFeatures(); // no media given, probably read features from disk
	ACMediaFeatures(FeaturesVector V, std::string myname="");
//	ACMediaFeatures(FeaturesVectorAsDouble V, std::string myname="",);
	virtual ~ACMediaFeatures()  {};
	//	virtual void calculate(void*) = 0;
	// the following are common to all features types and should NOT be redefined
	bool isComputed() {return _computed;};
	void setComputed() { _computed = true; };
	void setNeedsNormalization(int i) { needs_normalization = i; }
	int getNeedsNormalization() { return needs_normalization; }
	
	float getFeatureElement(int i);
	void addFeatureElement(float f);
	void setFeatureElement(int i, float f);
	FeaturesVector &getFeaturesVector() {return features_vector;};
	
	// this works only for 1D features -- does it belong here ?
	int getDiscretizedFeature();

	void dump(); // output in terminal
	void write(std::string);  // output in a file -- todo : define format
	// void read() {} // read from file -- cf. plugin ?
	
	std::string getName() {return name;};
	void setName(std::string namei) {this->name = namei;};

	void resize(int new_size) { features_vector.resize(new_size); }
	int getSize();
	
        //JU add distance management
        void setDistanceType(FeaturesVector::DistanceType distType){features_vector.setDistanceType(distType);};
        FeaturesVector::DistanceType getDistanceType(){return features_vector.getDistanceType();};
        
};

#endif // _ACMEDIAFEATURES_H
