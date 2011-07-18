/*
 This file is part of the FIRE -- Flexible Image Retrieval System
 
 FIRE is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 FIRE is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with FIRE; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "gabor.h"
#include <vector>
#include <complex>
#include <cmath>
#include <iostream>

using namespace std;

Gabor::Gabor(ACImageAnalysis* img_an) {
	analysed_image = img_an;
}

Gabor::~Gabor() {
}

// public method called from outside to start the gabor feature extraction
// controls the full process of extracting numPha_ * numFreq_ gabor features.
// returns no result. results are available via the getImage(...) function
// If the horizontal resp. vertical margin is set to a value > 0,
// the original image is enlarged by a black border of the given size,
// to decrease nonlinear effects at the image's edges. Anyway, this
// only makes sense if the image itself has a block background color.
void Gabor::calculate(int numPha_, int numFreq_, uint horizonalMargin_, uint verticalMargin_) {
	numPha = numPha_;
	numFreq = numFreq_;
	horizontalMargin = horizonalMargin_;
	verticalMargin = verticalMargin_;
	
	// adjust sizes of vectors that will hold the
	// arrays (=images) resulting from Gabor filtering
	// the "features" will then be the mean and std of these images
	
	xsize_ = analysed_image->getWidth(); 
	ysize_ = analysed_image->getHeight();
	zsize_ = numPha * numFreq;
	data_.resize(zsize_);
	for (uint c = 0; c < zsize_; ++c) {
		data_[c].resize(xsize_ * ysize_);
	}
    
	// fourier transformed image data, set by the init(...) method
	fftw_complex *bwTransformed = NULL;
	
	init(bwTransformed);
	extractGabor(bwTransformed);
	
	// final cleanup of allocated memory
	delete[] bwTransformed;
}


// returns the 'layer'-th gabor feature. for a frequency f and phase p
// the feature number is calculated as "f * # phases + p"
// the result is undefined if the call to this method is not
// preceded by a call to "calculate()"
IplImage* Gabor::getImage(int layer) {
	if (layer > zsize_ ) return 0;
	IplImage *newimage = cvCreateImage(cvSize(this->xsize_, this->ysize_), IPL_DEPTH_32F, 1 );
	BwImage Im(newimage);
	for (uint x = 0; x < this->xsize_; x++)
		for (uint y = 0; y < this->ysize_; y++) {
			double d = (*this)(x, y, layer);
			Im[x][y] = d;
			//Im[x][y] = (*this)(x, y, layer);
		
		}
	//XS test
	cvNamedWindow("test", CV_WINDOW_AUTOSIZE);
    cvShowImage("test", newimage);
	cvWaitKey(0);
	cvDestroyWindow("test");
	//
	return newimage;
}


// initializes the gabor feature extraction
// sets up the data structure and performs the fourier transformation
void Gabor::init(fftw_complex* &bwTransformed) {
	// Set width = height = nearest power of 2
	uint width = analysed_image->getWidth() + 2 * horizontalMargin;
	uint height = analysed_image->getHeight() + 2 * verticalMargin;
	double bwValue;
    
	// get larger dimension
	uint max = width;
	if (height > max) { max = height; }
	
	// find next largest power of 2
	maxnn = 1;
	while (maxnn < max) { maxnn *= 2; }
	
	paddedHeight = maxnn;
	paddedWidth = maxnn;
	dim = paddedHeight * paddedWidth;
	
	hh = height / 2;
	hb = width / 2;
	
	bwTransformed = new fftw_complex[dim];

	// Allocate memory for temp arrays
	fftw_complex* bwIn = new fftw_complex[dim];
	fftw_plan p = fftw_plan_dft_2d(paddedWidth, paddedHeight, bwIn, bwTransformed, FFTW_FORWARD, FFTW_ESTIMATE); 

	// Fill vectors with padded image
	int xoffset=paddedWidth/2-width/2;
	int yoffset=paddedHeight/2-height/2;
	unsigned int idx;
	
	for(uint y = 0; y < paddedHeight; y++) {
		for(uint x = 0; x < paddedWidth; x++) {
			idx = y * paddedWidth + x;
			bwIn[idx][0] = 0.0;
			bwIn[idx][1] = 0.0;
		}
	}
	
	// parse the data of the underlying image and
	// set the complex arrays for the fourier
	// transformation accordingly
	BwImage Im(analysed_image->getImage());
	
	cvNamedWindow("0", CV_WINDOW_AUTOSIZE);
	cvShowImage("0", analysed_image->getImage());
	cvWaitKey(0);
	cvDestroyWindow("0");
	
	for(uint x = 0; x < width; x++) {
		for(uint y = 0; y < height; y++) {
			if ((x < horizontalMargin) || (x >= width - horizontalMargin) ||
				(y < verticalMargin) || (y >= height - verticalMargin)) {
				bwValue = 0;
			} 
			else {
				bwValue =Im[y-verticalMargin][x-horizontalMargin]; // XS TODO x <-> y ??
			}
			idx = (y + yoffset) * paddedWidth + (x + xoffset);
			bwIn[idx][0] = bwValue;
			bwIn[idx][1] = 0.0;
		}
	}
	
	
	//XS TEST
	IplImage* fftimage  = cvCreateImage(cvSize(paddedWidth,paddedHeight),IPL_DEPTH_8U,1);
	BwImage Bwfftimage(fftimage);
	int k=0;
	for(int x=0; x< paddedWidth; x++){
		for (int y=0; y< paddedHeight; y++){
			Bwfftimage[x][y] = bwIn[k][0];
			k++;
		}
	}
	cvNamedWindow("bwIn", CV_WINDOW_AUTOSIZE);
	cvShowImage("bwIn", fftimage);
	cvWaitKey(0);
	cvReleaseImage(&fftimage);
	cvDestroyWindow("bwIn");
	// XS END TEST
	
	fftw_execute(p);
	
	//XS TEST
	IplImage* fftimage2  = cvCreateImage(cvSize(paddedWidth,paddedHeight),IPL_DEPTH_8U,1);
	BwImage Bwfftimage2(fftimage2);
	k=0;
	for(int x=0; x< paddedWidth; x++){
		for (int y=0; y< paddedHeight; y++){
			Bwfftimage2[x][y] = bwIn[k][0];
			k++;
		}
	}
	cvNamedWindow("bwIn2", CV_WINDOW_AUTOSIZE);
	cvShowImage("bwIn2", fftimage2);
	cvWaitKey(0);
	cvReleaseImage(&fftimage2);
	cvDestroyWindow("bwIn2");
	// XS END TEST

	//XS TEST
	IplImage* fftimage3  = cvCreateImage(cvSize(paddedWidth,paddedHeight),IPL_DEPTH_32S,1);
	BwImageFloat Bwfftimage3(fftimage3);
	k=0;
	for(int x=0; x< paddedWidth; x++){
		for (int y=0; y< paddedHeight; y++){
			Bwfftimage3[x][y] = bwTransformed[k][0];
			k++;
		}
	}
	cvNamedWindow("bwTransformed", CV_WINDOW_AUTOSIZE);
	cvShowImage("bwTransformed", fftimage3);
	cvWaitKey(0);
	cvReleaseImage(&fftimage3);
	cvDestroyWindow("bwTransformed");
	// XS END TEST
	
	fftw_destroy_plan(p);
	
	delete[] bwIn;
}

// calculates a filter for the given phase and frequency
void Gabor::calcFilter(complex<double>* &curFilter, unsigned int curPha, int curFreq) {
	// Erase old filter
	// XS we can't keep them, they depends on image size
	for (uint i = 0; i < dim; i++) {
		curFilter[i] = complex<double>(0.0, 0.0);
	}
	
	double alpha = sqrt(log(2.0)/2.0);
	double phasendiff = M_PI / ((double)GABOR_NUM_PHASES);
	double sigmafactor = alpha / (tan(phasendiff/2.0)*M_PI);
	double ratio = (3.0 * alpha) / (M_PI * sigmafactor);
	curFreq += GABOR_FREQ_START;
	
	double freq = sqrt(2.0)*pow(2.0,double(curFreq));
	
	double pha = (double(curPha)) * (M_PI / (double) numPha);
	
	double u0 = cos(pha)*freq;
	double v0 = sin(pha)*freq;
	double U = u0 * cos(pha) + v0 * sin(pha);
	double V = - u0 * sin(pha) + v0 * cos(pha);
	double deviation = sigmafactor / freq;
	
	double s = -2*M_PI*M_PI*deviation*deviation;
	
	double u,v,delta1,delta2;
	
	for (int y = -hh ; y< hh; y++) {
		for (int x = -hb; x<hb; x++) {
			u = x * cos(pha) + y * sin(pha);
			v = -x* sin(pha) + y * cos(pha);
			delta1= ratio*(u - U);
			delta2= v - V;
			
			int idxx, idxy;
			idxx=(x+paddedWidth)%paddedWidth;
			idxy=(y+paddedHeight)%paddedHeight;
			curFilter[(idxy*paddedWidth)+idxx] = exp(std::complex<double>(s*(delta1*delta1+delta2*delta2),0.0));
		}
	}
	
	curFilter[hh * paddedWidth + hb] = 0;
}


// extract the gabor features
// for each phase and each frequency, a filter is computed,
// which is applied to the fourier-transformed data.
// afterwards the inverse fourier transformation is performed,
// and the result data is copied back into the image
void Gabor::extractGabor(fftw_complex* bwTransformed) {
	
	fftw_complex* bwResult = new fftw_complex[dim];
	fftw_complex* bwSave = new fftw_complex[dim];
	complex<double>* curFilter = new complex<double>[dim];
	
	fftw_plan p = fftw_plan_dft_2d(paddedWidth, paddedHeight, bwTransformed, bwResult, FFTW_BACKWARD, FFTW_ESTIMATE); 
	
	for(uint aktFreq=0; aktFreq<numFreq; aktFreq++) {
		for(unsigned int aktPha=0; aktPha<numPha; aktPha++)  {
			
			// Calculate filter
			calcFilter(curFilter, aktPha, aktFreq);
			
			// copy transformed data
			memcpy(&(bwSave[0]), &(bwTransformed[0]), sizeof(fftw_complex) * dim);
			
			// Apply filter
			for(unsigned int y=0;y<paddedHeight;++y) {
				for(unsigned int x=0;x<paddedWidth;++x) {
					
					fftw_complex BWtt;
					BWtt[0] = bwTransformed[y * maxnn + x][0];
					BWtt[1] = bwTransformed[y * maxnn + x][1];

					fftw_complex filter;
					filter[0] = curFilter[y * paddedWidth + x].real();
					filter[1] = curFilter[y * paddedWidth + x].imag();
					
					// complex multiplication of 'BWtt' with 'filter'
					bwTransformed[y * maxnn + x][0] = (BWtt[0] * filter[0]) - (BWtt[1] * filter[1]);
					bwTransformed[y * maxnn + x][1] = (BWtt[0] * filter[1]) + (BWtt[1] * filter[0]); // here was a minus instead of the plus
				}
			}
			//XS TEST
			IplImage* fftimage3  = cvCreateImage(cvSize(paddedWidth,paddedHeight),IPL_DEPTH_8U,1);
			BwImage Bwfftimage3(fftimage3);
			int k=0;
			for(int x=0; x< paddedWidth; x++){
				for (int y=0; y< paddedHeight; y++){
					Bwfftimage3[x][y] = bwTransformed[k][0];
					k++;
				}
			}
			cvNamedWindow("bwTransformed", CV_WINDOW_AUTOSIZE);
			cvShowImage("bwTransformed", fftimage3);
			cvWaitKey(0);
			cvReleaseImage(&fftimage3);
			cvDestroyWindow("bwTransformed");
			// XS END TEST
			
			// Fourier-Transformation backwards
			//fftwnd_one(plan, bwTransformed, bwResult);
			fftw_execute(p);
			//XS TEST
			IplImage* fftimage4  = cvCreateImage(cvSize(paddedWidth,paddedHeight),IPL_DEPTH_8U,1);
			BwImage Bwfftimage4(fftimage3);
			k=0;
			for(int x=0; x< paddedWidth; x++){
				for (int y=0; y< paddedHeight; y++){
					Bwfftimage4[x][y] = bwResult[k][0];
					k++;
				}
			}
			cvNamedWindow("bwResult", CV_WINDOW_AUTOSIZE);
			cvShowImage("bwResult", fftimage4);
			cvWaitKey(0);
			cvReleaseImage(&fftimage4);
			cvDestroyWindow("bwResult");
			// XS END TEST
			
			// Save calculated feature
			copyToResult(bwResult, aktFreq * numPha + aktPha);
			
			// restore transformed data, needed for next extraction step
			memcpy(&(bwTransformed[0]), &(bwSave[0]), sizeof(fftw_complex) * dim);
		}
	}
	
	fftw_destroy_plan(p);
	
	delete[] curFilter;
	delete[] bwSave;
	delete[] bwResult;
}

// copies the output store in the hsResult and vResult arrays into
// the underlying imagefeature object for the given dimension
void Gabor::copyToResult(fftw_complex* bwResult, int d) {	
	int width = analysed_image->getWidth() + 2 * horizontalMargin;
	int height = analysed_image->getHeight() + 2 * verticalMargin;
	int xoffset=maxnn/2-width/2;
	int yoffset=maxnn/2-height/2;
	int idx;
	
	for(uint y = verticalMargin; y < height - verticalMargin; ++y) {
		for(uint x = horizontalMargin; x < width - horizontalMargin; ++x) {
			// consider the margins
			idx = (y + yoffset) * maxnn + (x + xoffset);
			(*this)(x - horizontalMargin, y - verticalMargin, d) = sqrt(bwResult[idx][0] * bwResult[idx][0] + bwResult[idx][1] * bwResult[idx][1]);
		}
	}
}

