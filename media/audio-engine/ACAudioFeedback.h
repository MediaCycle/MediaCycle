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
#ifndef ACAUDIOFEEDBACK_H
#define ACAUDIOFEEDBACK_H

#include <MediaCycle.h>

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

#include <sys/time.h>
#include "TiPhaseVocoder.h"
#include <pthread.h>

#ifdef USE_OPENALEXT
#include "alreverb.h"
#endif

#define DEG2RAD(x) (0.0174532925 * (x))
#define RAD2DEG(x) (57.295779578 * (x))

#ifdef USE_OPENAL
#define OPENAL_STREAM_MODE
//#define OPENAL_STATIC_MODE
#endif

#include <map>

typedef enum {
    ACAudioEngineSynchroModeNone				= 0,
    ACAudioEngineSynchroModeAutoBeat			= 1,		// multi-bar polyrhythm //OK
    ACAudioEngineSynchroModeAutoBarStretch		= 2,		// single-bar polyrythm
    ACAudioEngineSynchroModeAutoBar				= 3,		// straight beat sync, no polyrhythm, gap may be cause in some loops
    ACAudioEngineSynchroModeManual				= 4,		// position in loop controled from outside (scratch mode...) //OK
    ACAudioEngineSynchroModeDownbeatSimple		= 5,
} ACAudioEngineSynchroMode;

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

// conversion between AudioEngineSynchroMode and text string (e.g., to be used in the labels for the gui)
typedef std::map<std::string, ACAudioEngineSynchroMode> stringToAudioEngineSynchroModeMap;
static const stringToAudioEngineSynchroModeMap::value_type _initACAudioEngineSynchroMode[] = {
    stringToAudioEngineSynchroModeMap::value_type("None", ACAudioEngineSynchroModeNone), \
    stringToAudioEngineSynchroModeMap::value_type("AutoBeat", ACAudioEngineSynchroModeAutoBeat), \
    stringToAudioEngineSynchroModeMap::value_type("AutoBarStretch", ACAudioEngineSynchroModeAutoBarStretch), \
    stringToAudioEngineSynchroModeMap::value_type("AutoBar", ACAudioEngineSynchroModeAutoBar), \
    stringToAudioEngineSynchroModeMap::value_type("Manual",ACAudioEngineSynchroModeManual), \
    stringToAudioEngineSynchroModeMap::value_type("DownbeatSimple",ACAudioEngineSynchroModeDownbeatSimple)
};
static const stringToAudioEngineSynchroModeMap stringToAudioEngineSynchroMode(_initACAudioEngineSynchroMode, _initACAudioEngineSynchroMode + sizeof _initACAudioEngineSynchroMode / sizeof *_initACAudioEngineSynchroMode);
//usage: stringToAudioEngineSynchroModeMap::const_iterator iterm = stringToAudioEngineSynchroMode.find(s_mode);

// conversion between AudioEngineScaleMode and text string (e.g., to be used in the labels for the gui)
typedef std::map<std::string, ACAudioEngineScaleMode> stringToAudioEngineScaleModeMap;
static const stringToAudioEngineScaleModeMap::value_type _initAudioEngineScaleModeMap[] = {
    stringToAudioEngineScaleModeMap::value_type("None", ACAudioEngineScaleModeNone), \
    stringToAudioEngineScaleModeMap::value_type("Vocode", ACAudioEngineScaleModeVocode), \
    stringToAudioEngineScaleModeMap::value_type("Resample", ACAudioEngineScaleModeResample), \
    stringToAudioEngineScaleModeMap::value_type("SkipAndResample", ACAudioEngineScaleModeSkipAndResample)
};
static const stringToAudioEngineScaleModeMap stringToAudioEngineScaleMode(_initAudioEngineScaleModeMap, _initAudioEngineScaleModeMap + sizeof _initAudioEngineScaleModeMap / sizeof *_initAudioEngineScaleModeMap);
//usage: stringToAudioEngineScaleModeMap::const_iterator iterm = stringToAudioEngineScaleMode.find(s_mode);

