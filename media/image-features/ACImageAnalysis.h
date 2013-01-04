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
#include "gabor.h"
#include "ACImage.h"
#include "ACImageData.h"

// Uses FFTW3 library for FFT
#include <fftw3.h>
#include <complex>

#include <fstream> 

typedef unsigned int uint;
using std::vector;
using std::string;

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
	int getRows() ;
	int getCols() ;
	int getWidth() ;
	int getHeight() ;
	int getDepth();
	cv::Size getSize();
	int getNumberOfChannels();
	string getFileName() {return file_name;}

	inline cv::Mat& getImageMat(){return imgp_mat;}
	bool scaleImage(const cv::Mat& imgp_full_mat, const float _scale=0.0);

	// computation of features
	virtual void computeHuMoments(int thresh = 0){};
	virtual void computeFourierPolarMoments(int RadialBins=5, int AngularBins=8){};
	virtual void computeFourierMellinMoments(){};
	virtual void computeContourHuMoments(int thresh = 0){};
	virtual void computeGaborMoments(int mumax = 4, int numax = 2){};
	virtual void computeGaborMoments_fft(int numPha_ = 7, int numFreq_ = 5, uint horizonalMargin_ = 0, uint verticalMargin_ = 0){};
	virtual void computeColorMoments(int n = 4){};

	// computation of features - opencv 2.*
	void computeHoughLines(); // XS TODO add parameters
	void computeHoughLinesP(); // XS TODO add parameters
	void computeNumberOfFaces(const std::string& cascadeFile, const float& scale=1.3, const float& searchScaleFactor=1.2, const int& minNeighbors=4,  const int& flags=CV_HAAR_SCALE_IMAGE, const cv::Size& minFeatureSize=cv::Size(20, 20));
	
	// features accessors
	vector <float> getRawMoments(){return raw_moments;}
	vector <float> getHuMoments(){return hu_moments;}
	vector <float> getFourierPolarMoments(){return fourier_polar_moments;}
	vector <float> getFourierMellinMoments(){return fourier_mellin_moments;}
	vector <float> getContourHuMoments(){return contour_hu_moments;}
	vector <float> getGaborMoments(){return gabor_moments;}
	vector <float> getColorMoments(){return color_moments;}
	vector <float> getImageHistogram(){return image_histogram;}
	vector <float> getNumberOfHoughLines(){return number_of_hough_lines;} // dim=1
	vector <float> getNumberOfHoughLinesP(){return number_of_hough_linesp;} // dim=1
	vector <float> getNumberOfFaces(){return number_of_faces;}

	// handy
	void check_imgp(); 
	bool check_imgp_mat();

	// visual
	void showInWindow(const string); // existing window, closed elsewhere
	void showInNewWindow(const string); // new window created
	void closeNewWindow(const string); // new window deleted

	//I/O
//	void dumpContractionIndex(std::ostream &odump);
//	void dumpBoundingBoxRatio(std::ostream &odump);
	void dumpRawMoments(std::ostream &odump);
	void dumpHuMoments(std::ostream &odump);
	void dumpGaborMoments(std::ostream &odump);
	void dumpColorMoments(std::ostream &odump);
	void dumpContourHuMoments(std::ostream &odump);
	void dumpFourierPolarMoments(std::ostream &odump);
	void dumpFourierMellinMoments(std::ostream &odump);
//	void dumpAll(std::ostream &odump);
	

protected:
	cv::Mat imgp_mat;
	float scale;
	string file_name;

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
	vector <float> number_of_hough_lines; // dim=1
	vector <float> number_of_hough_linesp; // dim=1
	vector <float> number_of_faces;

	// XS TODO
//	vector <float[2]> hough_lines;

};

#endif	/* _ACIMAGEANALYSIS_H */
