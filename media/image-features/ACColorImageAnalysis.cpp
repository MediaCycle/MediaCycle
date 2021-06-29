/*
 *  ACColorImageAnalysis.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 8/06/10
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

#include "ACColorImageAnalysis.h"
#include "ACBWImageAnalysis.h"
#include "ACColorImageHistogram.h"

#include <iostream>
#include <cmath>

using std::cerr;
using std::cout;
using std::endl;

// ----------- uncomment this to get visual display using highgui and verbose -----
//#define VISUAL_CHECK
//#define VERBOSE


ACColorImageAnalysis::ACColorImageAnalysis() : ACImageAnalysis(){
	// needs setFileName or scaleImage later...
}


// always load in BGR mode 
// color conversion to HSV only if we compute color moments
ACColorImageAnalysis::ACColorImageAnalysis(const string& filename) : ACImageAnalysis(){
	reset();
	setFileName(filename);
	
	cv::Mat imgp_full_mat = cv::imread(filename, cv::IMREAD_COLOR); // BGR

	// original image (imgp_full_map) reduced to imgp_map
	// this reduces the memory use (imgp_full_map gets destroyed imediately)
	scaleImage(imgp_full_mat);
}

// constructor using opencv 2.* syntax
ACColorImageAnalysis::ACColorImageAnalysis(const cv::Mat& img_full_mat) : ACImageAnalysis(){
	reset();
	// here the filename is unknown
	scaleImage(img_full_mat);
}

/*ACColorImageAnalysis::ACColorImageAnalysis(ACMediaData* image_data) : ACImageAnalysis(){
	reset();
	setFileName(image_data->getFileName());	
	cv::Mat tmp = image_data->getData();
	cv::Mat img_full_mat (tmp);
	scaleImage(img_full_mat);
}*/

void ACColorImageAnalysis::reset(){
	imgp_mat = cv::Mat();
	bw_imgp_mat = cv::Mat();
	channel_img_mat.clear();
	HAS_CHANNELS = HAS_FFT = HAS_BW = false;
	original_width = original_height = 0;
	file_name = "";
}

void ACColorImageAnalysis::clean(){
	// XS TODO refine this !
	// HAS_COLOR_FFT
	if (HAS_FFT) {
		for (int i=0; i<3 ; i++) { 
			if (fft[i] != 0) fftw_free (fft[i]);
		}
	}
}

ACColorImageAnalysis::~ACColorImageAnalysis(){
	clean();
}

void ACColorImageAnalysis::makeBWImage(){
	if (HAS_BW) return;
	else {
		bw_imgp_mat = cv::Mat(this->getSize(), CV_8UC1);
		cv::cvtColor(imgp_mat,bw_imgp_mat, cv::COLOR_BGR2GRAY);
		if(!bw_imgp_mat.data)
			cerr << "<ACColorImageAnalysis::makeBWImage> : BW image empty" << endl;
		else
			HAS_BW = true;
	}
}


bool ACColorImageAnalysis::splitChannels(const std::string& cmode){ // "BGR" by default
	if (!imgp_mat.data){
		cerr << " <ACColorImageAnalysis::SplitChannels> : no image loaded yet " << endl;
		HAS_CHANNELS = false;
	}
	
	if (cmode == "BGR") {
		if (HAS_CHANNELS){
			cout << " Channels have already been split. Doing Nothing. " << endl;
		}
		else {
			// regular split on image that had not been split before
			channel_img_mat.clear(); // just to make sure
			for (int i = 0; i < 3; i++)
				channel_img_mat.push_back(cv::Mat (this->getSize(),CV_8UC1));
			cv::split(imgp_mat, channel_img_mat);
			HAS_CHANNELS = true;
		}
	}
	
	// split mode = HSV 
	else if (cmode == "HSV"){		
		removeChannels();
		HAS_CHANNELS = false;
		cv::Mat tmp_im_mat(this->getSize(),CV_8UC3); // tmp image for storing 3-channels image with new color model 
		cv::cvtColor(imgp_mat, tmp_im_mat, cv::COLOR_BGR2HSV);
		for (int i = 0; i < 3; i++)
			channel_img_mat.push_back(cv::Mat (this->getSize(),CV_8UC1));
		cv::split(tmp_im_mat, channel_img_mat);		
		HAS_CHANNELS = true;
	}
	else {
		cerr << " <ACColorImageAnalysis::SplitChannels> : unsupported color format " << cmode << endl;
		HAS_CHANNELS = false;
		return false;
	}
	for (int i = 0; i < 3; i++){
		if (!channel_img_mat[i].data) {
			cerr << "<ACColorImageAnalysis::splitChannels> : no data in channel " << i << endl;
			HAS_CHANNELS = false;
		}
	}
	return HAS_CHANNELS;
}

