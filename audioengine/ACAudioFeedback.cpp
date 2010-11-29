/*
 *  ACAudioFeedback.cpp
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

#include "ACAudioFeedback.h"
#include <iostream>
//DT : to have access to the media/audio functions
#include "ACAudio.h"
/*#include <mach/mach_init.h>
#include <mach/thread_policy.h>
#include <mach/task_policy.h>
*/

// #include <mach/mach.h>
//#include <mach/mach.h>
//#include <sched.h>

/*
// TODO - rm this, this is in the framework already
//Polls a (non-realtime) thread to find out how it is scheduled
//Results are undefined if an error is returned. Otherwise, the
//priority is returned in the address pointed to by the priority
//parameter, and whether or not the thread uses timeshare scheduling
//is returned at the address pointed to by the isTimeShare parameter
kern_return_t  GetStdThreadSchedule( mach_port_t    machThread,
									int            *priority,
									boolean_t      *isTimeshare )
{
	kern_return_t                       result = 0;
	thread_extended_policy_data_t       timeShareData;
	thread_precedence_policy_data_t     precidenceData;
	mach_msg_type_number_t		structItemCount;
	boolean_t				fetchDefaults = false;
	
	memset( &timeShareData, 0, sizeof( thread_extended_policy_data_t 
		   ));
	memset( &precidenceData, 0, sizeof( 
									   thread_precedence_policy_data_t ));
	
	if( 0 == machThread )
		machThread = mach_thread_self();
	
	if( NULL != isTimeshare )
	{
		structItemCount = THREAD_EXTENDED_POLICY_COUNT;
		result = thread_policy_get( machThread, THREAD_EXTENDED_POLICY,
								  (thread_policy_t)&timeShareData, &structItemCount, 
								   &fetchDefaults );
		*isTimeshare = timeShareData.timeshare;
		if( 0 != result )
			return result;
	}
	
	if( NULL != priority )
	{
		fetchDefaults = false;
		structItemCount = THREAD_PRECEDENCE_POLICY_COUNT;
		result = thread_policy_get( machThread, 
								   THREAD_PRECEDENCE_POLICY,
								   (thread_policy_t)&precidenceData, &structItemCount, 
								   &fetchDefaults );
		*priority = precidenceData.importance;
	}
	
	return result;
}

// Reschedules the indicated thread according to new parameters:
//
// machThread           The mach thread id. Pass 0 for the current thread.
// newPriority          The desired priority.
// isTimeShare          false for round robin (fixed) priority,
//                      true for timeshare (normal) priority
//
// A standard new thread usually has a priority of 0 and uses the
// timeshare scheduling scheme. Use pthread_mach_thread_np() to
// to convert a pthread id to a mach thread id
kern_return_t  RescheduleStdThread( mach_port_t    machThread,
								   int            newPriority,
								   boolean_t      isTimeshare )
{
	kern_return_t                       result = 0;
	thread_extended_policy_data_t       timeShareData;
	thread_precedence_policy_data_t     precidenceData;
	
	//Set up some variables that we need for the task
	precidenceData.importance = newPriority;
	timeShareData.timeshare = isTimeshare;
	if( 0 == machThread )
		machThread = mach_thread_self();
	
	//Set the scheduling flavor. We want to do this first, since doing so
	//can alter the priority
	result = thread_policy_set( machThread,
							   THREAD_EXTENDED_POLICY,
							   (thread_policy_t)&timeShareData,
							   THREAD_EXTENDED_POLICY_COUNT );
	
	if( 0 != result )
		return result;
	
	//Now set the priority
	return   thread_policy_set( machThread,
							   THREAD_PRECEDENCE_POLICY,
							   (thread_policy_t)&precidenceData,
							   THREAD_PRECEDENCE_POLICY_COUNT );
	
}
*/


int set_my_thread_priority(int priority) {
    struct sched_param sp;
	
	memset(&sp, 0, sizeof(struct sched_param));
    sp.sched_priority=priority;
    if (pthread_setschedparam(pthread_self(), SCHED_RR, &sp)  == -1) {
        //printf(“Failed to change priority.\n”);
        return -1;
    }
    return 0;
}

/*
int set_my_task_policy(void) {
    int ret;
    struct task_category_policy tcatpolicy;
	
    tcatpolicy.role = TASK_FOREGROUND_APPLICATION;
	
    if ((ret=task_policy_set(mach_task_self(),
							 TASK_CATEGORY_POLICY, (thread_policy_t)&tcatpolicy,
							 TASK_CATEGORY_POLICY_COUNT)) != KERN_SUCCESS) {
		//fprintf(stderr, “set_my_task_policy() failed.\n”);
		return 0;
    }
    return 1;
}

int set_realtime(int period, int computation, int constraint) {
    struct thread_time_constraint_policy ttcpolicy;
    int ret;
	
    ttcpolicy.period=period; // HZ/160
    ttcpolicy.computation=computation; // HZ/3300;
    ttcpolicy.constraint=constraint; // HZ/2200;
    ttcpolicy.preemptible=1;
	
    if ((ret=thread_policy_set(mach_thread_self(),
							   THREAD_TIME_CONSTRAINT_POLICY, (thread_policy_t)&ttcpolicy,
							   THREAD_TIME_CONSTRAINT_POLICY_COUNT)) != KERN_SUCCESS) {
		//fprintf(stderr, “set_realtime() failed.\n”);
		return 0;
    }
    return 1;
}
*/

int OPENAL_NUM_BUFFERS = 32;

#if defined(__APPLE__)
typedef ALvoid	AL_APIENTRY	(*alMacOSXRenderChannelCountProcPtr) (const ALint value);
ALvoid  alMacOSXRenderChannelCountProc(const ALint value)
{
	static	alMacOSXRenderChannelCountProcPtr	proc = NULL;
    
    if (proc == NULL) {
        proc = (alMacOSXRenderChannelCountProcPtr) alcGetProcAddress(NULL, (const ALCchar*) "alMacOSXRenderChannelCount");
    }
    
    if (proc)
        proc(value);
	
    return;
}

// SD TODO - This is related to OS-X specific extensions.
typedef ALvoid	AL_APIENTRY	(*alcMacOSXRenderingQualityProcPtr) (const ALint value);
ALvoid  alcMacOSXRenderingQualityProc(const ALint value)
{
	static	alcMacOSXRenderingQualityProcPtr	proc = NULL;
    
    if (proc == NULL) {
        proc = (alcMacOSXRenderingQualityProcPtr) alcGetProcAddress(NULL, (const ALCchar*) "alcMacOSXRenderingQuality");
    }
    
    if (proc)
        proc(value);
	
    return;
}
#endif

FILE *debug_vocoder;

ACAudioFeedback::ACAudioFeedback(ALCdevice* _device)
{
	prev_scrub_pos = 0;
	scrub_pos = 0;
	prev_scrub_time = 0;
	scrub_time = 0;
	scrub_speed = 0;
	reached_begin = 0;
	reached_end = 0;
	
	engine_running = 0;
	media_cycle = 0;
	
	int i;
	//
	debug_vocoder = fopen("/dupont/afac.pcm","w");
	//
	active_bpm   = 100;
	active_key	 = 0;
	active_tsnum = 4;
	active_tsden = 4;
	n_time_signatures = 0;
	desc_time_signatures = 0;	
	//
	mAngle = 0;
	mListenerDirection = 0;
	mListenerElevation = 0;
	mVelocityScaler = 0;
	mListenerPos[0] = 0.0;
	mListenerPos[1] = 0.0;
	mListenerPos[2] = 0.0;
	//
	active_loops = 0;
	//
	// Phase vocoder
	// SD TODO - VOCODER CALL
	//pv = new pv_complex*[OPENAL_NUM_BUFFERS];
	tpv = new TiPhaseVocoder[OPENAL_NUM_BUFFERS];
	for (i=0;i<OPENAL_NUM_BUFFERS;i++) {
		tpv[i].flagLoop = 0;
		tpv[i].f = 0;
		tpv[i].samples = 0;
	}
	tpv_winsize = 2048;
	pv_currentsample = new long int[OPENAL_NUM_BUFFERS];
	//
	device = _device;
	createOpenAL();
#ifdef OPENAL_STREAM_MODE
	createAudioEngine(44100, 512, 3);
	setTimeSignature(4, 4);
#endif
	ext_loop_length = 0;
}

ACAudioFeedback::~ACAudioFeedback()
{
	//
#ifdef OPENAL_STREAM_MODE
	deleteAudioEngine();
#endif
	//
	//deleteOpenAL();//CF moved to AudioEngine
}

//CF moved to AudioEngine
/*
void ACAudioFeedback::printDeviceList()
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

void ACAudioFeedback::getDeviceList(std::vector<std::string>& devices)
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
*/

void ACAudioFeedback::createOpenAL()
{
	int count;
	
	ALenum			error;
	ALCcontext		*newContext = NULL;
	//ALCdevice		*device = NULL;//CF promoted to class member
		
	// Loop ids for each OpenAL source
	loop_ids = new int[OPENAL_NUM_BUFFERS];
	for (count=0;count<OPENAL_NUM_BUFFERS;count++) {
		loop_ids[count] = -1;
	}
	
	// Used for Key normalization
	use_bpm = new float[OPENAL_NUM_BUFFERS];
	use_sample_start = new int[OPENAL_NUM_BUFFERS];
	use_sample_end = new int[OPENAL_NUM_BUFFERS];
	
	// Source Positions
	mSourcePos = new float*[OPENAL_NUM_BUFFERS];
	for (count=0;count<OPENAL_NUM_BUFFERS;count++) {
		mSourcePos[count] = new float[3];
	}

/*	
	// SD TODO - Allow the user to select the device, and probably the speaker configuration (stereo, 5.1...)
	// Create a new OpenAL Device: NULL -> default output device
	device = alcOpenDevice(NULL);
*/
	if (device != NULL)
	{
		// Create a new OpenAL Context
		newContext = alcCreateContext(device, 0);//CF make this choosable
		
		if (newContext != NULL)
		{
			// Make the new context the Current OpenAL Context
			alcMakeContextCurrent(newContext);
			
			// Create some OpenAL Buffer Objects
			// SD TODO 2009 nov - This is not needed anymore normally
			/* loop_buffers = new ALuint[2*OPENAL_NUM_BUFFERS];
			alGetError(); // clear error code
			alGenBuffers(OPENAL_NUM_BUFFERS, loop_buffers);
			if((error = alGetError()) != AL_NO_ERROR) {
				printf("Error Generating OpenAL Buffers!");
				exit(1);
			}*/ 
			
			// Create some OpenAL Source Objects
			loop_sources = new ALuint[OPENAL_NUM_BUFFERS];
			alGetError(); // clear error code
			// SD - Source Problem
			// alGenSources(OPENAL_NUM_BUFFERS, loop_sources);
			if(alGetError() != AL_NO_ERROR) 
			{
				printf("Error generating OpenAL Sources!\n");
				exit(1);
			}
	
		}
	}
	//alListenerf(AL_GAIN, 0.25);
}

