/**
 * @brief ACAudioFeatures.cpp
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

#include <stdio.h>
#include <sndfile.h>
#include <string.h>
#include "ACAudioFeatures.h"
#include <samplerate.h>
#include <iostream>
#include <Armadillo-utils.h>
#include <vector>
#include <map>

using std::vector;
using std::string;
using std::map;

using namespace arma;
using namespace std;

enum desc_idx{
	SPECTRAL_CENTROID,
	SPECTRAL_SPREAD,
	SPECTRAL_VARIATION,
	SPECTRAL_FLATNESS,
	SPECTRAL_FLUX,
	SPECTRAL_DECREASE,
	MFCC,
	DMFCC,
	DDMFCC,
	ZERO_CROSSING_RATE,
	ENERGY,
	LOUDNESS,
	LOG_ATTACK_TIME,
	ENERGY_MODULATION_FREQUENCY,
	ENERGY_MODULATION_AMPLITUDE,
	MODULATION,
	BURSTS,
	/*CHIRP,
	 PITCH,
	 HNR,*/
	CHROMA
};

ACMediaTimedFeature* computeFeature(float* data, string featureName, int samplerate, int nchannels, long length, int mfccNbChannels, int mfccNb, int windowSize, 	bool extendSoundLimits, int minOctave, int maxOctave){
	vector<string> descList;
	vector<ACMediaTimedFeature*> desc_mf;
	descList.push_back(featureName);
	desc_mf =  computeFeatures(data, descList, samplerate, nchannels, length, mfccNbChannels, mfccNb, windowSize, extendSoundLimits);
	descList.clear();
	return desc_mf[0];
}

std::vector<ACMediaTimedFeature*> computeFeatures(float* data, int samplerate, int nchannels, long length, int mfccNbChannels, int mfccNb, int windowSize, 	bool extendSoundLimits, int minOctave, int maxOctave){
	vector<string> descList;
	std::vector<ACMediaTimedFeature*> desc_amtf;
	descList.push_back("Spectral Centroid");
	descList.push_back("Spectral Spread");
	descList.push_back("Spectral Variation");
	descList.push_back("Spectral Flatness");
	descList.push_back("Spectral Flux");
	descList.push_back("Spectral Decrease");
	descList.push_back("MFCC");
	descList.push_back("DMFCC");
	descList.push_back("DDMFCC");
	descList.push_back("Zero Crossing Rate");
	descList.push_back("Energy");
	descList.push_back("Loudness");
	descList.push_back("Log Attack Time");
	descList.push_back("Energy Modulation Frequency");
	descList.push_back("Energy Modulation Amplitude");
	/*descList.push_back("Chirp GD");
	 descList.push_back("F0");
	 descList.push_back("HNR");*/
	descList.push_back("Chroma");
	//descList.push_back("Burst Segmentation");
	desc_amtf = computeFeatures(data, descList, samplerate, nchannels, length, mfccNbChannels, mfccNb, windowSize, extendSoundLimits);
	descList.clear();
	return desc_amtf;
}

