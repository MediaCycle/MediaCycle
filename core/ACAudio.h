/*
 *  ACAudio.h
 *  MediaCycle
 *
 *  @author Damien Tardieu
 *  @date 22/06/09
 *  @author XS
 *  @copyright (c) 2009 – UMONS - Numediart
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




#ifndef ACAUDIO_H
#define ACAUDIO_H

#include "ACMedia.h"
#include "ACMediaFeatures.h"
#include <string>
#include <cstring>
#include <sys/stat.h>
using namespace std;


class ACAudio : public ACMedia {
	// contains the *minimal* information about an audio
	// is this too much already ?
public:
	ACAudio();
	ACAudio(const ACAudio&);

	~ACAudio();
  
	void save(FILE* library_file);
	void saveACL(ofstream &library_file);
	void saveMCSL(ofstream &library_file);//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
	int loadACL(ifstream &library_file);
	int loadMCSL(ifstream &library_file);//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
	int load(FILE* library_file);
	int load_v1(FILE* library_file); 
	
//	void import(std::string _path); // XS 240210 : migrated to ACMedia
	void saveThumbnail(std::string _path);
	void* getThumbnailPtr() { return (void*)waveform; }
	int getThumbnailWidth() {return waveformLength;} // width in thumbnail frames, not samples
	int getThumbnailHeight() {return 0;} // width in thumbnail frames, not samples
	int getWidth() {return sample_end;}
	int getHeight() {return 0;}
	
	ACMediaData* extractData(std::string fname);
	
	void setSampleRate(int _sample_rate) { sample_rate = _sample_rate; }
	int getSampleRate() { return sample_rate; }
	void setChannels(int _channels) { channels = _channels; }
	int getChannels() { return channels; }
	void setSampleStart(int _sample_start) { sample_start = _sample_start; }
	int getSampleStart() { return sample_start; }
	void setSampleEnd(int _sample_end) { sample_end = _sample_end; }
	int getSampleEnd() { return sample_end; }
	void setNFrames(long _n_frames) { n_frames = _n_frames; }
	long getNFrames() { return n_frames; }
	float getDuration() {return (float)n_frames/(float) sample_rate;}

	void setWaveformLength(int _waveformLength) { waveformLength = _waveformLength; }
	int getWaveformLength() { return waveformLength; }
	void setWaveform(float *_waveform) { waveform = _waveform; }
	float* getWaveform() { return waveform; }
	
	void computeWaveform(float* samples_v);

private:	
	int sample_rate;
	int channels;
	int sample_start;
	int sample_end;
	//float time_stamp_start;	// seconds
	//float time_stamp_end;
	long n_frames;
	int waveformLength;
	float* waveform; // typically one value every 10ms
	// following are specific to loops or music
	float db;
	float bpm;
	int time_signature_num;
	int time_signature_den;
	int key;
	int acid_type;
};


#endif // ACAUDIO_H
