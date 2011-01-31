/**
 * @brief ACAudioFeatures.cpp
 * @author Xavier Siebert
 * @date 31/01/2011
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
#include "Armadillo-utils.h"
#include <vector>
#include <map>

using std::vector;
using std::string;
using std::map;

using namespace arma;

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
        BURSTS
};

ACMediaTimedFeature* computeFeature(float* data, string featureName, int samplerate, int nchannels, long length, int mfccNbChannels, int mfccNb, int windowSize, 	bool extendSoundLimits){
	vector<string> descList;
	vector<ACMediaTimedFeature*> desc_mf;
	descList.push_back(featureName);
	desc_mf =  computeFeatures(data, descList, samplerate, nchannels, length, mfccNbChannels, mfccNb, windowSize, extendSoundLimits);	
	descList.clear();
	return desc_mf[0];
}

std::vector<ACMediaTimedFeature*> computeFeatures(float* data, int samplerate, int nchannels, long length, int mfccNbChannels, int mfccNb, int windowSize, 	bool extendSoundLimits){
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
        //descList.push_back("Burst Segmentation");
	desc_amtf = computeFeatures(data, descList, samplerate, nchannels, length, mfccNbChannels, mfccNb, windowSize, extendSoundLimits);	
	descList.clear();
	return desc_amtf;
}

std::vector<ACMediaTimedFeature*> computeFeatures(float* data, vector<string> descList, int samplerate, int nchannels, long length, int mfccNbChannels, int mfccNb, int windowSize, 	bool extendSoundLimits){
	
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
	
	int fftSize=windowSize*2;
	int hopSize = windowSize /8;
	float * dataout;
	SF_INFO sfinfo2;
	SF_INFO sfinfo;
	sfinfo.samplerate = samplerate;
	sfinfo.frames = length;
	sfinfo.channels = nchannels;
	sfinfo.seekable = 1;

	int sr_hz = 22050;
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
	delete [] dataout;
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
	mat sfm_m(nbFrames, 4);
	double lat;
	rowvec ed_v;
	melfilter_m = melfilters(mfccNbChannels, fftSize, sr_hz);
// 	std::vector<ACMediaTimedFeature> descVec;
	
// 	descVec[1] = new ACMediaTimedFeature(nbFrames, 1, "sc");

	long index=0;
	// 	std::cout << "signal length = " << sfinfo2.frames << std::endl;
	// 	std::cout << "signal length = " << signal_v.n_elem << std::endl;
	// 	std::cout << "nbFrames = " << nbFrames << std::endl;
	
	for (long i=0; i < signal_v.n_elem-windowSize; i = i+hopSize){
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
			}
		}
		index++;
		prevFrameFFTabs_v = frameFFTabs2_v;
	}
	
	ACMediaTimedFeature* mfcc_tf;
	
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
			desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(zcr_v), std::string("Zero Crossing Rate")));
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
                        colvec BoundaryIndex_v;
                        BoundaryIndex_v=burstBoundaries(loud_v,time_v,0.125);
                        desc.push_back(new ACMediaTimedFeature(conv_to<fcolvec>::from(time_v), conv_to<fmat>::from(BoundaryIndex_v), std::string("BurstBoundaries")));
                       break;

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
	if (sum(x_v) < lim || sum(xPrev_v) < lim){
		sv = 1;
	}	
	else{
		sv = as_scalar(abs(cor(x_v, xPrev_v)));
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
// 		puts (sf_strerror (NULL)) ;
// 		return  1 ;
// 	}
// 	sf_write_float(outFile, dataout, outFrames);
// 	sf_close(outFile);
}
