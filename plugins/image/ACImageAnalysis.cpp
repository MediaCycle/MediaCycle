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

#include "ACImageAnalysis.h"
#include <iostream>
#include <cmath>

using std::cerr;
using std::cout;
using std::endl;
//const double PI = 4 * std::atan(1);

const int ACImageAnalysis::standard_width = 128; // 16 ?
const int ACImageAnalysis::standard_height = 128;

// ----------- uncomment this to get visual display using highgui and verbose -----
//#define VISUAL_CHECK
//#define VERBOSE


ACImageAnalysis::ACImageAnalysis(){
	clean();
	// needs setFileName or setImage later...
}

ACImageAnalysis::ACImageAnalysis(const string &filename){
	clean();
	setFileName(filename);
	IplImage *imgp_full = cvLoadImage(file_name.c_str(), CV_LOAD_IMAGE_COLOR);
	setImage(imgp_full);
	cvReleaseImage(&imgp_full);

}

ACImageAnalysis::ACImageAnalysis(IplImage* img){
	clean();
	// here the filename is unknown.	
	setImage(img);
}


void ACImageAnalysis::clean(){
	imgp = NULL;
	for (int i=0; i<3 ; i++) { 
		channel_img[i] = NULL;
		fft[i] = NULL;
	}
	HAS_CHANNELS = HAS_FFT = false;
	width = height = step = depth = channels = 0;
	original_width = original_height = 0;
	file_name = color_model = "";
}
	
ACImageAnalysis::~ACImageAnalysis(){
	if (imgp != NULL) cvReleaseImage(&imgp);
	if (HAS_CHANNELS) {
		for (int i=0; i<3 ; i++) { 
			if (channel_img[i] != NULL) cvReleaseImage (&channel_img[i]); 
		}
	}
	if (HAS_FFT) {
		for (int i=0; i<3 ; i++) { 
			if (fft[i] != NULL) fftw_free (fft[i]);
		}
	}
}

void ACImageAnalysis::setFileName(const string &filename){
	// XS TODO: test if file exists ?
	file_name=filename;	
}

int ACImageAnalysis::setImage(IplImage* imgp_full, float _scale){
	// returns 1 if it worked, 0 if not
	// original image reduced to imgp and destroyed
	try {
		if (!imgp_full) {
			cerr << "Check file name : " << file_name << endl;
			throw(string(" <ACAnalysedImage::setImage> CV_LOAD_IMAGE_COLOR : not a color image !"));
		}
	}
	catch (const string& not_image_file) {
		cerr << not_image_file << endl;
		return 0;
	}
	original_width = imgp_full->width;
	original_height = imgp_full->height;

	// XS -- DEBUG !!!!
	
	//workaround opencv peculiar definition of step
	//imgp_full->widthStep = original_width * imgp_full->nChannels;
	//	imgp_full->imageSize = original_width * original_height * imgp_full->nChannels;
	
	scale = _scale;
	// does not scale image if scale specified and > 0, otherwise rescale
	if (scale <= 0.0)
		scale = sqrt (standard_width * standard_height * 1.0 / (original_width * original_height));
	
	imgp = cvCreateImage(cvSize (scale*original_width, scale*original_height), imgp_full->depth, imgp_full->nChannels);
//	// SD TODO - This is stange cvCreateImage creates image with as a widthStep of 152
//	imgp->widthStep = imgp->width * imgp->nChannels;
//	imgp->imageSize = imgp->width * imgp->height * imgp->nChannels;
	cvResize(imgp_full, imgp, CV_INTER_CUBIC);
	check_imgp();
	color_model = "BGR"; // before splitchannels

	return 1;
}
	
void ACImageAnalysis::check_imgp(){
	// keep track of stuff -- really needed ?
	width = imgp->width;
	height = imgp->height;
	depth = imgp->depth;
	step = imgp->widthStep;
	channels = imgp->nChannels;
	
	try {
		if (!imgp) throw(string(" < MyColorImage::SetImage error> CV_LOAD_IMAGE_COLOR : not a color image !"));
	}
	catch (const string& not_image_file) {
		cerr << not_image_file << endl;
	}
}

