/*
 *  ACAudioData.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 7/04/11
 *  @copyright (c) 2011 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 */

#ifndef ACAUDIODATA_H
#define ACAUDIODATA_H

#if defined (SUPPORT_AUDIO)
#include "ACMediaData.h"
#include <sndfile.h>
#include <cstdlib>
#include <cstring>

class ACAudioData: public ACMediaData {
public:
	ACAudioData();
	~ACAudioData();
	ACAudioData(std::string _fname);

	void readData(std::string _fname);
	virtual void* getData() {return static_cast<void*>(audio_ptr);}
//	float* getData() {return audio_ptr;}
	void setData(float* _data, float _sample_number);

protected:
	virtual void init();

private:
	float getAudioLength() {return audio_frames;}

	float* audio_ptr;
	float  audio_frames;
	
};

#endif //defined (SUPPORT_AUDIO)
#endif // ACAUDIODATA_H
