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
	ACColorImageAnalysis(const string &filename, string _cmode="BGR");	
	ACColorImageAnalysis(IplImage*, string _cmode="BGR");
	ACColorImageAnalysis(ACMediaData* image_data, string _cmode="BGR");

    ~ACColorImageAnalysis();

	void reset();
	void clean();

	// channels
	int splitChannels(string col="BGR");
	void removeChannels(); 
	IplImage* getChannel(int i);
	
	// BW
	void makeBWImage();
	IplImage* getBWImage(){return bw_imgp;}

	// FFT
	void computeFFT2D(string col="BGR"); // or HSV 
	void computeFFT2D_complex(string col="BGR"); // or HSV 
	void computeFFT2D_centered(string col="BGR"); // or HSV 
	fftw_complex** getFFT2D(){return fft;}
	
	// computation of features
	void computeHuMoments(int thresh = 0);
	void computeFourierPolarMoments(int RadialBins=5, int AngularBins=8);
	void computeFourierMellinMoments();
	void computeContourHuMoments(int thresh = 0);
	void computeGaborMoments(int mumax = 7, int numax = 5);
	void computeColorMoments(int n = 4);
	void computeImageHistogram(int w, int h);

	// visual
	void showChannels(string col="BGR");
	void showChannels(string cmode, const char* w0, const char* w1, const char* w2);
	void showFFTInWindow(const std::string title);

	// channels
	IplImage *channel_img[3];
	bool HAS_CHANNELS;

private:
	fftw_complex *fft[3]; // one per channel
	bool HAS_FFT;

	IplImage *bw_imgp;
	bool HAS_BW;
	
};


#endif	/* _ACCOLORIMAGEANALYSIS_H */
