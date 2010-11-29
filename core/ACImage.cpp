/*
 *  ACImage[loop].cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 11/05/09
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

#if !defined (APPLE_IOS)

#include "ACImage.h"
#include <fstream>

using std::vector;
using std::string;

using std::ofstream;
using std::ifstream;

// ----------- class constants
const int ACImage:: default_thumbnail_width = 64;
const int ACImage:: default_thumbnail_height = 64;


//------------------------------------------------------------------

ACImage::ACImage() : ACMedia() {
	media_type = MEDIA_TYPE_IMAGE;
	thumbnail = 0;
	thumbnail_width = 0;
	thumbnail_height = 0;	
}

ACImage::~ACImage() {
	if (thumbnail) {
		cvReleaseImage(&thumbnail);
		thumbnail = 0;
	}
}

// XS: when we load from file, there is no need to have a pointer to the data passed to the plugin
// XS TODO check this; could combine the following 2 methods...
int ACImage::computeThumbnail(string _fname, int w, int h){
	thumbnail_width = this->checkWidth(w);
	thumbnail_height = this->checkHeight(h);

	IplImage* imgp_full = cvLoadImage(_fname.c_str(), CV_LOAD_IMAGE_COLOR);	
	thumbnail = cvCreateImage(cvSize (thumbnail_width, thumbnail_height), imgp_full->depth, imgp_full->nChannels);
	cvResize(imgp_full, thumbnail, CV_INTER_CUBIC);
	
	if (!thumbnail){
		cerr << "<ACImage::computeThumbnail> problem creating thumbnail" << endl;
		return -1;
	}
	cvReleaseImage(&imgp_full); // because  cvLoadImage == new()
	return 1;
}


int ACImage::computeThumbnail(ACMediaData* data_ptr, int w, int h){
	thumbnail_width = this->checkWidth(w);
	thumbnail_height = this->checkHeight(h);

	IplImage* imgp_full = data_ptr->getImageData();
	thumbnail = cvCreateImage(cvSize (thumbnail_width, thumbnail_height), imgp_full->depth, imgp_full->nChannels);
	cvResize(imgp_full, thumbnail, CV_INTER_CUBIC);

	if (!thumbnail){
		cerr << "<ACImage::computeThumbnail> problem creating thumbnail" << endl;
		return -1;
	}

	// *NOT* cvReleaseImage(&imgp_full); // because there is no new, we just access data_ptr->getImageData();
	return 1;
}

int ACImage::checkWidth(int w){
	if (w < 0){
		cerr << "<ACImage::checkWidth> width should be positive: " << w << endl;
		return -1;
	}
	else if (w == 0){
		w = default_thumbnail_width;
		cout << "<ACImage::checkWidth> using default width: " << w << endl;
	}
	return w;
}

int ACImage::checkHeight(int h){
	if (h < 0){
		cerr << "<ACImage::checkHeight> height should be positive: " << h << endl;
		return -1;
	}
	else if (h == 0){
		h = default_thumbnail_height;
		cout << "<ACImage::checkHeight> using default height: " << h << endl;
	}
	return h;
}

// returns a pointer to the data contained in the image
// AND computes a thumbnail at the same time.
ACMediaData* ACImage::extractData(string fname){
	ACMediaData* image_data = new ACMediaData(MEDIA_TYPE_IMAGE,fname);
	computeThumbnail(image_data, thumbnail_width , thumbnail_height);
	width = thumbnail_width;
	height = thumbnail_height;
	return image_data;
}

void ACImage::setData(IplImage* _data){
	if (data->getMediaType()==MEDIA_TYPE_NONE)
		data = new ACMediaData(MEDIA_TYPE_IMAGE);	
	else
		data->setMediaType(MEDIA_TYPE_IMAGE);
	data->setImageData(_data);
	this->height = _data->height;
	this->width = _data->width;
}

void ACImage::saveACLSpecific(ofstream &library_file) {
	// XS  TODO : or thumbnail width ??
	library_file << width << endl;
	library_file << height << endl;
}

int ACImage::loadACLSpecific(ifstream &library_file) {
	library_file >> width;
	library_file >> height;
	
	if (computeThumbnail(filename, height, width) != 1){
		cerr << "<ACImage::loadACLSpecific> : problem computing thumbnail" << endl;
		return 0;
	}
	
	return 1;
}
#endif//CF APPLE_IOS APPLE_IOS