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

ACColorImageAnalysis::ACColorImageAnalysis(const string &filename, string _cmode) : ACImageAnalysis(){
	reset();
	setFileName(filename);
	IplImage *imgp_full;
	if (_cmode == "BGR" || _cmode == "RGB"){
		imgp_full = cvLoadImage(file_name.c_str(), CV_LOAD_IMAGE_COLOR);
		// XS TODO check this
		color_model="BGR";
	}
	else if (_cmode == "HSV")
		cout << "XS TODO : load directly in HSV" << endl;
	
	scaleImage(imgp_full);
	cvReleaseImage(&imgp_full);
}

ACColorImageAnalysis::ACColorImageAnalysis(IplImage* img, string _cmode) : ACImageAnalysis(){
	reset();
	// here the filename is unknown
	scaleImage(img);
	
	// XS TODO check color model 
	color_model="BGR";	
}

ACColorImageAnalysis::ACColorImageAnalysis(cv::Mat img, string _cmode) : ACImageAnalysis(){
	reset();
	// here the filename is unknown
	// XS TODO migrate to 2.3
	IplImage tmp_img = img;
	scaleImage(&tmp_img);
	
	// XS TODO check color model 
	color_model="BGR";	
}

ACColorImageAnalysis::ACColorImageAnalysis(ACMediaData* image_data, string _cmode) : ACImageAnalysis(){
	reset();
	setFileName(image_data->getFileName());	
	IplImage* img = static_cast<IplImage*> (image_data->getData());
	scaleImage(img);
	
	// XS TODO check color model 
	color_model="BGR";
	
}

void ACColorImageAnalysis::reset(){
	imgp = 0;
	bw_imgp = 0;
	for (int i=0; i<3 ; i++) { 
		channel_img[i] = 0;
		fft[i] = 0;
	}
	HAS_CHANNELS = HAS_FFT = HAS_BW = false;
	original_width = original_height = 0;
	file_name = color_model = "";
}

void ACColorImageAnalysis::clean(){
	if (imgp != 0) cvReleaseImage(&imgp);
	if (bw_imgp != 0) cvReleaseImage(&bw_imgp);
	if (HAS_CHANNELS) {
		for (int i=0; i<3 ; i++) { 
			if (channel_img[i] != 0) cvReleaseImage (&channel_img[i]); 
		}
	}
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
		bw_imgp = cvCreateImage (cvSize (this->getWidth(), this->getHeight()), IPL_DEPTH_8U, 1); 
		cvCvtColor (imgp, bw_imgp, CV_BGR2GRAY);
		HAS_BW = true;
	}
}


int ACColorImageAnalysis::splitChannels(std::string cmode){ // or HSV
	int height = this->getHeight();
	int width = this->getWidth();
	int depth = this->getDepth();

	if (imgp == 0){
		cerr << " <ACColorImageAnalysis::SplitChannels> : no image loaded yet " << endl;
		return 0;
	}
	
	if (cmode == color_model) {
		if (HAS_CHANNELS){
			cout << " Channels have already been split. Doing Nothing. " << endl;
			return 1;
		}
		else {
			// regular split on image that had not been split before
			for (int i = 0; i < 3; i++)
				channel_img[i] = cvCreateImage (cvSize (width, height), depth, 1);
			cvSplit (imgp, channel_img[0], channel_img[1], channel_img[2], 0); 
			HAS_CHANNELS = true;
			return 1;
		}
	}
	// reverting split mode from BRG to HSV or vice-versa
	else {
		cout << "cmode :" << cmode << endl;
		cout << "color_model :" << color_model << endl;
		
		removeChannels();
		HAS_CHANNELS = false;
		IplImage *tmp_im = cvCreateImage (cvGetSize(imgp), IPL_DEPTH_8U, 3); 
		if (cmode == "HSV" && color_model == "BGR") {
			cvCvtColor (imgp, tmp_im, CV_BGR2HSV);
			color_model = "HSV";
		}
		else if (cmode == "BGR" && color_model == "HSV") {
			cvCvtColor (imgp, tmp_im, CV_HSV2BGR);
			color_model = "BGR";
		}
		else {
			cerr << " <ACColorImageAnalysis::SplitChannels> : unsupported color format: should be BGR or HSV " << endl;
			return 0;
		}
		for (int i = 0; i < 3; i++)
			channel_img[i] = cvCreateImage (cvSize (width, height), depth, 1);
		cvSplit (tmp_im, channel_img[0], channel_img[1], channel_img[2], 0); 
		HAS_CHANNELS = true;
		cvReleaseImage( &tmp_im );
		return 1;
	}
}

IplImage** ACColorImageAnalysis::getChannel(int i){
	if (!HAS_CHANNELS) splitChannels(); // default : RGB	
	if (imgp == 0){
		cerr << " <ACColorImageAnalysis::SplitChannels> : no image loaded yet " << endl;
		return 0;
	}
	if (i > this->getNumberOfChannels()-1 || i < 0) {
		cerr << "channel index out of range" << endl;
		return 0;
	}
	return &channel_img[i];
}

void ACColorImageAnalysis::removeChannels(){
	HAS_CHANNELS = false;
	for (int i = 0; i < 3; i++){
		cvReleaseImage(&channel_img[i]);
	}
}

