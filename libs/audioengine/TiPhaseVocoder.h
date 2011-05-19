/* 
 * File:   TiPhaseVocoder.h
 * Author: Alexis Moinet
 *
 * @date 15 janvier 2009
 * @copyright (c) 2009 – UMONS - Numediart
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

#ifndef _TIPHASEVOCODER_H
#define	_TIPHASEVOCODER_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "fft-sptk.h"

#define HOPSIZEFACTOR 4 // (winsize/hopsize) == 4
#define BYTESPERSAMPLE 2//2 bytes/16 bits per sample

#ifdef	__cplusplus
extern "C" {
#endif
    typedef struct {
        double *re;
        double *im;
    } COMPLEXD;
 
	
    typedef struct {
        FILE *f;
        long numberOfSamples;
        double currentSample; //double pour éviter l'accumulation d'erreurs d'arrondi
        int flagLoop;
        
        //Phase locking
        int lockingMode;
        int *peaksIndex;
        
        int winSize;
        int hopSize;
        double speed;//> 1 = faster, 0 < . < 1 = slower, < 0 = backward

        double factor;// 0<= factor < 1 = currentSample/hopsize - (int) (currentSample/hopsize)
        double normalizationFactor;

        long currentFrame;
        double *hanning;
        
        //COMPLEX *leftFFT;
        //COMPLEX *rightFFT;

        COMPLEXD leftFFT;
        COMPLEXD rightFFT;
        COMPLEXD outputFFT;

        double *dphase;
        double *outputAmplitude;
        double *outputPhase;
        
        //COMPLEX tmpoutput;
        double *output;

        int bufferPos;
        double *buffer;
		
        // SD
        short *samples;

    } TiPhaseVocoder;

double TiGetHanningSample (int i, int winsize) ;
int TiCreateHanningWindow(double *win,int winsize);
int TiFreeHanningWindow(double *win);
int TiWindowingS2D(short *data, double *dataw, int winsize,double *window);
int setCurrentSample(TiPhaseVocoder *tpv,double value) ;
int setCurrentSampleToNext(TiPhaseVocoder *tpv) ;
int initPV(TiPhaseVocoder *tpv);
int setMode(TiPhaseVocoder *tpv, int mode);
int setWinsize(TiPhaseVocoder* tpv, int winsize);
int reallocCOMPLEX(COMPLEXD *x,int size);
int switchCOMPLEX(COMPLEXD* x, COMPLEXD* y);
int freeCOMPLEX(COMPLEXD *x);
int TiFreePhaseVocoder(TiPhaseVocoder *tpv);
int getCurrentFrame(TiPhaseVocoder *tpv,int flagResetPhase);
int computeAmplitudeAndPhase(COMPLEXD *complexfft, int nfft);
int computeDeltaPhase(TiPhaseVocoder *tpv);
int computeOutputFrame(TiPhaseVocoder *tpv);
int doOLA(TiPhaseVocoder* tpv);
int findPeaks(double *data, int *datapeaks, int winsize, int neighbours);
int findMax(double *data, int winsize, int init);
int openFile(TiPhaseVocoder* tpv, const char* filename);
int closeFile(TiPhaseVocoder* tpv);

// SD
int setSamples(TiPhaseVocoder* tpv, short* datashort, int size, int freq);

#ifdef	__cplusplus
}
#endif

#endif	/* _TIPHASEVOCODER_H */

