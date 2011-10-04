/*
 *  ACColorImageAnalysis.h
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

#ifndef _ACCOLORIMAGEANALYSIS_H
#define	_ACCOLORIMAGEANALYSIS_H

#include "ACImageAnalysis.h"

class ACColorImageAnalysis: public ACImageAnalysis {
public:
	ACColorImageAnalysis();
	ACColorImageAnalysis(const std::string& filename);	
	ACColorImageAnalysis(const cv::Mat&);
	ACColorImageAnalysis(ACMediaData* image_data);

    ~ACColorImageAnalysis();

	void reset();
	void clean();

	// channels
	bool splitChannels(const string& col="BGR");
	void removeChannels(); 
	cv::Mat getChannel(int i);
	
	// BW
	void makeBWImage();
	cv::Mat& getBWImageMat(){return bw_imgp_mat;}

	// FFT
	void computeFFT2D(const std::string& col="BGR"); // or HSV 
	void computeFFT2D_complex(const std::string& col="BGR"); // or HSV 
	void computeFFT2D_centered(const std::string& col="BGR"); // or HSV 
	fftw_complex** getFFT2D(){return fft;}
	
	// computation of features
	void computeHuMoments(const int& thresh = 0);
	void computeFourierPolarMoments(const int& RadialBins=5, const int& AngularBins=8);
	void computeFourierMellinMoments();
	void computeContourHuMoments(const int& thresh = 0);
	void computeGaborMoments(const int& mumax = 4, const int& numax = 2);
	void computeGaborMoments_fft(const int& numPha_ = 4, const int& numFreq_ = 2, uint horizonalMargin_ = 0, uint verticalMargin_ = 0);
	void computeColorMoments(const int& n = 4, const string& cm = "HSV");
//	void computeImageHistogram(int w, int h); // XS TODO port 2.*
	
	// computation of features - opencv 2.*
	void computeHoughLines(); // XS TODO add parameters
	void computeHoughLinesP(); // XS TODO add parameters
	void computeNumberOfFaces(const std::string& cascadeFile, const float& scale=1.3, const float& searchScaleFactor=1.2, const int& minNeighbors=3,  const int& flags=CV_HAAR_SCALE_IMAGE, const cv::Size& minFeatureSize=cv::Size(20, 20));

	// visual
	void showChannels(const std::string& col="BGR");
	void showChannels(const std::string& cmode, const char* w0, const char* w1, const char* w2);
	void showFFTInWindow(const std::string& title);

private:
	fftw_complex *fft[3]; // one per channel
	bool HAS_FFT;

	// channels
	std::vector<cv::Mat> channel_img_mat;
	bool HAS_CHANNELS;
	
	cv::Mat bw_imgp_mat;

	bool HAS_BW;
	
};


#endif	/* _ACCOLORIMAGEANALYSIS_H */
