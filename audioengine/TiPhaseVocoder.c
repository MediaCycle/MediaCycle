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
  return ( 0.5 * (1.0 - cos (2.0*M_PI*(double)k/(double)(winsize-1))) );
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
    tpv->hanning = NULL;
    tpv->dphase = NULL;
    tpv->leftFFT.re = NULL;
    tpv->leftFFT.im = NULL;
    tpv->rightFFT.re = NULL;
    tpv->rightFFT.im = NULL;
    tpv->outputFFT.re = NULL;
    tpv->outputFFT.im = NULL;
    //tpv->tmpoutput = NULL;
    tpv->output = NULL;
    tpv->outputAmplitude = NULL;
    tpv->outputPhase = NULL;
    tpv->bufferPos = 0;
    tpv->buffer = NULL;
    tpv->currentFrame = -100;
    return 0;
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

        tpv->winSize = winsize;
        tpv->hopSize = (int) winsize/HOPSIZEFACTOR;
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
    short *tmpdata = NULL;
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
				memcpy(tmpdata,(tpv->samples)+currentFrame*(tpv->hopSize-1),tpv->winSize*sizeof(short));
			}
			else if (tpv->f) {
				result = fseek(tpv->f,currentFrame*(tpv->hopSize-1)*BYTESPERSAMPLE,SEEK_SET);
				nsamples = fread(tmpdata,BYTESPERSAMPLE,tpv->winSize,tpv->f);
			}
			
            TiWindowingS2D(tmpdata,tpv->leftFFT.re,tpv->winSize,tpv->hanning);
            memset(tpv->rightFFT.im,0,tpv->winSize*sizeof(double));

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
            memset(tpv->rightFFT.im,0,tpv->winSize*sizeof(double));

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

    if(flagResetPhase)
        resetPhase(tpv);
    
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
    int k;
    double complfactor;

    complfactor = 1 - tpv->factor;
    //printf("factor : %g\n",tpv->factor);
    //A*exp(j*P) = z = (x,y) --> (y,x)
    tpv->outputAmplitude[0] = complfactor * tpv->leftFFT.re[0] + tpv->factor * tpv->rightFFT.re[0];
    tpv->outputFFT.re[0] = tpv->outputAmplitude[0] * cos(tpv->outputPhase[0]);
    tpv->outputFFT.im[0] = tpv->outputAmplitude[0] * sin(tpv->outputPhase[0]);

    tpv->outputAmplitude[tpv->winSize/2] = complfactor * tpv->leftFFT.re[tpv->winSize/2] + tpv->factor * tpv->rightFFT.re[tpv->winSize/2];
    tpv->outputFFT.re[tpv->winSize/2] = tpv->outputAmplitude[tpv->winSize/2] * cos(tpv->outputPhase[tpv->winSize/2]);
    tpv->outputFFT.im[tpv->winSize/2] = tpv->outputAmplitude[tpv->winSize/2] * sin(tpv->outputPhase[tpv->winSize/2]);

    for(k=1;k<tpv->winSize/2;k++) {
        tpv->outputAmplitude[k] = complfactor * tpv->leftFFT.re[k] + tpv->factor * tpv->rightFFT.re[k];
        //tpv->outputFFT.re[k] = tpv->outputAmplitude[k] * sin(tpv->outputPhase[k]); //!!! sin to get the imaginary part (this is not a mistake)
        //tpv->outputFFT.im[k] = tpv->outputAmplitude[k] * cos(tpv->outputPhase[k]); // !!! cos to get the real part (this is not a mistake)
        tpv->outputFFT.re[k] = tpv->outputAmplitude[k] * cos(tpv->outputPhase[k]);
        tpv->outputFFT.im[k] = tpv->outputAmplitude[k] * sin(tpv->outputPhase[k]);
        tpv->outputFFT.re[tpv->winSize-k] = tpv->outputFFT.re[k];
        tpv->outputFFT.im[tpv->winSize-k] = -tpv->outputFFT.im[k];
        
        //transparent reconstruction :
        //tpv->outputFFT.re[k] = tpv->leftFFT.im[k];
        //tpv->outputFFT.im[k] = tpv->leftFFT.re[k];
    }

    //fft(y,x)
    //for (k=2044;k<2052;k++)
    //    printf("fft : %f +i* %f\n",tpv->outputFFT.re[k],tpv->outputFFT.im[k]);
    //printf("\n");

    ifftr(tpv->outputFFT.re,tpv->outputFFT.im,tpv->winSize);
    //ifftr(tpv->outputFFT.re,tpv->outputFFT.im,tpv->winSize);

    //for (k=2044;k<2052;k++)
    //    printf("ifft : %f +i* %f\n",tpv->outputFFT.im[k]/tpv->winSize,tpv->outputFFT.re[k]/tpv->winSize);
    //printf("\n");

    //Im()/NFFT
    for(k=0;k<tpv->winSize;k++) {
        //tpv->output[k] = tpv->outputFFT.im[k]/tpv->winSize;
        tpv->output[k] = tpv->outputFFT.re[k];
    }

    for(k=0;k<tpv->winSize/2+1;k++) {
        tpv->outputPhase[k] += tpv->dphase[k];
    }

    return 0;
}

int resetPhase(TiPhaseVocoder *tpv) {
    int k;
    for(k=0;k<tpv->winSize;k++)
        tpv->outputPhase[k] = tpv->leftFFT.im[k];
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
        tpv->buffer[k] += (tpv->output[m]*tpv->hanning[m]);
    }

    for(k=0;m<tpv->winSize;k++,m++){
        tpv->buffer[k] += tpv->output[m]*tpv->hanning[m];
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