// index i starts at 0
cv::Mat ACColorImageAnalysis::getChannel(int i){
	if (!HAS_CHANNELS) splitChannels(); // default : RGB	
	if (!imgp_mat.data){
		cerr << " <ACColorImageAnalysis::getChannel> : no image loaded yet " << endl;
		return cv::Mat();
	}
	if (i >= this->getNumberOfChannels() || i < 0) {
		cerr << "<ACColorImageAnalysis::getChannel> : channel index out of range" << endl;
		return cv::Mat();
	}
	return channel_img_mat[i];
}

void ACColorImageAnalysis::removeChannels(){
	HAS_CHANNELS = false;
	channel_img_mat.clear();
}

void ACColorImageAnalysis::computeFFT2D (const string& cmode){	
	int height = this->getHeight();
	int width = this->getWidth();

	splitChannels(cmode);
	const int cdim = height * (width / 2 + 1);
	const int area = width * height;
	double data_in[area];
	uchar* ddata;
	for	(int i = 0; i < 3; i++){	
		try {
			fft[i] = new fftw_complex [cdim];
		}
		catch (std::bad_alloc) {
			cerr << "Memory allocation problem in fft2d for fft of channel " << i << endl;
		}
		ddata = (uchar*) channel_img_mat[i].data;

		//		ddata = (uchar*) channel_img[i].data;
		for (int j = 0; j < area; j++)
			data_in[j] = (double) ddata[j];
		fftw_plan p = fftw_plan_dft_r2c_2d(height, width, data_in, fft[i], FFTW_ESTIMATE); 
		fftw_execute(p);
		fftw_destroy_plan(p);
	}
}

void ACColorImageAnalysis::computeFFT2D_centered (const string& cmode){
	// 0 0 will appear at the center of the FFT image
	int height = this->getHeight();
	int width = this->getWidth();

 	const int cdim = height * (width / 2 + 1);
	const int area = width * height;
	double data_in[area];
	uchar* ddata;
	splitChannels(cmode);
	for (int i = 0; i < 3; i++){	
		try {
			fft[i] = new fftw_complex [cdim];
		}
		catch (std::bad_alloc) {
			cerr << "Memory allocation problem in fft2d for fft of channel " << i << endl;
		}
		//XS TODO check this 2.*
		ddata = (uchar*) channel_img_mat[i].data;
		int k=0;
		for(int irow=0; irow< height; irow++){
			for (int icolumn=0; icolumn< width; icolumn++){
				data_in[k] = double(pow(float(-1),float(irow+icolumn)) * (double)(ddata[irow*width+icolumn] +128));
				k++;
			}
		}		
		
		fftw_plan p = fftw_plan_dft_r2c_2d(height, width, data_in, fft[i], FFTW_ESTIMATE); 
		fftw_execute(p);
		fftw_destroy_plan(p);
	}
}

// ------------------ features computation -----------------
void ACColorImageAnalysis::computeHuMoments(const int& thresh){ 
	makeBWImage();
	ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(this->getBWImageMat());
	bw_helper->computeHuMoments(thresh);
	hu_moments = bw_helper->getHuMoments();
	delete bw_helper;
}

void ACColorImageAnalysis::computeFourierPolarMoments(const int& RadialBins, const int& AngularBins){ 
	makeBWImage();
	ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(this->getBWImageMat());
	bw_helper->computeFourierPolarMoments(RadialBins, AngularBins);
	fourier_polar_moments = bw_helper->getFourierPolarMoments();
	delete bw_helper;
}

void ACColorImageAnalysis::computeFourierMellinMoments(){ 
	makeBWImage();
	ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(this->getBWImageMat());
	bw_helper->computeFourierMellinMoments();
	fourier_mellin_moments = bw_helper->getFourierMellinMoments();
	delete bw_helper;
}

