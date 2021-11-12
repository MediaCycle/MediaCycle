/**
 * @brief melfilters.cpp
 * @author Stéphane Dupont
 * @date 19/05/2011
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

#include "Armadillo-utils.h"
#include "melfilters.h"

using namespace arma;

mat melfilters(int nChannels, int fftSize, int sr_hz) {
	
	float fmin = 0;
	float fmax = (float)sr_hz/2;
	float f2b = (float)fftSize/(float)sr_hz;
	float bmax = fftSize/2;
	
	float mmin = freq2mel(fmin);
	float mmax = freq2mel(fmax);
	
	float chanDist = (mmax - mmin)/(nChannels+1);
	rowvec centerM_v = linspace<rowvec>(0, nChannels+1, nChannels+2);
	centerM_v = centerM_v*chanDist;
	centerM_v(nChannels+1) = mmax;
	
	//	centerM_v.save("centerM.txt", arma_ascii);
	
	rowvec centerB_v = mel2freq(centerM_v)*f2b;
	centerB_v(nChannels+1)=bmax-0.00001; // to ensure no size trouble
	
	//	centerB_v.save("centerB.txt", arma_ascii);
	
	mat melf_m = zeros<mat>(bmax, nChannels);
	//float tmp;
	
	//code compliant to MATLAB: filters linear in MEL
	float currentMel, dist;
	for (int iChan=1; iChan < nChannels+1; iChan++){
		//tmp=centerB_v(iChan+1);
		for (int iBin=centerB_v(iChan-1); iBin <= centerB_v(iChan+1); iBin++){
			if(iBin > centerB_v(iChan-1) & iBin < centerB_v(iChan)){
				currentMel = freq2mel(iBin/f2b);
				dist=centerM_v(iChan)-currentMel;
				melf_m(iBin, iChan-1) = 2*(1-dist/chanDist);
			}
			else if(iBin >= centerB_v(iChan) & iBin <= centerB_v(iChan+1)) {
				currentMel = freq2mel(iBin/f2b);
				dist=currentMel-centerM_v(iChan);
				melf_m(iBin, iChan-1) = 2*(1-dist/chanDist);			
			}
		}
		//melf_m.col(iChan-1) = f2b*melf_m.col(iChan-1)/(2*(centerB_v(iChan+1)-centerB_v(iChan-1)));
	}
	
	// old code:
	/*for (int iChan=1; iChan < nChannels+1; iChan++){
	 for (int iBin=0; iBin < bmax; iBin++){
	 if(iBin > centerB_v(iChan-1) & iBin < centerB_v(iChan)){
	 melf_m(iBin, iChan-1) = (iBin - centerB_v(iChan-1))/(centerB_v(iChan)-centerB_v(iChan-1));
	 }
	 else if(iBin >= centerB_v(iChan) & iBin <= centerB_v(iChan+1)) {
	 melf_m(iBin, iChan-1) = (iBin - centerB_v(iChan+1))/(centerB_v(iChan)-centerB_v(iChan+1));			
	 }
	 }
	 melf_m.col(iChan-1) = f2b*melf_m.col(iChan-1)/(2*(centerB_v(iChan+1)-centerB_v(iChan-1)));
	 }*/
	
	return melf_m;
}

mat chromafilters(int MinOctave, int MaxOctave, int fftSize, int sr_hz) {
	
    float bmax = fftSize/2;
    float f2b = (float)fftSize/(float)sr_hz;
    float center;
    int Bmin, Bmax;
	mat chromaf_m=zeros<mat>(bmax,12);
	
	int i,j;
	for(i=MinOctave;i<=MaxOctave; i++)
	{
		for(j=0;j<12;j++)
		{
			center= 440*pow((float)2,(float)((i-3)+((float)(j-9)/12)));
			Bmin=(int)((0.5+center/pow((float)2,((float)1/24)))*f2b);
			Bmax=(int)((0.5+center*pow((float)2,((float)1/24)))*f2b);
			
			if(Bmax<bmax&&Bmax>Bmin+1)
			{
				chromaf_m.submat(Bmin+1,j,Bmax-1,j)=ones<colvec>(Bmax-Bmin-1);
			}
		}
	}
	return chromaf_m;
}

