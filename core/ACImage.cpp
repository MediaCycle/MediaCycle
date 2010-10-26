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
// XS TODO check this
int ACImage::computeThumbnail(string _fname, int w, int h){
	if (w <=0 || h <=0){
		cerr << "<ACImage::computeThumbnail> dimensions should be positive: " << w << " x " << h << endl;
		return -1;
	}
	IplImage* imgp_full = cvLoadImage(_fname.c_str(), CV_LOAD_IMAGE_COLOR);	
	thumbnail = cvCreateImage(cvSize (w, h), imgp_full->depth, imgp_full->nChannels);
	cvResize(imgp_full, thumbnail, CV_INTER_CUBIC);
	
	if (!thumbnail){
		cerr << "<ACImage::computeThumbnail> problem creating thumbnail" << endl;
		return -1;
	}
	thumbnail_width = w;
	thumbnail_height = h;
	cvReleaseImage(&imgp_full); // because  cvLoadImage == new()
	return 1;
}


int ACImage::computeThumbnail(ACMediaData* data_ptr, int w, int h){
	if (w <=0 || h <=0){
		cerr << "<ACImage::computeThumbnail> dimensions should be positive: " << w << " x " << h << endl;
		return -1;
	}
	IplImage* imgp_full = data_ptr->getImageData();
	thumbnail = cvCreateImage(cvSize (w, h), imgp_full->depth, imgp_full->nChannels);
	cvResize(imgp_full, thumbnail, CV_INTER_CUBIC);

	if (!thumbnail){
		cerr << "<ACImage::computeThumbnail> problem creating thumbnail" << endl;
		return -1;
	}
	thumbnail_width = w;
	thumbnail_height = h;
	// *NOT* cvReleaseImage(&imgp_full); // because there is no new, we just access data_ptr->getImageData();
	return 1;
}

ACMediaData* ACImage::extractData(string fname){
	
	// XS todo : store the default header (16 or 64 below) size somewhere...
	ACMediaData* image_data = new ACMediaData(fname, MEDIA_TYPE_IMAGE);
	computeThumbnail(image_data, 64, 64);
	width = thumbnail_width;
	height = thumbnail_height;
	return image_data;
}

void ACImage::saveACLSpecific(ofstream &library_file) {
	
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