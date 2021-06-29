/*
 *  ACColorImageHistogram.cpp
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

#include "ACColorImageHistogram.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

//#define DEBUG_ME

// we trust the incoming info about color space !!
// i.e., if _cmode is not the mode of the color_img, it fails
// XS TODO add test for this !!

ACColorImageHistogram::ACColorImageHistogram(ACColorImageAnalysis* color_img, string _cmode, int _norm) {
	// XS TODO add tests
	if (_norm > 0) norm = _norm; // typically 1
	cmode = _cmode;
	im_src_mat = color_img->getImageMat();
	this->buildHistogram();
}

ACColorImageHistogram::ACColorImageHistogram(cv::Mat img_mat, string _cmode, int _norm) {
	// XS TODO add tests
	if (_norm > 0) norm = _norm; // typically 1
	cmode = _cmode;
	im_src_mat = img_mat;
	this->buildHistogram();
}

ACColorImageHistogram::~ACColorImageHistogram() {
}

bool ACColorImageHistogram::buildHistogram() {
	if (!im_src_mat.data) {
		cerr << "<ACColorImageHistogram::buildHistogram> invalid or empty image" << endl;
		return false;
	}
	if (im_src_mat.channels() !=3){
		cerr << "<ACColorImageHistogram::buildHistogram> need 3 channels instead of " << im_src_mat.channels()<< endl;
		return false;
	}
		
	// adjusting ranges and splitting image according to color mode
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
	else {
		cerr << "<ACColorImageHistogram::buildHistogram> invalid color mode" << endl;
		return false;
	}
	int channels[1];
	// XS TODO size vs scale 4.0
	for (int i = 0; i < 3; i++){
		channels[0]=i;
		const float *ranges [2];
		*ranges = range[i];
		int histSize = range[i][1] ; // /4... // XS TODO some scale factor -- could be 1 for 256 bins (or 180)
		cv::Mat histi;
		cv::calcHist( &im_src_mat, 1, channels, cv::Mat(), // do not use mask 
					 histi, 1, &histSize, ranges,
					 true, // the histogram is uniform 
					 false ); // no accumulation
		cv::normalize (histi, histi, 1.0, 0, cv::NORM_L1);
		this->histos.push_back(histi);
	}	
	// XS TODO add test
	return true;
}

//ACColorImageHistogram& ACColorImageHistogram::operator+(const ACColorImageHistogram& H) {
//    cout << "operator+=" << endl;
//	return *this += H;        // wild ? 
//}
//
//ACColorImageHistogram& ACColorImageHistogram::operator+= (const ACColorImageHistogram& H){
//	// XS TODO: tester si les deux histogrammes sont compatibles
//	for (int i = 0; i < 3; i++){
//		for (int j = 0; j < size; j++){
//			double x = cvGetReal1D(hist[i]->bins, j) + cvGetReal1D(H.hist[i]->bins, j) ;
//			cvSetReal1D(hist[i]->bins, j, x );
//		}
//	}
//	return *this;
//	// this will change the LHS of the assignment !
//}

void ACColorImageHistogram::normalize(const double& factor) {
	if (histos.size()!=3) {
		cout << "<ACColorImageHistogram::normalize> : not normalizing, histogram should have 3 channels, not " << histos.size() << endl;
		return;
	}
	// note : if the sum of the histogram is close to zero, it will not be normalized (otherwize divide by zero)
	for (int i = 0; i < 3; i++){
		cv::normalize(histos[i], histos[i], factor);
	}
	norm=factor;
}

void ACColorImageHistogram::show() {
	if (histos.size()!=3) {
		cout << "<ACColorImageHistogram::normalize> : not normalizing, histogram should have 3 channels, not " << histos.size() << endl;
		return;
	}
	
	// output image with 3 histograms
	cv::Mat hist_img_mat(cv::Size(256,100), CV_8UC3, cv::Scalar::all(0));
	
	// max_value = max of all 3 histograms
	// maxi = max per histogram
	double max_value=0, maxi=0; 
	
	for (int i = 0; i < 3; i++) {
		minMaxLoc(histos[i], 0, &maxi, 0, 0); 
		max_value = (maxi > max_value) ? maxi : max_value;   
	}
	vector<cv::Scalar> colorsBGR ;
	colorsBGR.push_back (CV_RGB(0,0,255)); //B
	colorsBGR.push_back (CV_RGB(0,255,0)); //G
	colorsBGR.push_back (CV_RGB(255,0,0)); //R
	
	double w_scale = ((float)hist_img_mat.cols)/256;	
	double h_scale = ((float)hist_img_mat.rows)/max_value;

	for (int i = 0; i < 3; i++) {
//		histos[i].convertTo(histos[i], histos[i].type(), h_scale); // rescale
		for (int j = 0; j < range[i][1]; j++){ // or histsize[i]
			cv::rectangle( hist_img_mat, cv::Point((int)j*w_scale , hist_img_mat.rows),
						  cv::Point((int)(j+1)*w_scale, hist_img_mat.rows -  histos[i].at<float>(j)*h_scale),
						  colorsBGR[i]);
		}
	}
	cv::namedWindow("Histogram", cv::WINDOW_AUTOSIZE);
	cv::imshow("Histogram", hist_img_mat);
	cv::waitKey(0);
}

cv::Mat ACColorImageHistogram::getChannel(int i){
	if (histos.size()!=3) {
		cerr << "<ACColorImageHistogram::getChannel> : histogram should have 3 channels, not " << histos.size() << endl;
		return cv::Mat();
	}
	else if (i < 0 || i > 2){
		cerr << " <ACColorImageHistogram::getChannel> : wrong index " << endl;
		return cv::Mat();
	}
	return histos[i];
}

float ACColorImageHistogram::getValue(int i,int j){
	if (histos.size()!=3) {
		cerr << "<ACColorImageHistogram::getChannel> : histogram should have 3 channels, not " << histos.size() << endl;
		return 0;
	}
	if (i < 0 || i > 2){
		cerr << " <ACColorImageHistogram::GetValue> : wrong index i " << endl;
		return 0;
	}
	if (j < range[i][0] || j > range[i][1]){
		cerr << " <ACColorImageHistogram::GetValue> : wrong index j" << endl;
		return 0;
	}
	return histos[i].at<float>(j);
}

void ACColorImageHistogram::computeMoments(int highest_order){
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
#ifdef DEBUG_ME
	showStats();
#endif // DEBUG_ME	
	for (int n=3; n<=highest_order; n++){
		double mom[3];
		for (int i = 0; i < 3; i++){
			mom[i] = 0.0;
			for (int j = 0; j < range[i][1]; j++){
				double x_i = j ; // XS was: (j+.5)) * binsize[i]
				double p_i = histos[i].at<float>(j);
				mom[i] += p_i * pow(abs( x_i - mean[i]),n) ;
			}
			// normalized moment 
			if (stdev[i] !=0){
				mom[i] = mom[i]/pow(stdev[i],n);
			}
			else{
				cout << "<ACColorImageHistogram::computeMoments> *WARNING* : channel " << i << " has zero stdev. cannot normalize moment" << endl;
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

double* ACColorImageHistogram::getMoment(int i){ // i starts at 1
	if (i <= 0 || i > moments.size()){
		cerr << "<ACColorImageHistogram::getMoment> invalid moment index:" << i << endl;
		return 0;
	}
	return moments[i-1];
}

void ACColorImageHistogram::computeStats(){
	// XS TODO test if empty
	
	for (int i = 0; i < 3; i++){
		mean[i] = 0.0;
		stdev[i] = 0.0;
		int sizei = range[i][1];
		for (int j = 0; j < sizei; j++){
			double x_i = j ; // XS was: (j+.5) * binsize
			double p_i = histos[i].at<float>(j);
			mean[i] += x_i * p_i ;
			stdev[i] += x_i * x_i * p_i ;
		}
		
		// don't do this if p_i contains 1/n (histogram normalized to 1)
		//		mean[i] /= sizei;
		//		stdev[i] /= sizei;
		
		stdev[i] = sqrt( ((stdev[i] - mean[i] * mean[i]) * sizei)/(sizei-1) );
	}
}

void ACColorImageHistogram::showStats(){
	cout << "mean " << mean[0] << " ; " << mean[1] << " ; "  << mean[2] << endl ;
	cout << "stdev " << stdev[0] << " ; " << stdev[1] << " ; " << stdev[2] << endl ;
}

//double ACColorImageHistogram::compare(ACColorImageHistogram *MyH2){
//	double dist = 0.0;
//	for (int i = 0; i < 3; i++){
//		double t = cvCompareHist (hist[i], MyH2->hist[i], CV_COMP_BHATTACHARYYA);
//		dist += t;//CHISQR); //CV_COMP_CORREL); // watch out definitions (BHATTACHARYYA, CORREL, ...)
//	}
//	return (dist/3.0); 
//}


// dumps full hiin terminal by default
void ACColorImageHistogram::dump(ostream &odump){
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < range[i][1]; j++){
			odump << histos[i].at<float>(j) << " "; 
		}; 
		odump << endl;
	}
}