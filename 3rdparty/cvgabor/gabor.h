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

#ifndef __GABOR_H__
#define __GABOR_H__

#include <vector>
#include <complex>
#include "ACOpenCVInclude.h"
//CF #include "ACImageAnalysis.h"

const int GABOR_NUM_PHASES = 4;
const int GABOR_NUM_FREQ = 2;
const int GABOR_FREQ_START = 2;

#include <fftw3.h>

typedef unsigned int uint;
class ACImageAnalysis;

// Gabor-Features extraction class
class Gabor {
public:
	
	// Creates a gabor object associated with the given image
    //CF Gabor(ACImageAnalysis* image);
    Gabor(cv::Mat& image);
	~Gabor();
	
	// public method called from outside to start the gabor feature extraction
	// controls the full process, but returns no result. results are 
	// available via the getImage(...) function
	// numPha * numFreq features will be extracted
	// If the horizontal resp. vertical margin is set to a value > 0,
	// the original image is enlarged by a black border of the given size,
	// to decrease nonlinear effects at the image's edges. Anyway, this
	// only makes sense if the image itself has a block background color.
	void calculate(int numPha = GABOR_NUM_PHASES, int numFreq = GABOR_NUM_FREQ, uint horizontalMargin = 0, uint verticalMargin = 0);
	
	// returns the 'layer'-th gabor feature. for a frequency f and phase p
	// the feature number is calculated as "f * # phases + p"
	// if a single gabor feature has been extract (using 'calculateSingleFeature(...)'),
	// the call must be 'getImage(0)'
	// the result is undefined if the call to this method is not
	// preceded by a call to "calculate()"
	cv::Mat getImage(int l);
	
	virtual double& operator()(uint x, uint y, uint c) {  return data_[c][y*xsize_+x];}

private:
	
	// initializes the gabor feature extraction
	// sets up the data structure and performs the fourier transformation
	void init(fftw_complex* &bwTransformed);
	
	// calculates a filter for the given phase and frequency
	void calcFilter(std::complex<double>* &curFilter, unsigned int curPha, int curFreq);
	
	// extract the gabor features
	// for each phase and each frequency, a filter is computed,
	// which is applied to the fourier-transformed data.
	// afterwards the inverse fourier transformation is performed,
	// and the result data is copied back into the image
	void extractGabor(fftw_complex* bwTransformed);
	
	// copies the output store in the hsResult and vResult arrays into
	// the underlying imagefeature object for the given dimension
	void copyToResult(fftw_complex* bwResult, int d);
	
	// the underlying image
    //CF ACImageAnalysis* analysed_image;
    cv::Mat analysed_image;
	
	// several dimension values
	unsigned int paddedWidth, paddedHeight;
	unsigned int dim, maxnn;
	int hh, hb;
	int xsize_, ysize_, zsize_ ;
	std::vector< std::vector<double> > data_;

	// virtual black borders around the image during fourier transformation
	uint horizontalMargin, verticalMargin;
	
	// number of phases and frequencies when extracting gabor features
	unsigned int numPha, numFreq;
	
};


#endif // __GABOR_H__

