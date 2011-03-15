/*
 *  ACImageHistogram.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 13/03/09
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

#include "ACImageHistogram.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

//#define DEBUG_ME

ACImageHistogram::ACImageHistogram(IplImage** channel_img, string _cmode, int _size, int _norm) {
	initialize(channel_img, _cmode, _size, _norm);
}

ACImageHistogram::ACImageHistogram(IplImage* img, string _cmode, int _size, int _norm) {
	// first split in 3 channels 
	IplImage *channel_img[3];
	for (int i = 0; i < 3; i++)
		channel_img[i] = cvCreateImage (cvSize (img->width, img->height), img->depth, 1);
	cvSplit (img, channel_img[0], channel_img[1], channel_img[2], 0); 

	initialize(channel_img, _cmode, _size, _norm);

	for (int i = 0; i < 3; i++)
		cvReleaseImage(&channel_img[i]);
}

ACImageHistogram::~ACImageHistogram() {
	for (int i=0; i<3 ; i++) 
		cvReleaseHist (&hist[i]);
}

void ACImageHistogram::initialize(IplImage** channel_img, string _cmode, int _size, int _norm) {
	if (_size > 0) size = _size;
	if (_norm > 0) norm = _norm; // typically 1
	cmode = _cmode;

	if (cmode == "RGB" || cmode == "BGR"){ // XS note: RGB not supported, BGR by default!
		range [0] [0]= 0 ;
		range [0] [1]= 256 ;
		range [1] [0]= 0 ;
		range [1] [1]= 256 ;
		range [2] [0]= 0 ;
		range [2] [1]= 256 ;
	}
	else if (cmode == "HSV"){
		range [0] [0]= 0 ;
		range [0] [1]= 180 ; // 360 ;
		range [1] [0]= 0 ;
		range [1] [1]= 256 ; // 100 ;
		range [2] [0]= 0 ;
		range [2] [1]= 256 ; // 100 ;
	}
	else 
		cerr << "<ACImageHistogram::ACImageHistogram> invalid color mode" << endl;
	
	for (int i = 0; i < 3; i++){
		float *ranges [2];
		*ranges = range[i];
		hist[i] = cvCreateHist (1, &size, CV_HIST_ARRAY, ranges, 1);
		cvCalcHist (&channel_img[i], hist[i], 0, 0);
		cvNormalizeHist (hist[i], norm);
	}
}

ACImageHistogram& ACImageHistogram::operator+(const ACImageHistogram& H) {
    cout << "operator+=" << endl;
	return *this += H;        // wild ? 
}

ACImageHistogram& ACImageHistogram::operator+= (const ACImageHistogram& H){
	// XS TODO: tester si les deux histogrammes sont compatibles
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < size; j++){
			double x = cvGetReal1D(hist[i]->bins, j) + cvGetReal1D(H.hist[i]->bins, j) ;
			cvSetReal1D(hist[i]->bins, j, x );
		}
	}
	return *this;
	// this will change the LHS of the assignment !
}

void ACImageHistogram::normalize(const double& factor) {
	// note : if the sum of the histogram is close to zero, it will not be normalized (otherwize divide by zero)
	for (int i = 0; i < 3; i++){
		cvNormalizeHist (hist[i], factor);
	}
	norm=factor;
}

void ACImageHistogram::show() {
	int ch_width = size;
	int histheight = 200 ; // for visual purposes only
	int bin_w = cvRound((double) ch_width / size);
	float max_value = 0; // for visual purposes only
	IplImage *hist_img = cvCreateImage(cvSize(ch_width * 3, histheight), 8, 1);
    cvSet(hist_img, cvScalarAll(255), 0); // set to white background     
	for (int i = 0; i < 3; i++) {
		cvGetMinMaxHistValue (hist[i], 0, &max_value, 0, 0); 
		for (int j = 0; j < size; j++){
            cvRectangle(hist_img,
						cvPoint(j * bin_w + i * ch_width, histheight),
						cvPoint((j + 1) * bin_w + i * ch_width, histheight * ( 1.0 - cvGetReal1D(hist[i]->bins, j) / max_value)),
						cvScalarAll(0), -1, 8, 0); // cvScalarAll(0) :histogram in black
		}
	}
	cvNamedWindow("Histogram", CV_WINDOW_AUTOSIZE);
	cvShowImage("Histogram", hist_img);
	cvWaitKey(0);
	cvReleaseImage(&hist_img);
	cvDestroyWindow("Histogram");
}

void ACImageHistogram::show(string wname) {
	// XS assuming window wname has been opened before
	int ch_width = size;
	int histheight = 200 ; // for visual purposes only
	int bin_w = cvRound((double) ch_width / size);
	float max_value = 0; // for visual purposes only
	IplImage *hist_img = cvCreateImage(cvSize(ch_width * 3, histheight), 8, 1);
    cvSet(hist_img, cvScalarAll(255), 0); // set to white background     
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < size; j++){
			cvGetMinMaxHistValue (hist[i], 0, &max_value, 0, 0); 
			cvRectangle(hist_img,
						cvPoint(j * bin_w + i * ch_width, histheight),
						cvPoint((j + 1) * bin_w + i * ch_width, histheight * ( 1.0 - cvGetReal1D(hist[i]->bins, j) / max_value)),
						cvScalarAll(0), -1, 8, 0); // cvScalarAll(0) :histogram in black
		}
	}
	cvShowImage(wname.c_str(), hist_img);
	cvWaitKey(10);
	cvReleaseImage(&hist_img);
}

CvHistogram* ACImageHistogram::getChannel(int i){
	if (i < 0 || i > 2){
		cerr << " <ACImageHistogram::GetChannel> : wrong index " << endl;
		return 0;
	}
	return hist[i];
}

float ACImageHistogram::getValue(int i,int j){
	if (i < 0 || i > 2){
		cerr << " <ACImageHistogram::GetValue> : wrong index i " << endl;
		return 0;
	}
	if (j < 0 || j > size){
		cerr << " <ACImageHistogram::GetValue> : wrong index j" << endl;
		return 0;
	}
	return cvQueryHistValue_1D(hist[i],j);
}

void ACImageHistogram::computeMoments(int highest_order){
	moments.clear();
	if (highest_order <=0){
		cerr << "moment order has to be positive" << endl;
		return;
	}
	this->computeStats(); // gives mean and stdev
	moments.push_back(mean);
	if (highest_order == 1) return;
	moments.push_back(stdev);
	if (highest_order == 2) return;
	double binsize[3];
	for (int i = 0; i < 3; i++){
		binsize[i] = (range[i][1]-range[i][0])/size;
	}
#ifdef DEBUG_ME
	showStats();
#endif // DEBUG_ME	
	for (int n=3; n<=highest_order; n++){
		double* mom;
		mom = new double[3];
		for (int i = 0; i < 3; i++){
			mom[i] = 0.0;
			for (int j = 0; j < size; j++){
				double x_i = (j+0.5) * binsize[i] ;
				double p_i = cvQueryHistValue_1D(hist[i],j);
				mom[i] += p_i * pow(( x_i - mean[i]),n) ;
			}
			// normalized moment 
			if (stdev[i] !=0){
				mom[i] = mom[i]/pow(stdev[i],n);
			}
			else{
				cout << "<ACImageHistogram::computeMoments> *WARNING* : channel " << i << " has zero stdev. cannot normalize moment" << endl;
			}
#ifdef DEBUG_ME
			cout << "test moment order " << n << "for hist " << i << " : " << mom[i] << endl;
#endif // DEBUG_ME
			//mom[i] = mom[i] * pow(size,n-1);
			//mom[i] = pow(mom[i], 1.0/n);
		}
		moments.push_back(mom);
	}
	return;
}

double* ACImageHistogram::getMoment(int i){ // i starts at 1
	if (i <= 0 || i > moments.size()){
		cerr << "<ACImageHistogram::getMoment> invalid moment index:" << i << endl;
		return 0;
	}
	return moments[i-1];
}

void ACImageHistogram::computeStats(){
	if (size ==0){
		cerr << "<ACImageHistogram::computeStats> : empty histogram" << endl;
		return;
	}
	else if (size == 1){
		cout << "<ACImageHistogram::computeStats> : histogram has only one bin..." << endl;
		return;
	}
	
	for (int i = 0; i < 3; i++){
		mean[i] = 0.0;
		stdev[i] = 0.0;

		double binsize = (range[i][1]-range[i][0])/size;
		for (int j = 0; j < size; j++){
			double x_i = (j+0.5) * binsize ;
			double p_i = cvQueryHistValue_1D(hist[i],j);
			mean[i] += x_i * p_i ;
			stdev[i] += x_i * x_i * p_i ;
		}
		// XS TODO
		// don't do this if p_i contains 1/n (histogram normalized... ??)
		//mean[i] /= size;
		//stdev[i] /= size;

		stdev[i] = sqrt( ((stdev[i] - mean[i] * mean[i]) * size)/(size-1) );
	}
}

void ACImageHistogram::showStats(){
	cout << "mean " << mean[0] << " ; " << mean[1] << " ; "  << mean[2] << endl ;
	cout << "stdev " << stdev[0] << " ; " << stdev[1] << " ; " << stdev[2] << endl ;
}

double ACImageHistogram::compare(ACImageHistogram *MyH2){
	double dist = 0.0;
	for (int i = 0; i < 3; i++){
		double t = cvCompareHist (hist[i], MyH2->hist[i], CV_COMP_BHATTACHARYYA);
		dist += t;//CHISQR); //CV_COMP_CORREL); // watch out definitions (BHATTACHARYYA, CORREL, ...)
	}
	return (dist/3.0); 
}

void ACImageHistogram::dump(string fout){
	for (int i = 0; i < 3; i++){
		stringstream stmp;
		stmp << fout << i << ".out" ;		
		ofstream out(stmp.str().c_str()); 
		for (int j = 0; j < size; j++){
			out << j << " " << cvQueryHistValue_1D(hist[i],j) << endl; 
		}; 
		out.close();
	}
}

void ACImageHistogram::dump(){
	// in terminal
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < size; j++){
			cout << j << " " << cvQueryHistValue_1D(hist[i],j) << endl; 
		}; 
	}
}