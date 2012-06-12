/*
 *  ACSensorData.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 02/05/11
 *  @copyright (c) 2011 – UMONS - Numediart
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

#if defined (SUPPORT_SENSOR)
#include "ACSensorData.h"
#include <string>
#include <iostream>

using namespace std;
using std::cerr;
using std::endl;
using std::string;

ACSensorData::ACSensorData(){ 
	this->init();
}

void ACSensorData::init() {
	media_type = MEDIA_TYPE_SENSOR;
	sensor_ptr=0;
}

ACSensorData::ACSensorData(std::string _fname) { 
	this->init();
	file_name=_fname;
	this->readData(_fname);
}

ACSensorData::~ACSensorData() {
	if (sensor_ptr != 0) {
		sensor_ptr->clear();
		delete sensor_ptr;
		sensor_ptr=0;
	}
}


void ACSensorData::setData(string* _data){
	
	if (sensor_ptr)
		delete sensor_ptr;
	
	
}

#endif //defined (SUPPORT_SENSOR)