void ACAudioFeedback::deleteOpenAL()
{
//CF moved to AudioEngine	
/*	
    ALCcontext	*context = NULL;
    //ALCdevice	*device = NULL;//CF
	//ALuint		*returnedNames;
*/	
	
	// SD TODO - Check this and remove comments
	/*
	// Delete the Sources
	alDeleteSources(NUM_BUFFERS_SOURCES, returnedNames);
	// Delete the Buffers
	alDeleteBuffers(NUM_BUFFERS_SOURCES, returnedNames);
	*/
	
//CF moved to AudioEngine	
/*	
	//Get active context
    context = alcGetCurrentContext(); // XS (0) added
    //Get device for active context
    device = alcGetContextsDevice(context);
    //Release context
    alcDestroyContext(context);
    //Close device
    alcCloseDevice(device);
*/ 
}

void *threadAudioEngineFunction(void *_audio_engine_arg)
{
	((ACAudioFeedback*)_audio_engine_arg)->threadAudioEngine();
}

void *threadAudioUpdateFunction(void *_audio_update_arg)
{
	((ACAudioFeedback*)_audio_update_arg)->threadAudioUpdate();
}

void ACAudioFeedback::startAudioEngine() {
#ifdef OPENAL_STREAM_MODE
	if (!engine_running)
		createAudioEngine(output_sample_rate, output_buffer_size, output_buffer_n);
#endif
}

void ACAudioFeedback::stopAudioEngine() {
#ifdef OPENAL_STREAM_MODE
	if (engine_running)
		deleteAudioEngine();
#endif
}

void ACAudioFeedback::createAudioEngine(int _output_sample_rate, int _output_buffer_size, int _output_num_buffers)
{
	int count;
	int local_output_buffer_n;
	
	timing = fopen("/AudioCycleProPackLibrary/timing","w");
	
	ALenum  error = AL_NO_ERROR;
	
	// Engine settings
	output_buffer_size = _output_buffer_size;
	output_buffer_n0	= _output_num_buffers;
	output_buffer_n	= _output_num_buffers+3;
	output_sample_rate = _output_sample_rate;
	
	// Synthsizer settgins
	loop_synchro_mode = new int[OPENAL_NUM_BUFFERS];
	loop_scale_mode = new int[OPENAL_NUM_BUFFERS];
	
	// Need to prepared some arrays
	//
	current_buffer = new int[OPENAL_NUM_BUFFERS];
	memset(current_buffer, 0, OPENAL_NUM_BUFFERS*sizeof(int));
        current_buffer_unqueue = new int[OPENAL_NUM_BUFFERS];
	memset(current_buffer_unqueue, 0, OPENAL_NUM_BUFFERS*sizeof(int));
	// 
	prev_sample_pos = new int[OPENAL_NUM_BUFFERS];
	output_buffer = new short[output_buffer_size];
	//	1/ buffers containing the full loops to be resynthetized
	loop_buffers_audio_engine = new short*[OPENAL_NUM_BUFFERS];
	//	2/ audio engine buffers that will be queued to OpenAL
	loop_buffers_audio_engine_stream = new ALuint*[OPENAL_NUM_BUFFERS];
	for (count=0;count<OPENAL_NUM_BUFFERS;count++) {
		if (count==0) {
			local_output_buffer_n = output_buffer_n0;
		}
		else {
			local_output_buffer_n = output_buffer_n;
		}
		loop_buffers_audio_engine_stream[count] = new ALuint[local_output_buffer_n];
		alGetError(); // clear error code
		alGenBuffers(local_output_buffer_n, loop_buffers_audio_engine_stream[count]);
		if((error = alGetError()) != AL_NO_ERROR) {
			printf("Error Generating OpenAL Buffers for audio engine AL_STREAM mode!");
			exit(1);
		}
	}
	
	pthread_attr_init(&audio_engine_attr);
	pthread_attr_init(&audio_update_attr);
	
	//pthread_attr_setdetachstate(&audio_engine_attr, PTHREAD_CREATE_JOINABLE);
	
	// SD TODO - Check wether the thread needs a large amount of stack
	// pthread_attr_setstacksize (&audio_engine_attr, audio_egine_stacksize);
	
	// SD TODO - Check if this is needed
	// audio_engine_once = PTHREAD_ONCE_INIT;
	// pthread_once(audio_engine_once_control, ACAudioFeedback::threadAudioEngineInit)
	
	pthread_mutexattr_init(&audio_engine_cond_mutex_attr);
	/////////// SD TODO - maybe set the process-share attribute which allows other process to see the conditional variable
	pthread_mutex_init(&audio_engine_cond_mutex, &audio_engine_cond_mutex_attr);
	pthread_mutexattr_destroy(&audio_engine_cond_mutex_attr);	
	
	pthread_condattr_init(&audio_engine_cond_attr);
	/////////// SD TODO
	pthread_cond_init(&audio_engine_cond, &audio_engine_cond_attr); //
	pthread_condattr_destroy(&audio_engine_cond_attr);
	
	pthread_mutexattr_init(&audio_engine_mutex_attr);
	/////////// SD TODO - maybe set the process-share attribute which allows other process to see the conditional variable
	pthread_mutex_init(&audio_engine_mutex, &audio_engine_mutex_attr);
	pthread_mutexattr_destroy(&audio_engine_mutex_attr);
	
	audio_engine_arg = (void*)this;
	pthread_create(&audio_engine, &audio_engine_attr, &threadAudioEngineFunction, audio_engine_arg);
	pthread_attr_destroy(&audio_engine_attr);
	
	audio_update_arg = (void*)this;
	pthread_create(&audio_update, &audio_update_attr, &threadAudioUpdateFunction, audio_update_arg);
	pthread_attr_destroy(&audio_update_attr);
	engine_running=1;
	
	createSourceSynchro(); // create dummy source for synchronization
}

void ACAudioFeedback::deleteAudioEngine()
{
// 	fclose(timing);
	
// 	// SD TODO - Clear Buffers
// 	//
 	pthread_cancel(audio_engine);
	pthread_cancel(audio_update);
// 	pthread_mutex_destroy(&audio_engine_mutex);
// 	pthread_cond_destroy(&audio_engine_cond);
// 	pthread_mutex_destroy(&audio_engine_cond_mutex);
// 	engine_running = 0;

	
}

// SD TODO - probably not needed
void ACAudioFeedback::threadAudioEngineInit()
{
}

void ACAudioFeedback::threadAudioEngine()
{
	
	/*
	// If need for a conditional variable
	pthread_mutex_lock(&audio_engine_cond_mutex);
	pthread_cond_wait(&audio_engine_cond, &audio_engine_cond_mutex);
	pthread_mutex_lock(&audio_engine_cond_mutex);
	pthread_mutex_unlock(&audio_engine_cond_mutex);
	// And in other thread
	pthread_mutex_lock(&audio_engine_cond_mutex);
	pthread_cond_signal(&audio_engine_cond);
	pthread_mutex_unlock(&audio_engine_cond_mutex);
	*/
	/*
	// If need for a mutex
	pthread_mutex_lock(&audio_engine_mutex);
	// pthread_mutex_trylock(&audio_engine_mutex); // maybe use this to avoid deadlocks
	pthread_mutex_unlock(&audio_engine_mutex);
	// And in other thread
	pthread_mutex_lock(&audio_engine_mutex);
	// pthread_mutex_trylock(&audio_engine_mutex); // maybe use this to avoid deadlocks
	pthread_mutex_unlock(&audio_engine_mutex);
	*/
	
	struct timeval  tv = {0, 0};
	struct timezone tz = {0, 0};
	
	gettimeofday(&tv, &tz);
	audio_engine_wakeup_time = (double)tv.tv_sec + tv.tv_usec / 1000000.0;
	audio_engine_fire_time = audio_engine_wakeup_time;
	
	float sleep_time = (float)output_buffer_size / (float)output_sample_rate / 8.0;
	int   sleep_time_usec = sleep_time * 1000000;
	
	time_from_start = 0;
	time_from_downbeat = 0;
	time_from_beat = 0;
	time_from_tatum = 0;
	downbeat_from_start = 0;
	
	
	prev_time_from_start = 0;
	prev_time_from_downbeat = 0;
	prev_time_from_beat = 0;
	prev_time_from_tatum = 0;
	prev_downbeat_from_start = 0;
	
	timeCodeDone = 0;
		
	double sdsleep;
	
	// MACH thread priority set
	int retvalue;
	/*mach_port_t machThread = mach_thread_self();
	int priority, newPriority;
	boolean_t isTimeshare, newIsTimeshare;
	*/
	
	retvalue = set_my_thread_priority(100);
	
	/*
	float periodicity;
	int period, computation, constraint;
	periodicity = 1.0/sleep_time;
	period = 266000000; // HZ;
	period = period/periodicity;
	computation = period/5;
	constraint = computation*2;
	retvalue = set_realtime(period, computation, constraint);
	*/
	
	/*
	retvalue = GetStdThreadSchedule(machThread, &priority, &isTimeshare);
	newPriority = 100;
	newIsTimeshare = 0;
	retvalue = RescheduleStdThread(machThread, newPriority, newIsTimeshare);
	retvalue = GetStdThreadSchedule(machThread, &priority, &isTimeshare);
	*/
	
	int slow_refresh;
	slow_refresh = 0;
	
	// Prepare end send buffers to audio rendering
	while (1) {
		
		pthread_testcancel();
		gettimeofday(&tv, &tz);
		audio_engine_current_time = (double)tv.tv_sec + tv.tv_usec / 1000000.0;
		
		sdtime = audio_engine_current_time-audio_engine_wakeup_time;
		
		// SD TODO - Check how this changes when using MIDI timecode
		if (1) { //audio_engine_current_time>=audio_engine_fire_time) {
			
			//fprintf(timing, "%f\n", ((sdtime-prevsdtime) * 1000));
			
			/*if (media_cycle) {
				media_cycle->setNeedsActivityUpdateLock(1);
				processAudioUpdate();
				media_cycle->setNeedsActivityUpdateLock(0);
			}*/
			
			pthread_mutex_lock(&audio_engine_mutex);
			
			//bool isPlaying = processAudioEngine();
			bool isPlaying = processAudioEngineNew();
			
			if (isPlaying)
				slow_refresh++;
			
			// SD BUG
			//timeCodeAudioEngine(sleep_time*output_sample_rate);
			
			audio_engine_fire_time += sleep_time;
			
			pthread_mutex_unlock(&audio_engine_mutex);
			
			// TODO - this should be done less frequently than frame rate
			// if (media_cycle && isPlaying && slow_refresh>=20) {
			if (media_cycle && slow_refresh>=40) {
				media_cycle->setNeedsDisplay(true);
				slow_refresh = 0;
			}
		}
		
		prevsdtime = sdtime;
		
		gettimeofday(&tv, &tz);
		audio_engine_current_time_2 = (double)tv.tv_sec + tv.tv_usec / 1000000.0;

		sdsleep = sleep_time_usec-(audio_engine_current_time_2-audio_engine_current_time)*1000000.0;
		if (sdsleep>0) {
			usleep(sdsleep);
		}
	}
}

