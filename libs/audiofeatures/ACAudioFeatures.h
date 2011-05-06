/**
 * @brief ACAudioFeatures.h
 * @author Jérôme Urbain
 * @date 06/05/2011
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

#ifndef _ACAUDIOFEATURES_H
#define _ACAUDIOFEATURES_H

#include <stdio.h>
#include <sndfile.h>
#include <string.h>
#include <armadillo>
#include <vector>
#include <ACMediaTimedFeature.h>
#include "fftw3.h"
#include "ifft-helper.h"


//#include "round.h"

ACMediaTimedFeature* computeFeature(float* data, 
									std::string featureName, 
																		 int samplerate, 
																		 int nchannels, 
																		 long length,
																		 int mfccNbChannels = 32, 
																		 int mfccNb = 13, 
																		 int windowSize=512, 	
																		 bool extendSoundLimits = false,
                                                                                                                                                 int minOctave=2,
                                                                                                                                                 int maxOctave=7);
std::vector<ACMediaTimedFeature*> computeFeatures(float* data, 
												  std::vector<std::string> descList, 
																									 int samplerate, 
																									 int nchannels, 
																									 long length, 
																									 int mfccNbChannels = 32, 
																									 int mfccNb = 13, 
																									 int windowSize=512, 	
																									 bool extendSoundLimits = false,
                                                                                                                                                                                                         int minOctave=2,
                                                                                                                                                                                                         int maxOctave=7);
std::vector<ACMediaTimedFeature*> computeFeatures(float* data, 
																									 int samplerate, 
																									 int nchannels, 
																									 long length, 
																									 int mfccNbChannels = 32, 
																									 int mfccNb = 13, 
																									 int windowSize=512, 	
																									 bool extendSoundLimits = false,
                                                                                                                                                                                                         int minOctave=2,
                                                                                                                                                                                                         int maxOctave=7);

int resample(float* datain, SF_INFO *sfinfo, float* dataout, SF_INFO* sfinfoout);
double spectralCentroid(arma::colvec x_v);
double spectralSpread(arma::colvec x_v);
arma::rowvec spectralFlatness(arma::colvec x_v, int fftSize, int sr_hz);
double spectralVariation(arma::colvec, arma::colvec);
double spectralFlux(arma::colvec x_v, arma::colvec xPrev_v);
double zcr(arma::colvec frame_v, int sr_hz, int frameSize);
double spectralDecrease(arma::colvec x_v);
double energyRMS(arma::colvec frame_v);
double loudness(arma::colvec x_v, arma::mat melfilter_m);
double logAttackTime(arma::colvec ener_v, int sr_hz);
arma::rowvec effectiveDuration(arma::colvec time_v, arma::colvec loud_v);
arma::rowvec mfcc(arma::colvec x_v, arma::mat melfilter_m, int mfccNb);
arma::colvec burstBoundaries(arma::colvec split_v, arma::colvec time_v, float minBurstDur);
arma::rowvec chirpGroupDelay(arma::colvec, int, int, float);
arma::colvec GetLPCresidual(arma::colvec, int);
arma::mat Abeer_EstimatePitch(arma::colvec, int, int, int);
arma::rowvec GetHNR(arma::colvec, int, int);
arma::mat compute_HNR(arma::colvec signal_v, arma::colvec f0, int hopSize, int samplerate, int Nperiods_HNR, arma::mat HNR_m);
arma::mat compute_pitch(arma::colvec res, int f0min, int f0max, int NiterPitch, int nbFrames, int samplerate, int hopSize);
arma::colvec levinson(const arma::colvec &R2, int order);
arma::cx_mat fftw_forward(arma::colvec x_m, int n);
// ifft 1D for real column vector
arma::colvec fftw_backward(arma::colvec x_m, int n);
arma::mat ifft(arma::mat x_m, int n);

#endif
