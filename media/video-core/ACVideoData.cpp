/*
 *  ACVideoData.cpp
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

#include "ACVideoData.h"
#include <string>
#include <iostream>
using std::cerr;
using std::endl;
using std::string;

ACVideoData::ACVideoData() { 
	this->init();
}

ACVideoData::ACVideoData(std::string _fname){ 
	this->init();
	file_name=_fname;
	this->readData(_fname);
}

void ACVideoData::init() {
	media_type = MEDIA_TYPE_VIDEO;
	video_ptr = 0;
}

ACVideoData::~ACVideoData() {
	delete video_ptr;
	video_ptr = 0;
}

// creates a pointer (video_ptr) to the beginning of the video
bool ACVideoData::readData(string _fname){
	if(_fname=="") return false;
	video_ptr = new cv::VideoCapture(_fname.c_str());		
	// check if video successfully opened 
	if (!video_ptr->isOpened()){ 
		// Either the video does not exist, or it uses a codec OpenCV does not support. 
		cerr << "<ACVideoData::readData> Could not initialize capturing from file " << _fname << endl;
		return false;
	}	
	return true;
}

// XS TODO check this copy 2.2
void ACVideoData::setData(cv::VideoCapture* _data){
	cvCopy(_data,video_ptr);		
	if( !video_ptr ) {
		// Either the video does not exist, or it uses a codec OpenCV does not support. 
		cerr << "<ACVideoData::setData> Could not set data" << endl;
	}	
}