void ACAudioFeedback::threadAudioUpdate()
{
	while (1) {
		pthread_testcancel();
		if (media_cycle) {
			media_cycle->setNeedsActivityUpdateLock(1);
			processAudioUpdate();
			media_cycle->setNeedsActivityUpdateLock(0);
		}
		usleep(10000);
	}
}

void ACAudioFeedback::timeCodeAudioEngine(int n_samples) {
	
	prev_time_from_start = time_from_start;
	prev_time_from_downbeat = time_from_downbeat;
	prev_time_from_beat = time_from_beat;
	prev_time_from_tatum = time_from_tatum;
	prev_downbeat_from_start = downbeat_from_start;
	
	//time_from_downbeat += (float)output_buffer_size / output_sample_rate / 2;
	time_from_start += (float)n_samples / output_sample_rate;
	
	time_from_downbeat += (float)n_samples / output_sample_rate;
	
	if (time_from_downbeat >= 60.0/active_bpm*4) {
		downbeat_from_start++;
		time_from_downbeat -= 60.0/active_bpm*4;
	}

	time_from_beat = time_from_downbeat;
	while (time_from_beat >= (60.0/active_bpm)) {
		time_from_beat -= 60.0/active_bpm;
	}

	time_from_tatum = time_from_beat;
	while (time_from_tatum >= (60.0/active_bpm/active_tstype)) {
		time_from_tatum -= 60.0/active_bpm/active_tstype;
	}
}

// This creates or delete sources according to actions made in other modules
void ACAudioFeedback::processAudioUpdate()
{
	int i;
	int loop_id;
	float x, y, z;
	
	vector<int>* mNeedsActivityUpdateMedia;
	mNeedsActivityUpdateMedia = media_cycle->getNeedsActivityUpdateMedia();
		
	for (i=0;i<(*mNeedsActivityUpdateMedia).size();i++) {
		loop_id = (*mNeedsActivityUpdateMedia)[i];
		const ACMediaNode &attribute = media_cycle->getMediaNode(loop_id);
		const ACPoint &p = attribute.getCurrentPosition();
		x=p.x;
		y=0;
		z=p.y;
		if (attribute.getActivity()>0) {
			createSourceWithPosition(loop_id, x, y, z);
		}
		else {
			deleteSource(loop_id);
		}
	}
	media_cycle->setNeedsActivityUpdateRemoveMedia();
}

bool ACAudioFeedback::processAudioEngineNew()
{	
	ALenum error;
	
	int count, current, current_unqueue;
	
	ALenum  format;
	ALvoid* data;
	ALsizei size;
	ALsizei freq;
	
	ALint	buffer_processed;
	ALint	buffer_queued;
	ALint	buffer_processed_1;
	ALint	buffer_queued_1;
	ALint  source_state;
	int		prev_sample_pos;
	int		sample_pos;
	int		sample_pos_limit;
	
	format = AL_FORMAT_MONO16;
	size = output_buffer_size;
	freq = output_sample_rate;
	
	ALuint local_new_buffer;
	
	int active_loops_counted = 0;
	
	ALuint local_buffer;
	
	timeCodeDone = 0;
	
	// add_buffers
	int add_buffers;
	
	add_buffers = 0;
	
	// SD NEW - First check for synchro loop
	count = 0;
	
	if (loop_ids[count]==-2) {
		
		// Detect queued buffers
		alGetSourcei(loop_sources[count], AL_BUFFERS_QUEUED, &buffer_queued);
		
		// Detect already processed used buffers
		alGetSourcei(loop_sources[count], AL_BUFFERS_PROCESSED, &buffer_processed);
		
		//printf ("synchro buffers %d, %d\n", buffer_queued, buffer_processed);

		if ( (buffer_queued<output_buffer_n0) || (buffer_processed>0) ) {
			add_buffers= 1;
		}
		
		for (int k=0;k<add_buffers;k++) {
			
			timeCodeAudioEngine(output_buffer_size);
			
			memset(output_buffer, 0, output_buffer_size*sizeof(short));
			data = output_buffer;
			current = current_buffer[count];
			current_unqueue = current_buffer_unqueue[count];
			
			// Unqueue processed buffers
			if (k<buffer_processed) {
				alSourceUnqueueBuffers(loop_sources[count], 1, &loop_buffers_audio_engine_stream[count][current_unqueue]);
				current_buffer_unqueue[count]++;
				if (current_buffer_unqueue[count] >= output_buffer_n0) {
					current_buffer_unqueue[count] = 0;
				}
			}
			
			// Fill with data
			alBufferData(loop_buffers_audio_engine_stream[count][current], format, data, size*2, freq);
			current_buffer[count]++;
			if (current_buffer[count] >= output_buffer_n0) {
				current_buffer[count] = 0;
			}
			
			// Queue prepared stream buffer to appropriate source
			alSourceQueueBuffers(loop_sources[count], 1, &loop_buffers_audio_engine_stream[count][current]);
		}
		
		alGetSourcei(loop_sources[count], AL_SOURCE_STATE, &source_state);
		if ( (source_state!=AL_PLAYING) ) {
			alSourcePlay(loop_sources[count]);
		}
		
	}
	count++;
	
	
	//while (active_loops_counted<active_loops)  {
	while (count<OPENAL_NUM_BUFFERS) {
		
		if (loop_ids[count]>=0) {
			
			active_loops_counted++;
			
			// Detect queued buffers
			alGetSourcei(loop_sources[count], AL_BUFFERS_QUEUED, &buffer_queued);
			
			// Detect already processed used buffers
			alGetSourcei(loop_sources[count], AL_BUFFERS_PROCESSED, &buffer_processed);
						
			// If source has just started, first fill with zeros.. beurk...
			if ( (add_buffers) && (buffer_queued==0) ) {
				for (int k=0;k<output_buffer_n0-add_buffers;k++) {
					
					memset(output_buffer, 0, output_buffer_size*sizeof(short));
					data = output_buffer;
					current = current_buffer[count];
					
					// Fill with data
					alBufferData(loop_buffers_audio_engine_stream[count][current], format, data, size*2, freq);
					
					// Queue prepared stream buffer to appropriate source
					alSourceQueueBuffers(loop_sources[count], 1, &loop_buffers_audio_engine_stream[count][current]);
					
					// Forward in buffers
					current_buffer[count]++;
					if (current_buffer[count] >= output_buffer_n) {
						current_buffer[count] = 0;
					}
				}
			}
			
			for (int k=0;k<buffer_processed;k++) {
				
				current_unqueue = current_buffer_unqueue[count];
				
				// Unqueue processed buffers
				alSourceUnqueueBuffers(loop_sources[count], 1, &loop_buffers_audio_engine_stream[count][current_unqueue]);
				
				// Forward in buffers
				current_buffer_unqueue[count]++;
				if (current_buffer_unqueue[count] >= output_buffer_n) {
					current_buffer_unqueue[count] = 0;
				}
			}
			
			for (int k=0;k<add_buffers;k++) {
				
				// Compute sample position and resynthesize buffer (may be several analysis frames)
				processAudioEngineSamplePosition(count, &prev_sample_pos, &sample_pos, &sample_pos_limit);
				
				//fprintf(timing, "%d %f\n", add_buffers, float(sample_pos));
				//fprintf(timing, "%f\n", float(sample_pos));
				processAudioEngineResynth(count, prev_sample_pos, sample_pos, sample_pos_limit, output_buffer);
				
				data = output_buffer;
				current = current_buffer[count];
			
				// Fill with data
				alBufferData(loop_buffers_audio_engine_stream[count][current], format, data, size*2, freq);
				
				// Queue prepared stream buffer to appropriate source
				alSourceQueueBuffers(loop_sources[count], 1, &loop_buffers_audio_engine_stream[count][current]);
				
				// Forward in buffers
				current_buffer[count]++;
				if (current_buffer[count] >= output_buffer_n) {
					current_buffer[count] = 0;
				}
				
			}
			
			// SD TODO - Check this
			alGetSourcei(loop_sources[count], AL_SOURCE_STATE, &source_state);
			if ( (source_state!=AL_PLAYING) ) {
				alSourcePlay(loop_sources[count]);
				//fprintf(timing, "WAS OFF\n");
			}
			
			// Detect queued buffers
			alGetSourcei(loop_sources[count], AL_BUFFERS_QUEUED, &buffer_queued_1);
			
			// Detect already processed used buffers
			alGetSourcei(loop_sources[count], AL_BUFFERS_PROCESSED, &buffer_processed_1);
			
			if (add_buffers) {
				//printf ("Synchro buffers %d, %d, %d - %d, %d\n", count, buffer_queued, buffer_processed, buffer_queued_1, buffer_processed_1);//CF
			}
			
		}
		
		count++;
		
	}
	
	if (add_buffers && active_loops_counted) {
		//printf ("========================================\n");//CF
	}	
	
	return (active_loops_counted >=1 ? true : false);
}

