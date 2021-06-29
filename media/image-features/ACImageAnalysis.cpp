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

const int ACImageAnalysis::standard_width = 640; //320;
const int ACImageAnalysis::standard_height = 480; //240;

// ----------- uncomment this to get visual display using highgui and verbose -----
//#define VISUAL_CHECK
//#define VERBOSE


ACImageAnalysis::ACImageAnalysis(){
}

void ACImageAnalysis::setFileName(const string &filename){
	// XS TODO: test if file exists using boost:filesystem ?
	file_name=filename;	
}

// returns true if it worked, false if not
// original image (imgp_full_map) reduced to imgp_map according to scale 
// (=> use a size appropriate for analysis, not necessarily the full-scale image)
// works whatever color code, whatever nchannels
bool ACImageAnalysis::scaleImage(const cv::Mat& imgp_full_mat, const float _scale){
	if (!imgp_full_mat.data) {
		cerr << "<ACImageAnalysis::scaleImage> : not a valid image !" << endl;
		cerr << "Check file name : " << file_name << endl;
		return false;
	}
	
	// keep original size (for records, it's not really used)
	original_width = imgp_full_mat.cols;
	original_height = imgp_full_mat.rows;
	
	if (_scale <= 0.0){
		if (standard_width !=0 && standard_height !=0 )
			// scale < 0 = automatically rescale image to standard size (const)
			scale = sqrt (standard_width * standard_height * 1.0 / (original_width * original_height));
		else
			scale = 1.0;
	}
	else if (_scale == 1.0 || scale == 1.0) {
		// nothing to do, just set scale parameter to 1
		scale = 1.0;
		imgp_full_mat.copyTo(imgp_mat);
		return check_imgp_mat(); 
	}
	else{
		// scale is specified and > 0, then use it (i.e. overwrites standard size)
		scale = _scale;
	}
	
//	cout << "new scale = " << scale << endl;
	cv::resize(imgp_full_mat, imgp_mat, cv::Size (scale*original_width, scale*original_height), 0, 0, cv::INTER_CUBIC);
	return check_imgp_mat(); 
}

void ACImageAnalysis::check_imgp(){
	try {
		if (!imgp_mat.data) throw(string(" < ACImageAnalysis::check_imgp error> : not a valid image !"));
	}
	catch (const string& not_image_file) {
		cerr << not_image_file << endl;
		exit(1);
	}
}

// checks if the image data is not empty
bool ACImageAnalysis::check_imgp_mat(){
	bool ok = true;
	if (!imgp_mat.data){
		cerr << "<ACImageAnalysis::check_imgp error> : not a valid image !" << endl;
		ok = false;
	}
	return ok;
}

int ACImageAnalysis::getWidth() {
	return this->getCols();
}

int ACImageAnalysis::getHeight() {
	return this->getRows();
}

int ACImageAnalysis::getCols() {
	if (!imgp_mat.data) return 0;
	return imgp_mat.cols;
}

int ACImageAnalysis::getRows() {
	if (!imgp_mat.data) return 0;
	return imgp_mat.rows;
}

int ACImageAnalysis::getDepth() {
	if (!imgp_mat.data) return 0;
	return imgp_mat.depth();
}

cv::Size ACImageAnalysis::getSize(){
	if (!imgp_mat.data)
		return cv::Size(0,0);
	return imgp_mat.size();

}

int ACImageAnalysis::getNumberOfChannels(){	
	if (!imgp_mat.data) return 0;
	return imgp_mat.channels();
}

// ------------------ I/O  -----------------
// XS better: make a class for vector<float> with a >> method ?
void ACImageAnalysis::dumpHuMoments(ostream &odump) {
	cout << "Hu moments for image " << this->getFileName() << endl;
	for (int i = 0; i < int(hu_moments.size()); i++){
		odump << setw(10);
		odump << hu_moments[i];
	}
	odump << endl;
}

void ACImageAnalysis::dumpGaborMoments(ostream &odump) {
	cout << "Gabor moments for image " << this->getFileName() << endl;
	for (int i = 0; i < int(gabor_moments.size()); i++){
		odump << setw(10);
		odump << gabor_moments[i];
	}
	odump << endl;
}

void ACImageAnalysis::dumpColorMoments(ostream &odump) {
	cout << "Color moments for image " << this->getFileName() << endl;
	for (int i = 0; i < int(color_moments.size()); i++){
		odump << setw(10);
		odump << color_moments[i];
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
	cv::imshow(title.c_str(), imgp_mat);
}

void ACImageAnalysis::showInNewWindow(const std::string title){
	cv::namedWindow(title.c_str(), cv::WINDOW_AUTOSIZE);
    cv::imshow(title.c_str(), imgp_mat);
}

void ACImageAnalysis::closeNewWindow(const std::string title){
	cv::destroyWindow(title.c_str());
}

bool ACImageAnalysis::saveInFile (string filename){
	if(!imgp_mat.data) return false;
	bool ok = false;
	if( cv::imwrite(filename, imgp_mat )) {
		cout << filename << " has been written successfully." << endl;
		ok = true;	
	}
	else { 
		cout << "error writing " << filename << endl;
		ok = false;
	}
	return ok;
}