// SD TODO - This approach to filterbank may not be optimal because filterbank matrix contains a lot of zeros
// + add overlap argument to be able to overlap filters more or less
arma::mat allfilters(int bandNbrs, int freqScale, int filterShape, float minFreq, float maxFreq, int fftSize, int analysisSampleRate) {
	
	float f2b = (float)fftSize/(float)analysisSampleRate;
	float bmax = fftSize/2;
	float chanDist;
	arma::rowvec center_v;
	arma::rowvec centerB_v;
	arma::mat filterbank_m ;
	float current, dist;
	rowvec minmax = arma::rowvec(2);
	filterbank_m = zeros<mat>(bmax, bandNbrs);
	minmax(0) = minFreq;
	minmax(1) = maxFreq;
	
	switch (freqScale) {
		case FREQ_SCALE_LIN:
			break;
		case FREQ_SCALE_BARK:
			break;
		case FREQ_SCALE_MEL:
			minmax = freq2mel(minmax);
			break;
		case FREQ_SCALE_LOG:
			minmax = log(minmax);
			break;
		default:
			break;
	}
	
	// SD TODO - correct triangle so that filter spacing is the same than for rectangle
	// SD TODO - alow to adjust level of overlap
	// SD TODO - check sammpl erate conversion, screws up the fingerprint, why?
	
	switch (filterShape) {
		case FILTER_SHAPE_TRIANGLE:
			chanDist = (minmax(1)-minmax(0))/(bandNbrs+1);
			center_v = linspace<arma::rowvec>(minmax(0), minmax(1), bandNbrs+2);
			break;
		case FILTER_SHAPE_TRAPEZE:
			break;
		case FILTER_SHAPE_RECTANGLE:
		default:
			chanDist = (minmax(1)-minmax(0))/(bandNbrs);
			center_v = linspace<arma::rowvec>(minmax(0), minmax(1), bandNbrs+1);
			break;
	}
	
	switch (freqScale) {
		case FREQ_SCALE_LIN:
			centerB_v = center_v * f2b;
			break;
		case FREQ_SCALE_BARK:
			break;
		case FREQ_SCALE_MEL:
			centerB_v = mel2freq(center_v) * f2b;
			break;
		case FREQ_SCALE_LOG:
			centerB_v = exp(center_v) * f2b;
			break;
		default:
			break;
	}
	
	// center_v(nChannels+1) = lmax; // TO CHECK
	// centerB_v(nChannels+1) = bmax-0.00001; // to ensure no size trouble
	
	int overlap = 3;
	int leftc;
	int rightc;
	
	switch (filterShape) {
		case FILTER_SHAPE_TRIANGLE:
			for (int iChan=1; iChan < bandNbrs+1; iChan++) {
				leftc = iChan-overlap;
				if (leftc<0) { leftc = 0; }
				rightc = iChan+overlap;
				if (rightc>bandNbrs+1) { rightc = bandNbrs+1; }
				for (int iBin=centerB_v(leftc); iBin <= centerB_v(rightc); iBin++) {
					if ( (iBin > centerB_v(leftc)) && (iBin < centerB_v(iChan)) ) {
						current = log(iBin/f2b);
						dist = center_v(iChan) - current;
						filterbank_m(iBin, iChan-1) = 2*(1-dist/(chanDist*overlap));
					}
					else if ( (iBin >= centerB_v(iChan)) && (iBin <= centerB_v(rightc)) ) {
						current = log(iBin/f2b);
						dist = current - center_v(iChan);
						filterbank_m(iBin, iChan-1) = 2*(1-dist/(chanDist*overlap));			
					}
				}
			}
			break;
		case FILTER_SHAPE_TRAPEZE:
			break;
		case FILTER_SHAPE_RECTANGLE:
		default:
			for (int iChan=1; iChan < bandNbrs+1; iChan++) {
				for (int iBin=centerB_v(iChan-1); iBin <= centerB_v(iChan); iBin++) {
					if ( (iBin > centerB_v(iChan-1)) && (iBin < centerB_v(iChan)) ) {
						filterbank_m(iBin, iChan-1) = 1;
					}
				}
			}			
			break;
	}
	
	return filterbank_m;
}