bool ACAudioFeedback::processAudioEngine()
{	
	ALenum error;
	
	int count, current, current_unqueue;
	
	ALenum  format;
	ALvoid* data;
	ALsizei size;
	ALsizei freq;
	
	ALint	buffer_processed;
	ALint	buffer_queued;
	ALint  source_state;
	int		prev_sample_pos;
	int		sample_pos;
	int		sample_pos_limit;
	
	format = AL_FORMAT_MONO16;
	size = output_buffer_size;
	freq = output_sample_rate;
	
	ALuint local_new_buffer;
		
	//for (count=0;count<active_loops;count++) {
	int active_loops_counted = 0;
	
	ALuint local_buffer;
	
	timeCodeDone = 0;

	count = 0;
	
	//while (active_loops_counted<active_loops)  {
	while (count<OPENAL_NUM_BUFFERS) {
		
		if (loop_ids[count]>=0) {
			
			active_loops_counted++;
			
			// Detect queued buffers
			alGetSourcei(loop_sources[count], AL_BUFFERS_QUEUED, &buffer_queued);
			if (buffer_queued<output_buffer_n) {
				// TODO SD
				// we may have got a problem here
				printf ("%d: missing buffers %d\n", count, (output_buffer_n-buffer_queued));
			}
			
			// Detect already processed used buffers
			alGetSourcei(loop_sources[count], AL_BUFFERS_PROCESSED, &buffer_processed);
			
			if ( (buffer_queued<output_buffer_n) || (buffer_processed>0) ) {
					
				// Unqueue already used buffers
				/*if (buffer_processed==1) {
					alSourceUnqueueBuffers(loop_sources[count], buffer_processed, loop_buffers_audio_engine_stream[count]);
					// printf ("buffer processed %d\n",buffer_processed);
				}
				else if (buffer_processed>1) {
					alSourceUnqueueBuffers(loop_sources[count], buffer_processed, loop_buffers_audio_engine_stream[count]);
					printf ("buffer processed %d\n",buffer_processed);
				}*/
				
				int add_buffers;
				if (buffer_queued<output_buffer_n) { add_buffers=output_buffer_n-buffer_queued; }
				else {add_buffers = buffer_processed;}
                if(!buffer_queued){
					add_buffers=2;
                }
                else if (add_buffers) {
					add_buffers= 1;
				}
				
				for (int k=0;k<add_buffers;k++) {
					
					/////////CF printf ("%d: buffer processed %d\n", count, buffer_processed);
					//printf ("buffer processed %d\n",buffer_processed);
					
					// Compute sample position and resynthesize buffer (may be several analysis frames)
					processAudioEngineSamplePosition(count, &prev_sample_pos, &sample_pos, &sample_pos_limit);
				
					//fprintf(timing, "%d %f\n", add_buffers, float(sample_pos));
					//fprintf(timing, "%f\n", float(sample_pos));
					processAudioEngineResynth(count, prev_sample_pos, sample_pos, sample_pos_limit, output_buffer);
					
					// SD TOTO - check this
					//if (!timeCodeDone) {
					 timeCodeDone += 1;
					if (timeCodeDone>=active_loops) {
						// SD TODO - why did I need / 2
						timeCodeAudioEngine(output_buffer_size);
						timeCodeDone -= active_loops;
					}
					 
					/*if ( (!timeCodeDone) ) {
						timeCodeAudioEngine(output_buffer_size);
						timeCodeDone = 1;
					}*/
					
					//}
					
					// SD - nov 2009 ) alternate method, recreate new buffers because I may have a problem with the order 
					//		when I reintroduce 2 or more buffers
					/*
					 data = output_buffer;
					current = current_buffer[count];
					//alGenBuffers(1, &(loop_buffers_audio_engine_stream[count][current]));
					
					alDeleteBuffers(1, &loop_buffers_audio_engine_stream[count][current]);

					alGenBuffers(1, &local_buffer);
					if((error = alGetError()) != AL_NO_ERROR) {
						printf("Error Regenerating OpenAL Buffers for audio engine AL_STREAM !\n");
						//exit(1);
					}
					alBufferData(local_buffer, format, data, size*2, freq);
					//alBufferData(loop_buffers_audio_engine_stream[count][current], format, data, size*2, freq);
					current_buffer[count]++;
					if (current_buffer[count] >= output_buffer_n) {
						current_buffer[count] = 0;
					}
					
					// Queue prepared stream buffer to appropriate source
					alSourceQueueBuffers(loop_sources[count], 1, &local_buffer);
					*/
					
					data = output_buffer;
					current = current_buffer[count];
                                        current_unqueue= current_buffer_unqueue[count];
					// Fill buffer
					if (k<buffer_processed) {
						alSourceUnqueueBuffers(loop_sources[count], 1, &loop_buffers_audio_engine_stream[count][current_unqueue]);
                                                current_buffer_unqueue[count]++;
                                                if (current_buffer_unqueue[count] >= output_buffer_n) {
                                                    current_buffer_unqueue[count] = 0;
                                                }
                                        }
					
					alBufferData(loop_buffers_audio_engine_stream[count][current], format, data, size*2, freq);
					current_buffer[count]++;
					if (current_buffer[count] >= output_buffer_n) {
						current_buffer[count] = 0;
					}
				
					// Queue prepared stream buffer to appropriate source
					alSourceQueueBuffers(loop_sources[count], 1, &loop_buffers_audio_engine_stream[count][current]);
					 
				}
				
			}
			
			// SD TODO - Check this
			alGetSourcei(loop_sources[count], AL_SOURCE_STATE, &source_state);
			if ( (source_state!=AL_PLAYING) ) {
				alSourcePlay(loop_sources[count]);
				//fprintf(timing, "WAS OFF\n");
			}
		}
		
		count++;
		
	}
	
	return (active_loops_counted >=1 ? true : false);
}

void ACAudioFeedback::setScrub(float scrub) {
	
	float alpha;
	float local_scrub_speed;
	float delta_time;
	struct timeval  tv = {0, 0};
	struct timezone tz = {0, 0};
	double current_scrub_time;
	
	alpha = 0.05;
	
	gettimeofday(&tv, &tz);
	current_scrub_time = (double)tv.tv_sec + tv.tv_usec / 1000000.0;
	current_scrub_time -= audio_engine_wakeup_time;
	
	prev_scrub_pos = scrub_pos;
	scrub_pos = scrub / 100;
	
	prev_scrub_time = scrub_time;
	scrub_time = current_scrub_time;
	delta_time = scrub_time - prev_scrub_time;
	
	local_scrub_speed = (scrub_pos - prev_scrub_pos);
	if (local_scrub_speed>0.5) {
		local_scrub_speed -= 1.0;
	}
	else if (local_scrub_speed<-0.5) {
		local_scrub_speed += 1.0;
	}
	local_scrub_speed /= delta_time;
	local_scrub_speed *= 10;
	if ( signbit(local_scrub_speed)==signbit(scrub_speed) ) {
		scrub_speed = alpha*local_scrub_speed + (1-alpha)*scrub_speed;
	}
	else {
		scrub_speed = local_scrub_speed;
	}
}

