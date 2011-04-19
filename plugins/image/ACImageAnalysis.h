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
//#include "gabor.h"
#include "ACImageHistogram.h"
#include "ACImageData.h"

// Uses FFTW3 library for FFT
#include <fftw3.h>
#include <complex>

#include <fstream> 

using std::vector;
using std::string;


// --------------------------------------------------------------------

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
		return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));
	}
	IplImage* getImage(){return imgp;}
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

    virtual ~ACImageAnalysis(){};
	
	// general I/O, settings
	virtual void reset(){};
	virtual void clean(){};
	void setFileName(const string &filename);
	bool saveInFile(string fileout); 
	
	//accessors 
	int getWidth() ;
	int getHeight() ;
	int getDepth();
	int getNumberOfChannels();
	string getFileName() {return file_name;}
	string getColorModel() {return color_model;}

	// image reduced to a size appropriate for analysis 
	IplImage* getImage(){return imgp;}
	int scaleImage(IplImage* img, float _scale=0.0);
		
	// computation of features
	virtual void computeHuMoments(int thresh = 0){};
	virtual void computeFourierPolarMoments(int RadialBins=5, int AngularBins=8){};
	virtual void computeFourierMellinMoments(){};
	virtual void computeContourHuMoments(int thresh = 0){};
	virtual void computeGaborMoments(int mumax = 7, int numax = 5){};
	virtual void computeColorMoments(int n = 4){};

	// features accessors
	vector <float> getRawMoments(){return raw_moments;}
	vector <float> getHuMoments(){return hu_moments;}
	vector <float> getFourierPolarMoments(){return fourier_polar_moments;}
	vector <float> getFourierMellinMoments(){return fourier_mellin_moments;}
	vector <float> getContourHuMoments(){return contour_hu_moments;}
	vector <float> getGaborMoments(){return gabor_moments;}
	vector <float> getColorMoments(){return color_moments;}
	vector <float> getImageHistogram(){return image_histogram;}

	// handy
	void check_imgp(); 

	// visual
	void showInWindow(const string); // existing window, closed elsewhere
	void showInNewWindow(const string); // new window created
	void closeNewWindow(const string); // new window deleted

	//I/O
//	void dumpContractionIndex(std::ostream &odump);
//	void dumpBoundingBoxRatio(std::ostream &odump);
	void dumpRawMoments(std::ostream &odump);
	void dumpHuMoments(std::ostream &odump);
	void dumpContourHuMoments(std::ostream &odump);
	void dumpFourierPolarMoments(std::ostream &odump);
	void dumpFourierMellinMoments(std::ostream &odump);
//	void dumpAll(std::ostream &odump);
	

protected:
	IplImage *imgp; // not imgp_full
	float scale;
	
	string file_name;
	string color_model; // "BGR" or "HSV" : these are already in IPLimage, but not used in OpenCV (see manual !)

	// default parameters for image to be analysed
	static const int standard_width ;
	static const int standard_height ;
	
	// original image size (image not stored in memory)
	int original_width, original_height;
	
	// features
	vector <float> color_moments;
	vector <float> gabor_moments;
	vector <float> raw_moments;
	vector <float> hu_moments;
	vector <float> contour_hu_moments;
	vector <float> fourier_polar_moments;
	vector <float> fourier_mellin_moments;
	vector <float> image_histogram;

};

#endif	/* _ACIMAGEANALYSIS_H */
