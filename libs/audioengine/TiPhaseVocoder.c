/*
 * File:   TiPhaseVocoder.c
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
#include "TiPhaseVocoder.h"

double TiGetHanningSample (int k, int winsize) {
  return ( 0.5 * (1.0 - cos (2.0*M_PI*(double)k/(double)(winsize))) );
}

int TiCreateHanningWindow(double *win,int winsize) {
    int k;
    
    for(k=0;k<winsize;k++)
        win[k] = TiGetHanningSample(k,winsize);

    return 0;
}

int TiFreeHanningWindow(double *win) {
    free(win);
    return 0;
}

int TiWindowingS2D(short *data, double *dataw, int winsize,double *window) {
    int k;
    double norm = 1;//2^15 -1;

    for (k = 0; k < winsize; k++) {
        dataw[k] = data[k]*window[k]/norm;
    }

    return 0;
}

int setCurrentSample(TiPhaseVocoder *tpv,double value) {
    //boundaries check :
    //if in-boundaries, does nothing
    //else : loop (should happen only once btw - 'while' is used 'just in case' but it could be replaced by 'if');

    if (tpv->flagLoop) {
        while(value >= tpv->numberOfSamples)
            value -= tpv->numberOfSamples;
        while (value < 0)
            value += tpv->numberOfSamples;
    }

    tpv->currentSample = value;

    return 0;
}

int setCurrentSampleToNext(TiPhaseVocoder *tpv) {
    long tmp;

    tmp = tpv->currentSample + tpv->speed * tpv->hopSize;
    setCurrentSample(tpv,tmp);

    return 0;
}

int initPV(TiPhaseVocoder *tpv){
    setCurrentSample(tpv,0.0);//might want to add an offset here ?
	if (tpv->hanning)
		free(tpv->hanning);
    tpv->hanning = 0;
	if (tpv->dphase)
		free(tpv->dphase);
    tpv->dphase = 0;
	if (tpv->leftFFT.re)
		free(tpv->leftFFT.re);
    tpv->leftFFT.re = 0;
	if (tpv->leftFFT.im)
		free(tpv->leftFFT.im);
    tpv->leftFFT.im = 0;
	if (tpv->rightFFT.re)
		free(tpv->rightFFT.re);
    tpv->rightFFT.re = 0;
	if (tpv->rightFFT.im)
		free(tpv->rightFFT.im);
    tpv->rightFFT.im = 0;
	if (tpv->outputFFT.re)
		free(tpv->outputFFT.re);
    tpv->outputFFT.re = 0;
	if (tpv->outputFFT.im)
		free(tpv->outputFFT.im);
    tpv->outputFFT.im = 0;
	if (tpv->output)
		free(tpv->output);
    //tpv->tmpoutput = 0;
    tpv->output = 0;
	if (tpv->outputAmplitude)
		free(tpv->outputAmplitude);
    tpv->outputAmplitude = 0;
	if (tpv->outputPhase)
		free(tpv->outputPhase);
    tpv->outputPhase = 0;
    tpv->bufferPos = 0;
	if (tpv->buffer)
		free(tpv->buffer);
    tpv->buffer = 0;
	
    tpv->currentFrame = -100;
    tpv->lockingMode = 0;
	if (tpv->peaksIndex)
		free(tpv->peaksIndex);
    tpv->peaksIndex = 0;
	tpv->needResetPhase = 1;

    tpv->normalizationFactor = 1/1.5;//corresponds to a shift of 1/4 between each hanning-windowed frame. 1/2.0 if shift=1/8

    return 0;
}

int setLockingMode(TiPhaseVocoder *tpv, int mode) {
    //adapt boundaries as needed (for now, two possible working modes : 0 and 1)
    if (mode >=0 && mode < 2) { 
        tpv->lockingMode = mode;
        return 0;
    } else {
        return -1;
    }
}

int setWinsize(TiPhaseVocoder *tpv, int winsize) {
    if (winsize >= 0) {
        tpv->hanning = (double *) realloc(tpv->hanning,winsize*sizeof(double));
        TiCreateHanningWindow(tpv->hanning,winsize);
        tpv->dphase = (double *) realloc(tpv->dphase,winsize*sizeof(double));

        reallocCOMPLEX(&tpv->leftFFT,winsize);
        reallocCOMPLEX(&tpv->rightFFT,winsize);
        reallocCOMPLEX(&tpv->outputFFT,winsize);
        //reallocCOMPLEX(&tpv->tmpoutput,winsize);
        
        tpv->output = (double *) realloc(tpv->output,winsize*sizeof(double));
        tpv->outputAmplitude = (double *) realloc(tpv->outputAmplitude,winsize*sizeof(double));
        tpv->outputPhase = (double *) realloc(tpv->outputPhase,winsize*sizeof(double));
        tpv->buffer = (double *) realloc(tpv->buffer,winsize*sizeof(double));

		memset(tpv->dphase, 0, winsize*sizeof(double));
		memset(tpv->output, 0, winsize*sizeof(double));
		memset(tpv->outputAmplitude, 0 , winsize*sizeof(double));
		memset(tpv->outputPhase, 0 , winsize*sizeof(double));
		memset(tpv->buffer, 0, winsize*sizeof(double));
		
        tpv->winSize = winsize;
        tpv->hopSize = (int) winsize/HOPSIZEFACTOR;
        
        //peaks for the phase locking
        //TODO : alloc only if lockingMode==1 (although it is not very memory intensive, one frame of int ...)
        tpv->peaksIndex = (int *) realloc(tpv->peaksIndex, (tpv->winSize) * sizeof (int));
    }
 
    return 0;
}
int reallocCOMPLEX(COMPLEXD *x,int size) {
    x->re = (double *) realloc(x->re,size*sizeof(double));
    x->im = (double *) realloc(x->im,size*sizeof(double));
    return 0;
}
int switchCOMPLEX(COMPLEXD *x,COMPLEXD *y) {
    double *tmp;

    tmp = x->re;
    x->re  = y->re;
    y->re = tmp;

    tmp = x->im;
    x->im  = y->im;
    y->im = tmp;

    return 0;

}
int freeCOMPLEX(COMPLEXD *x) {
    free(x->re);
    free(x->im);

    return 0;
}
int TiFreePhaseVocoder(TiPhaseVocoder *tpv) {
    free(tpv->hanning);
    free(tpv->dphase);
    freeCOMPLEX(&tpv->leftFFT);
    freeCOMPLEX(&tpv->rightFFT);
    freeCOMPLEX(&tpv->outputFFT);
    //freeCOMPLEX(&tpv->tmpoutput);
    free(tpv->output);
    free(tpv->outputAmplitude);
    free(tpv->outputPhase);

    return 0;
}

int getCurrentFrame(TiPhaseVocoder *tpv,int flagResetPhase) {
    double tmppos;
    int currentFrame, nsamples, result, k;
    short *tmpdata = 0;
    FILE *logf;

    tmppos = tpv->currentSample/tpv->hopSize;
    currentFrame = (int) tmppos;
    tpv->factor = tmppos - currentFrame;//wheighting factor between leftFrame and rightFrame (0 < . < 1)
    
    // SD
    int toread;
	
    if (currentFrame != tpv->currentFrame) {
        if (currentFrame == tpv->currentFrame+1) {
            //switch right & left + get frameright + compute fftright + delta-phase
            switchCOMPLEX(&tpv->leftFFT,&tpv->rightFFT);

            tmpdata = (short *) malloc((tpv->winSize)*sizeof(short));
			memset(tmpdata, 0, (tpv->winSize)*sizeof(short));
			
			// SD
            if (tpv->samples) {
				toread = tpv->numberOfSamples-currentFrame*(tpv->hopSize+1);
				if (tpv->winSize<toread) {
					toread = tpv->winSize;
				}
				if (toread>0) {
					memcpy(tmpdata,(tpv->samples)+currentFrame*(tpv->hopSize+1),toread*sizeof(short));
				}
			}
			else if (tpv->f) {
				result = fseek(tpv->f,currentFrame*(tpv->hopSize+1)*BYTESPERSAMPLE,SEEK_SET);
				nsamples = fread(tmpdata,BYTESPERSAMPLE,tpv->winSize,tpv->f);
			}
			
            TiWindowingS2D(tmpdata,tpv->rightFFT.re,tpv->winSize,tpv->hanning);
            memset(tpv->rightFFT.im,0,tpv->winSize*sizeof(double));
            //fprintf("%f +i* %f\n",tmpdataw[tpv->winsize/2].re,tmpdataw[tpv->winsize/2].im);
            fftr(tpv->rightFFT.re,tpv->rightFFT.im,tpv->winSize);

            computeAmplitudeAndPhase(&tpv->rightFFT,tpv->winSize);//could be (winsize/2 +1)
        } else if (currentFrame == tpv->currentFrame-1) {
            //switch left to right + get frameleft + compute fftleft + delta-phase
            switchCOMPLEX(&tpv->leftFFT,&tpv->rightFFT);

            tmpdata = (short *) malloc((tpv->winSize)*sizeof(short));
			memset(tmpdata, 0, (tpv->winSize)*sizeof(short));
			
			// SD
			if (tpv->samples) {
				toread = tpv->numberOfSamples-currentFrame*(tpv->hopSize-1);
				if (tpv->winSize<toread) {
					toread = tpv->winSize;
				}
				if (toread>0) {
					memcpy(tmpdata,(tpv->samples)+currentFrame*(tpv->hopSize-1),toread*sizeof(short));
				}
			}
			else if (tpv->f) {
				result = fseek(tpv->f,currentFrame*(tpv->hopSize-1)*BYTESPERSAMPLE,SEEK_SET);
				nsamples = fread(tmpdata,BYTESPERSAMPLE,tpv->winSize,tpv->f);
			}
			
            TiWindowingS2D(tmpdata,tpv->leftFFT.re,tpv->winSize,tpv->hanning);
            memset(tpv->leftFFT.im,0,tpv->winSize*sizeof(double));

            fftr(tpv->leftFFT.re,tpv->leftFFT.im,tpv->winSize);

            computeAmplitudeAndPhase(&tpv->leftFFT,tpv->winSize);
        } else {
            //logf = fopen("log.bin","wb");
            tmpdata = (short *) malloc((tpv->winSize+tpv->hopSize)*sizeof(short));
			memset(tmpdata, 0, (tpv->winSize+tpv->hopSize)*sizeof(short));
			
			// SD
            if (tpv->samples) {
				toread = tpv->numberOfSamples-currentFrame*tpv->hopSize;
				if (tpv->winSize+tpv->hopSize<toread) {
					toread = tpv->winSize+tpv->hopSize;
				}
				if (toread>0) {
					memcpy(tmpdata,(tpv->samples)+currentFrame*tpv->hopSize,toread*sizeof(short));
				}
			}
			else if (tpv->f) {
				result = fseek(tpv->f,currentFrame*tpv->hopSize*BYTESPERSAMPLE,SEEK_SET);
				nsamples = fread(tmpdata,BYTESPERSAMPLE,(tpv->winSize+tpv->hopSize),tpv->f);
			}
			            
			TiWindowingS2D(tmpdata,tpv->leftFFT.re,tpv->winSize,tpv->hanning);
            memset(tpv->leftFFT.im,0,tpv->winSize*sizeof(double));

/*
            for (k=2044;k<2052;k++)
                printf("data : %f +i* %f\n",tpv->leftFFT.re[k],tpv->leftFFT.im[k]);
            printf("\n");
            
            fwrite(tpv->leftFFT.re,sizeof(double),tpv->winSize,logf);
            fwrite(tpv->leftFFT.im,sizeof(double),tpv->winSize,logf);
*/

            fftr(tpv->leftFFT.re,tpv->leftFFT.im,tpv->winSize);

