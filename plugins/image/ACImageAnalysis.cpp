/*
 *  ACImageAnalysis.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 8/10/09
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

// when loading an image, *always* create
// 1) BW image
// 2) 3 channels (BGR by default, HSV optional)

#include "ACImageAnalysis.h"
#include <iostream>
#include <iomanip> // for setw
#include <cmath>

using namespace std;

//const double PI = 4 * std::atan(1);

const int ACImageAnalysis::standard_width = 0; // 16 ?
const int ACImageAnalysis::standard_height = 0;

// ----------- uncomment this to get visual display using highgui and verbose -----
#define VISUAL_CHECK
#define VERBOSE


ACImageAnalysis::ACImageAnalysis(){
}

void ACImageAnalysis::setFileName(const string &filename){
	// XS TODO: test if file exists using boost:filesystem ?
	file_name=filename;	
}

int ACImageAnalysis::scaleImage(IplImage* imgp_full, float _scale){
	// returns 1 if it worked, 0 if not
	// original image reduced to imgp and destroyed
	// works whatever color code, whatever nchannels
	try {
		if (!imgp_full) {
			cerr << "Check file name : " << file_name << endl;
			throw(string(" <ACImageAnalysis::scaleImage> : not a valid image !"));
		}
	}
	catch (const string& not_image_file) {
		cerr << not_image_file << endl;
		return 0;
	}
	
	// nothing to do, just set scale parameter to 1
	if (_scale = 1.0) {
		scale = 1.0;
		imgp=cvCloneImage(imgp_full);
		check_imgp(); 
		return 1;
	}
	// scale < 0 = automatically rescale image to standard size (const)
	else if (_scale <= 0.0 && standard_width !=0 && standard_height !=0 )
		scale = sqrt (standard_width * standard_height * 1.0 / (original_width * original_height));
	else
		// scale is specified and > 0, then use it (i.e. overwrites standard size)
		scale = _scale;
	
	// keep original size (for records, it's not really used)
	original_width = imgp_full->width;
	original_height = imgp_full->height;
	
	imgp = cvCreateImage(cvSize (scale*original_width, scale*original_height), imgp_full->depth, imgp_full->nChannels);
	cvResize(imgp_full, imgp, CV_INTER_CUBIC);
	check_imgp(); 
	return 1;
	
	// old comment, keep an eye on it:
	// SD TODO - This is stange cvCreateImage creates image with as a widthStep of 152
	// imgp->widthStep = imgp->width * imgp->nChannels;
	// imgp->imageSize = imgp->width * imgp->height * imgp->nChannels;
}

void ACImageAnalysis::check_imgp(){
	try {
		if (!imgp) throw(string(" < ACImageAnalysis::check_imgp error> : not a valid image !"));
	}
	catch (const string& not_image_file) {
		cerr << not_image_file << endl;
		exit(1);
	}
}

int ACImageAnalysis::getWidth() {
	if (imgp == NULL) return 0;
	return imgp->width;
}

int ACImageAnalysis::getHeight() {
	if (imgp == NULL) return 0;
	return imgp->height;
}

int ACImageAnalysis::getDepth() {
	if (imgp == NULL) return 0;
	return imgp->depth;
}

int ACImageAnalysis::getNumberOfChannels(){	
	if (imgp == NULL) return 0;
	return imgp->nChannels;
}

// ------------------ I/O  -----------------
// XS better: make a class for vector<float> with a >> method ?
void ACImageAnalysis::dumpHuMoments(ostream &odump) {
	for (int i = 0; i < int(hu_moments.size()); i++){
		odump << setw(10);
		odump << hu_moments[i];
	}
	odump << endl;
}

void ACImageAnalysis::dumpContourHuMoments(ostream &odump) {
	for (int i = 0; i < int(contour_hu_moments.size()); i++){
		odump << setw(10);
		odump << contour_hu_moments[i];
	}
	odump << endl;
}

void ACImageAnalysis::dumpRawMoments(ostream &odump) {
	for (int i = 0; i < int(raw_moments.size()); i++){
		odump << setw(10);
		odump << raw_moments[i];
	}
	odump << endl;
}

void ACImageAnalysis::dumpFourierPolarMoments(ostream &odump) {
	for (int i = 0; i < int(fourier_polar_moments.size()); i++){
		odump << setw(10);
		odump << fourier_polar_moments[i] << " ";
	}
	odump << endl;
}

void ACImageAnalysis::dumpFourierMellinMoments(ostream &odump) {
	for (int i = 0; i < int(fourier_mellin_moments.size()); i++){
		odump << setw(10);
		odump << fourier_mellin_moments[i] << " ";
	}
	odump << endl;
}

// ------------------ visual output  -----------------

void ACImageAnalysis::showInWindow(const std::string title){
    cvShowImage(title.c_str(), imgp);
}

void ACImageAnalysis::showInNewWindow(const std::string title){
	cvNamedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);
    cvShowImage(title.c_str(), imgp);
}

void ACImageAnalysis::closeNewWindow(const std::string title){
	cvDestroyWindow(title.c_str());
}

int ACImageAnalysis::saveInFile (string filename){
	if(imgp == NULL) return 0;
	const char *filename_char = filename.c_str();
	if( cvSaveImage(filename_char, imgp )) {
		cout << filename << " has been written successfully." << endl;
		return 1;	
	}
	else { 
		cout << "error writing " << filename << endl;
		return -1;
	}
	return 0;
}


