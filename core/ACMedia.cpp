/*
 *  ACMedia.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 23/09/09
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

#include "ACMedia.h"
#include <iostream>
using namespace std;
ACMedia::ACMedia() { 
	mid = -1;
	width = 0;
	height = 0;
	duration = 0.0;
	features_vectors.resize(0);
}

ACMedia::~ACMedia() { 
	// XS TODO Clean up cout ! 
	// XS check that this is common to all media -- is it really called
	// AM yes it is called
	cout << "XS debug -- ACMedia destructor" <<endl;
	vector<ACMediaFeatures*> ::iterator iter;
	for (iter = features_vectors.begin(); iter != features_vectors.end(); iter++) {
		delete *iter;//needed erase call destructor of pointer (i.e. none since it's just a pointer) not of pointee ACMediaFeatures
		//features_vectors.erase(iter); //will cause segfault. besides the vector is automatically emptied, no need to erase.
	}	
}

ACMediaFeatures* ACMedia::getFeaturesVector(int i){ 
	if (i < int(features_vectors.size()) )
		return features_vectors[i]; 
	else {
		std::cerr << "ACMedia::getFeaturesVector : out of bounds " << i << " > " << features_vectors.size() << std::endl;
	}
	return NULL;
}

ACMediaFeatures* ACMedia::getFeaturesVector(string feature_name) { 
	int i;
	for (i=0;i<int(features_vectors.size());i++) {
		if (!(feature_name.compare(features_vectors[i]->getName()))) {
			return features_vectors[i];
		}
	}
	std::cerr << "ACMedia::getFeaturesVector : not found feature named " << feature_name << std::endl;
	return 0;
}

int ACMedia::import(std::string _path, int id, ACPluginManager *acpl ) {
	std::cout << "importing..." << _path << std::endl;
	this->filename=_path;
	this->filename_thumbnail = _path;
	int import_ok = 1;
	
	if (id>=0) this->setId(id);
	
	// get info about width, height, thumbnail, ...
	// and return a pointer to the data
	ACMediaData* data_ptr = this->extractData(this->getFileName());
	if (data_ptr==NULL){
		import_ok = 0;
		cerr << "<ACMedia::import> failed accessing data for media number: " << id << endl;
		return 0;
	}
	
	//compute features with available plugins
	if (acpl) {
		for (int i=0;i<acpl->getSize();i++) {
			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
				if (acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType() == this->getType()
					&& acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_FEATURES) {
					ACPlugin* plugin =  acpl->getPluginLibrary(i)->getPlugin(j);
					
					//vector<ACMediaFeatures*> afv = plugin->calculate(this->getFileName());
					vector<ACMediaFeatures*> afv = plugin->calculate(data_ptr, this);
					
					//another option :
					//ACMediaFeatures *af = acpl->getPluginLibrary(i)->calculate(j,this->getFileName());
					if (afv.size()==0){
						import_ok = 0;
						cerr << "<ACMedia::import> failed importing feature from plugin: " << plugin->getName() << endl;
					}
					else {
						for (int Iafv=0; Iafv<afv.size(); Iafv++)
							this->addFeaturesVector(afv[Iafv]);
						import_ok = 1;
					}
				}
				else {
					cerr << "<ACMedia::import> compatibility problem with plugins" << endl;
					cerr << "acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType(): " << acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType()  << endl;
					cerr << "this->getType(): " << this->getType() << endl;
					cerr << "acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType(): " << acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() << endl;
					cerr << "PLUGIN_TYPE_FEATURES: "<< PLUGIN_TYPE_FEATURES << endl;
				}
			}
		}
	}
	delete data_ptr;
	return import_ok;
}