// XS TODO: this is BW histogram !
//void ACColorImageAnalysis::computeImageHistogram(const int& w, const int& h){ 
//	makeBWImage();
//	ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(this->getBWImage());
//	bw_helper->computeImageHistogram(w, h);
//	image_histogram = bw_helper->getImageHistogram();
//	delete bw_helper;
//}

void ACColorImageAnalysis::computeContourHuMoments(const int& thresh){ 
	makeBWImage();
	ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(this->getBWImageMat());
	bw_helper->computeContourHuMoments(thresh);
	contour_hu_moments = bw_helper->getContourHuMoments();
	delete bw_helper;
}

void ACColorImageAnalysis::computeGaborMoments(const int& mumax,  const int& numax){ // default 7, 5
	makeBWImage();
	ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(this->getBWImageMat());
	bw_helper->computeGaborMoments(mumax,numax);
	gabor_moments = bw_helper->getGaborMoments();
	delete bw_helper;
}

void ACColorImageAnalysis::computeGaborMoments_fft(const int& numPha_, const int& numFreq_, uint horizonalMargin_, uint verticalMargin_){ // default 7, 5, 0, 0
	makeBWImage();
	ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(this->getBWImageMat());
	bw_helper->computeGaborMoments_fft(numPha_, numFreq_, horizonalMargin_, verticalMargin_);
	gabor_moments = bw_helper->getGaborMoments();
	delete bw_helper;
}

// n = number of moments to compute (default n = 4)
// cm = color model to be used (default cm = "HSV")
void ACColorImageAnalysis::computeColorMoments(const int& n, const string& cm){
	color_moments.clear();
	ACColorImageHistogram* tmp_hist;
	if (cm=="HSV"){
		cv::Mat hsv_mat;
		cv::cvtColor(imgp_mat, hsv_mat, cv::COLOR_BGR2HSV);
		tmp_hist= new ACColorImageHistogram(hsv_mat, cm);
	}
	else //if (cm =="BGR")
		tmp_hist= new ACColorImageHistogram(this, "BGR");
	// old way:
	//ACColorImageHistogram* tmp_hist  = new ACColorImageHistogram(imgp_mat, "BGR"); // XS TODO 2.* : imgp_mat
	
	tmp_hist->computeMoments(n);
	
	for (int i=1;i<=n;i++){
		for (int channel=0; channel<this->getNumberOfChannels(); channel++){
			color_moments.push_back (tmp_hist->getMoment(i)[channel]) ;
#ifdef VERBOSE
			cout << tmp_hist->getMoment(i)[channel] << endl;
#endif // VERBOSE
		}	
	}
#ifdef VISUAL_CHECK
	tmp_hist->show();
#endif // VISUAL_CHECK
	
#ifdef VERBOSE
	tmp_hist->showStats();
#endif // VERBOSE
	delete tmp_hist;	
}

// XS TODO add parameters
// probabilistic Hough Lines
void ACColorImageAnalysis::computeHoughLinesP(){
	number_of_hough_linesp.clear();
	cv::Mat src = this->getImageMat();	
	cv::Size size = src.size();
	
	cv::Mat src2(size, CV_8UC1);
	cv::cvtColor( src, src2, cv::COLOR_BGR2GRAY );
	
	cv::Mat dst(size, CV_8UC1);

	cv::Canny( src2, dst, 70, 230, 3);
	vector<cv::Vec4i> lines;
	cv::HoughLinesP( dst, lines, 1, CV_PI/180, 80, 30, 10 );
	number_of_hough_linesp.push_back(lines.size());
#ifdef VERBOSE
	cout << "HoughLinesP detected " << lines.size() << " lines" << endl;
#endif // VERBOSE

#ifdef VISUAL_CHECK
	cv::Mat color_dst;
	cv::cvtColor( dst, color_dst, CV_GRAY2BGR );

	for( size_t i = 0; i < lines.size(); i++ )
	{
		line( color_dst, cv::Point(lines[i][0], lines[i][1]),
			 cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0,0,255), 2, 8 );
	}
	
	cv::namedWindow( "Source", 1 );
	cv::imshow( "Source", src );
	cv::namedWindow( "Detected Lines", 1 );
	cv::imshow( "Detected Lines", color_dst );
	cv::waitKey(0);
