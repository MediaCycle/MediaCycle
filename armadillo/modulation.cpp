/**
 * @brief modulation.cpp
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

#include "modulation.h"

void modulation(colvec xi_v, int fs, mat &modFr_m, mat &modAmp_m, colvec &modTime_v){

	int oldfs = fs;
	rowvec freqBandHz_v = "4 50;";
	double fmax = 50;
	double fmin = 4;
	
// 	if (fs > 4*fmax && fs > 100){
// 		int nfs = max(4*fmax, 100);
// 		int dfsrate = (fs+1)/nfs;
// 		nfs = fs / dfsrate;
// 		x = downsample(x, dfsrate);
//     fs  = nfs;
// 	}

	int windowsize = pow(2.0, (int)nextpow2(2/fmin * fs));
	int hopsize = windowsize/4;
	int mlag = (2/fmin * fs)+1;
	colvec fenetre_v = blackman(windowsize);
	double norma = sum(fenetre_v);
	int fftsize = pow(2.0, (nextpow2(windowsize)+1));
	int fmin_bin = fmin * fftsize / fs;
	int fmax_bin = fmax * fftsize / fs;
	rowvec freqBand_v = freqBandHz_v  * fftsize / fs;	
	long nbFrames = std::max((long)(xi_v.n_elem-windowsize)/hopsize+1, (long)1);
	colvec frame_v = colvec(windowsize);
	colvec frameW_v = colvec(windowsize);
	rowvec p_v(2);
	colvec sampleidx = linspace(0, windowsize-1, windowsize);
	colvec pos2_v, pos3_v;
 	icolvec pos_v;
	colvec frameFFTabs_v;
 	colvec frameFFTabs2_v(fftsize/2);
 	colvec frameFFTabs3_v;
	double max_value, max_pos;
	ucolvec sort_pos;
	double xMean;
	colvec x_v;
	if (xi_v.n_elem < windowsize){
		x_v.set_size(windowsize);
		x_v.rows(0, xi_v.n_rows-1) = xi_v;
	}
	else{
		x_v = xi_v;
	}

	modAmp_m.zeros(nbFrames, freqBandHz_v.n_elem-1);
	modFr_m.zeros(nbFrames, freqBandHz_v.n_elem-1);
	modTime_v.zeros(nbFrames);
	
	long counter=0;

	if ((nbFrames) > 1){
		modTime_v = windowsize/(2*fs) + linspace(0, nbFrames-1, nbFrames) * hopsize/fs;
	}
	else{
		modTime_v(0) = windowsize/(2*fs);
	}
	for (long i=0; i < x_v.n_elem-windowsize; i = i+hopsize){
		frame_v = x_v.rows(i,std::min(i+windowsize-1, (long)x_v.n_rows));
		xMean = mean(frame_v);
		frame_v = frame_v-xMean;
		p_v = linefit(sampleidx, frame_v);
		frame_v = frame_v - p_v(0) * sampleidx + p_v(1);

		frameW_v = x_v.rows(i,i+windowsize-1)%fenetre_v;
		
		frameFFTabs_v = abs(fft(frameW_v*2/norma, fftsize));
 		frameFFTabs2_v = frameFFTabs_v.rows(0,fftsize/2-1);
		pos_v = findpeaks(frameFFTabs2_v, 2);
		
		
		if (pos_v.n_elem > 0){
			for (int b = 0; b < freqBand_v.n_elem-1; b++){
				pos2_v = find( (pos_v >= freqBand_v(b)) % (pos_v <= freqBand_v(b+1))>0 );
				pos3_v.set_size(pos2_v.n_elem);
				frameFFTabs3_v.set_size(pos2_v.n_elem);
				if (pos2_v.n_elem > 0){
					for (int k = 0; k < pos2_v.n_elem; k++){
						pos3_v(k) = pos_v(pos2_v(k));
						frameFFTabs3_v(k) = frameFFTabs2_v(pos3_v(k));
					}
					max_value = max(frameFFTabs3_v);
					sort_pos = sort_index(frameFFTabs3_v,1);
					max_pos = sort_pos(0);
					modFr_m(counter,b) = (pos3_v(max_pos)) * fs / (fftsize) ;
					modAmp_m(counter,b)  = max_value / xMean;
					//				f0_ph(k,b) = angle(xfft(pos3_v(max_pos),k));
				}
				else{
					modFr_m(counter,b) = 0;
					modAmp_m(counter,b)  = 0;
				}
			}
		}
		counter++;
	}
}
