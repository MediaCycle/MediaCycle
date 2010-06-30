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
#include "ACAudio.h"

ACAudioEngine::ACAudioEngine()
{
	createOpenAL();
	feedback = new ACAudioFeedback(device);
	recorder = new ACAudioRecorder(device);
}

ACAudioEngine::~ACAudioEngine()
{
	delete feedback;
	delete recorder;
	deleteOpenAL();
}

void ACAudioEngine::setMediaCycle(MediaCycle *media_cycle)
{
	this->media_cycle = media_cycle;
	feedback->setMediaCycle(media_cycle);
	recorder->setMediaCycle(media_cycle);	
}

void ACAudioEngine::createOpenAL()
{
	device = NULL;
	// SD TODO - Allow the user to select the device, and probably the speaker configuration (stereo, 5.1...)
	// Create a new OpenAL Device: NULL -> default output device
	device = alcOpenDevice(NULL);
}

void ACAudioEngine::deleteOpenAL()
{
	 ALCcontext	*context = NULL;
	 //ALCdevice	*device = NULL;//CF
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

void ACAudioEngine::printDeviceList()
{
    const ALCchar* deviceList = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	
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
}

void ACAudioEngine::getDeviceList(std::vector<std::string>& devices)
{
    devices.clear();	
    const ALCchar* deviceList = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	
    if (deviceList)
    {
        while (strlen(deviceList) > 0)
        {
            devices.push_back(deviceList);
            deviceList += strlen(deviceList) + 1;
        }
    }
}

void ACAudioEngine::printCaptureDeviceList()
{
    const ALCchar* deviceList = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
	
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
}