#endif // VISUAL_CHECK

}

// XS TODO add parameters
// standard Hough Lines
void ACColorImageAnalysis::computeHoughLines(){
	number_of_hough_lines.clear();
	cv::Mat src = this->getImageMat();	
	cv::Size size = src.size();
	
	cv::Mat src2(size, CV_8UC1);
	cv::cvtColor( src, src2, cv::COLOR_BGR2GRAY );
	
	cv::Mat dst(size, CV_8UC1);
	
	cv::Canny( src2, dst, 70, 230, 3);
	vector<cv::Vec2f> lines;
	cv::HoughLines( dst, lines, 1, CV_PI/180, 100 );	
	number_of_hough_lines.push_back(lines.size());
#ifdef VERBOSE
	cout << "HoughLines detected " << lines.size() << " lines" << endl;
#endif // VERBOSE
	
#ifdef VISUAL_CHECK
	cv::Mat color_dst;
	cv::cvtColor( dst, color_dst, CV_GRAY2BGR );
	for( size_t i = 0; i < lines.size(); i++ ){
		float rho = lines[i][0];
		float theta = lines[i][1];
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		cv::Point pt1(round(x0 + 1000*(-b)),
					  round(y0 + 1000*(a)));
		cv::Point pt2(round(x0 - 1000*(-b)),
					  round(y0 - 1000*(a)));
		cv::line( color_dst, pt1, pt2, cv::Scalar(0,0,255), 3, 8 );
	}
	
	cv::namedWindow( "Source", 1 );
	cv::imshow( "Source", src );
	cv::namedWindow( "Detected Lines", 1 );
	cv::imshow( "Detected Lines", color_dst );
	cv::waitKey(0);
#endif // VISUAL_CHECK
}

// cf. detectAndDrawFaces + detectManyObjects/detectLargestObject
// * scale = How much to shrink the image before face detection (to run faster)
//           A value upto 1.3f will make it run faster and detect faces almost as reliably.
// * flags = cv::CASCADE_SCALE_IMAGE 	// detectManyObjects
// * flags = CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_DO_ROUGH_SEARCH // detectLargestObject
//           For Haar detectors, detectLargestObject() should be faster than detectManyObjects().
// * cv::Size minFeatureSize = cv::Size(20, 20); // Smallest object size.
// * float searchScaleFactor = 1.2; // How detailed should the search be. Must be larger than 1.0.
// * int minNeighbors = 4;	// How much the detections should be filtered out. This should depend on how bad false detections are to your system. 
                            // minNeighbors=2 means lots of good+bad detections, and minNeighbors=6 means only good detections are given but some are missed.



