/*
 *  ACAudioRecorder.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 28/06/10
 *  @copyright (c) 2010 – UMONS - Numediart
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
 *  Based on http://loulou.developpez.com/tutoriels/openal/capture/
 */

#ifndef HEADER_ACAUDIORECORDER
#define HEADER_ACAUDIORECORDER

#include <MediaCycle.h>
class MediaCycle;

#ifdef USE_OPENAL
#include <al.h>
#include <alc.h>
#ifdef USE_OPENALEXT
#include <alext.h>
#endif
#endif
#ifdef USE_PORTAUDIO
#include <portaudio.h>
#endif

#include <sndfile.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <pthread.h>

class ACAudioRecorder {
	
public:
#ifdef USE_OPENAL
	ACAudioRecorder(ALCdevice* _device, int samplerate, int buffersize);
#endif
#ifdef USE_PORTAUDIO
	ACAudioRecorder(PaStream * _stream, int samplerate, int buffersize);
#endif
	~ACAudioRecorder();
	
	// MediaCycle to query database and browser
	MediaCycle				*media_cycle;
	void setMediaCycle(MediaCycle *media_cycle) { this->media_cycle = media_cycle; };
	
private:
	void init();
#ifdef USE_OPENAL
	ALCdevice* device;
	ALCdevice* captureDevice;
#endif
#ifdef USE_PORTAUDIO
	PaStream *stream;
#endif
	bool isRecording;

public:
	void getCaptureDeviceList(std::vector<std::string>& devices);
	bool isCaptureAvailable(const char* deviceName = 0);
	bool initCapture(const char* deviceName = 0);
	void shutdownCapture();	
	void startCapture();	
	void stopCapture();
	void threadCaptureEngine();
	void threadCaptureUpdate();	
#ifdef USE_OPENAL
	void saveSound(const std::string& filename, const std::vector<ALshort>& samples);
#endif
#ifdef USE_PORTAUDIO
	void saveSound(const std::string& filename, const std::vector<short>& samples);
#endif
	
private:
	
	int audio_samplerate;
	int audio_buffersize;

#ifdef USE_OPENAL
	std::vector<ALshort> samples;
#endif
#ifdef USE_PORTAUDIO
	std::vector<short> samples;
#endif
	pthread_t	   capture_engine;
	pthread_attr_t capture_engine_attr;
	void* capture_engine_arg;
	pthread_once_t capture_engine_once;
	pthread_mutex_t capture_engine_mutex;
	pthread_mutexattr_t capture_engine_mutex_attr;
	pthread_cond_t capture_engine_cond;
	pthread_condattr_t capture_engine_cond_attr;	
	pthread_mutex_t capture_engine_cond_mutex;
	pthread_mutexattr_t capture_engine_cond_mutex_attr;
};
#endif
