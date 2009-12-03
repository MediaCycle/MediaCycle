/*
 *  ACAudioFeedback.h
 *  AudioCycle
 *
 *  @author Stéphane Dupont
 *  @date 21/10/08
 *  @copyright (c) 2008 – UMONS - Numediart
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

#include "MediaCycleLight.h"

#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#include <MyOpenALSupport.h>
#include <sys/time.h>
#include <TiPhaseVocoder.h>

#define DEG2RAD(x) (0.0174532925 * (x))
#define RAD2DEG(x) (57.295779578 * (x))

#define OPENAL_STREAM_MODE
//#define OPENAL_STATIC_MODE

typedef enum {
	ACAudioEngineSynchroModeNone				= 0,
	ACAudioEngineSynchroModeAutoBeat			= 1,		// multi-bar polyrhythm
	ACAudioEngineSynchroModeAutoBarStretch		= 2,		// single-bar polyrythm
	ACAudioEngineSynchroModeAutoBar				= 3,		// straight bea sync, no polyrhythm, gap may be cause in some loops
	ACAudioEngineSynchroModeManual				= 4,		// position in loop controled from outside (scratch mode...)
	ACAudioEngineSynchroModeDownbeatSimple		= 5,
} ACAUdioEngineSynchroMode;

typedef enum {
	ACAudioEngineScaleModeNone					= 0,
	ACAudioEngineScaleModeVocode				= 1,		// time-stretch with pitch preservation
	ACAudioEngineScaleModeResample				= 2,		// time-stretch without pitch preservation (better for scratch mode) 
	ACAudioEngineScaleModeSkipAndResample		= 3,
} ACAudioEngineScaleMode;

typedef enum {
	ACAudioEngineTimeSignatureTypeBinary		= 2,
	ACAudioEngineTimeSignatureTypeTernary		= 3,
} ACAudioEngineTimeSignature;

class ACMediaFeedback {
	
public:
	ACMediaFeedback() {};
	~ACMediaFeedback() {};
	
};

class ACAudioFeedback : public ACMediaFeedback {
	
public:
	ACAudioFeedback();
	~ACAudioFeedback();
	
	// MediaCycle to querry database and browser
	MediaCycle				*media_cycle;
	void setMediaCycle(MediaCycle *media_cycle) { this->media_cycle = media_cycle; };
	
	// OpenAL general settings
	void setListenerGain(float gain);
	void setRenderChannels(int channels);
	void setRenderQuality(int quality);
	void setDistanceModel(int model);
	void setDopplerFactor(float doppler);
	void setSpeedOfSound(float speed);	
	
	// set tune BPM and Key
	void setBPM(float BPM);
	void setKey(int key);
	void setTimeSignature(int tsnum, int tsden);

	// Scrub/Scratch control
	void setScrub(float scrub);
	
	// OpenAL listener settings (from browser)
	void setListenerPosition(float x, float y, float z);
	void setListenerElevation(float y);
	void setListenerOrientation(float orientation, float *x_velocity, float *z_velocity);
	void setListenerVelocity(float velocity, float *x_velocity, float *z_velocity);
	
	// OpenAL create and delete buffer and sources
	int getLoopId(int slot);
	int getLoopSlot(int loop_id);
	int createSource(int loop_id);
	int createSourceWithPosition(int loop_id, float x, float y, float z);
	int deleteSource(int loop_id);
	int setSourcePosition(int loop_id, float x, float y, float z);	
	
	// other (less crucial) effects possible with OpenAL
	int setSourcePitch(int loop_id, float pitch);
	int setSourceGain(int loop_id, float gain);
	int setSourceRolloffFactor(int loop_id, float rolloff_factor); // use by distance attenuation model
	int setSourceReferenceDistance(int loop_id, float reference_distance);
	int setSourceMaxDistance(int loop_id, float max_distance);
	int setSourceVelocity(int loop_id, float velocity); 
	
	// Engine
	void threadAudioEngine();
	void startAudioEngine();
	void stopAudioEngine();

private:
	// Table for interpreting time-signatures
	int		n_time_signatures;
	int     **desc_time_signatures;
	// Musical Properties (entered by user or sequencer software)
	float	active_bpm;
	int		active_key;
	int		active_tsnum;
	int		active_tsden;
	int		active_tstype;
	int		active_db;	
	// OpenAL listener variables
	float	mListenerPos[3];
	float	mListenerElevation;
	float	mAngle;
	float	mListenerDirection;
	float	mVelocityScaler;
	// OpenAL source variables
	// SD TODO - port this to structure created by RS 
	ALuint* loop_buffers;
	ALuint* loop_sources;
	int*	loop_ids;
	float*  use_bpm;
	int*    use_sample_start;
	int*    use_sample_end;
	int		active_loops;
	float**	mSourcePos;
	//
	int timeCodeDone; 
	
	// OpenAL init and delete
	void createOpenAL();
	void deleteOpenAL();
	
	/////////////////////////////////////////////////////////////////////////////

	// SD TODO - VOCODER CALL
	// pv_complex **pv;
	TiPhaseVocoder *tpv;
	long int *pv_currentsample;
	int tpv_winsize;
	
	/////////////////////////////////////////////////////////////////////////////
	
	// Audio Engine thread
	pthread_t	   audio_engine;
	pthread_attr_t audio_engine_attr;
	int audio_engine_stacksize;
	void* audio_engine_arg;
	pthread_once_t audio_engine_once;
	pthread_mutex_t audio_engine_mutex;
	pthread_mutexattr_t audio_engine_mutex_attr;
	pthread_cond_t audio_engine_cond;
	pthread_condattr_t audio_engine_cond_attr;	
	pthread_mutex_t audio_engine_cond_mutex;
	pthread_mutexattr_t audio_engine_cond_mutex_attr;
	
	// Audio Engine Variables
	// engine settings
	int	  output_buffer_size;		// size of output audio buffer in samples
	int   output_buffer_n;
	int   output_sample_rate;
	// synthesize settings
	int	  *loop_synchro_mode;
	int	  *loop_scale_mode;
	// engine variables
	double	audio_engine_wakeup_time;
	double	audio_engine_current_time;
	double	audio_engine_current_time_2;
	double	audio_engine_fire_time;
	short*	 output_buffer;
	short**  loop_buffers_audio_engine;
	ALuint** loop_buffers_audio_engine_stream;
	int*	 prev_sample_pos;
	int*	 current_buffer;
	float		 time_from_start, time_from_downbeat, time_from_beat, time_from_tatum;
	long int		 downbeat_from_start;
	float scrub_pos;
	
	// ...
	FILE *timing;
	
	// Audio Engine Functions
	void createAudioEngine(int _output_sample_rate, int _output_buffer_size, int _output_num_buffers);
	void deleteAudioEngine();
	void threadAudioEngineInit();
	void timeCodeAudioEngine(int n_samples);
	void processAudioEngine();
	void processAudioUpdate();
	void processAudioEngineSamplePosition(int _loop_slot, int *_sample_pos);
	void processAudioEngineResynth(int _loop_slot, int _sample_pos, short *_output_buffer);
	void setMIDITimeCode();
	
};