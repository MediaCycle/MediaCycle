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

ACMediaFeatures*& ACMedia::getFeatures(int i){ 
	if (i < int(features.size()) )
		return features[i]; 
	else {
		std::cerr << "ACMedia::getFeatures : out of bounds" << i << " > " << features.size() << std::endl;
//		return (ACMediaFeatures*&)NULL; // duh ?
	}
}

int ACMedia::import(std::string _path, int id, ACPluginManager *acpl ){
	std::cout << "importing..." << _path << std::endl;
	filename=_path;
	int import_ok = 1;
	
	if (id>=0) this->setId(id);
	
	//compute features with available plugins
	if (acpl) {
		for (int i=0;i<acpl->getSize();i++) {
			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
				if (acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType() == this->getType()
					&& acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_FEATURES) {
					ACMediaFeatures *af = acpl->getPluginLibrary(i)->getPlugin(j)->calculate(this->getFileName());
					//another option :
					//ACMediaFeatures *af = acpl->getPluginLibrary(i)->calculate(j,this->getFileName());
					if (af == NULL)
						import_ok = 0;
					else {
						this->addFeatures(af);
						import_ok = 1;
					}
					// XS : addFeatures only if it did not fail !
				}
			}
		}
	}
	return import_ok;
}