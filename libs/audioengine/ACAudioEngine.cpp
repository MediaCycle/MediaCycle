/*
 *  ACAudioEngine.cpp
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
 */

#include "ACAudioEngine.h"
#include <iostream>
//DT : to have access to the media/audio functions
#include <ACAudio.h>

#ifdef USE_PORTAUDIO
int threadAudioEngineCallbackFunction(const void *inputBuffer, void *outputBuffer,
{
	ACAudioEngine *localengine;
	localengine = (ACAudioEngine*)userData;
	short *localbuffer = (short*)outputBuffer;
	short *outputbuffer;
	
	if ( !(localengine->getFeedback()) ) {
		return 0;
	}
	
	unsigned int i;
	//float *in = (float*)inputBuffer;
	//float *out = (float*)outputBuffer;
	
	/*
	if () {
		localengine->getFeedback()->threadAudioEngineInit();
	}
	*/
	
	localengine->getFeedback()->threadAudioEngineFrame();
	
	outputbuffer = localengine->getFeedback()->getOutputBufferMixed();
	
	// recover mixed signal here and send to pa stream
	for( i=0; i<framesPerBuffer; i++ ) {
		localbuffer[i] = outputbuffer[i];
		// SD TODO - take care of stereo and multiple channels, not ready yet
	}	
	
    return 0;
}
#endif

ACAudioEngine::ACAudioEngine(int samplerate, int buffersize)
{
	feedback = 0;
	recorder = 0;
	
	audio_samplerate = samplerate;
	audio_buffersize = buffersize;
	
#ifdef USE_OPENAL
	createOpenAL();
	feedback = new ACAudioFeedback(device, audio_samplerate, audio_buffersize);
	recorder = new ACAudioRecorder(device, audio_samplerate, audio_buffersize);
#endif
#ifdef USE_PORTAUDIO
	audio_callback = threadAudioEngineCallbackFunction;
	audio_userdata = this;
	createPAStream();
	feedback = new ACAudioFeedback(stream, audio_samplerate, audio_buffersize);
	recorder = new ACAudioRecorder(stream, audio_samplerate, audio_buffersize);
#endif
}

ACAudioEngine::~ACAudioEngine()
{
#ifdef USE_OPENAL
	deleteOpenAL();
#endif
#ifdef USE_PORTAUDIO
	deletePAStream();
#endif
	if (feedback) {delete feedback; feedback = 0;}
	if (recorder) {delete recorder; recorder = 0;}
	media_cycle = 0;
}

void ACAudioEngine::setMediaCycle(MediaCycle *media_cycle)
{
	this->media_cycle = media_cycle;
	feedback->setMediaCycle(media_cycle);
	recorder->setMediaCycle(media_cycle);	
}

#ifdef USE_OPENAL
void ACAudioEngine::createOpenAL()
{
	device = 0;
	// SD TODO - Allow the user to select the device, and probably the speaker configuration (stereo, 5.1...)
	// Create a new OpenAL Device: 0 -> default output device
	device = alcOpenDevice(0);
}

void ACAudioEngine::deleteOpenAL()
{
	 ALCcontext	*context = 0;
	 //ALCdevice	*device = 0;//CF
	 //ALuint		*returnedNames;
	// SD TODO - Check this and remove comments
	/*
	 // Delete the Sources
	 alDeleteSources(NUM_BUFFERS_SOURCES, returnedNames);
	 // Delete the Buffers
	 alDeleteBuffers(NUM_BUFFERS_SOURCES, returnedNames);
	 */
	 //Get active context
	 context = alcGetCurrentContext(); // XS (0) added
	 //Get device for active context
	 device = alcGetContextsDevice(context);
	 //Release context
	 alcDestroyContext(context);
	 //Close device
	 alcCloseDevice(device);
}
#endif
#ifdef USE_PORTAUDIO
void ACAudioEngine::createPAStream()
{
	PaError err = paNoError;
	PaStreamParameters outputParameters;
	
	/* -- initialize PortAudio -- */
    err = Pa_Initialize();
    if( err != paNoError )
		std::cerr <<  "PortAudio error: " << Pa_GetErrorText( err ) << std::endl;
	
	
    /* -- setup input and output -- */
    outputParameters.device = Pa_GetDefaultOutputDevice(); // default output device
    outputParameters.channelCount = 1;
    outputParameters.sampleFormat = paInt16;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = 0;
	
    /* -- setup stream -- */
    err = Pa_OpenStream(&stream,
						0, //no input
						&outputParameters,
						audio_samplerate,
						audio_buffersize,
						paClipOff,      // we won't output out of range samples so don't bother clipping them
						audio_callback, // no callback, use blocking API
						audio_userdata); // no callback, so no callback userData
    /*
	err = Pa_OpenStream(&stream,
						0, //no input
						&outputParameters,
						44100,
						32,
						paClipOff,      // we won't output out of range samples so don't bother clipping them
						0, // no callback, use blocking API
						0 ); // no callback, so no callback userData
	 */
	
    if( err != paNoError )
		printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
	
    /* -- start stream -- */
    err = Pa_StartStream( stream );
    if( err != paNoError )
		printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
}

void ACAudioEngine::deletePAStream()
{
	PaError err = paNoError;
	/* -- Now we stop the stream -- */
    err = Pa_StopStream( stream );
    if( err != paNoError )
		printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
	
    /* -- don't forget to cleanup! -- */
    err = Pa_CloseStream( stream );
    if( err != paNoError )
		printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
	
    err = Pa_Terminate();
	if( err != paNoError )
		printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
	
	std::cout << "PortAudio closed" << std::endl;
}
#endif

void ACAudioEngine::printDeviceList()
{
#ifdef USE_OPENAL
    const ALCchar* deviceList = alcGetString(0, ALC_DEVICE_SPECIFIER);
	
    if (deviceList)
    {
        while (strlen(deviceList) > 0)
        {
			std::cout << "Audio device available: " << deviceList << std::endl;
            deviceList += strlen(deviceList) + 1;
			
        }
    }
	else
		std::cout << "No compliant audio device available" << std::endl;
#endif
#ifdef USE_PORTAUDIO
	int numDevices, i;
	const   PaDeviceInfo *deviceInfo;
	
    numDevices = Pa_GetDeviceCount();
    if( numDevices < 0 )
    {
        std::cout << "ERROR: Pa_CountDevices returned 0x" << numDevices << std::endl;
    }
	
	if (numDevices) {
		for( i=0; i<numDevices; i++ )
		{
			deviceInfo = Pa_GetDeviceInfo( i );
			std::cout << "Audio device available: " << deviceInfo->name << std::endl;
		}
	} else {
		std::cout << "No compliant audio device available" << std::endl;
	}
#endif
}

void ACAudioEngine::getDeviceList(std::vector<std::string>& devices)
{
    devices.clear();	
#ifdef USE_OPENAL
    const ALCchar* deviceList = alcGetString(0, ALC_DEVICE_SPECIFIER);
	
    if (deviceList)
    {
        while (strlen(deviceList) > 0)
        {
            devices.push_back(deviceList);
            deviceList += strlen(deviceList) + 1;
        }
    }
#endif
#ifdef USE_PORTAUDIO
	int numDevices, i;
	const   PaDeviceInfo *deviceInfo;
	
    numDevices = Pa_GetDeviceCount();
	
	if (numDevices) {
		for( i=0; i<numDevices; i++ )
		{
			deviceInfo = Pa_GetDeviceInfo( i );
			devices.push_back(deviceInfo->name);
		}
	}
#endif
}

void ACAudioEngine::printCaptureDeviceList()
{
#ifdef USE_OPENAL
    const ALCchar* deviceList = alcGetString(0, ALC_CAPTURE_DEVICE_SPECIFIER);
	
    if (deviceList)
    {
        while (strlen(deviceList) > 0)
        {
			std::cout << "Audio capture device available: " << deviceList << std::endl;
            deviceList += strlen(deviceList) + 1;
			
        }
    }
	else
		std::cout << "No compliant audio capture device available" << std::endl;
#endif
#ifdef USE_PORTAUDIO
	std::cout << "Audio capture device available: not implemented yet (using portaudio)" << std::endl;
#endif
}