void ACColorImageAnalysis::computeNumberOfFaces(const string& cascadeFile, const float& scale, const float& searchScaleFactor, const int& minNeighbors, const int& flags, const cv::Size& minFeatureSize){
	number_of_faces.clear();
	vector<cv::Rect> faces;
	cv::CascadeClassifier classif;
	bool showRect = true ; // display rectangle around face if true, otherwise circle
	cv::Scalar color = CV_RGB(0,255,0) ; // green
	int thickness = 2;
//	string cascadeFileHaar = "/Users/xavier/numediart/Project14.5-DiskHover/tests/haarcascade_frontalface_alt.xml";
//	string cascadeFileLBP = "/Users/xavier/numediart/Project14.5-DiskHover/tests/lbpcascade_frontalface.xml";

	if( !classif.load(cascadeFile)) {
		cerr << "<ACColorImageAnalysis::computeNumberOfFaces> Could not load classifier cascade: " << cascadeFile << endl;
		return;
	}
#ifdef VISUAL_CHECK
	string title="FaceDetector";
	cv::namedWindow(title);
#endif // VISUAL_CHECK

	cv::Mat imgp_mat = this->getImageMat();	
	cv::Mat gray;
	
	cv::cvtColor(imgp_mat,gray,cv::COLOR_BGR2GRAY);
	cv::Size size = imgp_mat.size();
	
	bool scaled=false;
	// Possibly shrink the image, to run faster.
	if (scale < 0.9999f || scale > 1.0001f){
		int smallWidth = round(imgp_mat.cols/scale);
		int smallHeight = round(imgp_mat.rows/scale);
		cv::resize( gray, gray, cv::Size(smallHeight,smallWidth), 0,0, cv::INTER_LINEAR );
		scaled=true;
	}
	
	// Standardize the brightness and contrast, so that dark images look better.
	cv::equalizeHist( gray, gray );
	
	// Detect objects in the small greyscale image.
	classif.detectMultiScale( gray, faces, searchScaleFactor, minNeighbors, flags, minFeatureSize );
	
	// Resize the results if the image was temporarily scaled smaller
	if (scaled) {
		vector<cv::Rect>::iterator r;
		for (r = faces.begin(); r != faces.end(); r++ )
		{
			r->x = round(r->x * scale);
			r->y = round(r->y * scale);
			r->width = round(r->width * scale);
			r->height = round(r->height * scale);
		}
	}
	
	number_of_faces.push_back(faces.size());
#ifdef VERBOSE
	cout << "computeNumberOfFaces detected " << faces.size() << " faces" << endl;
#endif // VERBOSE
	
#ifdef VISUAL_CHECK
	cv::Mat imgOut; 
	imgp_mat.copyTo(imgOut);

	// Loop through each of the detected faces, to display them.
	for( vector<cv::Rect>::const_iterator r = faces.begin(); r != faces.end(); r++ )
	{
		if (showRect)
		{
			// Render a rectangle
			cv::Point topleft, bottomright;
			topleft.x = r->x;
			topleft.y = r->y;
			bottomright.x = r->x + r->width - 1;
			bottomright.y = r->y + r->height - 1;
			cv::rectangle( imgOut, topleft, bottomright, color, thickness, CV_AA);	// Draw anti-aliased lines
		}
		else
		{
			// Render a circle
			cv::Point center;
			int radius;
			center.x = r->x + cvRound(r->width/2.0f);
			center.y = r->y + cvRound(r->height/2.0f);
			radius = cvRound(r->width/2.0f);
			cv::circle( imgOut, center, radius, color, thickness, CV_AA);	// Draw anti-aliased circles
		}
	}  
	
	cv::imshow( title, imgOut );
	cout << "Press any key in the window to continue ..." << endl;
	cv::waitKey(0);
	cv::destroyWindow(title);	
	
#endif // VISUAL_CHECK

}




// ------------------ visual output functions -----------------
void ACColorImageAnalysis::showChannels(const string& cmode){
	// cmode default = "BGR"
	splitChannels(cmode);
	cv::namedWindow("0", cv::WINDOW_AUTOSIZE);
	cv::imshow("0", channel_img_mat[0]);
	cv::namedWindow("1", cv::WINDOW_AUTOSIZE);
	cv::imshow("1", channel_img_mat[1]);
	cv::namedWindow("2", cv::WINDOW_AUTOSIZE);
	cv::imshow("2", channel_img_mat[2]);
	cv::waitKey(0);
	cv::destroyWindow("0");
	cv::destroyWindow("1");
	cv::destroyWindow("2");	
}

void ACColorImageAnalysis::showFFTInWindow(const string& title){
	int height = this->getHeight();
	int width = this->getWidth();

	cv::Mat fftimage[3];
	string t[3];
	
	for(int ic=0; ic< 3; ic++){
		std::ostringstream oss;
		fftimage[ic] = cv::Mat(width,height,8,1);
		oss << title << ic;
		t[ic] = oss.str();
	}
	
	for(int ic=0; ic< 3; ic++){
		cv::namedWindow(t[ic].c_str() ,cv::WINDOW_AUTOSIZE);
		int k = 0;
		for(int i=0; i< width; i++){
			for (int j=0; j< height; j++){
				fftimage[ic].data[i*width + j] = (uchar)(sqrt(fft[ic][k][0]*fft[ic][k][0]+fft[ic][k][1]*fft[ic][k][1]));
				k++;
			}
		}
		cv::waitKey(0);
		cv::imshow(t[ic].c_str(),fftimage[ic]);
	}
	cv::waitKey(0);
	for(int ic=0; ic< 3; ic++){
		//cvReleaseImage(&fftimage[ic]);
		cv::destroyWindow(t[ic].c_str());
	}
}