// SD TODO - should be passed to this funciton: the buffer size + time code + normalize time code to buffer size + controler positions
void ACAudioFeedback::processAudioEngineSamplePosition(int _loop_slot, int *_prev_sample_pos, int *_sample_pos, int *_sample_pos_limit)
{
	int frame_pos;
	
	// ACAudioLoop *audio_loop;
	int loop_id;
	int size;
	
	float local_bpm;
	
	loop_id = loop_ids[_loop_slot];
	// audio_loop = media_cycle->getAudioLibrary()->getMedia(loop_id);
	size = use_sample_end[_loop_slot] - use_sample_start[_loop_slot];
		
	
	// DT : made sample_start and end actually useful
	int sample_start = ((ACAudio*) media_cycle->getLibrary()->getMedia(loop_id))->getSampleStart();
	int sample_end = ((ACAudio*) media_cycle->getLibrary()->getMedia(loop_id))->getSampleEnd();
	int sample_size = sample_end - sample_start;
	
	switch (loop_synchro_mode[_loop_slot]) {
		case ACAudioEngineSynchroModeDownbeatSimple:
			*_sample_pos  = time_from_downbeat * output_sample_rate;
			reached_end = 0;
			reached_begin = 0;
			while (*_sample_pos>=size) {
				reached_end = 1;
				*_sample_pos -= size;
			}
			while (*_sample_pos<0) {
				reached_begin = 1;
				*_sample_pos += size;
			}
			break;
		case ACAudioEngineSynchroModeAutoBeat:
			if (use_bpm[_loop_slot]) {
				float nbeats;
				nbeats = ((sample_end-sample_start)/output_sample_rate) / 60.0 * local_bpm;
				*_prev_sample_pos = prev_time_from_downbeat * output_sample_rate * (active_bpm / use_bpm[_loop_slot]);
				*_sample_pos = time_from_downbeat * output_sample_rate * (active_bpm / use_bpm[_loop_slot]);
				*_sample_pos_limit = (60.0/active_bpm*4) * output_sample_rate * (active_bpm / use_bpm[_loop_slot]);	
				if (size<*_sample_pos_limit) {
					*_sample_pos_limit = size;
				}
				/*if (nbeats>4.1) {
					*_prev_sample_pos += (prev_downbeat_from_start%2) * 4 * output_sample_rate * (60.0 / use_bpm[_loop_slot]);
					*_sample_pos += (downbeat_from_start%2) * 4 * output_sample_rate * (60.0 / use_bpm[_loop_slot]);
				}*/
			}
			// SD 2009 dec - In auto beat, loops with no BPM should be played at normal speed
			else {
			}
			
		/*
			int loop_n_beats;
			loop_n_beats = (int)((float)size/output_sample_rate/audio_loop->bpm*60);
			
			//int	  loop_n_downbeats = (int)loop_n_beats/4;
			int phrase_duration; // number of downbeat per phrase...
			phrase_duration = 4;
			float time_from_phrasedownbeat = (downbeat_from_start % phrase_duration) * 4 * (60/active_bpm); 
			*_sample_pos += time_from_phrasedownbeat * output_sample_rate * (active_bpm / audio_loop->bpm);
			
			if (loop_n_beats<2) {
				if (*_sample_pos >= size) {
					*_sample_pos = -1;
				}
			}
			else {
				while (*_sample_pos >= size) {
					*_sample_pos -= size;
				}
			}
		 */			
			break;
		case ACAudioEngineSynchroModeAutoBarStretch:
			
			break;
		case ACAudioEngineSynchroModeAutoBar:
			
			break;
		case ACAudioEngineSynchroModeManual:
			reached_end = 0;
			reached_begin = 0;
			//*_sample_pos = (int)(scrub_pos * size / 100);
			*_sample_pos = prev_sample_pos[_loop_slot] + (output_buffer_size)*scrub_speed;
			*_sample_pos_limit = size;
			while (*_sample_pos>=size) {
				reached_end = 1;
				*_sample_pos -= size;
			}
			while (*_sample_pos<0) {
				reached_begin = 1;
				*_sample_pos += size;
			}
			if (sdtime-scrub_time>0.25) {
				scrub_speed *= 0.9;
			}
			break;
		case ACAudioEngineSynchroModeNone:
			// no synchronization, just play the loop as it is
			if (prev_sample_pos[_loop_slot]<0) {
				*_sample_pos = 0;
			}
			else {
				// SD TODO - why do we need / 2
				// *_sample_pos  = prev_sample_pos[_loop_slot] + output_buffer_size / 2;
				reached_end = 0;
				reached_begin = 0;
				*_sample_pos  = prev_sample_pos[_loop_slot] + output_buffer_size;
				*_sample_pos_limit = size;
				while (*_sample_pos>=size) {
					reached_end = 1;
					*_sample_pos -= size;
				}
				while (*_sample_pos<0) {
					reached_begin = 1;
					*_sample_pos += size;
				}
			}
			break;
	}
	
	if (use_bpm[_loop_slot]) {
		frame_pos = (float)*_sample_pos / (float) output_sample_rate / 0.01 / (100.0 / use_bpm[_loop_slot]); // SD TODO - hop size should be used.... 
	}
	else {
		frame_pos = (float)*_sample_pos / (float) output_sample_rate / 0.01 / (100.0 / 76.0); // SD TODO - hop size should be used.... 
	}
	media_cycle->setSourceCursor(loop_id, frame_pos);
}

// SD TODO - should be passed a pointer to the loop samples to make the engine separate from ACAudioFeedback
//		output buffer size should be passed too
void ACAudioFeedback::processAudioEngineResynth(int _loop_slot, int _prev_sample_pos, int _sample_pos, int _sample_pos_limit, short *_output_buffer)
{
	int i;
	
	// ACAudioLoop *audio_loop;
	int loop_id;
	int size;
	int copied, local_pos, tocopy;
	float local_pos_f, local_pos_frac;
	
	loop_id = loop_ids[_loop_slot];
	// audio_loop = media_cycle->getAudioLibrary()->getMedia(loop_id);
	size = use_sample_end[_loop_slot] - use_sample_start[_loop_slot];
	
	void *source;
	void *destination;
	
	float bpm_ratio;
	float pitch_ratio = 0;
	
	if (loop_synchro_mode[_loop_slot] == ACAudioEngineSynchroModeAutoBeat) {
		size = _sample_pos_limit;
		prev_sample_pos[_loop_slot] = _prev_sample_pos;
	}
	
	bpm_ratio = ((float)_sample_pos - (float)prev_sample_pos[_loop_slot]);
	
	// SD TODO - recover code to implements this
	// SD TODO - this alsa has to resample to the selected output sample rate, for instance 44.1K
	switch (loop_scale_mode[_loop_slot]) {
		case ACAudioEngineScaleModeVocode:
			
			if (bpm_ratio<0) {
				bpm_ratio += size;
			}
			bpm_ratio /= (float)output_buffer_size;
			
			memset(_output_buffer, 0, output_buffer_size*sizeof(short));

			if (size>0) {
				
				local_pos_f = prev_sample_pos[_loop_slot];
				local_pos = floor(local_pos_f);

				int k;  
				int kk;
				float *os;
				kk = 0;
				//int niter = output_buffer_size / 512;
				int niter = output_buffer_size / 512; //tpv[_loop_slot].hopSize;
				
				pv_currentsample[_loop_slot] = prev_sample_pos[_loop_slot];
				
				int startbuffer;
				
				if (local_pos>=0) {
									
				for (i=0;i<niter;i++) {
					// SD TODO - VOCODER CALL
					// First test using the numediart audioskimming code - not conclusive (too slow)
					/*
					pv_complex_change_rate_pitch (pv[_loop_slot],bpm_ratio,pitch_ratio);
					play_hopsyn_curses (pv[_loop_slot],&(pv_currentsample[_loop_slot]),0,size);
					os = (float *) pv[_loop_slot]->hop_out_buffer;
					for(k=0;k<512;k++) {
						_output_buffer[kk++] = *(os+k); //hard-coded but not correct
					}  
					*/
					// Second try with new Alexis vocoder
					tpv[_loop_slot].speed = bpm_ratio;
					if (0) //local_pos<4096)
						getCurrentFrame(&(tpv[_loop_slot]),1);
					else
						getCurrentFrame(&(tpv[_loop_slot]),0);
					//setCurrentSampleToNext(&(tpv[_loop_slot]));
					if (1)
						//setCurrentSample(&(tpv[_loop_slot]),pv_currentsample[_loop_slot]);
						//setCurrentSample(&(tpv[_loop_slot]),local_pos);
						setCurrentSample(&(tpv[_loop_slot]),local_pos);
					else
						setCurrentSampleToNext(&(tpv[_loop_slot]));
					
					local_pos_f += bpm_ratio*512; //bpm_ratio;
					if (local_pos_f>=size) {
						local_pos_f -= size;
					}
					if (local_pos_f<=0) {
						local_pos_f = 0;
					}
					local_pos = floor(local_pos_f);
					
					// other possibilitty
					// int setCurrentSample(TiPhaseVocoder *tpv,double value) ;
					doOLA(&(tpv[_loop_slot]));
					startbuffer = tpv[_loop_slot].bufferPos - tpv[_loop_slot].hopSize;
					if (startbuffer >= 0) {
						//datapointer = tpv.buffer+startbuffer; tpv.hopSize;
						for(k=0;k<tpv[_loop_slot].hopSize;k++) {
							_output_buffer[kk++] = *((double*)tpv[_loop_slot].buffer+startbuffer+k)/2;
						}
					}
					else {
						//tpv.buffer+startbuffer+tpv.winSize; -startbuffer;
						//if (tpv.bufferPos > 0)
						//	tpv.buffer; tpv.bufferPos;	
						for(k=0;k<-startbuffer;k++) {
							_output_buffer[kk++] = *((double*)tpv[_loop_slot].buffer+startbuffer+tpv[_loop_slot].winSize+k)/2;
						}	
						if (tpv[_loop_slot].bufferPos > 0) {
							for(k=0;k<tpv[_loop_slot].bufferPos;k++) {
								_output_buffer[kk++] = *((double*)tpv[_loop_slot].buffer+k)/2;
							}
						}
					}
					

				}
				}
				//else {
				//	memset(_output_buffer, 0, output_buffer_size*sizeof(char));
				//}
				//fwrite(_output_buffer,sizeof(short),output_buffer_size,debug_vocoder);
			}
			
			/*if (size>0) {
				local_pos_f = prev_sample_pos[_loop_slot];
				local_pos = floor(local_pos_f);
				//if (_sample_pos>=0) {
				if (local_pos>=0) {
					for (i=0;i<output_buffer_size;i++) {
						_output_buffer[i] = loop_buffers_audio_engine[_loop_slot][local_pos];
						local_pos_f += bpm_ratio;
						if (local_pos_f>=size) {
							local_pos_f -= size;
						}
						if (local_pos_f<=0) {
							local_pos_f = 0;
						}
						local_pos = floor(local_pos_f);
					}
				}
				else {
					for (i=0;i<output_buffer_size;i++) {
						_output_buffer[i] = 0;
					}
				}
			}	*/					
			break;
		case ACAudioEngineScaleModeSkipAndResample:
			if (size>0) {
				local_pos_f = prev_sample_pos[_loop_slot];
				local_pos = floor(local_pos_f);
				//if (_sample_pos>=0) {
				if (local_pos>=0) {
					for (i=0;i<output_buffer_size;i++) {
						_output_buffer[i] = loop_buffers_audio_engine[_loop_slot][local_pos];
						local_pos_f += 1; //bpm_ratio;
						if (local_pos_f>=size) {
							local_pos_f -= size;
						}
						if (local_pos_f<=0) {
							local_pos_f = 0;
						}
						local_pos = floor(local_pos_f);
					}
				}
				else {
					for (i=0;i<output_buffer_size;i++) {
						_output_buffer[i] = 0;
					}
				}
			}
			break;
		case ACAudioEngineScaleModeResample:
			// simple internal resample for testing purposes
			// no filtering
			// but still loops
			// Scratch ... why do we need * 2.0
			
			/*if (reached_end) {
				bpm_ratio += size;
			}
			else if (reached_begin) {
				bpm_ratio -= size;
			}*/
			while (bpm_ratio<0) {
				bpm_ratio += size;
			}
			bpm_ratio /= (float)output_buffer_size;
			
			memset(_output_buffer, 0, output_buffer_size*sizeof(short));
			
			if (size>0) {
				local_pos_f = prev_sample_pos[_loop_slot];
				/*if (local_pos_f>=size) {
					local_pos_f -= size;
				}
				if (local_pos_f<=0) {
					local_pos_f = 0;
				}*/				
				// printf ("%d - %f - %d - %f\n", size, local_pos_f, _sample_pos, bpm_ratio);
				local_pos = floor(local_pos_f);
				local_pos_frac = local_pos_f - local_pos;
				//if (_sample_pos>=0) {
				if (local_pos>=0) {
					for (i=0;i<output_buffer_size;i++) {
						if (local_pos<size-1) {
							_output_buffer[i] = (1.0-local_pos_frac)*(float)loop_buffers_audio_engine[_loop_slot][local_pos]
											+(local_pos_frac)*(float)loop_buffers_audio_engine[_loop_slot][(local_pos+1)%size];
						}
						else {
							_output_buffer[i] = 0;
						}
						//_output_buffer[i] = loop_buffers_audio_engine[_loop_slot][local_pos];
						local_pos_f += bpm_ratio;
						if (local_pos_f>=size) {
							local_pos_f -= size;
						}
						if (local_pos_f<=0) {
							local_pos_f = 0;
						}
						local_pos = floor(local_pos_f);
						local_pos_frac = local_pos_f - local_pos;
					}
				}
				else {
					for (i=0;i<output_buffer_size;i++) {
						_output_buffer[i] = 0;
					}
				}
			}			
			break;
		case ACAudioEngineScaleModeNone:
			// skipping or repeating parts of the sound
			if (size>0) {
				// to avoid problems with very short loops, or long buffers
				copied = 0;
				local_pos = _sample_pos;
				while (copied<output_buffer_size) {
					tocopy = min(size-local_pos, output_buffer_size-copied);
					source = (short*)loop_buffers_audio_engine[_loop_slot]+local_pos;
					destination = (short*)(_output_buffer)+copied;
					memcpy(destination, source, tocopy*sizeof(short));
					copied += tocopy;
					local_pos += tocopy;
					if (local_pos>=size) {
						local_pos = 0;
					}
				}
			}
			break;
	}
	prev_sample_pos[_loop_slot] = _sample_pos;
}