std::vector<ACMediaTimedFeature*> computeFeatures(float* data, vector<string> descList, int samplerate, int nchannels, long length,
												  int mfccNbChannels, int mfccNb, int windowSize, bool extendSoundLimits, int minOctave, int maxOctave) {
	
	if ((mfccNbChannels & (mfccNbChannels-1)) != 0){
		std::cerr << "Number of mfcc channels should be a power of two" << std::endl;
		exit(1);
	}
	
	if ((windowSize & (windowSize-1)) != 0){
		std::cerr << "Window size should be a power of two"  << std::endl;
		exit(1);
	}
	
	map<string, desc_idx> descMap;
	descMap["Spectral Centroid"] = SPECTRAL_CENTROID;
	descMap["Spectral Spread"] = SPECTRAL_SPREAD;
	descMap["Spectral Variation"] = SPECTRAL_VARIATION;
	descMap["Spectral Flatness"] = SPECTRAL_FLATNESS;
	descMap["Spectral Flux"] = SPECTRAL_FLUX;
	descMap["Spectral Decrease"] = SPECTRAL_DECREASE;
	descMap["MFCC"] = MFCC;
	descMap["DMFCC"] = DMFCC;
	descMap["DDMFCC"] = DDMFCC;
	descMap["Zero Crossing Rate"] = ZERO_CROSSING_RATE;
	descMap["Energy"] = ENERGY;
	descMap["Loudness"] = LOUDNESS;
	descMap["Log Attack Time"] = LOG_ATTACK_TIME;
	descMap["Energy Modulation Frequency"] = ENERGY_MODULATION_FREQUENCY;
	descMap["Energy Modulation Amplitude"] = ENERGY_MODULATION_AMPLITUDE;
	descMap["Burst Segmentation"] = BURSTS;
	/*descMap["Chirp GD"] = CHIRP;
	 descMap["F0"] = PITCH;
	 descMap["HNR"] = HNR;*/
	descMap["Chroma"] = CHROMA;
	
	int fftSize = windowSize * 2;
	int hopSize = windowSize / 8;
	
	float * dataout;
	SF_INFO sfinfo2;
	SF_INFO sfinfo;
	sfinfo.samplerate = samplerate;
	sfinfo.frames = length;
	sfinfo.channels = nchannels;
	sfinfo.seekable = 1;
	
	int sr_hz = 22050;
	//windowSize = (int)round(0.03*(float)sr_hz); // window size forced to 30ms et shift of 10ms
	//hopSize = (int)round(0.01*(float)sr_hz);
	double srcRatio = (double) sr_hz/ (double) samplerate;
	int outFrames = (int) (length * srcRatio * nchannels + 1);
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
			signal_v(i+windowSize/2) = dataout[i*nchannels];
		for (long i = 0; i < windowSize/2; i++)
			signal_v(i+sfinfo2.frames+windowSize/2 - 1) = 0;
	}
	else{
		signal_v.set_size(sfinfo2.frames);
		for (long i = 0; i < sfinfo2.frames; i++)
			signal_v(i) = dataout[i*nchannels]; // we keep only channel 1
	
	}
	
	/*int sr_hz = 8250;
	 windowSize=0.03*sr_hz;
	 hopSize=0.01*sr_hz;
	 float f2b = (float)fftSize/(float)sr_hz;
	 float b2f = 1/f2b;
	 colvec signal_v;
	 if (extendSoundLimits){
	 signal_v.set_size(sfinfo.frames+windowSize);// = colvec(dataout, sfinfo2.frames);
	 for (long i = 0; i < windowSize/2; i++)
	 signal_v(i) = 0;
	 for (long i = 0; i < sfinfo.frames; i++)
	 signal_v(i+windowSize/2) = data[i*nchannels];
	 for (long i = 0; i < windowSize/2; i++)
	 signal_v(i+sfinfo.frames+windowSize/2 - 1) = 0;
	 }
	 else{
	 signal_v.set_size(sfinfo.frames);
	 for (long i = 0; i < sfinfo.frames; i++)
	 signal_v(i) = data[i*nchannels]; // we keep only channel 1
	 }*/
	
	colvec frame_v = colvec(windowSize);
	colvec frameW_v = colvec(windowSize);
 	colvec window_v = blackman(windowSize);
	//colvec window_v=ones<colvec>(windowSize); //no framing
 	colvec frameFFTabs_v;
 	colvec frameFFTabs2_v(fftSize/2);
	colvec prevFrameFFTabs_v;
	prevFrameFFTabs_v.zeros(fftSize/2);
	long nbFrames = (long)(signal_v.n_elem-windowSize)/hopSize+1;
	int env_fs = (int) sr_hz/hopSize;
	mat melfilter_m;
	mat chromafilter_m;
	std::vector<ACMediaTimedFeature*> desc;
	
	colvec sc_v(nbFrames);
	colvec ss_v(nbFrames);
	colvec sv_v;
	sv_v.zeros(nbFrames);
	colvec sf_v(nbFrames);
	colvec zcr_v(nbFrames);
	colvec sd_v(nbFrames);
	colvec ener_v(nbFrames);
	colvec loud_v(nbFrames);
	colvec time_v(nbFrames);
	mat mfcc_m(nbFrames, mfccNb);
	mat chroma_m(nbFrames, 12);
	mat sfm_m(nbFrames, 4);
	double lat;
	rowvec ed_v;
	melfilter_m = melfilters(mfccNbChannels, fftSize, sr_hz);
	chromafilter_m=chromafilters(minOctave, maxOctave, fftSize, sr_hz);
	
	
	// pour features Marie Baudart
	//int fftSize=1024;			//number of points in fft computation
	float r_chirpGD=1.12;		//radius of analysis circle where the chirp z-transform will be computed
	int orderLPC=12;			//LPC order
	int f0min=50;				//minimum fondamental frequency
	int f0max=600;				//maximum fondamental frequency
	int NiterPitch=1;			//number of iterations in pitch estimation
	int Nperiods_HNR=5;			//number of pitch periods used to calculate HNR
	
	colvec window_hanning = hanning(windowSize);
	colvec frameW_h = colvec(windowSize);
	
	mat GDspectrogram=zeros<mat>(nbFrames,fftSize/2-1);
	rowvec vec1=rowvec(fftSize/2-1);
	rowvec vec2=rowvec(fftSize/2-1);
	colvec ChirpGD=zeros<colvec>(nbFrames);
	colvec inv(windowSize);
	colvec res=zeros<colvec>(signal_v.n_elem);
	//colvec f0=zeros<colvec>(nbFrames);
	//colvec ValTmp=zeros<colvec>(nbFrames);
	mat F0_and_Conf(nbFrames,2);
	mat HNR_m=zeros<mat>(nbFrames,4);
	//mat result=zeros<mat>(nbFrames,7);
	
	
	
	// 	std::vector<ACMediaTimedFeature> descVec;
	
	// 	descVec[1] = new ACMediaTimedFeature(nbFrames, 1, "sc");
	
	long index=0;
	// 	std::cout << "signal length = " << sfinfo2.frames << std::endl;
	// 	std::cout << "signal length = " << signal_v.n_elem << std::endl;
	// 	std::cout << "nbFrames = " << nbFrames << std::endl;
	
	for (long i=0; i <= signal_v.n_elem-windowSize; i = i+hopSize){
#ifdef USE_DEBUG
		std::cout << "ACAudioFeatures: computing features on bin " << (float)i/(float)hopSize << "/" << (int)((float)(signal_v.n_elem-windowSize)/(float)hopSize) << "\r" << std::flush; //CF won't work if multithreaded/parallelized
#endif
		time_v(index) = ((double)i+((double)windowSize*(1.0-(double)extendSoundLimits))/2.0)/(double)sr_hz;
		frame_v = signal_v.rows(i,i+windowSize-1);
		frameW_v = signal_v.rows(i,i+windowSize-1)%window_v;
 		frameFFTabs_v = abs(fft(frameW_v, fftSize));
 		frameFFTabs2_v = frameFFTabs_v.rows(0,fftSize/2-1);
		//frameFFTabs2_v.save("frameFFTabs2.txt", arma_ascii);
		
		for (int iDesc = 0; iDesc < descList.size(); iDesc++){
			switch (descMap[descList[iDesc]]) {
				case SPECTRAL_CENTROID:
					sc_v(index) = spectralCentroid(frameFFTabs2_v)*b2f;
					break;
				case SPECTRAL_SPREAD:
					ss_v(index) = spectralSpread(frameFFTabs2_v)*b2f;
					break;
				case SPECTRAL_FLATNESS:
					sfm_m.row(index) = spectralFlatness(frameFFTabs2_v, fftSize, sr_hz);
					break;
				case SPECTRAL_VARIATION:
					sv_v(index) = spectralVariation(frameFFTabs2_v, prevFrameFFTabs_v);
					break;
				case SPECTRAL_FLUX:
					sf_v(index) = spectralFlux(frameFFTabs2_v, prevFrameFFTabs_v);
					break;
				case ZERO_CROSSING_RATE:
					zcr_v(index) = zcr(frame_v, sr_hz, windowSize);
					break;
				case SPECTRAL_DECREASE:
					sd_v(index) = spectralDecrease(frameFFTabs2_v);
					break;
				case ENERGY:
					ener_v(index) = energyRMS(frame_v);
					break;
				case LOUDNESS:
					loud_v(index) = loudness(frameFFTabs2_v, melfilter_m);
					break;
				case MFCC:
					mfcc_m.row(index) = mfcc(frameFFTabs2_v, melfilter_m, mfccNb);
					break;
				case CHROMA:
					chroma_m.row(index)= trans(trans(chromafilter_m)*(frameFFTabs2_v));
					break;
					/*case CHIRP:
					 GDspectrogram.row(index)=chirpGroupDelay(frameW_v,fftSize, windowSize, r_chirpGD);
					 break;
					 case PITCH: //!!! HNR requires that too!
					 frameW_h = frame_v%window_hanning;	// % : element-wise multiplication
					 inv=GetLPCresidual(frameW_h,orderLPC);
					 res.rows(round((double)(i)),round((double)(i))+windowSize-1)=res.rows(round((double)(i)),round((double)(i))+windowSize-1)+inv; //JU: res.cols instead?
					 break;*/
			}
		}
		index++;
		prevFrameFFTabs_v = frameFFTabs2_v;
	}
	
	ACMediaTimedFeature* mfcc_tf;
	ACMediaTimedFeature* chroma_tf;
	
	mfcc_m.print();
	loud_v.print();
	
	for (int iDesc = 0; iDesc < descList.size(); iDesc++){
		switch (descMap[descList[iDesc]]) {
			case SPECTRAL_CENTROID:
				if (!sc_v.is_finite() ){
					std::cout << "sc_v is not finite" << std::endl;
					exit(1);
				}
				desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(sc_v), std::string("Spectral Centroid")));
				break;
			case SPECTRAL_SPREAD:
				if (!ss_v.is_finite()){
					std::cout << "ss_v is not finite" << std::endl;
					exit(1);
				}
				desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(ss_v), std::string("Spectral Spread")));
				break;
			case SPECTRAL_FLATNESS:
				if (!sfm_m.is_finite()){
					std::cout << "sfm_m is not finite" << std::endl;
					exit(1);
				}
				desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(sfm_m), std::string("Spectral Flatness")));
				break;
			case SPECTRAL_VARIATION:
				if (!sv_v.is_finite()){
					std::cout << "sv_v is not finite" << std::endl;
					exit(1);
				}
				desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(sv_v), std::string("Spectral Variation")));
				break;
			case SPECTRAL_FLUX:
				desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(sf_v), std::string("Spectral Flux")));
				break;
			case ZERO_CROSSING_RATE:
				if (!zcr_v.is_finite()){
					std::cout << "zcr_v is not finite" << std::endl;
					exit(1);
				}
				desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(zcr_v), std::string("ZCR")));
				break;
			case SPECTRAL_DECREASE:
				if (!sd_v.is_finite() ){
					std::cout << "sd_v is not finite" << std::endl;
					exit(1);
				}
				desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(sd_v), std::string("Spectral Decrease")));
				break;
			case ENERGY:
				if (!ener_v.is_finite()){
					std::cout << "ener_v is not finite" << std::endl;
					exit(1);
				}
				desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(ener_v), std::string("Energy")));
				break;
			case LOUDNESS:
				if (!loud_v.is_finite()){
					std::cout << "loud_v is not finite" << std::endl;
					exit(1);
				}
				desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(loud_v), std::string("Loudness")));
				break;
			case MFCC:
			{
				if (!mfcc_m.is_finite() ){
					std::cout << "mfcc_m is not finite" << std::endl;
					exit(1);
				}
				mfcc_tf = new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(mfcc_m), std::string("MFCC"));
				desc.push_back(mfcc_tf);
				break;
			}
			case DMFCC:
				desc.push_back(mfcc_tf->delta());
				break;
			case DDMFCC:
				desc.push_back(mfcc_tf->delta()->delta());
				break;
			case BURSTS:
			{
				colvec BoundaryIndex_v;
				BoundaryIndex_v=burstBoundaries(loud_v,time_v,0.125);
				desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(BoundaryIndex_v), std::string("BurstBoundaries")));
				break;
			}
			case CHROMA:
			{
				if (!chroma_m.is_finite() ){
					std::cout << "chroma_m is not finite" << std::endl;
					exit(1);
				}
				chroma_tf = new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(chroma_m), std::string("Chroma"));
				desc.push_back(chroma_tf);
				break;
			}
                /*case CHIRP:
				 {
				 
				 ChirpGD.row(0)=0;
				 for (int j=0; j<GDspectrogram.n_rows-1 ;j++){ // just changed the "limits: only one 0 at the beginning, and we go to the last frame n_rows-1 instead of n_rows-2
				 vec1=abs(GDspectrogram.row(j+1)-GDspectrogram.row(j));
				 vec2=abs(GDspectrogram.row(j));
				 ChirpGD(j+1)=100*sum(vec1)/sum(vec2);
				 }
				 desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(ChirpGD), std::string("ChirpGD")));
				 break;
				 }
				 case PITCH:
				 {
				 F0_and_Conf=compute_pitch(res,f0min,f0max,NiterPitch,nbFrames, sr_hz, hopSize);
				 desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(F0_and_Conf), std::string("F0 and Val")));
				 break;
				 }
				 case HNR:
				 {
				 HNR_m=compute_HNR(signal_v,F0_and_Conf.col(0), hopSize, sr_hz, Nperiods_HNR, HNR_m);
				 desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(HNR_m), std::string("HNR")));
				 break;
				 }*/
				
				// 		case MODULATION: // Added to the list if any modulation feature is required
				// 			mat modFr_m;
				// 			mat modAmp_m;
				// 			colvec modTime_v;
				// 			modulation(ener_v, env_fs, modFr_m, modAmp_m, modTime_v);
				// 			break;
				// 		case ENERGY_MODULATION_AMPLITUDE:
				// 			desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(modTime_v), conv_to<fmat>::from(modAmp_m), std::string("Energy Modulation Amplitude")));
				// 			break;
				// 		case ENERGY_MODULATION_FREQUENCY:
				// 			desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(modTime_v), conv_to<fmat>::from(modFr_m), std::string("Energy Modulation Frequency")));
				// 			break;
				// 		case LOG_ATTACK_TIME:
				// 			lat = logAttackTime(ener_v, env_fs);
				// 			fcolvec lat_v(1);
				// 			lat_v(0) = lat;
				// 			desc.push_back(new ACMediaTimedFeature(zero_v, lat_v, std::string("Log Attack Time")));
				// 			break;
				//		case EFFECTIVE_DURATION:
				//			ed_v = effectiveDuration(time_v, loud_v);
				//			desc.push_back(new ACMediaTimedFeature(zero_v, conv_to<fmat>::from(ed_v), std::string("Effective Duration")));
				//			break;
		}
	}
	descMap.clear();
	delete [] dataout;
	return desc;
}

