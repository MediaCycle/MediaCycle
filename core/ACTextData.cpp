/*
 *  ACTextData.cpp
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

#if defined (SUPPORT_TEXT)
#include "ACTextData.h"
#include <string>
#include <iostream>
#include "textFile.h"
using namespace std;
using std::cerr;
using std::endl;
using std::string;

ACTextData::ACTextData(){ 
	this->init();
}

void ACTextData::init() {
	media_type = MEDIA_TYPE_TEXT;
	text_ptr=0;
}

ACTextData::ACTextData(std::string _fname) { 
	this->init();
	file_name=_fname;
	this->readData(_fname);
}

ACTextData::~ACTextData() {
	if (text_ptr != 0) delete text_ptr;
}

void ACTextData::readData(std::string _fname){ 
	if (text_ptr != NULL) { 
		delete text_ptr;
		text_ptr=0;
	}
	
	text_ptr = textFileRead(_fname);
    if( (text_ptr == NULL)) {
		cerr << "<ACMediaData::readTextData> file can not be read !" << endl;
	}
	cout << file_name<<"\n";
	cout << (*text_ptr)<<"\n";
}

void ACTextData::setData(string* _data){
	
	if (text_ptr)
		delete text_ptr;
	text_ptr=new string;
	(*text_ptr)=(*_data);
}

#endif //defined (SUPPORT_TEXT)