/*
            for (k=2044;k<2052;k++)
                printf("fft : %f +i* %f\n",tpv->leftFFT.re[k],tpv->leftFFT.im[k]);
            
            fwrite(tpv->leftFFT.re,sizeof(double),tpv->winSize,logf);
            fwrite(tpv->leftFFT.im,sizeof(double),tpv->winSize,logf);

            fclose(logf);
            printf("\n");
*/

            TiWindowingS2D(tmpdata+tpv->hopSize,tpv->rightFFT.re,tpv->winSize,tpv->hanning);
            memset(tpv->rightFFT.im,0,tpv->winSize*sizeof(double));

            fftr(tpv->rightFFT.re,tpv->rightFFT.im,tpv->winSize);


            computeAmplitudeAndPhase(&tpv->leftFFT,tpv->winSize);
            computeAmplitudeAndPhase(&tpv->rightFFT,tpv->winSize);
        }

        computeDeltaPhase(tpv);
        tpv->currentFrame = currentFrame;
    }

    if ( (flagResetPhase) || (tpv->needResetPhase) ) {
        resetPhase(tpv);
		tpv->needResetPhase = 0;
	}
	
    computeOutputFrame(tpv);

    if (tmpdata)
        free(tmpdata);

    return 0;
}
int computeAmplitudeAndPhase(COMPLEXD *complexfft, int nfft) {
    int k;
    double re,im;

    for (k=0;k<nfft/2+1;k++) {
        re = complexfft->re[k];
        im = complexfft->im[k];
        complexfft->re[k] = sqrt(re*re + im*im);//amplitude
        complexfft->im[k] = atan2(im,re);//phase
    }
    return 0;
}

