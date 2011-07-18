/*
 *  ACBWImageAnalysis.h
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

#ifndef _ACBWIMAGEANALYSIS_H
#define	_ACBWIMAGEANALYSIS_H

#include "ACImageAnalysis.h"

class ACBWImageAnalysis: public ACImageAnalysis {
public:
	ACBWImageAnalysis();
	ACBWImageAnalysis(const string &filename);	
	ACBWImageAnalysis(IplImage*);
	
    ~ACBWImageAnalysis();
	
	void reset();
	void clean();

	// FFT
	void computeFFT2D(); 
	void computeFFT2D_complex(); 
	void computeFFT2D_centered(); 
	fftw_complex* getFFT2D(){return fft;}
	
	// computation of features
	void computeHuMoments(int thresh = 0);
	void computeContourHuMoments(int thresh = 0);
	void computeGaborMoments(int mumax = 4, int numax = 2);
	void computeGaborMoments_fft(int numPha_ = 4, int numFreq_ = 2, uint horizonalMargin_ = 0, uint verticalMargin_ = 0);
	void computeColorMoments(int n = 4);
	void computeFourierPolarMoments(int RadialBins=7, int AngularBins=10);
	void computeFourierMellinMoments();
	void computeImageHistogram(int w, int h);

	// visual output functions 
	void showThreshold(int thresh);
	void showContours(int thresh);
	void showFFTInWindow(const std::string title);
	void showFFTComplexInWindow(const std::string title);
	void showLogPolarInWindow(const std::string title);

	// I/O
	bool savePGM(const std::string file_name);

private:
	fftw_complex *fft; // on grayscale image
	bool HAS_FFT;

};


#endif	/* _ACBWIMAGEANALYSIS_H */
