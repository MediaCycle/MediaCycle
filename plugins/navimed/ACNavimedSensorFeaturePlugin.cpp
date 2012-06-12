/*
 *  ACNavimedSensorFeaturePlugin.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 9/06/12
 *  @copyright (c) 2012 – UMONS - Numediart
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
#include "ACNavimedSensorFeaturePlugin.h"
#include "ACMedia.h"
#include "NavimedSensor.h"

#include <stdlib.h>
#include <time.h>

#include<iostream>
using namespace std;

ACNavimedSensorFeaturePlugin::ACNavimedSensorFeaturePlugin(): ACFeaturesPlugin() {
    this->mMediaType = MEDIA_TYPE_SENSOR;
    this->mName = "NavimedSensorFeature";
    this->mDescription = "first version to test bioparameter vizualisation";
    this->mId = "";
}

ACNavimedSensorFeaturePlugin::~ACNavimedSensorFeaturePlugin() {
}


std::vector<ACMediaFeatures*> ACNavimedSensorFeaturePlugin::calculate(ACMediaData* aData, ACMedia* theMedia, bool _save_timed_feat) {
	std::vector<ACMediaFeatures*> sensorFeatures;
	NavimedSensorData* sensorData=dynamic_cast<NavimedSensorData*>(aData);
	std::map<std::string,float>* lMap=sensorData->getData();
	std::map<std::string,float>::iterator it;
	srand ( time(NULL) );
	for (it=lMap->begin();it!=lMap->end();it++){
		FeaturesVector descVect(false);
		float randMod=(float)rand()/RAND_MAX/10.f;
		descVect.push_back(it->second+randMod);
		ACMediaFeatures* descFeat=new ACMediaFeatures(descVect,it->first);
		sensorFeatures.push_back(descFeat);
	}
	
	return sensorFeatures;
}

