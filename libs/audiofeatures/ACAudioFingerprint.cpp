/**
 * @brief ACAudioFingerprint.cpp
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

#include "ACAudioFingerprint.h"

using std::vector;
using std::string;
using std::map;

using namespace arma;
using namespace std;

// SD TODO - functione here to be moved to algorithmic library

int resample(float* datain, SF_INFO *sfinfo, float* dataout, SF_INFO* sfinfoout) {
	
	SRC_DATA	src_data ;
	int destSr_hz = 16000;
	double srcRatio = (double) destSr_hz/ (double) sfinfo->samplerate;
	int inlength =  sfinfo->frames;
	int outFrames = (int) (inlength * srcRatio + 1);
	//	dataout = new float[outFrames];
	src_data.end_of_input = 0 ; /* Set this later. */
	/* Start with zero to force load in while loop. */
	src_data.input_frames = inlength;
	src_data.data_in = datain;
	src_data.data_out = dataout;
	src_data.src_ratio = srcRatio;
	src_data.output_frames = outFrames;
	// SD - SRC_LINEAR to get faster processing, default mode is very slow
	src_simple(&src_data, SRC_LINEAR, sfinfo->channels);
	dataout = src_data.data_out;
	
	sfinfoout->channels = sfinfo->channels;
	sfinfoout->format = sfinfo->format;
	sfinfoout->sections = sfinfo->sections;
	sfinfoout->seekable = sfinfo->seekable;
	sfinfoout->frames = outFrames;
	sfinfoout->samplerate = destSr_hz;
}

colvec haitsma(colvec frame_v, colvec prevframe_v, int bandNbr) {
	
	float tmp;
	colvec tmp_v = colvec(bandNbr);
	for (int i=0;i<bandNbr-1;i++) {
		tmp = (frame_v[i+1]-frame_v[i])-(frame_v[i+1]-frame_v[i]);
		if (tmp>0) {
			tmp_v[i] = 1;
		}
		else {
			tmp_v[i] = 0;
		}
	}
}

ACAudioFingerprint::ACAudioFingerprint(int _sampleRate, int _nChannels) {
	
	sampleRate = _sampleRate;
	nChannels = _nChannels;
}

ACAudioFingerprint::~ACAudioFingerprint() {
	
}

void ACAudioFingerprint::setSampleRate(int _sampleRate, int _nChannels) {
	
	sampleRate = _sampleRate;
	nChannels = _nChannels;
}

void ACAudioFingerprint::setParameters(int _analysisSampleRate,
								  int _windowSize,
								  int _windowShift,
								  int _windowType,
								  int _minFreq,
								  int _maxFreq,
								  int _bandNbr,
								  int _filterShape,
								  int _freqScale) {
	
	analysisSampleRate = _analysisSampleRate;
	windowSize = _windowSize;
	windowShift = _windowShift;
	windowType = _windowType;
	minFreq = _minFreq;
	maxFreq = _maxFreq;
	bandNbr = _bandNbr;
	filterShape = _filterShape;
	freqScale = _freqScale;
	
	fftSize = 1;
	while (fftSize<windowSize) {
		fftSize *= 2;
	}
	
	switch (windowType) {
		case WINDOW_TYPE_HANNING:
			window_v = hanning(windowSize);
			break;
		case WINDOW_TYPE_HAMMING:
			window_v = hamming(windowSize);
			break;
		case WINDOW_TYPE_BLACKMANN:
			window_v = blackman(windowSize);
			break;
		case WINDOW_TYPE_RECTANGLE:
		default:
			window_v = ones<colvec>(windowSize);
			break;
	}
	
	filterbank_m = allfilters(bandNbr, freqScale, filterShape, minFreq, maxFreq, fftSize, analysisSampleRate);
	
	frame_v = colvec(windowSize);
	frameW_v = colvec(windowSize);
	frameFFTabs_v = colvec(fftSize);
 	frameFFTabs2_v = colvec(fftSize/2);
	prevFrameFFTabs2_v = colvec(fftSize/2);
	frameFilterBank_v = colvec(bandNbr);
	prevFrameFilterBank_v = colvec(bandNbr);
	prevFrameFilterBank_v.zeros(bandNbr);
}

std::vector<ACMediaTimedFeature*> ACAudioFingerprint::computeStream(float *data, long length) {

}

std::vector<ACMediaTimedFeature*> ACAudioFingerprint::compute(float *data, long length) {
	
	std::vector<ACMediaTimedFeature*> desc;
	
	float * dataout;
	SF_INFO sfinfo2;
	SF_INFO sfinfo;
	sfinfo.samplerate = sampleRate;
	sfinfo.frames = length;
	sfinfo.channels = nChannels;
	sfinfo.seekable = 1;
	
	double srcRatio = (double) analysisSampleRate / (double) sampleRate;
	int outFrames = (int) (length * srcRatio * nChannels + 1);
	dataout = new float[outFrames];
	int t = resample(data, &sfinfo, dataout, &sfinfo2);
	
	signal_v.set_size(sfinfo2.frames);
	for (long i = 0; i < sfinfo2.frames; i++) {
		signal_v(i) = dataout[i*nChannels]; // we keep only channel 1
	}
	delete [] dataout;
	
	long nbFrames = (long)(signal_v.n_elem-windowSize)/windowShift+1;
	
	colvec time_v(nbFrames);

	mat haitsma_m(nbFrames, bandNbr-1); // -1 because subtraction on the freq axis
	
	long index=0;
	
	for (long i=0; i < signal_v.n_elem-windowSize; i = i+windowShift) {
		
		time_v(index) = ((double)i+((double)windowSize*(1.0))/2.0)/(double)analysisSampleRate;
		
		frame_v = signal_v.rows(i,i+windowSize-1);
		
		// SD - is this correct?
		frameW_v = signal_v.rows(i,i+windowSize-1)%window_v;
		
		frameFFTabs_v = abs(fft(frameW_v, fftSize));
		
		frameFFTabs2_v = frameFFTabs_v.rows(0,fftSize/2-1);		
		
		frameFilterBank_v = trans(trans(filterbank_m)*(frameFFTabs2_v));
				
		haitsma_m.row(index) = haitsma(frameFilterBank_v, prevFrameFilterBank_v, bandNbr);
		
		index++;
		
		prevFrameFilterBank_v = frameFilterBank_v;
	}
	
	desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(haitsma_m), std::string("Haitsma")));

	return desc;
}