void ACAudioFeedback::setMIDITimeCode() 
{
}

void ACAudioFeedback::setTimeSignature(int tsnum, int tsden) 
{
	active_tsnum = tsnum;
	active_tsden = tsden;
	
	// SD TODO - write code for time signature analysis
	
	active_tstype = ACAudioEngineTimeSignatureTypeBinary;
	
	/*
	 for (count=0;count<n_time_signatures;count++) {
	 if ( (desc_time_signatures[count][0]==tsnum) && (desc_time_signatures[count][1]==tsden) ) {
	 active_tstype = desc_time_signatures[count][2];
	 }
	 }
	 */
}

void ACAudioFeedback::setBPM(float bpm) 
{
	active_bpm = bpm;
}

void ACAudioFeedback::setKey(int key)
{
	active_key = key;
}

// OpenAL general settings
void ACAudioFeedback::setListenerGain(float gain)
{
	alListenerf(AL_GAIN, gain);
}

void ACAudioFeedback::setRenderChannels(int channels)
{	
	//CF UInt32
	unsigned int setting = (channels == 0) ? alcGetEnumValue(NULL, "ALC_RENDER_CHANNEL_COUNT_MULTICHANNEL") : alcGetEnumValue(NULL, "ALC_RENDER_CHANNEL_COUNT_STEREO");
	// TODO SD - Check wether this allows to support multichannel (f.i. 5.1) rendering
	#if defined(__APPLE__)
		alMacOSXRenderChannelCountProc((const ALint) setting);
	#endif
}

void ACAudioFeedback::setRenderQuality(int quality)
{
	//CF UInt32
	unsigned int setting = (quality == 0) ? alcGetEnumValue(NULL, "ALC_SPATIAL_RENDERING_QUALITY_LOW") : alcGetEnumValue(NULL, "ALC_SPATIAL_RENDERING_QUALITY_HIGH");
	// TODO SD - This will activate OS-X specific extension for HRTF.
	#if defined(__APPLE__)
		alcMacOSXRenderingQualityProc((const ALint) setting);
	#endif
}

void ACAudioFeedback::setDistanceModel(int model)
{
	alDistanceModel(model);	
}

void ACAudioFeedback::setDopplerFactor(float doppler)
{
	alDopplerFactor(doppler);
}

void ACAudioFeedback::setSpeedOfSound(float speed)
{
	alSpeedOfSound(speed);
}

void ACAudioFeedback::setListenerPosition(float x, float y, float z)
{
	mListenerElevation = y;
	mListenerPos[0] = x;
	mListenerPos[1] = y;
	mListenerPos[2] = z;
	alListenerfv(AL_POSITION, mListenerPos);
}

void ACAudioFeedback::setListenerElevation(float y)
{
	mListenerElevation = y;
	mListenerPos[1] = mListenerElevation;
	alListenerfv(AL_POSITION, mListenerPos);
}


void ACAudioFeedback::setListenerOrientation(float _orientation, float *x_velocity, float *z_velocity)
{
	mAngle = _orientation;
	
	ALenum  error = AL_NO_ERROR;
	float	rads = DEG2RAD(mAngle);
	float	orientation[6] = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0};
	
	orientation[0] = cos(rads);
	orientation[1] = sin(rads);
	mListenerDirection = RAD2DEG(atan2(orientation[1], orientation[0]));
	
	// Change OpenAL Listener's Orientation
	orientation[0] = sin(rads);
	orientation[1] = 0.0;
	orientation[2] = -cos(rads);	
	
	alListenerfv(AL_ORIENTATION, orientation);
	if((error = alGetError()) != AL_NO_ERROR)
		printf("Error Setting Listener Orientation");
	
	setListenerVelocity(mVelocityScaler, x_velocity, z_velocity);		
}

void ACAudioFeedback::setListenerVelocity(float _velocity, float *x_velocity, float *z_velocity)
{
	mVelocityScaler = _velocity;
	
	ALenum  error = AL_NO_ERROR;
	float	rads = DEG2RAD(mAngle);
	float	velocity[3] = {	0.0, 0.0, 0.0};	
	
	// Change OpenAL Listener's Orientation
	velocity[0] = sin(rads) * mVelocityScaler;
	velocity[1] = 0.0;
	velocity[2] = -cos(rads) * mVelocityScaler;
	
	if (x_velocity) *x_velocity = velocity[0];	
	if (z_velocity) *z_velocity = velocity[2];	
	
	alListenerfv(AL_VELOCITY, velocity);
	if((error = alGetError()) != AL_NO_ERROR)
		printf("Error Setting Listener Velocity");	
}

int ACAudioFeedback::getLoopId(int slot)
{
	return loop_ids[slot];
}

int ACAudioFeedback::getLoopSlot(int loop_id) 
{
	int count;
	int loop_slot;
	loop_slot = -1;
	for (count=0;count<OPENAL_NUM_BUFFERS;count++) {
		if(loop_ids[count]==loop_id) {
			loop_slot = count;
		}
	}	
	return loop_slot;
}

void ACAudioFeedback::setLoopSynchroMode(int _loop_id, ACAudioEngineSynchroMode _synchro_mode)
{
	int _loop_slot = getLoopSlot(_loop_id); 
	if (_loop_slot != -1)
	{	
		if (loop_synchro_mode != NULL)
		{	
			loop_synchro_mode[_loop_slot] = _synchro_mode;
		}	
	}
	//else {//some error message?};
}

void ACAudioFeedback::setLoopScaleMode(int _loop_id, ACAudioEngineScaleMode _scale_mode)
{
	int _loop_slot = getLoopSlot(_loop_id); 
	if (_loop_slot != -1)
	{	
		if (loop_scale_mode != NULL)
		{	
			loop_scale_mode[_loop_slot] = _scale_mode;
		}	
	}
	//else {//some error message?};
}

int ACAudioFeedback::createSource(int loop_id)
{
	return createSourceWithPosition(loop_id, 0, 0, 0);
}

int ACAudioFeedback::createSourceSynchro() 
{	
	float   loop_pos[3];
	int loop_slot;
	ALuint	loop_source;
	
	loop_pos[0] = 0;
	loop_pos[1] = 0;
	loop_pos[2] = 0;
	
	loop_slot = 0;
	
	alGenSources(1, &(loop_sources[loop_slot]));
	loop_source = loop_sources[loop_slot];
		
#ifdef OPENAL_STREAM_MODE
	
	pthread_mutex_lock(&audio_engine_mutex);
	
	loop_ids[loop_slot] = -2; 
	
	// Turn Looping OFF - audio engine that controls the looping (done in audio engine thread)
	alSourcei(loop_source, AL_LOOPING, AL_FALSE);
	// Set Source Position
	alSourcefv(loop_source, AL_POSITION, loop_pos);
	// Set Source Reference Distance
	alSourcef(loop_source, AL_REFERENCE_DISTANCE, 5.0f);
		
	alSourcePlay(loop_source);
			
	active_loops++;
		
	pthread_mutex_unlock(&audio_engine_mutex);
	
#endif
	
	return 0;
}

