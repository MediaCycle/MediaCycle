/**
 * @brief AGSynthesis.h
 * @author Damien Tardieu
 * @date 08/07/2010
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

#ifndef AGSYNTHESIS_H
#define AGSYNTHESIS_H

#include <vector>
#include "ACAudio.h"
#include "Armadillo-utils.h"
#include <stdio.h>
#include <sndfile.h>
#include <string.h>
#include "ACAudioFeatures.h"
#include <iostream>
#include <map>
#include "MediaCycle.h"

using namespace arma;

enum AGMethod{
	AG_METHOD_SIMPLE=0,
	AG_METHOD_SQUEEZED=1,
	AG_METHOD_PADDED = 2
};

enum AGMapping{
	AG_MAPPING_NONE=0,
	AG_MAPPING_MEAN=1,
	AG_MAPPING_MEANVAR = 2
};

class AGSynthesis {
 public:
	AGSynthesis();
	~AGSynthesis(){};
	bool compute(long targetId, std::vector<long> garinIds);
	bool compute(long targetId, set<int> selectedNodes);
	
	AGMethod getMethod(){return method;};
	void setMethod(AGMethod met){this->method=met;};
	AGMapping getMapping(){return mapping;};
	void setMapping(AGMapping map){this->mapping = map;};
	float getRandomness(){return randomness;};
	void setRandomness(float rand){randomness = rand;};
	float getThreshold(){return this->threshold;};
	void setThreshold(float thresh){this->threshold = thresh;};
	float* getSound(){return synthesisSound;};
	long getLength(){return synthesisLength;};

	void setMediaCycle(MediaCycle* mc){this->mediacycle = mc;};

	bool saveAsWav(string);
	
 private:
	float* synthesisSound;
	long synthesisLength;
	MediaCycle* mediacycle;
	long synthesisID;
	
	// param
	AGMethod method;
	AGMapping mapping;
	float randomness;
	float threshold;
	
	colvec extractSamples(ACAudio* audioGrain);
	mat extractDescMatrix(ACMediaLibrary* lib, string featureName, std::vector<long> mediaIds);
	mat extractDescMatrix(ACMediaLibrary* lib, std::vector<string> featureList, std::vector<long> mediaIds);
};

#endif