double spectralCentroid(colvec x_v){
	colvec sc_v(1);
	if (sum(x_v) == 0){
		sc_v(0) = 0;
	}
	else{
		colvec in_v = linspace<colvec>(0, x_v.n_rows-1, x_v.n_rows);
		sc_v = weightedMean(in_v, x_v);
	}
	return sc_v(0);
}

double spectralSpread(colvec x_v){
	colvec ss_v(1);
	if (sum(x_v) == 0){
		ss_v(0) = 0;
	}
	else{
		colvec in_v = linspace<colvec>(0, x_v.n_rows-1, x_v.n_rows);
		ss_v = weightedStdDeviation(in_v, x_v);
	}
	return ss_v(0);
}

rowvec spectralFlatness(colvec x_v, int fftSize, int sr_hz){
	rowvec sfm_v(4);
	rowvec band_hz_v = "250 500 1000 2000 4000;";
	float f2b = (float)fftSize/(float)sr_hz;
	urowvec band_bin_v;
	band_bin_v = conv_to<urowvec>::from(band_hz_v * f2b);
	
	colvec fftBand_v;
	for (int iBand = 0; iBand < band_bin_v.n_elem-1; iBand++){
		fftBand_v = x_v.rows(band_bin_v(iBand), band_bin_v(iBand+1)-1);
		if (sum(fftBand_v) == 0){
			sfm_v(iBand) = 1;
		}
		else{
			sfm_v(iBand) = as_scalar(exp(sum(log(fftBand_v))/fftBand_v.n_elem)) / as_scalar(mean(fftBand_v));
		}
	}
	return sfm_v;
}

