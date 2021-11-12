/*
 *  ACAudioData.cpp
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

#if defined (SUPPORT_AUDIO)
#include "ACAudioData.h"
#include <string>
#include <iostream>
using std::cerr;
using std::endl;
using std::string;

ACAudioData::ACAudioData(){ 
	this->init();
}

void ACAudioData::init() {
	media_type = MEDIA_TYPE_AUDIO;
	audio_ptr = 0;
	audio_frames = 0;
}

ACAudioData::ACAudioData(std::string _fname) { 
	this->init();
	file_name=_fname;
	this->readData(_fname);
}

ACAudioData::~ACAudioData() {
	if (audio_ptr != 0) delete [] audio_ptr;
}

bool ACAudioData::readData(std::string _fname){ 
	if(_fname=="")return false;
	SF_INFO sfinfo;
	SNDFILE* testFile;
	if (! (testFile = sf_open (_fname.c_str(), SFM_READ, &sfinfo))){  
		/* Open failed so print an error message. */
		printf ("Not able to open input file %s.\n", _fname.c_str()) ;
		/* Print the error message from libsndfile. */
		puts (sf_strerror (0)) ;
		return false;
		//		exit(1);
	}
	audio_ptr = new float[(long) sfinfo.frames * sfinfo.channels];
	audio_frames = sfinfo.frames;
	sf_readf_float(testFile, audio_ptr, sfinfo.frames);
	sf_close(testFile);
	return true;
}

void ACAudioData::setData(float* _data, float _sample_number){
	audio_frames = _sample_number;
	audio_ptr = (float *)malloc( _sample_number * sizeof( float ));
	memcpy(audio_ptr,_data,audio_frames* sizeof( float ));
	if( !audio_ptr ) {
		cerr << "<ACAudioData::setData> Could not set data" << endl;
	}
}

#endif //defined (SUPPORT_AUDIO)