int ACAudioFeedback::createSourceWithPosition(int loop_id, float x, float y, float z) 
{	
	// ACAudioLoop *audio_loop;

	int count;
	int loop_slot;
	
	ALenum  error = AL_NO_ERROR;
	ALenum  format;
	ALvoid* data;
	short*  datashort, *datashort2;
	short*	datas;
	ALsizei size;
	ALsizei freq;
	char*	loop_file;
	ALuint	loop_source;
	float   loop_pos[3];
	
	vector<float> local_feature;
	float local_bpm;
	int local_key;
	int local_acid_type;
	
	// Check if there are enough available sources left
	if (active_loops>=OPENAL_NUM_BUFFERS) {
		return 1;
	}
	
	// Check if specified loop is already active or not and find free source/buffer
	loop_slot = -1;
	for (count=0;count<OPENAL_NUM_BUFFERS;count++) {
		if( (loop_id != -1) && (loop_ids[count]==loop_id) ) {
			return 2;
		}
		if ( (loop_slot==-1) && (loop_ids[count]==-1) ) {
			loop_slot = count;
		}
	}
		
	// SD - bug workaround....
	// loop_slot = loop_id;
	
	// CF: temporary workaround as the ACUserLog tree and the ACLoopAttributes vector in ACMediaBrowser are not sync'd 
	int media_id = loop_id; // or media_cycle->getBrowser()->getMediaNode(loop_id).getMediaId(); 
	if (media_cycle->getBrowser()->getMode() == AC_MODE_NEIGHBORS)
		media_id = media_cycle->getBrowser()->getUserLog()->getMediaIdFromNodeId(loop_id);
	// audio_loop = media_cycle->getAudioLibrary()->getMedia(loop_id);
	loop_file = (char*)(media_cycle->getMediaFileName(media_id)).c_str();
	//loop_buffer = loop_buffers[loop_slot];
	
	// SD - Source Problem
	alGenSources(1, &(loop_sources[loop_slot]));
	
	loop_source = loop_sources[loop_slot];
	loop_pos[0] = x;
	loop_pos[1] = y;
	loop_pos[2] = z;
	local_bpm = 0;
	local_feature = media_cycle->getFeaturesVectorInMedia(media_id, "bpm");
	if ((local_feature).size()) {
		if ((local_feature).size()==1) {
			local_bpm = (local_feature)[0];
		}
	}
	local_key = 0;
	local_feature = media_cycle->getFeaturesVectorInMedia(media_id, "key");
	if ((local_feature).size()) {
		if ((local_feature).size()==1) {
			local_key = int((local_feature)[0]);
		}
	}
	// SD TODO - Acid type not yet used
	local_acid_type = 0;
	local_feature = media_cycle->getFeaturesVectorInMedia(media_id, "acid_type");
	if ((local_feature).size()) {
		if ((local_feature).size()==1) {
			local_acid_type = int((local_feature)[0]);
		}
	}
	// local_bpm = 120;
	// local_key = 65;
	// local_acid_type = 2;
	
	data = 0;
	
	// CF Cross-platform ALUT attempt...
	#ifdef WIN32 
		//data = alutLoadMemoryFromFile (loop_file, &format, &size, (ALfloat*) &freq);
		ALboolean al_bool;
		alutLoadWAVFile(loop_file, &format, &data, &size, &freq, &al_bool); 
	#elif defined(__APPLE__)
		#if defined(APPLE_LEOPARD)
			// TODO SD - This is OS-X specific. Should be changed.
			CFStringRef fileName = CFStringCreateWithCString(kCFAllocatorDefault, (const char*)(loop_file), kCFStringEncodingUTF8);
			CFStringRef fileNameEscaped = CFURLCreateStringByAddingPercentEscapes(NULL, fileName, NULL, NULL, kCFStringEncodingUTF8);
			CFURLRef	fileURL = CFURLCreateWithString(NULL, fileNameEscaped, NULL);	
			data = MyGetOpenALAudioData(fileURL, &size, &format, &freq);
			CFRelease(fileURL);
			CFRelease(fileNameEscaped);
			CFRelease(fileName);
			//CF by this (working!): alutLoadWAVFile(loop_file, &format, &data, &size, &freq);
		#else	
			//data = alutLoadMemoryFromFile (loop_file, &format, &size, (ALfloat*) &freq);
			alutLoadWAVFile(loop_file, &format, &data, &size, &freq);
		#endif
	#else 
		ALboolean al_bool;
		alutLoadWAVFile((ALbyte *) loop_file, &format, &data, &size,&freq, &al_bool);
	#endif
	
	// Convert to single channel (mono). OpenAl stereo sources are not spatialized indeed.
	// DT: To make sample_start and end actually work
	//	int sample_size;// = media_cycle->getWidth(loop_id);
	//	int sample_start = 0;
	//	int sample_end;
	int sample_start = ((ACAudio*) media_cycle->getLibrary()->getMedia(loop_id))->getSampleStart();
	int sample_end = ((ACAudio*) media_cycle->getLibrary()->getMedia(loop_id))->getSampleEnd();
	int sample_size;
	//int sample_size = sample_end - sample_start;
	
	switch (format) {
	case AL_FORMAT_MONO16:
		sample_size = size/2;
		break;
	case AL_FORMAT_STEREO8:
		sample_size = size/2;
		break;
	case AL_FORMAT_STEREO16:
		sample_size = size/4;
		break;
	}

	//sample_end = sample_size + sample_start;
	//size = sample_size;
	int segment_size = sample_end - sample_start;
	datashort = new short[segment_size];
	datas = (short*)data;
	if (format ==  AL_FORMAT_STEREO16) {
		for (count=sample_start;count<sample_end;count++) {
			datashort[count-sample_start] = (datas[2*count]+datas[2*count+1])/2;
		}
		format = AL_FORMAT_MONO16;
	}
	else if (format ==  AL_FORMAT_MONO16) {
		for (count=sample_start;count<sample_end;count++) {
			datashort[count-sample_start] = (datas[count]);
		}
	}
	
	// Normalization to same pitch using resampling
	// This will allow a demo with a phase vocoder
	// float bpm = audio_loop->bpm;
	// SD TODO - CLEAN THIS
	/*audio_loop->bpm = 90;
	 audio_loop->key = 0;
	 */
	
	local_key = local_key%12;
	float resample_ratio;
	if (local_key<=6) {
		resample_ratio = 1.0/(pow(2.0,local_key/12.0));
		//audio_loop->key -= key;
	}
	else {
		resample_ratio = 2.0/(pow(2.0,local_key/12.0));
		//audio_loop->key += (12-key);
	}
	// resample_ratio = 1;
	
	use_bpm[loop_slot] = local_bpm * resample_ratio;
	use_sample_end[loop_slot] = (int)((float)(sample_end-sample_start) / resample_ratio);
	use_sample_start[loop_slot] = 0; //(int)((float)sample_start / resample_ratio);
	int prevsize = segment_size;
	int segment_size_resample = use_sample_end[loop_slot] - use_sample_start[loop_slot];
	datashort2 = new short[segment_size_resample];
	int i;
	float j=0;
	for (i=0;i<segment_size_resample;i++) {
		datashort2[i] = datashort[(int)(j)];
		j += resample_ratio;
		if (j>=prevsize) {
			j = prevsize-1;
		}
	}
	delete[] datashort;
	datashort = datashort2;
	size = segment_size_resample;
	
#ifdef OPENAL_STREAM_MODE
	
	pthread_mutex_lock(&audio_engine_mutex);

	loop_ids[loop_slot] = loop_id; 
	
	// Turn Looping OFF - audio engine that controls the looping (done in audio engine thread)
	alSourcei(loop_source, AL_LOOPING, AL_FALSE);
	// Set Source Position
	alSourcefv(loop_source, AL_POSITION, loop_pos);
	// Set Source Reference Distance
	alSourcef(loop_source, AL_REFERENCE_DISTANCE, 5.0f);
	
	// Don't send buffers - audio engine send the buffer (done in the audio engine thread)
	// Just Start Playing Sound
	// SD TODO - Check that this is OK for OpenAl to start playing a source that hs no buffer
	
	// alSourcef(loop_source, AL_GAIN, 0.25);
	
	alSourcePlay(loop_source);
	
	// buffer has to be kept for our real-tim audio engine
	loop_buffers_audio_engine[loop_slot] = datashort;
	// DT: make sample start work
	prev_sample_pos[loop_slot] = use_sample_start[loop_slot]; // SD TODO
	//current_buffer[loop_slot] = 0;

	loop_synchro_mode[loop_slot] = ACAudioEngineSynchroModeAutoBeat;
	loop_scale_mode[loop_slot] = ACAudioEngineScaleModeResample; //CF: the Vocode mode sounds dirty, the Resample mode introduces a click at the beginning
	
	if (!use_bpm[loop_slot]) {
		loop_synchro_mode[loop_slot] = ACAudioEngineSynchroModeNone;
	}
	//loop_synchro_mode[loop_slot] = ACAudioEngineSynchroModeAutoBeat;  //ACAudioEngineSynchroModeManual; //CF scrub test
	active_loops++;
	
	//pv[loop_slot] = pv_complex_curses_init2(datashort,size,freq,NULL,1.0,0,2048,512,3,2); //hard-coded
	setSamples(&(tpv[loop_slot]),(short*)datashort,(int)size,(int)freq);
	initPV(&(tpv[loop_slot]));
	setWinsize(&(tpv[loop_slot]),tpv_winsize);
	tpv[loop_slot].speed = 1.0;
	
	pv_currentsample[loop_slot] = 0;
	   
	pthread_mutex_unlock(&audio_engine_mutex);
		
#elif OPENAL_STATIC_MODE	
	
	loop_ids[loop_slot] = loop_id; 
	
	// Turn Looping ON
	alSourcei(loop_source, AL_LOOPING, AL_TRUE);
	// Set Source Position
	alSourcefv(loop_source, AL_POSITION, loop_pos);
	// Set Source Reference Distance
	alSourcef(loop_source, AL_REFERENCE_DISTANCE, 5.0f);
	
	// Attach Audio Data to OpenAL Buffer
	alBufferData(loop_buffer, format, datashort, size, freq);
	
	// Attach OpenAL Buffer to OpenAL Source
	alSourceQueueBuffers(loop_source, 1, &loop_buffer);
	
	// Start Playing Sound
	alSourcePlay(loop_source);
	
	if((error = alGetError()) != AL_NO_ERROR) {
		printf("Error attaching buffer to source");
		exit(1);
	}
	
	// In this case, the buffer is not needed anymore, it has been copied by OpenAL
	delete[] datashort;
	
	active_loops++;
	
#endif
	
	// Release the audio data
	free(data);

return 0;
}