void ACColorImageAnalysis::computeFFT2D (string cmode){	
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
		ddata = (uchar*) channel_img[i]->imageData;
		for (int j = 0; j < area; j++)
			data_in[j] = (double) ddata[j];
		fftw_plan p = fftw_plan_dft_r2c_2d(height, width, data_in, fft[i], FFTW_ESTIMATE); 
		fftw_execute(p);
		fftw_destroy_plan(p);
	}
}

void ACColorImageAnalysis::computeFFT2D_centered (string cmode){
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
		ddata = (uchar*) channel_img[i]->imageData;
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
void ACColorImageAnalysis::computeHuMoments(int thresh){ 
	makeBWImage();
	ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(this->getBWImage());
	bw_helper->computeHuMoments(thresh);
	hu_moments = bw_helper->getHuMoments();
	// XS debug
	// bw_helper->dumpHuMoments(cout);

	delete bw_helper;
}

void ACColorImageAnalysis::computeFourierPolarMoments(int RadialBins, int AngularBins){ 
	makeBWImage();
	ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(this->getBWImage());
	bw_helper->computeFourierPolarMoments(RadialBins, AngularBins);
	fourier_polar_moments = bw_helper->getFourierPolarMoments();
	delete bw_helper;
}

void ACColorImageAnalysis::computeFourierMellinMoments(){ 
	makeBWImage();
	ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(this->getBWImage());
	bw_helper->computeFourierMellinMoments();
	fourier_mellin_moments = bw_helper->getFourierMellinMoments();
	delete bw_helper;
}

// XS TODO: this is BW histogram !
void ACColorImageAnalysis::computeImageHistogram(int w, int h){ 
	makeBWImage();
	ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(this->getBWImage());
	bw_helper->computeImageHistogram(w, h);
	image_histogram = bw_helper->getImageHistogram();
	delete bw_helper;
}

void ACColorImageAnalysis::computeContourHuMoments(int thresh){ 
	makeBWImage();
	ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(this->getBWImage());
	bw_helper->computeContourHuMoments(thresh);
	contour_hu_moments = bw_helper->getContourHuMoments();
	delete bw_helper;
}

void ACColorImageAnalysis::computeGaborMoments(int mumax, int numax){ // default 7, 5
	makeBWImage();
	ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(this->getBWImage());
	bw_helper->computeGaborMoments(mumax,numax);
	gabor_moments = bw_helper->getGaborMoments();
	delete bw_helper;
}

void ACColorImageAnalysis::computeGaborMoments_fft(int numPha_, int numFreq_, uint horizonalMargin_, uint verticalMargin_){ // default 7, 5, 0, 0
	makeBWImage();
	ACBWImageAnalysis *bw_helper = new ACBWImageAnalysis(this->getBWImage());
	bw_helper->computeGaborMoments_fft(numPha_, numFreq_, horizonalMargin_, verticalMargin_);
	gabor_moments = bw_helper->getGaborMoments();
	delete bw_helper;
}

// n = number of moments to compute (default n = 4)
// cm = color model to be used (default cm = "HSV")
void ACColorImageAnalysis::computeColorMoments(int n, string cm){
	color_moments.clear();
	//ACColorImageHistogram* tmp_hist  = new ACColorImageHistogram(this, "HSV");
	// old way:
	ACColorImageHistogram* tmp_hist  = new ACColorImageHistogram(imgp, "BGR");
	
	//tmp_hist->normalize(1);
	//tmp_hist->computeStats();
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


// ------------------ visual output functions -----------------
void ACColorImageAnalysis::showChannels(string cmode){
	// cmode default = "BGR"
	splitChannels(cmode);
	cvNamedWindow("0", CV_WINDOW_AUTOSIZE);
    cvShowImage("0", channel_img[0]);
	cvNamedWindow("1", CV_WINDOW_AUTOSIZE);
    cvShowImage("1", channel_img[1]);
	cvNamedWindow("2", CV_WINDOW_AUTOSIZE);
    cvShowImage("2", channel_img[2]);
	cvWaitKey(0); // XS remove ?
	cvDestroyWindow("0");
	cvDestroyWindow("1");
	cvDestroyWindow("2");	
}

void ACColorImageAnalysis::showFFTInWindow(const std::string title){
	int height = this->getHeight();
	int width = this->getWidth();

	IplImage* fftimage[3];
	string t[3];
	
	for(int ic=0; ic< 3; ic++){
		std::ostringstream oss;
		fftimage[ic] = cvCreateImage(cvSize(width,height),8,1);
		oss << title << ic;
		t[ic] = oss.str();
	}
	
	for(int ic=0; ic< 3; ic++){
		cvNamedWindow(t[ic].c_str() ,CV_WINDOW_AUTOSIZE);
		int k = 0;
		for(int i=0; i< width; i++){
			for (int j=0; j< height; j++){
				fftimage[ic]->imageData[i*width + j] = (uchar)(sqrt(fft[ic][k][0]*fft[ic][k][0]+fft[ic][k][1]*fft[ic][k][1]));
				k++;
			}
		}
		cvWaitKey(0);
		cvShowImage(t[ic].c_str(),fftimage[ic]);
	}
	cvWaitKey(0);
	for(int ic=0; ic< 3; ic++){
		cvReleaseImage(&fftimage[ic]);
		cvDestroyWindow(t[ic].c_str());
	}
}