int computeDeltaPhase(TiPhaseVocoder *tpv) {
    int k;

    for(k=0;k<tpv->winSize/2+1;k++) {
        tpv->dphase[k] = tpv->rightFFT.im[k] - tpv->leftFFT.im[k];//im = phase after a call to computeAmplitudeAndPhase
    }
    
    return 0;
}
int computeOutputFrame(TiPhaseVocoder *tpv) {
	//NB : ifft(Z) == fft(Z'*j)'*j/NFFT
    // z = x+j*y --> Re(ifft(x,y)) = Im(fft(y,x))/NFFT (cf.matlab & SPTK)
    int k, npeaks, p, pindex, start, stop;
    double complfactor, globaldphase;

    complfactor = 1 - tpv->factor;
    //printf("factor : %g\n",tpv->factor);
    //A*exp(j*P) = z = (x,y) --> (y,x)

    //output amplitude computation
    for (k = 0; k <= tpv->winSize / 2; k++) {
        // interpolation doesn't seem to improve the result that much on music
        // TODO: compare w/ vs w/o interpolation for ambient noise
        tpv->outputAmplitude[k] = complfactor * tpv->leftFFT.re[k] + tpv->factor * tpv->rightFFT.re[k]; //tpv->leftFFT.re[k];//
    }

    //given amplitude & phase : compute Re and Im part of the fft
    tpv->outputFFT.re[0] = tpv->outputAmplitude[0] * cos(tpv->outputPhase[0]);
    tpv->outputFFT.im[0] = tpv->outputAmplitude[0] * sin(tpv->outputPhase[0]);

    tpv->outputFFT.re[tpv->winSize / 2] = tpv->outputAmplitude[tpv->winSize / 2] * cos(tpv->outputPhase[tpv->winSize / 2]);
    tpv->outputFFT.im[tpv->winSize / 2] = tpv->outputAmplitude[tpv->winSize / 2] * sin(tpv->outputPhase[tpv->winSize / 2]);

    for (k = 1; k < tpv->winSize / 2; k++) {
        tpv->outputFFT.re[k] = tpv->outputAmplitude[k] * cos(tpv->outputPhase[k]);
        tpv->outputFFT.im[k] = tpv->outputAmplitude[k] * sin(tpv->outputPhase[k]);
        tpv->outputFFT.re[tpv->winSize - k] = tpv->outputFFT.re[k];
        tpv->outputFFT.im[tpv->winSize - k] = -tpv->outputFFT.im[k];

        //transparent reconstruction :
        //tpv->outputFFT.re[k] = tpv->leftFFT.im[k];
        //tpv->outputFFT.im[k] = tpv->leftFFT.re[k];
    }

    //Get the real data from the symetrical fft
    ifftr(tpv->outputFFT.re, tpv->outputFFT.im, tpv->winSize);

    for (k = 0; k < tpv->winSize; k++) {
        //if division by NFFT is needed, it can be done here (for now it is already done in ifftr)
        tpv->output[k] = tpv->outputFFT.re[k];
    }
    
	//output phase computation
    switch (tpv->lockingMode) {
        case 0:
            //normal phase vocoder
            for (k = 0; k < tpv->winSize / 2 + 1; k++) {
                tpv->outputPhase[k] += tpv->dphase[k];
            }
            break;
        case 1:
            //phase-locked vocoder
            //This phase locking is probably patented : http://www.google.com/patents?id=kaAEAAAAEBAJ
            //Therefore I suppose it should either be licenced or removed
            //in any case, it does not improve results when slowing down noisy ambient sound (cf. evs wave files)
            //so we might as well NOT use it and find some other interesting and patent-free approach
            npeaks = findPeaks(tpv->outputAmplitude, tpv->peaksIndex, tpv->winSize / 2 + 1, 2);
            if (npeaks > 0) {
                p = 0;
                start = 0;
                do {
                    pindex = tpv->peaksIndex[p];
                    tpv->outputPhase[pindex] += tpv->dphase[pindex];
                    globaldphase = tpv->outputPhase[pindex] - tpv->leftFFT.im[pindex];

                    if (p + 1 < npeaks) {
                        stop = (tpv->peaksIndex[p + 1] + pindex) / 2;
                    } else {
                        stop = tpv->winSize / 2;
                    }

                    for (k = start; k < pindex; k++) {
                        tpv->outputPhase[k] = tpv->leftFFT.im[k] + globaldphase;
                    }
                    for (k = pindex + 1; k <= stop; k++) {
                        tpv->outputPhase[k] = tpv->leftFFT.im[k] + globaldphase;
                    }
                    start = stop + 1;
                    p++;
                } while (stop < tpv->winSize / 2);
            }
            break;
/*      case 2:
            //hybrid mode (searching for patent/previous art)
            //this mode eats up 5 more percent of CPU (~4-5% becomes ~10-11% when in use)
            for (k = 0; k < tpv->winSize - tpv->hopSize; k++) {
                    tpv->randdata[k] = tpv->randdata[k + tpv->hopSize];
            }
            //this part of the code takes more than 5% of CPU (i.e. it *doubles* the CPU usage)
            //since it's only about generating random numbers + fft + getPhase
            //we could pre-compute those phase values (e.g. for 5 seconds of noise),
            //store them in a file and load them whenever needed
            //(+ loop if sound is longer than 5s)
            // that has been tested in Matlab and works fine
            gauss_drand(tpv->randdata + tpv->winSize - tpv->hopSize - 1, tpv->hopSize); //1-2% CPU
            TiWindowingD2D(tpv->randdata, tpv->randFFT.re, tpv->winSize, tpv->hanning); //1% CPU
            memset(tpv->randFFT.im, 0, tpv->winSize * sizeof (double)); //~0%
            fftr(tpv->randFFT.re, tpv->randFFT.im, tpv->winSize); //2-3% CPU
            computePhase(&tpv->randFFT, tpv->winSize); //<1% CPU
            for (k = 0; k < tpv->winSize / 2 + 1; k++) {
                    tpv->outputPhase[k] = tpv->randFFT.im[k];
            }
            break;
 */
        default:
            printf("error : invalid mode (%d)\n", tpv->lockingMode);
    }

    return 0;
}

