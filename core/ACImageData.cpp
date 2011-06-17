/*
 *  ACImageData.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 7/04/11
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

#if defined (SUPPORT_IMAGE)
#include "ACImageData.h"
#include <string>
#include <iostream>
using std::cerr;
using std::endl;
using std::string;


ACImageData::ACImageData() { 
	this->init();
}

ACImageData::ACImageData(std::string _fname) { 
	this->init();
	file_name=_fname;
	this->readData(_fname);
}

void ACImageData::init() {
	media_type = MEDIA_TYPE_IMAGE;
	image_ptr = 0;
}

ACImageData::~ACImageData() {
	if (image_ptr != 0) cvReleaseImage(&image_ptr);
	image_ptr = 0;
}

void ACImageData::readData(std::string _fname){
	if(_fname=="") return;
	image_ptr = cvLoadImage(_fname.c_str(), CV_LOAD_IMAGE_COLOR);	

	try {
		if (!image_ptr) {
			cerr << "Check file name : " << _fname << endl;
			throw(string(" <ACImageData::readData> CV_LOAD_IMAGE_COLOR : not a color image !"));
		}
	}
	catch (const string& not_image_file) {
		cerr << not_image_file << endl;
	}
}

void ACImageData::setData(IplImage* _data){
	cvCopy(_data,image_ptr);		
	if( !image_ptr ) {
		cerr << "<ACImageData::setData> Could not set data" << endl;
	}
}


#endif //defined (SUPPORT_IMAGE)