int ACImageAnalysis::splitChannels(std::string cmode){ // or HSV
	if (imgp == NULL){
		cerr << " <MyColorImage::SplitChannels> : no image loaded yet " << endl;
		return 0;
	}
	
	if (cmode == color_model) {
		if (HAS_CHANNELS){
			cout << " Channels have already been split. Doing Nothing. " << endl;
			return 1;
		}
		else {
			for (int i = 0; i < 3; i++)
				channel_img[i] = cvCreateImage (cvSize (width, height), depth, 1);
			cvSplit (imgp, channel_img[0], channel_img[1], channel_img[2], NULL); 
			HAS_CHANNELS = true;
			return 1;
		}
	}
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
			cerr << " <MyColorImage::SplitChannels> : unsupported color format: should be BGR or HSV " << endl;
			return 0;
		}
		for (int i = 0; i < 3; i++)
			channel_img[i] = cvCreateImage (cvSize (width, height), depth, 1);
		cvSplit (tmp_im, channel_img[0], channel_img[1], channel_img[2], NULL); 
		HAS_CHANNELS = true;
		cvReleaseImage( &tmp_im );
		return 1;
	}
}

IplImage* ACImageAnalysis::getChannel(int i){
	if (imgp == NULL){
		cerr << " <MyColorImage::SplitChannels> : no image loaded yet " << endl;
		return NULL;
	}
	// XS  TODO : could split the channels here
	if (!HAS_CHANNELS) return NULL;
	if (i > channels-1 || i < 0) {
		cerr << "channel index out of range" << endl;
		return NULL;
	}
	return channel_img[i];
}

void ACImageAnalysis::removeChannels(){
	HAS_CHANNELS = false;
	for (int i = 0; i < 3; i++){
		cvReleaseImage(&channel_img[i]);
	}
}

void ACImageAnalysis::FFT2D (string cmode){
	splitChannels(cmode);
 	const int cdim = height * (width / 2 + 1);
	const int area = width * height;
	double data_in[area];
	uchar* ddata;
	for (int i = 0; i < 3; i++){	
		try {
			fft[i] = new fftw_complex [cdim];
		}
		catch (std::bad_alloc) {
			cerr << "Memory allocation problem in fft2d for fft of channel " << i << endl;
		}
 		ddata = (uchar*) channel_img[i]->imageData;
		cout << "checking ddata[0] for channel" << " i : " << (double) ddata[0] <<endl;
		for (int j = 0; j < area; j++)
			data_in[j] = (double) ddata[j];
		cout << "doing fft for channel " << i << endl;
		fftw_plan p = fftw_plan_dft_r2c_2d(height, width, data_in, fft[i], FFTW_ESTIMATE); 
		fftw_execute(p);
		cout << "fft cleanup : " << endl;
		fftw_destroy_plan(p);
	}	
 	HAS_FFT = true;
}

void ACImageAnalysis::FFT2D_centered (string cmode){
	// 0 0 will appear at the center of the FFT image
	splitChannels(cmode);
 	const int cdim = height * (width / 2 + 1);
	const int area = width * height;
	double data_in[area];
	uchar* ddata;
	for (int i = 0; i < 3; i++){	
		try {
			fft[i] = new fftw_complex [cdim];
		}
		catch (std::bad_alloc) {
			cerr << "Memory allocation problem in fft2d for fft of channel " << i << endl;
		}
 		ddata = (uchar*) channel_img[i]->imageData;
		cout << "checking ddata[0] for channel" << " i : " << (double) ddata[0] <<endl;
		int k=0;
		for(int irow=0; irow< height; irow++){
			for (int icolumn=0; icolumn< width; icolumn++){
				data_in[k] = double(pow(float(-1),float(irow+icolumn)) * (double)(ddata[irow*width+icolumn] +128));
				k++;
			}
		}		
		
		cout << "doing fft for channel " << i << endl;
		fftw_plan p = fftw_plan_dft_r2c_2d(height, width, data_in, fft[i], FFTW_ESTIMATE); 
		fftw_execute(p);
		cout << "fft cleanup : " << endl;
		fftw_destroy_plan(p);
	}	
	HAS_FFT = true;
}

