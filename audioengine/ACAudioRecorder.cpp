/*
 *  ACAudioRecorder.cpp
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

#include "ACAudioRecorder.h"
#include <iostream>
//DT : to have access to the media/audio functions
#include "ACAudio.h"

void ACAudioRecorder::getCaptureDeviceList(std::vector<std::string>& devices)
{
    // Empty the list
    devices.clear();
#ifdef USE_OPENAL
    // Get the available capture devices
    const ALCchar* deviceList = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
	
    if (deviceList)
    {
        // Extract the device names contained in the returned list
        while (strlen(deviceList) > 0)
        {
			devices.push_back(deviceList);
            deviceList += strlen(deviceList) + 1;
        }
    }
#endif
#ifdef USE_PORTAUDIO
#warning "TODO"
#endif
}

bool ACAudioRecorder::isCaptureAvailable(const char* deviceName)
{
#ifdef USE_OPENAL
	if (device != NULL)
	{
		// Make sure that audio recording is supported by the soundcard
		if (alcIsExtensionPresent(device, "ALC_EXT_CAPTURE") == AL_FALSE)
		{
			std::cerr << "Audio recording is not supported by your system." << std::endl;
			return false;
		}
		else
			return true;
	}
	else
	{
		std::cerr << "Impossible to open the audio device." << std::endl;
		return false;
	}
#endif
#ifdef USE_PORTAUDIO
	return false;
#endif
}		

bool ACAudioRecorder::initCapture(const char* deviceName)
{
#ifdef USE_OPENAL
	if (device != NULL)
	{
		// Make sure that audio recording is supported by the soundcard
		if (alcIsExtensionPresent(device, "ALC_EXT_CAPTURE") == AL_FALSE)
		{
			std::cerr << "Audio recording is not supported by your system." << std::endl;
			return false;
		}
		
		// Open the device
		// SD TODO - check this - audio_samplerate twice
		captureDevice = alcCaptureOpenDevice(deviceName, audio_samplerate, AL_FORMAT_MONO16, audio_samplerate);
		if (!captureDevice)
		{
			std::cerr << "Impossible to open the recording device." << std::endl;
			return false;
		}	
		return true;
	}
	else
	{
		std::cerr << "Impossible to open the audio device." << std::endl;
		return false;
	}
#endif
#ifdef USE_PORTAUDIO
	return false;
#endif
}
	
void ACAudioRecorder::shutdownCapture()
{
#ifdef USE_OPENAL
	alcCaptureCloseDevice(captureDevice);
#endif
}	

void *threadCaptureEngineFunction(void *_capture_engine_arg)
{
	((ACAudioRecorder*)_capture_engine_arg)->threadCaptureEngine();
}

void ACAudioRecorder::startCapture()
{
#ifdef USE_OPENAL
	initCapture(NULL);//CF could be switched somewhere else, so as to specify the capture device
	
    // Starting the capture OpenAL-wise
    alcCaptureStart(captureDevice);

	// Empty the samples buffer (could be done later)
	samples.resize(0);

	isRecording = true;
	
	//CF Ripped and adapted from ACAudioFeedback
	
	pthread_attr_init(&capture_engine_attr);
	pthread_mutexattr_init(&capture_engine_cond_mutex_attr);
	/////////// SD TODO - maybe set the process-share attribute which allows other process to see the conditional variable
	pthread_mutex_init(&capture_engine_cond_mutex, &capture_engine_cond_mutex_attr);
	pthread_mutexattr_destroy(&capture_engine_cond_mutex_attr);	

	pthread_condattr_init(&capture_engine_cond_attr);
	/////////// SD TODO
	pthread_cond_init(&capture_engine_cond, &capture_engine_cond_attr); //
	pthread_condattr_destroy(&capture_engine_cond_attr);
	
	pthread_mutexattr_init(&capture_engine_mutex_attr);
	/////////// SD TODO - maybe set the process-share attribute which allows other process to see the conditional variable
	pthread_mutex_init(&capture_engine_mutex, &capture_engine_mutex_attr);
	pthread_mutexattr_destroy(&capture_engine_mutex_attr);
	
	capture_engine_arg = (void*)this;
	pthread_create(&capture_engine, &capture_engine_attr, &threadCaptureEngineFunction, capture_engine_arg);
	pthread_attr_destroy(&capture_engine_attr);
	std::cout << "This is when the capture actually starts" << std::endl;
#endif
}

void ACAudioRecorder::stopCapture()
{	
	pthread_cancel(capture_engine);
	isRecording = false;

#ifdef USE_OPENAL		
	// Stop the recording
    alcCaptureStop(captureDevice);
	
    // Empty the soundcard buffer containing possible leftover samples
    ALCint samplesAvailable;
    alcGetIntegerv(captureDevice, ALC_CAPTURE_SAMPLES, 1, &samplesAvailable);
    if (samplesAvailable > 0)
    {
        std::size_t Start = samples.size();
        samples.resize(Start + samplesAvailable);
        alcCaptureSamples(captureDevice, &samples[Start], samplesAvailable);
    }
	
    // Save the samples in an audio file
    saveSound("capture.wav", samples);
	
	// Empty the samples buffer (could be done later)
	samples.resize(0);
	
	// Shutdown capture
    //shutdownCapture();//CF pb: it also disables the sound feedback...
#endif
}	

void ACAudioRecorder::threadCaptureEngine()
{
	while (1)
    {
		pthread_testcancel();

#ifdef USE_OPENAL
		pthread_mutex_lock(&capture_engine_mutex);
		
        // Retrieve the number of available samples
        ALCint samplesAvailable;
        alcGetIntegerv(captureDevice, ALC_CAPTURE_SAMPLES, 1, &samplesAvailable);
		
        // Read these samples and add them to the vector
        if (samplesAvailable > 0)
        {
            std::size_t start = samples.size();
            samples.resize(start + samplesAvailable);
            alcCaptureSamples(captureDevice, &samples[start], samplesAvailable);
        }
		pthread_mutex_unlock(&capture_engine_mutex);
#endif
    }
}

#ifdef USE_OPENAL
void ACAudioRecorder::saveSound(const std::string& filename, const std::vector<ALshort>& samples)
#endif
#ifdef USE_PORTAUDIO
void ACAudioRecorder::saveSound(const std::string& filename, const std::vector<short>& samples)
#endif
{
	SF_INFO fileInfo;
	fileInfo.channels   = 1;
	fileInfo.samplerate = audio_samplerate;
	fileInfo.format     = SF_FORMAT_PCM_16 | SF_FORMAT_WAV;
	
	SNDFILE* file = sf_open(filename.c_str(), SFM_WRITE, &fileInfo);
	if (!file)
	{
		std::cerr << "Impossible to create the audio file." << std::endl;
		return;
	}
	
	sf_write_short(file, &samples[0], samples.size());
	sf_close(file);
}	