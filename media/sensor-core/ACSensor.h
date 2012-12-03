/*
 *  ACSensor.h
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 22/10/10
 *  @copyright (c) 2010 – UMONS - Numediart
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

#ifndef ACSENSOR_H
#define ACSENSOR_H

#include "ACMedia.h"
#include "ACMediaFeatures.h"
#include <string>
#include <cstring>
#include "ACSensorData.h"

class ACSensor : public ACMedia {
	// contains the *minimal* information about a Sensor
protected:
	ACSensorData* data;
public:
	ACSensor();
    ACSensor(const ACSensor& m);
	~ACSensor();
private:
    void init();
	
public:
	void deleteData();
	void saveACLSpecific(std::ofstream &library_file);
	int loadACLSpecific(std::ifstream &library_file);
	bool extractData(std::string fname);
	ACMediaData* getMediaData(){return data;} // XS TODO : needs dynamic_cast<ACMediaData*> (data) ??;
	
	virtual void* getThumbnailPtr(){return 0;} // XS TODO change this
};
#endif // ACSENSOR_H