class ACAudioFeedback {

public:
#ifdef USE_OPENAL
    ACAudioFeedback(ALCdevice* _device, int samplerate, int buffersize);
#endif
#ifdef USE_PORTAUDIO
    ACAudioFeedback(PaStream *_stream, int samplerate, int buffersize);
#endif
    ~ACAudioFeedback();

    bool getSpeakerConfigurationsList(std::vector<std::string>& configs);

    // MediaCycle to query database and browser
    MediaCycle				*media_cycle;
    void setMediaCycle(MediaCycle *media_cycle) { this->media_cycle = media_cycle; }

    // OpenAL general settings
    void setListenerGain(float gain);
    void setRenderChannels(int channels);
    void setRenderQuality(int quality);
    void setDistanceModel(int model);
    void setDopplerFactor(float doppler);
    void setSpeedOfSound(float speed);

    // set tune BPM and Key
    void setBPM(float BPM);
    void setGain(float gain);
    void setKey(int key);
    void setTimeSignature(int tsnum, int tsden);
    void setResampleRatio(float resample);

    // Scrub/Scratch control
    void setScrub(float scrub);
    void setSkip(int loop_id, int frame);

    // Synchro and Scale modes
    void setLoopSynchroMode(int _loop_id, ACAudioEngineSynchroMode _synchro_mode);
    void setLoopScaleMode(int _loop_id, ACAudioEngineScaleMode _scale_mode);
    void updateSynchroMode(ACAudioEngineSynchroMode _synchro_mode);
    void updateScaleMode(ACAudioEngineScaleMode _scale_mode);
    void setDefaultSynchroMode(ACAudioEngineSynchroMode _synchro_mode);
    void setDefaultScaleMode(ACAudioEngineScaleMode _scale_mode);
    void forceDefaultSynchroMode(bool _force);
    void forceDefaultScaleMode(bool _force);

    // OpenAL listener settings (from browser)
    void setListenerPosition(float x, float y, float z);
    void setListenerElevation(float y);
    void setListenerOrientation(float orientation, float *x_velocity, float *z_velocity);
    void setListenerVelocity(float velocity, float *x_velocity, float *z_velocity);

    // OpenAL create and delete buffer and sources
    int getLoopId(int slot);
    int getLoopSlot(int loop_id);
    int createSource(int loop_id);
    int createExtSource(float* _buffer, int _length);
    int createSourceWithPosition(int loop_id, float x, float y, float z);
    int createSourceSynchro();
    int deleteSource(int loop_id);
    int deleteExtSource();
    void loopExtSource();
    void stopExtSource();
    int setSourcePosition(int loop_id, float x, float y, float z);

    // other (less crucial) effects possible with OpenAL
    int setSourcePitch(int loop_id, float pitch);
    int setSourceResampleRatio(int loop_id, float resample);
    int setSourceBPM(int loop_id, float bpm);
    int setSourceGain(int loop_id, float gain);
    int setSourceRolloffFactor(int loop_id, float rolloff_factor); // use by distance attenuation model
    int setSourceReferenceDistance(int loop_id, float reference_distance);
    int setSourceMaxDistance(int loop_id, float max_distance);
    int setSourceVelocity(int loop_id, float velocity);
    int setSourceScaleMode(int loop_id, ACAudioEngineScaleMode mode);
    int setSourceSynchroMode(int loop_id, ACAudioEngineSynchroMode mode);

    // Engine
    void threadAudioEngine();
    void threadAudioUpdate();
    void startAudioEngine();
    void stopAudioEngine();
    /*
    // Devices
    void printDeviceList();
    void getDeviceList(std::vector<std::string>& devices);
     */
#ifdef USE_OPENAL
    void setDevice(ALCdevice* _device){device = _device;}
#endif
    void threadAudioEngineInit();
    void threadAudioEngineFrame();
    short *getOutputBufferMixed();

private:
    // Table for interpreting time-signatures
    int		n_time_signatures;
    int     **desc_time_signatures;
    // Musical Properties (entered by user or sequencer software)
    float	active_bpm;
    float active_gain;
    float prev_active_bpm;
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
#ifdef USE_OPENAL
    // OpenAL source variables
    // SD TODO - port this to structure created by RS
    ALuint* loop_buffers;
    ALuint* loop_sources;
    ALuint	ext_loop_source;
    ALuint ext_loop_buffer;
#endif
    float*  resample_ratios;
    float active_resample_ratio;
    int ext_loop_length;
    int*	loop_ids;
    float*  use_bpm;
    int*    use_sample_start;
    int*    use_sample_end;
    int		active_loops;
    float**	mSourcePos;
    //
    int timeCodeDone;