// ------------------ features computation -----------------

void ACImageAnalysis::computeHuMoments(int mmax, int thresh){ // default 7, 100
	hu_moments.clear();
	
	if (imgp == NULL){
		cerr << " <ACImageAnalysis::computeHuMoments() error> missing image !" << endl;
		return;
	}
	// XS we create again a BW image -- have to do the same for Gabor
	IplImage *BWimg = cvCreateImage (cvSize (this->getWidth(), this->getHeight()), IPL_DEPTH_8U, 1); 
	IplImage* gray = cvCreateImage( cvGetSize(BWimg), 8, 1 );
	cvZero( gray); 
	
	cvCvtColor (imgp, BWimg, CV_BGR2GRAY);
	CvMemStorage*  storage  = cvCreateMemStorage(0);
	CvSeq* contours = 0;
	cvThreshold( BWimg, BWimg, thresh, 255, CV_THRESH_BINARY ); 
	cvFindContours( BWimg, storage, &contours ); // XS TODO : more parameters ?
	// CvArr* img, CvSeq* contour, CvScalar external_color, CvScalar hole_color, int max_level, 
	// int thickness = 1,  int line_type = 8, CvPoint offset  = cvPoint(0,0) 	); 
	if( contours ) cvDrawContours( gray, contours, cvScalarAll(255), cvScalarAll(255), thresh ); 
		
#ifdef VISUAL_CHECK
		cvNamedWindow( "ImageC", 1 ); 
		cvShowImage( "ImageC", imgp ); 
		cvNamedWindow( "Image", 1 ); 
		cvShowImage( "Image", BWimg ); 
		cvNamedWindow( "Contours", 1 ); 
		cvShowImage( "Contours", gray ); 
		cvWaitKey(0); 
		cvDestroyWindow("Image");
		cvDestroyWindow("ImageC");
		cvDestroyWindow("Contours");
#endif // VISUAL_CHECK
	CvMoments mymoments;
	CvHuMoments myHumoments ;
	// XS TODO : add option to discriminate these 
	//	cvContourMoments (gray, &mymoments); // contour moments
	cvContourMoments (BWimg, &mymoments); // image moments
	
	cvGetHuMoments(&mymoments, &myHumoments);
	cvReleaseImage(&BWimg);
	cvReleaseImage(&gray);
	
	cvReleaseMemStorage(&storage );	
	hu_moments.push_back (myHumoments.hu1) ;
	hu_moments.push_back (myHumoments.hu2) ;
	hu_moments.push_back (myHumoments.hu3) ;
	hu_moments.push_back (myHumoments.hu4) ;
	hu_moments.push_back (myHumoments.hu5) ;
	hu_moments.push_back (myHumoments.hu6) ;
	hu_moments.push_back (myHumoments.hu7) ;
	
#ifdef VERBOSE
	cout << " Hu Moments :" << endl;
	cout << " ---------- " << endl;
#endif
	
	for (int i = 0; i < mmax; i++){
#ifdef VERBOSE
		cout << "raw Hu moment " << i << " : " << hu_moments[i] << endl ;
		cout << " ---------- ---------- ---------- " << endl;
#endif
		if (hu_moments[i] != 0) {
			hu_moments[i] = hu_moments[i] / fabs(hu_moments[i]) * log (fabs(hu_moments[i]));
		}
	}
#ifdef VERBOSE
	for (int i = 0; i < mmax; i++)
		cout << "log Hu moment " << i << " : " << hu_moments[i] << endl ;
	cout << " ---------- ---------- ---------- " << endl;
#endif
}

