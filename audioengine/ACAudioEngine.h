/*
 *  ACAudioEngine.h
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

#ifndef HEADER_ACAUDIOENGINE
#define HEADER_ACAUDIOENGINE

#include "MediaCycle.h"

#include "ACAudioFeedback.h"
#include "ACAudioRecorder.h"

class ACAudioEngine {
	
public:
	ACAudioEngine();
	~ACAudioEngine();
	
	// MediaCycle to query database and browser
	MediaCycle				*media_cycle;
	void setMediaCycle(MediaCycle *media_cycle);

private:
	ACAudioFeedback* feedback;
	ACAudioRecorder* recorder;
	ALCdevice* device;
	
public:	
	ACAudioFeedback* getFeedback(){return feedback;}
	ACAudioRecorder* getRecorder(){return recorder;}
	
	// OpenAL init and delete
	void createOpenAL();
	void deleteOpenAL();
	
	// OpenAL general settings
	void setListenerGain(float gain){feedback->setListenerGain(gain);}
	void setRenderChannels(int channels){feedback->setRenderChannels(channels);}
	void setRenderQuality(int quality){feedback->setRenderQuality(quality);}
	void setDistanceModel(int model){feedback->setDistanceModel(model);}
	void setDopplerFactor(float doppler){feedback->setDopplerFactor(doppler);}
	void setSpeedOfSound(float speed){feedback->setSpeedOfSound(speed);}	
	
	// set tune BPM and Key
	void setBPM(float BPM){feedback->setBPM(BPM);}
	void setKey(int key){feedback->setKey(key);}
	void setTimeSignature(int tsnum, int tsden){feedback->setTimeSignature(tsnum, tsden);}

	// Scrub/Scratch control
	void setScrub(float scrub){feedback->setScrub(scrub);}
	
	// Synchro and Scale modes
	void setLoopSynchroMode(int _loop_id, ACAudioEngineSynchroMode _synchro_mode){feedback->setLoopSynchroMode(_loop_id, _synchro_mode);}
	void setLoopScaleMode(int _loop_id, ACAudioEngineScaleMode _scale_mode){feedback->setLoopScaleMode(_loop_id, _scale_mode);}
	//void setCurrentLoopSynchroMode(ACAudioEngineSynchroMode synchro);
	//void setCurrentLoopScaleMode(ACAUdioEngineScaleMode scale);

	// OpenAL listener settings (from browser)
	void setListenerPosition(float x, float y, float z){feedback->setListenerPosition(x,y,z);}
	void setListenerElevation(float y){feedback->setListenerElevation(y);}
	void setListenerOrientation(float orientation, float *x_velocity, float *z_velocity){feedback->setListenerOrientation(orientation, x_velocity, z_velocity);}
	void setListenerVelocity(float velocity, float *x_velocity, float *z_velocity){feedback->setListenerVelocity(velocity, x_velocity, z_velocity);}
	
	// OpenAL create and delete buffer and sources
	int getLoopId(int slot){feedback->getLoopId(slot);}
	int getLoopSlot(int loop_id){feedback->getLoopSlot(loop_id);}
	int createSource(int loop_id){feedback->createSource(loop_id);}
	int createSourceWithPosition(int loop_id, float x, float y, float z){feedback->createSourceWithPosition(loop_id, x, y, z);}
	int deleteSource(int loop_id){feedback->deleteSource(loop_id);}
	int setSourcePosition(int loop_id, float x, float y, float z){feedback->setSourcePosition(loop_id, x, y, z);}	
	
	// other (less crucial) effects possible with OpenAL
	int setSourcePitch(int loop_id, float pitch){feedback->setSourcePitch(loop_id, pitch);}
	int setSourceGain(int loop_id, float gain){feedback->setSourceGain(loop_id,gain);}
	int setSourceRolloffFactor(int loop_id, float rolloff_factor){feedback->setSourceRolloffFactor(loop_id, rolloff_factor);} // use by distance attenuation model
	int setSourceReferenceDistance(int loop_id, float reference_distance){feedback->setSourceReferenceDistance(loop_id, reference_distance);}
	int setSourceMaxDistance(int loop_id, float max_distance){feedback->setSourceMaxDistance(loop_id, max_distance);}
	int setSourceVelocity(int loop_id, float velocity){feedback->setSourceVelocity(loop_id, velocity);} 
	
	// Devices
	void printDeviceList();//{feedback->printDeviceList();}
	void getDeviceList(std::vector<std::string>& devices);//{feedback->getDeviceList(devices);}
	void printCaptureDeviceList();//{recorder->printDeviceList();}
	void getCaptureDeviceList(std::vector<std::string>& devices){recorder->getCaptureDeviceList(devices);}
	bool isCaptureAvailable(const char* deviceName = NULL){return recorder->isCaptureAvailable(deviceName);}
	
	// Feedback Engine
	void threadAudioEngine(){feedback->threadAudioEngine();}
	void threadAudioUpdate(){feedback->threadAudioUpdate();}
	void startAudioEngine(){feedback->startAudioEngine();}
	void stopAudioEngine(){feedback->stopAudioEngine();}
	
	// Recorder
	bool initCapture(const char* deviceName = NULL){recorder->initCapture(deviceName);}
	void startCapture(){recorder->startCapture();}
	void stopCapture(){recorder->stopCapture();}
	
};
#endif