    // OpenAL init and delete
#ifdef USE_OPENAL
    ALCdevice		*device;
    void createOpenAL();
    void deleteOpenAL();
#endif
#ifdef USE_PORTAUDIO
    PaStream		*stream;
    // SD - check those two
    void createPAStream();
    void deletePAStream();
#endif

    /////////////////////////////////////////////////////////////////////////////

    // SD TODO - VOCODER CALL
    // pv_complex **pv;
    TiPhaseVocoder *tpv;
    long int *pv_currentsample;
    int tpv_winsize;

    /////////////////////////////////////////////////////////////////////////////

    // Audio Engine thread
    pthread_t	   audio_engine_t;
    pthread_attr_t audio_engine_attr;

    pthread_t	   audio_update;
    pthread_attr_t audio_update_attr;

    int audio_engine_stacksize;

    void* audio_engine_arg;
    void* audio_update_arg;

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
    int   output_buffer_n0;
    int   output_buffer_n;
    int   output_sample_rate;
    // synthesize settings
    int	  *loop_synchro_mode;
    int	  *loop_scale_mode;
    ACAudioEngineSynchroMode default_synchro_mode;
    ACAudioEngineScaleMode default_scale_mode;
    bool force_default_synchro_mode;
    bool force_default_scale_mode;

    // engine variables
    double	audio_engine_wakeup_time;
    double	audio_engine_current_time;
    double	audio_engine_current_time_2;
    //double	audio_engine_fire_time;
    short*	 output_buffer;
    short*	 output_buffer_mixed;
    short**  loop_buffers_audio_engine;
#ifdef USE_OPENAL
    ALuint** loop_buffers_audio_engine_stream;
#endif
#ifdef USE_PORTAUDIO
    unsigned int loop_buffers_audio_engine_stream;
#endif
    int slow_refresh;
    int*	 prev_sample_pos;
    int*	 current_buffer;
    int*     current_buffer_unqueue;
    float		 time_from_start, time_from_downbeat, time_from_beat, time_from_tatum;
    float		 prev_time_from_start, prev_time_from_downbeat, prev_time_from_beat, prev_time_from_tatum;
    long int		 downbeat_from_start;
    long int		 prev_downbeat_from_start;
    float prev_scrub_pos, scrub_pos, scrub_speed;
    float prev_scrub_time, scrub_time;
    double sdtime, prevsdtime;
    int reached_end, reached_begin;

    int loop_slot_syncho;

    int engine_running;
    // ...
    FILE *timing;

    int audio_samplerate;
    int audio_buffersize;

    // Audio Engine Functions
    void createAudioEngine(int _output_sample_rate, int _output_buffer_size, int _output_num_buffers);
    void deleteAudioEngine();
    void timeCodeAudioEngine(int n_samples);
#ifdef USE_OPENAL
    bool processAudioEngine();
#endif
    void processAudioEngineMix();
    bool processAudioEngineNew();
    void processAudioUpdate();
    void processAudioEngineSamplePosition(int _loop_slot, int *_prev_sample_pos, int *_sample_pos, int *_sample_pos_limit);
    void processAudioEngineResynth(int _loop_slot, int _prev_sample_pos, int _sample_pos, int _sample_pos_limit, short *_output_buffer);
    void setMIDITimeCode();

#ifdef USE_OPENALEXT
    EFXEAXREVERBPROPERTIES reverb;
    ALuint effect, slot;
#endif

};
#endif //ACAUDIOFEEDBACK_H
