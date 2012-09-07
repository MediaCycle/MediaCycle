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

#include <set>
#include <map>
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
	std::map<std::string,float>* lMap=(std::map<std::string,float>*)( sensorData->getData());
	std::map<std::string,float>::iterator it;
	srand ( time(NULL) );
	for (it=lMap->begin();it!=lMap->end();it++){
		FeaturesVector descVect(false);
		float randMod=(float)rand()/RAND_MAX/10.f;
		descVect.push_back(it->second+randMod);
		ACMediaFeatures* descFeat=new ACMediaFeatures(descVect,it->first);
		sensorFeatures.push_back(descFeat);
		cout << descFeat->getName()<<"\t"<<descFeat->getFeatureElement(0) <<endl ;
	}
	
	return sensorFeatures;
}

preProcessInfo ACNavimedSensorFeaturePlugin::update(ACMedias media_library){
	set<std::string> *featureKeyList=new set<std::string>;
        for (ACMedias::iterator it1=media_library.begin();it1!=media_library.end(); it1++){
                std::vector<ACMediaFeatures*> featsVect=it1->second->getAllFeaturesVectors();
		std::vector<ACMediaFeatures*>::iterator it2;
		for (it2=featsVect.begin();it2!=featsVect.end();it2++){
			if (featureKeyList->find((*it2)->getName())==featureKeyList->end()){
				featureKeyList->insert((*it2)->getName());
			}
		}
	}
	std::set<std::string>::iterator it2;
	for (it2=featureKeyList->begin();it2!=featureKeyList->end();it2++)
		cout<<(*it2)<<endl;
	cout<<"Index size:"<<featureKeyList->size()<<endl;
	return ((void*)featureKeyList);
}
std::vector<ACMediaFeatures*> ACNavimedSensorFeaturePlugin::apply(preProcessInfo info,ACMedia* theMedia){
	
	cout<<"preproc feat of"<<theMedia->getFileName();
	std::vector<ACMediaFeatures*> desc;
	set<string> *featureKeyList=(set<string>*) (info);
	set<string>::iterator it;
	map<string,ACMediaFeatures*> featMap;
	for (it=featureKeyList->begin();it!=featureKeyList->end();it++){
		FeaturesVector descVect;
		//ACMediaFeatures* featsVect=theMedia->getFeaturesVector((*it));
		//if (featsVect!=0){
		//	descVect.push_back(featsVect->getFeatureElement(0));
		//}
		//else{
			descVect.push_back(-1000.f);
		//}
		ACMediaFeatures* descFeat=new ACMediaFeatures(descVect,(*it));
		desc.push_back(descFeat);
		featMap[*it]=descFeat;
		//cout << descFeat->getName()<<"\t"<<descFeat->getFeatureElement(0) <<endl ;
	}
	vector<ACMediaFeatures*> featVects=theMedia->getAllFeaturesVectors();
	vector<ACMediaFeatures*>::iterator it2;
	for (it2=featVects.begin();it2!=featVects.end();it2++){
		string locName=(*it2)->getName();
		if (featMap.find(locName)!=featMap.end()){
			ACMediaFeatures* locFeat=featMap[locName];
			if (locFeat!=0)
				locFeat->setFeatureElement(0,(*it2)->getFeatureElement(0));
			locFeat=0;
		}
		else {
			//cout<<"ACNavimedSensorFeaturePlugin::apply problem: non indexed feature"<<endl;
		}

	}
	featMap.clear();
	cout<<"size:"<<desc.size()<<endl;
	return desc;
	
}

void ACNavimedSensorFeaturePlugin::freePreProcessInfo(preProcessInfo &info){
	set<string> *featureKeyList=(set<string>*) (info);
	featureKeyList->clear();
	cout<<((set<string>*)info)->size()<<endl;
	delete featureKeyList;
	featureKeyList=0;
	info=0;

}