//CF check bit depth and sampling rate later...
int ACAudioFeedback::createExtSource(float* _buffer, int _length){
	
	//pthread_mutex_lock(&audio_engine_mutex);
	if (ext_loop_length != 0)
		deleteExtSource();	

	if (alGetError() != AL_NO_ERROR){
		std::cerr << "createExtSource, openAL error : " << alGetError() << std::endl;
		exit(1);
	}
		
	short* buffer_short = new short[_length];
	
	for (int i=0;i<_length;i++){
		buffer_short[i] = _buffer[i]*32767;
	}	
	
	ext_loop_length = _length;
	
	alGenBuffers(1, &ext_loop_buffer);
	if (alGetError() != AL_NO_ERROR){
		std::cerr << "createExtSource, openAL error : " << alGetError() << std::endl;
		exit(1);
	}
	
	alBufferData(ext_loop_buffer, AL_FORMAT_MONO16, buffer_short, _length * sizeof(short), 44100);
	
	if (alGetError() != AL_NO_ERROR){
		std::cerr << "createExtSource, openAL error : " << alGetError() << std::endl;
		exit(1);
	}
	
	alGenSources(1, &ext_loop_source);
	
	// On attache le tampon contenant les échantillons audio à la source
	alSourcei(ext_loop_source, AL_BUFFER, ext_loop_buffer);
	alSourcei(ext_loop_source, AL_LOOPING, AL_TRUE);

	//pthread_mutex_unlock(&audio_engine_mutex);
	delete[] buffer_short;
	
	return 0;	
}	

/*
For streaming, and hence real-time control, we'll have:
alSourcei(.., AL_LOOPING, AL_FALSE);
alGetSourcei(AL_BUFFERS_PROCESSED);
alSourceUnqueueBuffers();
refillBuffers();
alSourceQueueBuffers();
*/

int ACAudioFeedback::deleteSource(int loop_id) 
{
	ALenum error;
	
	int buffer_queued;
	int buffer_processed;
	int current;
	int current_unqueue;
	int i;
	
	int loop_slot;
	ALuint	loop_buffer;
	ALuint	loop_source;
	loop_slot = getLoopSlot(loop_id);
	if (loop_slot==-1) {
		return 1;
	}
	
	//loop_buffer = loop_buffers[loop_slot];
	loop_source = loop_sources[loop_slot];
	
#ifdef OPENAL_STREAM_MODE
	pthread_mutex_lock(&audio_engine_mutex);
	
	printf("Loop slot %d\n", loop_slot);
	
	// Stop source play
	alSourceStop(loop_sources[loop_slot]);
	// Detach buffer from source
	// alSourcei(loop_source, AL_BUFFER, 0);
	
	alGetSourcei(loop_sources[loop_slot], AL_BUFFERS_QUEUED, &buffer_queued);
	printf("%d, Queued buffers %d\n", loop_slot, buffer_queued);
	
	alGetSourcei(loop_sources[loop_slot], AL_BUFFERS_PROCESSED, &buffer_processed);
	printf("%d, Processed buffers %d\n", loop_slot, buffer_processed);
	
	current = 0;
	current_unqueue = current_buffer_unqueue[loop_slot];	
	// Unqueue processed buffers
	if (i<buffer_queued) {
		alSourceUnqueueBuffers(loop_sources[loop_slot], 1, &loop_buffers_audio_engine_stream[loop_slot][current_unqueue]);
		current_buffer_unqueue[loop_slot]++;
		if (current_buffer_unqueue[loop_slot] >= output_buffer_n) {
			current_buffer_unqueue[loop_slot] = 0;
		}
	}
	
	error = alGetError();
	if(error == AL_INVALID_VALUE) {
		printf("Error Unqueue Buffers invalid value %d!\n", current);
		//exit(1);
	}
	if(error == AL_INVALID_NAME) {
		printf("Error Unqueue Buffers invalid name %d!\n", current);
		//exit(1);
	}
	if(error == AL_INVALID_OPERATION) {
		printf("Error Unqueue Buffers invalid operation %d!\n", current);
		//exit(1);
	}
	//alSourcei(loop_source, AL_BUFFER, 0);
			
	// SD - Source Problem
	alDeleteSources(1, &(loop_sources[loop_slot]));

	// SD TODO - Check wether maybe remaining buffers have to be detached from source
	current_buffer[loop_slot] = 0;
	current_buffer_unqueue[loop_slot] = 0;

	// has been reserved in createSourceWithPosition, need to delete here
	delete[] loop_buffers_audio_engine[loop_slot];

	loop_ids[loop_slot] = -1;
	active_loops--;
	
	printf("%d, Done - %d - %d \n", loop_slot, active_loops, loop_source);

	pthread_mutex_unlock(&audio_engine_mutex);
	
#elif OPENAL_STATIC_MODE
	
	loop_ids[loop_slot] = -1;
	active_loops--;
	
	// Stop source play
	alSourceStop(loop_source);
	// Detach buffer from source
	alSourcei(loop_source, AL_BUFFER, 0);
	
#endif
	
return 0;
}

int ACAudioFeedback::deleteExtSource()
{
	stopExtSource();
	if (alGetError() != AL_NO_ERROR){
		std::cerr << "createExtSource, openAL error : " << alGetError() << std::endl;
		exit(1);
	}	
	alDeleteSources(1, &ext_loop_source);
	if (alGetError() != AL_NO_ERROR){
		std::cerr << "createExtSource, openAL error : " << alGetError() << std::endl;
		exit(1);
	}
	alDeleteBuffers(1, &ext_loop_buffer);
	if (alGetError() != AL_NO_ERROR){
		std::cerr << "createExtSource, openAL error : " << alGetError() << std::endl;
		exit(1);
	}
	ext_loop_length = 0;
	return 0;
}

void ACAudioFeedback::loopExtSource()
{
	if (ext_loop_length > 0)
		alSourcePlay(ext_loop_source);
	if (alGetError() != AL_NO_ERROR){
		std::cerr << "createExtSource, openAL error : " << alGetError() << std::endl;
		exit(1);
	}	

}	

void ACAudioFeedback::stopExtSource()
{
	if (ext_loop_length > 0)
		alSourceStop(ext_loop_source);
	if (alGetError() != AL_NO_ERROR){
		std::cerr << "createExtSource, openAL error : " << alGetError() << std::endl;
		exit(1);
	}	
	
}	

int ACAudioFeedback::setSourcePosition(int loop_id, float x, float y, float z)
{
	int loop_slot;	
	ALuint	loop_source;
	loop_slot = getLoopSlot(loop_id);
	if (loop_slot==-1) {
		return 1;
	}
	mSourcePos[loop_slot][0] = x;
	mSourcePos[loop_slot][1] = y;
	mSourcePos[loop_slot][2] = z;
	loop_source = loop_sources[loop_slot];
	alSourcefv(loop_source, AL_POSITION, mSourcePos[loop_slot]);
	return 0;
}

int ACAudioFeedback::setSourcePitch(int loop_id, float pitch)
{
	if (pitch > 0.5 && pitch < 2.0) //due to AL_PITCH
	{
		int loop_slot;	
		ALuint	loop_source;
		loop_slot = getLoopSlot(loop_id);
		if (loop_slot==-1) {
			return 1;
		}
		loop_source = loop_sources[loop_slot];
		alSourcef(loop_source, AL_PITCH, pitch);//CF find another equivalent that accepts a pitch lower than 0.5
		return 0;
	}
	else
		return 1;
}

int ACAudioFeedback::setSourceGain(int loop_id, float gain)
{
	int loop_slot;	
	ALuint	loop_source;
	loop_slot = getLoopSlot(loop_id);
	if (loop_slot==-1) {
		return 1;
	}
	loop_source = loop_sources[loop_slot];
	alSourcef(loop_source, AL_GAIN, gain);
	return 0;
}

int ACAudioFeedback::setSourceRolloffFactor(int loop_id, float rolloff_factor)
{
	int loop_slot;	
	ALuint	loop_source;
	loop_slot = getLoopSlot(loop_id);
	if (loop_slot==-1) {
		return 1;
	}
	loop_source = loop_sources[loop_slot];
	alSourcef(loop_source, AL_ROLLOFF_FACTOR, rolloff_factor);
	return 0;
}

int ACAudioFeedback::setSourceReferenceDistance(int loop_id, float reference_distance)
{
	int loop_slot;	
	ALuint	loop_source;
	loop_slot = getLoopSlot(loop_id);
	if (loop_slot==-1) {
		return 1;
	}
	loop_source = loop_sources[loop_slot];
	alSourcef(loop_source, AL_REFERENCE_DISTANCE, reference_distance);
	return 0;
}

int ACAudioFeedback::setSourceMaxDistance(int loop_id, float max_distance)
{
	int loop_slot;	
	ALuint	loop_source;
	loop_slot = getLoopSlot(loop_id);
	if (loop_slot==-1) {
		return 1;
	}
	loop_source = loop_sources[loop_slot];
	alSourcef(loop_source, AL_MAX_DISTANCE, max_distance);
	return 0;
}

int ACAudioFeedback::setSourceVelocity(int loop_id, float velocity)
{
	// SD TODO
	return 0;
}