double spectralVariation(colvec x_v, colvec xPrev_v){
	double sv;
	//XS-SD 240810: typo || not | ; added lim parameter
	double lim = 1e-5;
	//	double tmp;
	if (sum(x_v) < lim || sum(xPrev_v) < lim){
		sv = 1;
	}
	else{
		sv = as_scalar(abs(cor(x_v, xPrev_v)));
	}
	// SD 070211
	if ( (sv<0) || (sv>1) ) {
		sv = 1;
	}
	return sv;
}

double spectralFlux(colvec x_v, colvec xPrev_v){
	double sf;
	colvec dX_v = x_v-xPrev_v;
	// half wave rectifier
	colvec y_v = (abs(dX_v) + dX_v)/2;
	sf = as_scalar(sum(y_v));
	return sf;
}


double zcr(colvec frame_v, int sr_hz, int frameSize){
	double zz = as_scalar(sum((frame_v.rows(1,frame_v.n_rows-1) % frame_v.rows(0,frame_v.n_rows-2)) < 0)) / (double)frameSize * (double)sr_hz;
	return zz;
}

double spectralDecrease(colvec x_v){
	double sd;
	if (sum(x_v) == 0){
		sd = 0;
	}
	else{
		colvec f_v = linspace<colvec>(1, x_v.n_rows-1, x_v.n_rows-1);
		sd = 1/as_scalar(sum(x_v.rows(1,x_v.n_rows-1))) * as_scalar(sum( (x_v.rows(1,x_v.n_rows-1) - x_v(0)) /f_v));
	}
	return sd;
}

