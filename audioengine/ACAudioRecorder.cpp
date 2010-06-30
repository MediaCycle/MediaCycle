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
}

bool ACAudioRecorder::isCaptureAvailable(const char* deviceName)
{
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
}		

bool ACAudioRecorder::initCapture(const char* deviceName)
{
	if (device != NULL)
	{
		// Make sure that audio recording is supported by the soundcard
		if (alcIsExtensionPresent(device, "ALC_EXT_CAPTURE") == AL_FALSE)
		{
			std::cerr << "Audio recording is not supported by your system." << std::endl;
			return false;
		}
		
		// Open the device
		captureDevice = alcCaptureOpenDevice(deviceName, 44100, AL_FORMAT_MONO16, 44100);
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
}
	
void ACAudioRecorder::shutdownCapture()
{
	alcCaptureCloseDevice(captureDevice);
}	

void *threadCaptureEngineFunction(void *_capture_engine_arg)
{
	((ACAudioRecorder*)_capture_engine_arg)->threadCaptureEngine();
}
void ACAudioRecorder::startCapture()
{
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
}

void ACAudioRecorder::stopCapture()
{	
	pthread_cancel(capture_engine);
	isRecording = false;
	
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
}	

void ACAudioRecorder::threadCaptureEngine()
{
	while (1)
    {
		pthread_testcancel();
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
    }
}

void ACAudioRecorder::saveSound(const std::string& filename, const std::vector<ALshort>& samples)
{
	SF_INFO fileInfo;
	fileInfo.channels   = 1;
	fileInfo.samplerate = 44100;
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