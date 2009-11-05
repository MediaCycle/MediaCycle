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

#include <iostream>
#include <string>
#include <vector>

#include "ACFeaturesTypes.h"

class ACMediaFeatures  {	
protected:
	FeatureType _type;
	FeaturesVector features_vector; // vector<float>
	bool _computed ;
	static DistanceType dist_type; // XS check: derive static => 1 per subclass ?
	std::string name;
public:
	ACMediaFeatures(); // no media given, probably read features from disk
	virtual ~ACMediaFeatures()  {};
	//	virtual void calculate(void*) = 0;
	// the following are common to all features types and should NOT be redefined
	const FeatureType& getType() const {return _type;};
	bool isComputed() {return _computed;};
	void setComputed() { _computed = true; };
	float getFeature(int i);
	void addFeature(float f);
	int getDiscretizedFeature(int scale, int nbSteps);
	void setFeature(int i, float f);
	FeaturesVector getAllFeatures() {return features_vector;};
	void dump(); // output in terminal
	void write(std::string);  // output in a file -- todo : define format
	// void read() {} // read from file -- cf. plugin ?
	
	std::string getName() {return name;};
	void setName(std::string namei) {this->name = namei;};
	// XS TODO : the following 2 have to be discussed with SD
	void resize(int new_size) { features_vector.resize(new_size); };
	int size();
	
};

#endif // _ACMEDIAFEATURES_H