double energyRMS(colvec frame_v){
	double dc  = as_scalar(mean(frame_v));
	double ener = as_scalar(sqrt(sum(square(frame_v-dc))/frame_v.n_rows));
	return ener;
}

double logAttackTime(colvec ener_v, int sr_hz){
	double maxEner = max(ener_v);
	ucolvec posD_v = find(ener_v > .2 * maxEner);
	double posD = posD_v(0);
	ucolvec posF_v = find(ener_v > .9 * maxEner);
	double posF = posF_v(0);
	double lat = ((posF-posD)/sr_hz);
	return lat;
}


rowvec mfcc(colvec x_v, mat melfilter_m, int mfccNb){
	//colvec x2_v = log10(trans(melfilter_m)*(x_v+math::eps()));
	colvec x2_v = log(trans(melfilter_m)*(x_v)+math::eps()); // log as MAtlab does
	colvec xdct_v = dct(x2_v, x2_v.n_elem);
	rowvec mfcc_v = trans(dct(x2_v, x2_v.n_elem));
	mfcc_v = mfcc_v.cols(0, mfccNb-1);
	return mfcc_v;
}

double loudness(colvec x_v, mat melfilter_m){
	double loud = as_scalar(sum(pow(trans(melfilter_m)*(x_v+math::eps()),.6)));
	return loud;
}

rowvec effectiveDuration(colvec time_v, colvec loud_v){
	double Mv = max(loud_v);
	ucolvec pos_v = find(loud_v>.15*Mv);
	double start_sec = time_v(pos_v(0));
	double stop_sec = time_v(pos_v(pos_v.n_rows-1));
	double ed_sec = stop_sec -start_sec;
	double ed_sp = pos_v(pos_v.n_rows-1)-pos_v(0);
	rowvec ed_v(4);
	ed_v(0) = ed_sec;
	ed_v(1) = start_sec;
	ed_v(2) = stop_sec;
	ed_v(3) = ed_sp;
	return ed_v;
}

colvec burstBoundaries(colvec split_v, colvec time_v, float minBurstDur){
    int i=1, k, Nbursts=0;
    int Nvalues=(int)time_v.n_elem;
    colvec BB_v=zeros<colvec>(Nvalues);
    bool candidate;
    while(time_v(i)<minBurstDur){
        i++;
    }
    while(time_v(i)<time_v(Nvalues-1)+time_v(0)-minBurstDur&&i<Nvalues-2){
        candidate=true;
        k=i-1;
        while(k>0&&time_v(k)+minBurstDur>=time_v(i)){
            if(split_v(k)<=split_v(i))
            {
                candidate=false;
                break;
            }
            k--;
        }
        k=i+1;
        while(k<Nvalues-1&&time_v(k)-minBurstDur<=time_v(i)){
            if(split_v(k)<=split_v(i))
            {
                candidate=false;
                break;
            }
            k++;
        }
        if(candidate){
            BB_v(i)=1;
        }
		
        i++;
    }
    return BB_v;
}

int resample(float* datain, SF_INFO *sfinfo, float* dataout, SF_INFO* sfinfoout){
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
	// 	std::cout << srcRatio << std::endl;
	// 	std::cout << outFrames << std::endl;
	
	// 	SNDFILE* outFile;
	// 	if (! (outFile = sf_open ("./test.aiff", SFM_WRITE, &sfinfo2))){
	// 		/* Open failed so print an error message. */
	// 		printf ("Not able to open input file %s.\n", "test.aiff") ;
	// 		/* Print the error message from libsndfile. */
	// 		puts (sf_strerror (0)) ;
	// 		return  1 ;
	// 	}
	// 	sf_write_float(outFile, dataout, outFrames);
	// 	sf_close(outFile);
}


