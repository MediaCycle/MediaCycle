/*
 *  ACImageAnalysis.h
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

#ifndef _ACIMAGEANALYSIS_H
#define	_ACIMAGEANALYSIS_H
#include <vector>
#include <string>
#include "ACOpenCVInclude.h"

#include "cvgabor.h"
#include "ACImageHistogram.h"

// Uses FFTW3 library for FFT
#include <fftw3.h>
#include <complex>

using std::vector;
using std::string;

//http://www.cs.iit.edu/~agam/cs512/lect-notes/opencv-intro/opencv-intro.html#SECTION00053000000000000000
// C++ wrapper around IplImage that allows convenient (and hopefully fast) pixel access

template<class T> class Image {
private:
	IplImage* imgp;
public:
	Image(IplImage* img=0) {imgp=img;}
	~Image(){imgp=0;}
	void operator=(IplImage* img) {imgp=img;}
	inline T* operator[](const int rowIndx) {
	return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));}
};

typedef struct{
	unsigned char b,g,r;
} BgrPixel;

typedef struct{
	float b,g,r;
} BgrPixelFloat;

typedef Image<BgrPixel>       BgrImage;
typedef Image<BgrPixelFloat>  BgrImageFloat;
typedef Image<unsigned char>  BwImage;
typedef Image<float>          BwImageFloat;

// --------------------------------------------------------------------

class ACImageAnalysis {
public:
    ACImageAnalysis();
	ACImageAnalysis(const string &filename);	
	ACImageAnalysis(IplImage*);

    ~ACImageAnalysis();
	
	// general I/O, settings
	void clean();
	int initialize();
	void setFileName(const string &filename);
	int saveInFile(string fileout); // XS TODO: void in video
	
	//accessors 
	inline int getWidth() {return width;}
	inline int getHeight() {return height;}
	inline int getDepth() {return depth;}
	int getNumberOfChannels(){return channels;}
	string getFileName() {return file_name;}
	string getColorModel(){return color_model;}

	// image reduced to a size appropriate for analysis 
	IplImage* getImage(){return imgp;}
	int setImage(IplImage* img, float _scale=0.0);
	
	// channels
	int splitChannels(string col="BGR");
	void removeChannels(); 
	IplImage* getChannel(int i);
	
	// FFT
	void FFT2D(string col="BGR"); // or HSV 
	void FFT2D_centered(string col="BGR"); // or HSV 
	fftw_complex** getFFT2D(){return fft;}
	
	// computation of features
	void computeHuMoments(int mmax = 7, int thresh = 100);
	void computeGaborFeatures(int mumax = 7, int numax = 5);
	void computeColorMoments(int n = 4);

	// features accessors
	vector <float> getHuMoments(){return hu_moments;}
	vector <float> getGaborFeatures(){return gabor_moments;}
	vector <float> getColorFeatures(){return color_moments;}

	// handy
	void check_imgp(); 

	// visual
	void showInWindow(const string, bool has_win=false);
	void showChannels(string col="BGR");
	void showChannels(string cmode, const char* w0, const char* w1, const char* w2);
	void showFFTInWindow(const string);
	

private:
	IplImage *imgp; // not imgp_full
	int width, height, step, depth, channels;
	float scale;
	
	string file_name;
	string color_model; // "BGR" or "HSV" : these are already in IPLimage, but not used in OpenCV (see manual !)

	// default parameters for image to be analysed
	static const int standard_width ;
	static const int standard_height ;
	
	// original image size (image not stored in memory)
	int original_width, original_height;

	// channels
	IplImage *channel_img[3];
	bool HAS_CHANNELS;
	
	// FFT
	fftw_complex *fft[3]; // one per channel
	bool HAS_FFT;
	
	// features
	vector <float> color_moments;
	vector <float> gabor_moments;
	vector <float> hu_moments;
	
	
	
};

#endif	/* _ACIMAGEANALYSIS_H */