int resetPhase(TiPhaseVocoder *tpv) {
    int k;
    for(k=0;k<tpv->winSize;k++)
        tpv->outputPhase[k] = tpv->leftFFT.im[k];
}

int findPeaks(double *data, int *datapeaks, int winsize, int neighbours) {
	int start, stop, candidate, p = 0, k = 0, flag = 0;

	//This algorithm can certainly be improved (SOA C/C++ in peak detection ?)
	while (k < winsize) {
		start = k - neighbours;
		stop = k + neighbours;
		if (start < 0) start = 0;
		if (stop >= winsize) stop = winsize - 1;

		//slight optimization : if (k < candidate) in previous loop
		//then we look for max in [candidate-neigh:candidate+neigh]
		//but the [cand-neigh:candidate] part has already been made
		//in the previous loop --> flag to avoid redoing the max lookup
		//gain 0.2s on 100000 iterations (3,1s without vs. 2,9s with)
		//which means a 6% improvement in time usage (pretty useless ?)
		if (flag) {
			candidate = findMax(data + start, stop - start + 1, candidate - start) + start;
		} else {
			candidate = findMax(data + start, stop - start + 1, 0) + start;
		}
		if (candidate == k) {
			datapeaks[p++] = k;
			k += neighbours + 1;
		} else {
			if (k < candidate) {
				k = candidate;
				flag = 1;
			} else {
				k++;
			}
		}
	}

	return p; //returns number of peaks detected
}

