/**
 * @brief ACAudioFingerprint.h
 * @author Stéphane Dupont
 * @date 12/05/2011
 * @copyright (c) 2011 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
*/

#ifndef _ACAUDIOFINGERPRINT_H
#define _ACAUDIOFINGERPRINT_H

#include <stdio.h>
#include <sndfile.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <map>
//#include <armadillo>
#include <ACMediaTimedFeature.h>
#include <samplerate.h>
#include <Armadillo-utils.h>

// SD TODO - mv this to algo library

// SD TODO - add level of overlap of filters

class ACAudioFingerprint {

public:
	
	ACAudioFingerprint(int sampleRate, int nChannels);
	~ACAudioFingerprint();
	
	void setSampleRate(int sampleRate, int nChannels);
	
	void setParameters(int analysisSampleRate = 5512.5,
				  int windowSize = 2048,
				  int windowShift = 64,
				  int windowType = WINDOW_TYPE_HANNING,
				  int minFreq = 200,
				  int maxFreq = 2000,
				  int bandNbr = 33,
				  int filterShape = FILTER_SHAPE_RECTANGLE,
				  int freqScale = FREQ_SCALE_LOG);

	std::vector<ACMediaTimedFeature*> computeStream(float *data, long length);

	std::vector<ACMediaTimedFeature*> compute(float *data, long length);	
	
private:
	
	int sampleRate;
	int nChannels;
	int analysisSampleRate;
	int windowSize;
	int windowShift;
	int windowType;
	int minFreq;
	int maxFreq;
	int freqScale;
	int bandNbr;
	int filterShape;
	int fftSize;

	arma::colvec window_v;
	arma::mat filterbank_m;

	arma::colvec signal_v;
	arma::colvec frame_v;
	arma::colvec frameW_v;
 	arma::colvec frameFFTabs_v;
 	arma::colvec frameFFTabs2_v;
	arma::colvec prevFrameFFTabs2_v;
	arma::colvec frameFilterBank_v;
	arma::colvec prevFrameFilterBank_v;
};										
												  
#endif
