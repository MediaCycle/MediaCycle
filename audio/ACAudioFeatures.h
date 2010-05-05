/**
 * @brief ACAudioFeatures.h
 * @author Damien Tardieu
 * @date 05/05/2010
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

#ifndef _ACAUDIOFEATURES_H
#define _ACAUDIOFEATURES_H

#include <stdio.h>
#include <sndfile.h>
#include <string.h>
#include <armadillo>
#include <vector>
#include "ACMediaTimedFeature.h"

using namespace arma;


std::vector<ACMediaTimedFeatures*> computeFeatures(float* data, int samplerate, int nchannels, int length,int mfccNbChannels = 16, int mfccNb = 13, int windowSize=512, 	bool extendSoundLimits = false); 
int resample(float* datain, SF_INFO *sfinfo, float* dataout, SF_INFO* sfinfoout);
double spectralCentroid(colvec x_v);
double spectralSpread(colvec x_v);
double spectralVariation(colvec, colvec);
double spectralFlux(colvec x_v, colvec xPrev_v);
double zcr(colvec frame_v, int sr_hz, int frameSize);
double spectralDecrease(colvec x_v);
double energyRMS(colvec frame_v);
double loudness(colvec x_v, mat melfilter_m);
double logAttackTime(colvec ener_v, int sr_hz);
rowvec effectiveDuration(colvec time_v, colvec loud_v);
rowvec mfcc(colvec x_v, mat melfilter_m, int mfccNb);

#endif
