/*
 *  NavimedSensor.cpp
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

#include "NavimedSensor.h"


#if defined (SUPPORT_SENSOR)
#include "NavimedSensor.h"
#include "NavimedReader.h"

#include <stdlib.h>

using namespace std;
NavimedSensorData::NavimedSensorData():ACSensorData(){
}

NavimedSensorData::~NavimedSensorData(){
	if (sensor_ptr!=0){
		sensor_ptr->clear();
		delete sensor_ptr;
		sensor_ptr=0;
	}
}
NavimedSensorData::NavimedSensorData(std::string _fname):ACSensorData(){
	file_name=_fname;
	this->readData(_fname);
}
string bioParamConvToStr(int bioparam){
	switch (bioparam){
		case -2:
			return string("--");
			break;
		case -1:
			return string("-");
			break;
		case 0:
			return string("0");
			break;
		case 1:
			return string("+");
			break;
		case 2:
			return string("++");
			break;
		default:
			return string("0");
	}
}

bool NavimedSensorData::readData(std::string _fname){
	if (sensor_ptr != NULL) {
		delete sensor_ptr;
		sensor_ptr=0;
	}
	navimedReader reader(_fname);
	if (reader.isNavimed()){
		std::string desc;
		
		//here we push in the map sensor_ptr the different bio parameter
		float value;
		
		label=reader.getReference()+string("/GB:");
		sensor_ptr=new map<std::string,float>;
		string paramName="Globules blancs";
		if (reader.getBioParam(paramName,value)==false){
			delete sensor_ptr;
			sensor_ptr=0;
			return false;
		}
		label+=bioParamConvToStr(value)+string("/Hg:");
		(*sensor_ptr)[paramName]=value;
		 paramName="Hémoglobine";
		if (reader.getBioParam(paramName,value)==false){
			delete sensor_ptr;
			sensor_ptr=0;
			return false;
		}
		label+=bioParamConvToStr(value)+string("/GR:");
		(*sensor_ptr)[paramName]=value;
		 paramName="Globules rouges";
		if (reader.getBioParam(paramName,value)==false){
			delete sensor_ptr;
			sensor_ptr=0;
			return false;
		}
		label+=bioParamConvToStr(value)+string("/Ht:");
		(*sensor_ptr)[paramName]=value;
		 paramName="Hématocrite";
		if (reader.getBioParam(paramName,value)==false){
			delete sensor_ptr;
			sensor_ptr=0;
			return false;
		}
		label+=bioParamConvToStr(value)+string("/VGM:");
		(*sensor_ptr)[paramName]=value;
		 paramName="Vol.globulaire moyen";
		if (reader.getBioParam(paramName,value)==false){
			delete sensor_ptr;
			sensor_ptr=0;
			return false;
		}
		label+=bioParamConvToStr(value);
		(*sensor_ptr)[paramName]=value;
		
		cout << label<<"\n";
		//	cout << (*sensor_ptr)<<"\n";
		return true;
	}
	else {
		return false;
	}
	
}


NavimedSensor::NavimedSensor():ACSensor(){
}

bool NavimedSensor::extractData(std::string fname){
	if (data){
		delete data;
		data=0;
	}
	// XS todo : store the default header (16 or 64 below) size somewhere...
	data = new NavimedSensorData(fname);
	if (data!=0){
		if (data->getData()==NULL){
			delete data;
			data=0;
		}
		else
			label=data->getLabel();
	}
	//computeThumbnail(image_data, 64, 64);
	//width = thumbnail_width;
	//height = thumbnail_height;
	return true;
}


#endif