void ACImageAnalysis::computeGaborFeatures(int mumax, int numax){ // default 7, 5
	gabor_moments.clear();
	if (imgp == NULL) {
		cerr << " <ACImageAnalysis::computeGaborFeatures() error> missing image !" << endl;
		return;
	}
	IplImage *BWimg = cvCreateImage (cvSize (this->getWidth(), this->getHeight()), IPL_DEPTH_8U, 1); 
	cvCvtColor (imgp, BWimg, CV_BGR2GRAY);
	CvGabor *gabor = new CvGabor();
	for (int i = 0; i < mumax; i++){	
		for (int j = 0; j < numax; j++){
			gabor->Reset(i,j);
			double *tmpft = new double[2];
			tmpft = gabor->getMeanAndStdevs(BWimg); // ?
			
			gabor_moments.push_back (tmpft[0]) ;
			gabor_moments.push_back (tmpft[1]) ;
			delete [] tmpft;
#ifdef VISUAL_CHECK
			IplImage *kernel = cvCreateImage( cvSize(gabor->GetMaskWidth(), gabor->GetMaskWidth()), IPL_DEPTH_8U, 1);
 			kernel = gabor->get_image(CV_GABOR_REAL);
			cvNamedWindow("Gabor Kernel", CV_WINDOW_AUTOSIZE);
			cvShowImage("Gabor Kernel", kernel);
			cvWaitKey(0);
			cvDestroyWindow("Gabor Kernel");			
			
			IplImage *magimg = cvCreateImage(cvSize(BWimg->width,BWimg->height), IPL_DEPTH_8U, 1);
			gabor->conv_img(BWimg, magimg, CV_GABOR_MAG);
			cvNamedWindow("Magnitude Response",CV_WINDOW_AUTOSIZE);
			cvShowImage("Magnitude Response",magimg);
			cvWaitKey(0);
			cvDestroyWindow("Magnitude Response");		
			cvReleaseImage( &kernel );	
			cvReleaseImage( &magimg );	
#endif
		}
	}
	cvReleaseImage( &BWimg );	
	//	cvReleaseImage( &image );	// no, we did not create it!!
}

void ACImageAnalysis::computeColorMoments(int n){
	// n = number of moments to compute
	color_moments.clear();
	ACImageHistogram* tmp_hist  = new ACImageHistogram(imgp, color_model);
	
	tmp_hist->normalize(1);
	tmp_hist->getStats();
	tmp_hist->computeMoments(n);
	
	// normalized first 
	//for (int i = 0; i < 3; i++){
//		for (int j = 0; j < tmp_hist->getSize(); j++){
//			// XS TODO : add option to store the moments of the histogram
//			color_moments.push_back (tmp_hist->getValue(i,j)) ;
//		}
//	}
		
	for (int i=1;i<=n;i++){
		for (int channel=0; channel<this->getNumberOfChannels(); channel++){
			color_moments.push_back (tmp_hist->getMoment(i)[channel]) ;
			cout << tmp_hist->getMoment(i)[channel] << endl;
		}	
	}
#ifdef VISUAL_CHECK
	tmp_hist->show();
#endif // VISUAL_CHECK

	//	averageHistogram->ShowStats();
	delete tmp_hist;
	
}

// ------------------ visual output functions -----------------

void ACImageAnalysis::showInWindow(const std::string title, bool has_win){
	// using highgui
	if (not has_win)
		cvNamedWindow(title.c_str(), CV_WINDOW_AUTOSIZE);
    cvShowImage(title.c_str(), imgp);
}

void ACImageAnalysis::showChannels(string cmode){
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

void ACImageAnalysis::showChannels(string cmode, const char* w0, const char* w1, const char* w2){
	// XS no default cmode here
	splitChannels(cmode);
    cvShowImage(w0, channel_img[0]);
    cvShowImage(w1, channel_img[1]);
    cvShowImage(w2, channel_img[2]);
}

void ACImageAnalysis::showFFTInWindow(const std::string title){
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


