/**
 * @brief ACAudioFeatures.cpp
 * @author Damien Tardieu
 * @date 11/03/2010
 * @copyright (c) 2010 – UMONS - Numediart
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

#include <stdio.h>
#include <sndfile.h>
#include <string.h>
#include "ACAudiofeatures.h"
#include <samplerate.h>
#include <iostream>
#include "Armadillo-utils.h"
#include <vector>

std::vector<ACMediaTimedFeatures*> computeFeatures(float* data, int samplerate, int nchannels, int length, int mfccNbChannels, int mfccNb, int windowSize, 	bool extendSoundLimits){
	
	if ((mfccNbChannels & (mfccNbChannels-1)) != 0){
		std::cerr << "Number of mfcc channels should be a power of two" << std::endl;
		exit(1);
	}

	if ((windowSize & (windowSize-1)) != 0){
		std::cerr << "Window size should be a power of two"  << std::endl;
		exit(1);
	}
	
	int fftSize=windowSize*2;
	int hopSize = windowSize /2;
	float * dataout;
	SF_INFO sfinfo2;
	SF_INFO sfinfo;
	sfinfo.samplerate = samplerate;
	sfinfo.frames = length;
	sfinfo.channels = nchannels;

	int sr_hz = 22050;
	double srcRatio = (double) sr_hz/ (double) samplerate;
	int outFrames = (int) (length * srcRatio + 1);
	float f2b = (float)fftSize/(float)sr_hz;
	float b2f = 1/f2b;
	dataout = new float[outFrames];
	int t = resample(data, &sfinfo, dataout, &sfinfo2);
	colvec signal_v;
	if (extendSoundLimits){
		signal_v.set_size(sfinfo2.frames+windowSize);// = colvec(dataout, sfinfo2.frames);
		for (long i = 0; i < windowSize/2; i++)
			signal_v(i) = 0;
		for (long i = 0; i < sfinfo2.frames; i++)
			signal_v(i+windowSize/2) = dataout[i];
		for (long i = 0; i < windowSize/2; i++)
			signal_v(i+sfinfo2.frames+windowSize/2) = 0;
	}		
	else{
		signal_v.set_size(sfinfo2.frames);// = colvec(dataout, sfinfo2.frames);
		for (long i = 0; i < sfinfo2.frames; i++)
			signal_v(i) = dataout[i];
	}
	colvec frame_v = colvec(windowSize);
	colvec frameW_v = colvec(windowSize);
 	colvec window_v = blackman(windowSize);
 	colvec frameFFTabs_v;
 	colvec frameFFTabs2_v(fftSize/2);
	
	long nbFrames = (long)(signal_v.n_elem-windowSize)/hopSize+1;
	int env_fs = (int) sr_hz/hopSize;
	mat melfilter_m;
	std::vector<ACMediaTimedFeatures*> desc;	

	colvec sc_v(nbFrames);
	colvec ss_v(nbFrames);
	colvec zcr_v(nbFrames);
	colvec sd_v(nbFrames);
	colvec ener_v(nbFrames);
	colvec time_v(nbFrames);
	mat mfcc_m(nbFrames, mfccNb);
	double lat;
	
	melfilter_m = melfilters(mfccNbChannels, fftSize, sr_hz);
// 	std::vector<ACMediaTimedFeature> descVec;
	
// 	descVec[1] = new ACMediaTimedFeature(nbFrames, 1, "sc");

	long index=0;
	// 	std::cout << "signal length = " << sfinfo2.frames << std::endl;
	// 	std::cout << "signal length = " << signal_v.n_elem << std::endl;
	// 	std::cout << "nbFrames = " << nbFrames << std::endl;
	
	for (long i=0; i < signal_v.n_elem-windowSize; i = i+hopSize){
		time_v(index) = ((double)i+(double)windowSize/2.0)/(double)sr_hz;
		frame_v = signal_v.rows(i,i+windowSize-1);
		frameW_v = signal_v.rows(i,i+windowSize-1)%window_v;
 		frameFFTabs_v = abs(fft(frameW_v, fftSize));
 		frameFFTabs2_v = frameFFTabs_v.rows(0,fftSize/2-1);
		sc_v(index) = spectralCentroid(frameFFTabs2_v)*b2f;
		ss_v(index) = spectralSpread(frameFFTabs2_v)*b2f;
		zcr_v(index) = zcr(frame_v, sr_hz, windowSize);
		sd_v(index) = spectralDecrease(frameFFTabs2_v);
 		ener_v(index) = energyRMS(frame_v);
		mfcc_m.row(index) = mfcc(frameFFTabs2_v, melfilter_m, mfccNb);
		index++;
		//    
		// 		frame_v.save("frame.txt", arma_ascii);
		//  		window_v.save("window.txt", arma_ascii);
	}

	desc.push_back(new ACMediaTimedFeatures(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(sc_v), std::string("Spectral Centroid")));
	desc.push_back(new ACMediaTimedFeatures(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(ss_v), std::string("Spectral Spread")));
	desc.push_back(new ACMediaTimedFeatures(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(zcr_v), std::string("Zero Crossing Rate")));
	desc.push_back(new ACMediaTimedFeatures(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(sd_v), std::string("Spectral Decrease")));
	desc.push_back(new ACMediaTimedFeatures(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(ener_v), std::string("Energy")));
	ACMediaTimedFeatures* mfcc_tf = new ACMediaTimedFeatures(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(mfcc_m), std::string("MFCC"));
	desc.push_back(mfcc_tf);
	desc.push_back(mfcc_tf->delta());
	mat modFr_m;
	mat modAmp_m;
	colvec modTime_v;

	modulation(ener_v, env_fs, modFr_m, modAmp_m, modTime_v);
	desc.push_back(new ACMediaTimedFeatures(conv_to<fcolvec>::from(modTime_v), conv_to<fmat>::from(modFr_m), std::string("Energy Modulation Frequency")));
	desc.push_back(new ACMediaTimedFeatures(conv_to<fcolvec>::from(modTime_v), conv_to<fmat>::from(modAmp_m), std::string("Energy Modulation Amplitude")));
	lat = logAttackTime(ener_v, env_fs);
	modFr_m.save("modFr.txt", arma_ascii);
	modAmp_m.save("modAmp.txt", arma_ascii);


	std::cout << "lat = " << lat << std::endl;
	return desc;
}

double spectralCentroid(colvec x_v){
	colvec in_v = linspace<colvec>(0, x_v.n_rows-1, x_v.n_rows);
	colvec sc_v = weightedMean(in_v, x_v);
	return sc_v(0);
}

double spectralSpread(colvec x_v){
	colvec in_v = linspace<colvec>(0, x_v.n_rows-1, x_v.n_rows);
	colvec ss_v = weightedStdDeviation(in_v, x_v);
	return ss_v(0);
}

double zcr(colvec frame_v, int sr_hz, int frameSize){
	double zz = as_scalar(sum((frame_v.rows(1,frame_v.n_rows-1) % frame_v.rows(0,frame_v.n_rows-2)) < 0)) / (double)frameSize * (double)sr_hz;
	return zz;
}

double spectralDecrease(colvec x_v){
	colvec f_v = linspace<colvec>(1, x_v.n_rows-1, x_v.n_rows-1);
	double sd = 1/as_scalar(sum(x_v.rows(1,x_v.n_rows-1))) * as_scalar(sum( (x_v.rows(1,x_v.n_rows-1) - x_v(0)) /f_v));
	return sd;
}

double energyRMS(colvec frame_v){
	double dc  = as_scalar(mean(frame_v));
	double ener = as_scalar(sqrt(sum(square(frame_v-dc))/frame_v.n_rows));
	return ener;
}

double logAttackTime(colvec ener_v, int sr_hz){
	double maxEner = max(ener_v);
	colvec posD_v = find(ener_v > .2 * maxEner);
	double posD = posD_v(0);
	colvec posF_v = find(ener_v > .9 * maxEner);
	double posF = posF_v(0);
	double lat = ((posF-posD)/sr_hz);
	return lat;
}


rowvec mfcc(colvec x_v, mat melfilter_m, int mfccNb){
	colvec x2_v = log(trans(melfilter_m)*(x_v+math::eps()));
	//	colvec x2_v = trans(melfilter_m)*x_v;
	rowvec mfcc_v = trans(dct(x2_v, x2_v.n_elem));
	mfcc_v = mfcc_v.cols(0, mfccNb-1);
	return mfcc_v;
}

int resample(float* datain, SF_INFO *sfinfo, float* dataout, SF_INFO* sfinfoout){
	SRC_DATA	src_data ;
	int destSr_hz = 22050;
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
	src_simple(&src_data, 0, 1);
	dataout = src_data.data_out;

	sfinfoout->channels = sfinfo->channels;
	sfinfoout->format = sfinfo->format;
	sfinfoout->sections = sfinfo->sections;
	sfinfoout->seekable = sfinfo->seekable;
	sfinfoout->frames = outFrames;
	sfinfoout->samplerate = destSr_hz;
	// 	std::cout << srcRatio << std::endl;
	// 	std::cout << outFrames << std::endl;

// 	SNDFILE* outFile;
// 	if (! (outFile = sf_open ("./test.aiff", SFM_WRITE, &sfinfo2))){  
// 		/* Open failed so print an error message. */
// 		printf ("Not able to open input file %s.\n", "test.aiff") ;
// 		/* Print the error message from libsndfile. */
// 		puts (sf_strerror (NULL)) ;
// 		return  1 ;
// 	}
// 	sf_write_float(outFile, dataout, outFrames);
// 	sf_close(outFile);
}
