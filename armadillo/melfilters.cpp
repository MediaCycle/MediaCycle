/**
 * @brief melfilters.cpp
 * @author Jerome Urbain
 * @date 22/09/2010
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

#include "Armadillo-utils.h"
#include "melfilters.h"

mat melfilters(int nChannels, int fftSize, int sr_hz){


	float fmin = 0;
	float fmax = (float)sr_hz/2;
	float f2b = (float)fftSize/(float)sr_hz;
	float bmax = fftSize/2;
	
	float mmin = freq2mel(fmin);
	float mmax = freq2mel(fmax);

	float chanDist = (mmax - mmin)/(nChannels+1);
	rowvec centerM_v = linspace<rowvec>(0, nChannels+1, nChannels+2);
	centerM_v = centerM_v*chanDist;
	centerM_v(nChannels+1) = mmax;

	//	centerM_v.save("centerM.txt", arma_ascii);

	rowvec centerB_v = mel2freq(centerM_v)*f2b;
	
	//	centerB_v.save("centerB.txt", arma_ascii);
	
	mat melf_m = zeros<mat>(bmax, nChannels);

        //code compliant to MATLAB: filters linear in MEL
        float currentMel, dist;
	for (int iChan=1; iChan < nChannels+1; iChan++){
		for (int iBin=centerB_v(iChan-1); iBin <= centerB_v(iChan+1); iBin++){
			if(iBin > centerB_v(iChan-1) & iBin < centerB_v(iChan)){
                            currentMel=freq2mel(iBin/f2b);
                            dist=centerM_v(iChan)-currentMel;
                            melf_m(iBin, iChan-1) = 2*(1-dist/chanDist);
			}
			else if(iBin >= centerB_v(iChan) & iBin <= centerB_v(iChan+1)) {
                            currentMel=freq2mel(iBin/f2b);
                            dist=currentMel-centerM_v(iChan);
                            melf_m(iBin, iChan-1) = 2*(1-dist/chanDist);			
			}
		}
		//melf_m.col(iChan-1) = f2b*melf_m.col(iChan-1)/(2*(centerB_v(iChan+1)-centerB_v(iChan-1)));
	}

        // old code:
        /*for (int iChan=1; iChan < nChannels+1; iChan++){
		for (int iBin=0; iBin < bmax; iBin++){
			if(iBin > centerB_v(iChan-1) & iBin < centerB_v(iChan)){
				melf_m(iBin, iChan-1) = (iBin - centerB_v(iChan-1))/(centerB_v(iChan)-centerB_v(iChan-1));
			}
			else if(iBin >= centerB_v(iChan) & iBin <= centerB_v(iChan+1)) {
				melf_m(iBin, iChan-1) = (iBin - centerB_v(iChan+1))/(centerB_v(iChan)-centerB_v(iChan+1));			
			}
		}
		melf_m.col(iChan-1) = f2b*melf_m.col(iChan-1)/(2*(centerB_v(iChan+1)-centerB_v(iChan-1)));
	}*/

	
	return melf_m;
}