int findMax(double *data, int winsize, int init) {
	int k, index;
	double max;

	if (init > 0 && init < winsize) {
		index = init;
		max = data[init];
	} else {
		index = 0;
		max = data[0];
	}
	for (k = index + 1; k < winsize; k++) {
		if (data[k] > max) {
			index = k;
			max = data[k];
		}
	}

	return index;
}

int doOLA(TiPhaseVocoder *tpv) {
    //the samples generated by this function are located in tpv->buffer[tpv->bufferPos-tpv->hopSize:tpv->bufferPos-1]
    int k,m, resetPos;

    resetPos = tpv->bufferPos - tpv->hopSize;

    if (resetPos >= 0) {
        for (k=resetPos;k<tpv->bufferPos;k++) {
            tpv->buffer[k] = 0.0;
        }
    } else {
        resetPos += tpv->winSize;
        for (k=resetPos;k<tpv->winSize;k++) {
            tpv->buffer[k] = 0.0;
        }
        for (k=0;k<tpv->bufferPos;k++) {
            tpv->buffer[k] = 0.0;
        }

    }

    for (k=tpv->bufferPos,m=0;k<tpv->winSize;k++,m++) {
        tpv->buffer[k] += tpv->output[m]*tpv->hanning[m]*tpv->normalizationFactor;
    }

    for(k=0;m<tpv->winSize;k++,m++){
        tpv->buffer[k] += tpv->output[m]*tpv->hanning[m]*tpv->normalizationFactor;
    }

    tpv->bufferPos += tpv->hopSize;

    if (tpv->bufferPos >= tpv->winSize) {
        tpv->bufferPos -= tpv->winSize;
    }

    return 0;
}

int openFile(TiPhaseVocoder *tpv, const char* filename) {
    tpv->f = fopen(filename,"rb");
    
    if (tpv->flagLoop) {
        fseek(tpv->f,0,SEEK_END);
        tpv->numberOfSamples = ftell(tpv->f)/BYTESPERSAMPLE;
        fseek(tpv->f,0,SEEK_SET);
    }
    return 0;
}

int closeFile(TiPhaseVocoder *tpv) {
    fclose(tpv->f);
}

int setSamples(TiPhaseVocoder* tpv, short* datashort, int size, int freq) {
	tpv->numberOfSamples = size;
	if (tpv->samples) {
		free(tpv->samples);
		tpv->samples = 0;
	}
	tpv->samples = (short*) malloc(size*sizeof(short));
	memcpy(tpv->samples, datashort, size*sizeof(short));
    return 0;
}
