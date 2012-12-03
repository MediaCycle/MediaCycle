/*
 *  ACSensor.cpp
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

#include "ACSensor.h"
using namespace std;


ACSensor::ACSensor() : ACMedia() {
    this->init();
}

ACSensor::ACSensor(const ACSensor& m) : ACMedia(m) {
    this->init();
}

void ACSensor::init(){
    media_type = MEDIA_TYPE_SENSOR;
    data =NULL;
}

ACSensor::~ACSensor(){
	if (data!=NULL)
		delete data;
}

void ACSensor::deleteData(){
	if (data)
		delete data;
	data=0;
}
void ACSensor::saveACLSpecific(ofstream &library_file) {
	
	library_file << endl;
}

int ACSensor::loadACLSpecific(ifstream &library_file) {
	

	
	return 1;
}

bool ACSensor::extractData(string fname){
	if (data){
		delete data;
		data=0;
	}
	data = new ACSensorData(fname);
	if (data!=0){
		if (data->getData()==NULL){
			delete data;
			data=0;
		}
		else
			label=data->getLabel();
	}
	return true;
}