/*
 mat compute_pitch(colvec res, int f0min, int f0max, int NiterPitch, int nbFrames, int samplerate, int hopSize)
 {
 res=res/as_scalar(max(abs(res)));
 //res.save("res.mat",raw_ascii);
 colvec f0=zeros<colvec>(nbFrames);
 float tmp;
 colvec ValTmp=zeros<colvec>(nbFrames);
 colvec param(2);
 
 mat output(nbFrames,2);
 
 //PitchEstimation
 int F0min=f0min;
 int F0max=f0max;
 int nbrZero=round((double)(0.01*ceil(100*0.120/2)/((double)hopSize/samplerate)));
 int stop, index;
 double F0meanEst;
 for(int k=1; k<=NiterPitch; k++){
 cout << " Pitch Estimation - Iteration " << k <<endl;
 stop = round((double)(120*samplerate)/1000);
 colvec win_blackman = blackman(stop);
 colvec seg(stop);
 f0.rows(0,nbrZero-1)=zeros<colvec>(nbrZero);  //cols instead of rows?
 ValTmp.rows(0,nbrZero-1)=zeros<colvec>(nbrZero);    //cols instead of rows?
 index=0;
 for (float i=0; i < res.n_elem-stop; i = i+hopSize){
 seg = res.rows(round((double)(i)),round((double)(i))+stop-1);  // cols instead of rows?
 seg=seg%win_blackman;
 param = Abeer_EstimatePitch(seg,samplerate,F0min,F0max);
 tmp=param(0);
 f0(nbrZero+index)=param(0);
 ValTmp(nbrZero+index)=param(1);
 index++;
 }
 uvec posiTmp=find(ValTmp>0.1);
 if (posiTmp.n_elem<1){  //JU: is this if - else useful?
 F0min=50;
 F0max=600;
 }
 else{
 colvec f0Tmp(posiTmp.n_elem);
 for (int j=0; j<posiTmp.n_elem; j++){
 f0Tmp(j)=f0(posiTmp(j));
 }
 F0meanEst=median(f0Tmp);
 F0min=round((double)(0.55*0.95*F0meanEst));
 F0max=round((double)(1.9*0.95*F0meanEst));
 if (F0min<50){
 F0min=50;
 }
 if (F0max>600){
 F0max=600;
 }
 }
 
 }
 output.col(0)=f0;
 output.col(1)=ValTmp;
 return output;
 }
 
 
 mat compute_HNR(colvec signal_v, colvec f0, int hopSize, int samplerate, int Nperiods_HNR, mat HNR_m)
 {
 cout << " HNR Calculation" <<endl;
 int ystart, yend;
 float F0_curr, N0_curr, ks;
 for (long i=1; i <= f0.n_elem; i++){
 ks = round((double)((i-1)*hopSize));
 F0_curr = f0(i-1);
 if(F0_curr<=0)      //JU: added to prevent division by zero
 {
 HNR_m.row(i-1)=zeros<rowvec>(4);
 continue;
 }
 N0_curr = 1 / F0_curr * samplerate;     //pitch period
 ystart = round((double)(ks - float(Nperiods_HNR*N0_curr/2)))-1;
 yend = round((double)(ks + float(Nperiods_HNR*N0_curr/2))) - 2;
 if((yend - ystart + 1)%2 == 0){
 yend=yend-1;
 }
 if (ystart < 0){
 continue;
 }
 if (yend > signal_v.n_elem){
 continue;
 }
 colvec frame_HNR_v = signal_v.rows(ystart,yend);
 HNR_m.row(i-1)=GetHNR(frame_HNR_v, F0_curr, samplerate);
 }
 return HNR_m;
 }
 
 
 rowvec chirpGroupDelay(colvec x_v, int fftSize, int WindowSizeSample, float r){
 int halfFFTSize=round((double)(fftSize)/2);
 colvec exponentialEnvelope=colvec(WindowSizeSample-1);
 colvec diff_x_v=colvec(x_v.n_elem-1);
 cx_mat fft_x_v;
 colvec abs_fft_x_v(x_v.n_elem-1);
 colvec ifft_tmp(x_v.n_elem-1);
 colvec zeroPhaseData(x_v.n_elem-1);
 colvec f0zeroPhaseData_exp(x_v.n_elem-1);
 cx_mat fourierTrans(fftSize,1);
 colvec angFFT(halfFFTSize);
 colvec diff_angFFT(halfFFTSize-1);
 rowvec chirpGroupDelay=rowvec(halfFFTSize-1);
 colvec zeroPhaseData_exp(x_v.n_elem-1);
 //colvec a(x_v.n_elem-1);
 //colvec ifft_x_v(x_v.n_elem-1);
 
 for (int n=0; n<WindowSizeSample-1; n++){		//zeroPhase data has length of N-1
 //for (int n=0; n<pow2; n++){
 exponentialEnvelope(n)=exp(log(1/r)*n);		//this is needed for computation of z-transform using fft
 }
 
 //obtain zero-pha version
 for (long i=0; i < x_v.n_elem-1; i++){
 diff_x_v(i)=x_v(i+1)-x_v(i);
 }
 
 //fft_x_v=fftw_forward(diff_x_v,pow2);
 fft_x_v=fftw_forward(diff_x_v,diff_x_v.n_elem);
 abs_fft_x_v=abs(fft_x_v);
 
 ifft_tmp=fftw_backward(abs_fft_x_v,abs_fft_x_v.n_elem);
 
 zeroPhaseData=real(ifft_tmp);
 
 //chirp z-transform calculation using fft,...multiplication with an exponential function is sufficient
 zeroPhaseData_exp=zeroPhaseData%exponentialEnvelope;
 fourierTrans=fft(zeroPhaseData_exp,fftSize);
 
 double* b = new double[halfFFTSize];
 double* c = new double[halfFFTSize];
 for (int i=0; i<halfFFTSize; i++){
 b[i]=real(fourierTrans(i));
 c[i]=imag(fourierTrans(i));
 }
 for (int i=0; i<halfFFTSize; i++){
 complex <double> fourtrans (b[i],c[i]);
 angFFT(i)=arg(fourtrans);
 }
 for (int i=0; i < halfFFTSize-1; i++){
 diff_angFFT(i)=-(angFFT(i+1)-angFFT(i));
 }
 chirpGroupDelay=trans(diff_angFFT)-sum(diff_angFFT)/halfFFTSize;
 
 delete[] b;
 delete[] c;
 
 return chirpGroupDelay;
 }
 
 colvec GetLPCresidual(colvec frameWH_v,int orderLPC){
 int m=frameWH_v.n_rows;
 float d = log(float(2*m)-1)/log(2.0);
 int pow2=pow(2.0, int(d)+1);				//find power of 2 higher than 2*m-1
 cx_colvec fft_frameWH_v(pow2);
 colvec tmp(pow2);
 colvec a(pow2);
 colvec R(pow2);
 colvec coeffLPC(orderLPC+1);
 colvec firOut(orderLPC+frameWH_v.n_elem);
 colvec inv(frameWH_v.n_elem);
 
 //Linear Predictor Coefficients
 //Compute autocorrelation vector
 fft_frameWH_v = fft(frameWH_v, pow2);
 tmp=square(abs(fft_frameWH_v));
 for (int i=0; i<pow2; i=i+2){
 a(i) = tmp(i/2);				//compute vector a for the ifft
 }
 for (int i=3; i<pow2; i=i+2){
 a(i) = 0;
 }
 a(1) = tmp(pow2/2);
 R = ifft(a,pow2);
 R = R/m;
 
 coeffLPC = levinson(R,orderLPC);
 
 //Return only real coefficients for the predictor
 coeffLPC = real(coeffLPC);
 
 //Compute the output of the FIR filter
 firOut=conv(coeffLPC,frameWH_v);
 for(int i=0;i<frameWH_v.n_elem;i++){
 inv(i)=firOut(i);
 }
 
 inv=inv*sqrt(sum(square(frameWH_v))/sum(square(inv)));
 
 return inv;
 }
 
 mat Abeer_EstimatePitch(colvec seg,int fs,int f0min,int f0max){	//METHOD WITH FRAME-BASED FILTERING
 double maxi;
 double F0frame;
 double ratio1;
 u32 ind;
 cx_colvec seg_fft(fs);
 colvec Spec(fs/2);
 rowvec Errors=zeros<rowvec>(f0max);
 colvec out(2);
 
 seg=seg-mean(seg);
 seg_fft=fftw_forward(seg,fs);
 
 Spec=abs(seg_fft.rows(0,fs/2-1));
 Spec=Spec/as_scalar(sqrt(sum(square(Spec))));
 
 for (int freq=f0min; freq<=f0max; freq++){
 Errors(freq-1)=(Spec(freq-1)+Spec(2*freq-1)+Spec(3*freq-1)+Spec(4*freq-1)+Spec(5*freq-1))-(Spec(round((double)(1.5*freq))-1)+Spec(round((double)(2.5*freq))-1)+Spec(round((double)(3.5*freq))-1)+Spec(round((double)(4.5*freq))-1));
 }
 
 maxi=max(Errors);
 int p=0;
 while(Errors(p)<maxi)
 {
 p++;
 }  
 F0frame=p+1;
 ratio1=maxi;
 
 out(0)=F0frame;
 out(1)=ratio1;
 
 return out;
 }
 
 
 rowvec GetHNR(colvec x_v, int f0, int fs){
 long NBins = x_v.n_elem;
 float N0 = round(double(fs) / f0);
 float N0_delta = round((double)(N0 * 0.1));  // search 10% either side
 double max_val;
 u32 ind;
 colvec Nfreqs(4);
 Nfreqs << 500 << endr << 1500 << endr << 2500 << endr << 3500 << endr;
 cx_mat y;
 colvec logy(NBins);
 colvec ay(NBins);
 colvec ayseg(2*N0_delta+1);
 colvec abs_ayseg(2*N0_delta+1);
 colvec diff_ayseg(ayseg.n_elem-1);
 colvec s_ayseg(ayseg.n_elem-1);
 rowvec n = zeros<rowvec>(4);
 int Ef;
 int l_inx, r_inx;
 float peakinx = x_v.n_elem/(2*N0);
 uvec temp1, temp2;
 int midL = round(double(ay.n_elem) / 2);
 float Hdelta = float(f0) / fs * x_v.n_elem;
 int fstart;
 float Bdf;
 
 x_v = x_v%hamming(NBins);
 y = fftw_forward(x_v, NBins);
 logy = log10(abs(y));
 ay = fftw_backward(logy, NBins);
 
 // find possible rahmonic peaks
 for (int k=1; k<=peakinx; k++){
 ayseg = ay.rows(round((double)(k*N0-N0_delta))-1, round((double)(k*N0+N0_delta)-1));
 abs_ayseg=abs(ayseg);
 
 int p=0;
 while(abs_ayseg(p)<max_val)
 {
 p++;
 }
 ind=p+1;
 
 
 
 // lifter out each rahmonic peak
 for (int i=0; i < ayseg.n_elem-1; i++){
 diff_ayseg(i)=ayseg(i+1)-ayseg(i);
 }
 for (int i=0; i < diff_ayseg.n_elem; i++){
 if(diff_ayseg(i)>0){
 s_ayseg(i)=1;
 }
 else if(diff_ayseg(i)<0){
 s_ayseg(i)=-1;
 }
 else{
 s_ayseg(i)=0;
 }
 }
 
 // find first derivative sign change
 if(ind!=1){
 colvec temp = flipud(s_ayseg.rows(0,ind-2));
 temp1 = find(temp != 1, 1, "first")+1;
 if(temp1.is_empty()){
 l_inx=-1;
 }
 else{
 l_inx = as_scalar(ind - temp1) + 1;
 l_inx = l_inx + k*N0 - N0_delta - 1;
 }
 }
 else{
 l_inx=-1;
 }
 if(ind!=s_ayseg.n_elem && ind!=s_ayseg.n_elem+1){
 colvec tmp2 = s_ayseg.rows(ind, s_ayseg.n_elem-1);
 temp2 = find(tmp2 == 1, 1, "first")+1;
 if(temp2.is_empty()){
 r_inx=-1;
 }
 else{
 r_inx = as_scalar(ind + temp2);
 r_inx = r_inx + k*N0 - N0_delta - 1;
 }
 }
 else{
 r_inx=-1;
 }
 
 // lifter out the peak
 if((r_inx!=-1) && (l_inx!=-1)){
 ay.rows(l_inx-1,r_inx-1) = zeros<colvec>(r_inx-l_inx+1);
 }
 }
 
 // put the signal back together
 for(int j=0;j<(ay.n_elem-midL);j++){
 ay(midL+j) = ay(midL-1-j);
 }
 
 colvec Nap = real(fftw_forward(ay,ay.n_elem));
 colvec N = Nap;
 colvec Ha = logy - Nap;  // approximated harmonic spectrum
 
 // calculate baseline corrections
 for (double f=Hdelta+0.0001; f<=round((double)(x_v.n_elem)/2); f=f+Hdelta){
 fstart = ceil(f - Hdelta);
 Bdf = abs(min(Ha.rows(fstart-1,round((double)(f)-1))));
 N.rows(fstart-1,round((double)(f)-1)) = N.rows(fstart-1,round((double)(f)-1)) - Bdf;
 }
 
 // calculate the average HNR
 colvec H = logy - N;  //note that N is valid only for 1:length(N)/2
 for (int k=0; k<4; k++){
 Ef = round((double)(Nfreqs(k)) / fs * x_v.n_elem);  // equivalent cut off frequency
 n(k) = 20*mean(H.rows(0,Ef-1)) - 20*mean(N.rows(0,Ef-1));
 }
 
 return n;
 }
 
 
 colvec levinson(const colvec &R2, int order){
 colvec R = R2;
 R[0] = R[0] * (1. + 1.e-9);
 
 if (order < 0) order = R.n_elem - 1;
 double k, alfa, s;
 double *any = new double[order+1];
 double *a = new double[order+1];
 int j, m;
 vec out(order + 1);
 
 a[0] = 1;
 alfa = R[0];
 if (alfa <= 0) {
 out.reset();
 out[0] = 1;
 return out;
 }
 for (m = 1;m <= order;m++) {
 s = 0;
 for (j = 1;j < m;j++) {
 s = s + a[j] * R[m-j];
 }
 
 k = -(R[m] + s) / alfa;
 if (fabs(k) >= 1.0) {
 cout << "levinson : panic! abs(k)>=1, order " << m << ". Aborting..." << endl ;
 for (j = m;j <= order;j++) {
 a[j] = 0;
 }
 break;
 }
 for (j = 1;j < m;j++) {
 any[j] = a[j] + k * a[m-j];
 }
 for (j = 1;j < m;j++) {
 a[j] = any[j];
 }
 a[m] = k;
 alfa = alfa * (1 - k * k);
 }
 for (j = 0;j < out.n_elem;j++) {
 out[j] = a[j];
 }
 delete any;
 delete a;
 return out;
 }
 
 cx_mat fftw_forward(colvec x_m, int n){
 fftw_complex *out;
 fftw_complex *in;
 fftw_plan p;
 colvec real(n);
 colvec imag(n);
 int sigSize;
 
 in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * n);
 out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * n);
 
 sigSize = std::min(n, int(x_m.n_elem));
 
 for(int i=0;i<sigSize;i++){
 in[i][0]=x_m.row(i);
 in[i][1]=0;
 }
 for(int i=sigSize;i<n;i++){
 in[i][0]=0;
 in[i][1]=0;
 }
 
 p = fftw_plan_dft_1d(n, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
 
 fftw_execute(p); // repeat as needed
 
 for(int i=0;i<n;i++){
 real(i)=out[i][0];
 imag(i)=out[i][1];
 
 }
 
 cx_mat outFFT=cx_mat(real, imag);
 
 fftw_destroy_plan(p);
 fftw_free(in);
 fftw_free(out);
 
 return outFFT;
 }
 
 colvec fftw_backward(colvec x_m, int n){
 double *out;
 fftw_complex *in;
 fftw_plan p;
 colvec outIFFT(n);
 int sigSize;
 
 in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * n);
 out = (double*) fftw_malloc(sizeof(double) * n);
 
 sigSize = std::min(n, int(x_m.n_elem));
 
 for(int i=0;i<sigSize;i++){
 in[i][0]=x_m.row(i);
 in[i][1]=0;
 }
 
 for(int i=sigSize;i<n;i++){
 in[i][0]=0;
 in[i][1]=0;
 }
 
 p = fftw_plan_dft_c2r_1d(n, in, out, FFTW_ESTIMATE);
 
 fftw_execute(p); // repeat as needed
 
 for(int i=0;i<n;i++){
 outIFFT(i)=out[i]/n;	//Divided by n because of "scale factor" in ifft
 }
 
 fftw_destroy_plan(p);
 fftw_free(in);
 fftw_free(out);
 
 return outIFFT;
 }
 
 mat ifft(mat x_m, int n){
 
 mat y_m;
 if (x_m.n_rows > 1){
 
 y_m.set_size(n, x_m.n_cols);
 for (unsigned int i = 0; i < x_m.n_cols; i++)
 y_m.col(i) = ifft_helper(x_m.col(i),n);
 }
 // only one line several columns
 else if (x_m.n_cols > 1){
 y_m.set_size(1, n);
 y_m.row(0) = trans(ifft_helper(trans(x_m.row(0)),n));
 }
 // one line one col
 else{
 y_m = ones<mat>(1,n)*x_m(0,0)/n;
 }
 return y_m;
 }
 */