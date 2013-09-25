/**
 * @brief A plugin that provides an audio engine based on STK and RtAudio.
 * @author Christian Frisson
 * @date 10/11/2012
 * @copyright (c) 2012 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
 */

#ifndef ACAudioStkEngineRendererPlugin_H
#define	ACAudioStkEngineRendererPlugin_H

#include <MediaCycle.h>
#include <ACPlugin.h>
#include <ACPluginQt.h>
//#include <QtGui>

#include <RtAudio.h>

#ifdef __APPLE__
#define __OS_MACOSX__
#elif defined (UNIX)
#define __OS_LINUX__
#elif defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
#define __OS_WINDOWS__
#endif
#include <Mutex.h>

#include <ACAudioStkFileWvIn.h>
#include <ACAudioStkFileLoop.h>
#include <ACAudioStkGranulate.h>

#include "ACAudioStkFreeVerb.h"
#include "RtMidi.h"

class ACAudioStkEngineRendererPlugin : public QObject, public ACPluginQt, public ACMediaRendererPlugin
{
    Q_OBJECT
    Q_INTERFACES(ACPluginQt)
public:
    ACAudioStkEngineRendererPlugin();
    virtual ~ACAudioStkEngineRendererPlugin();
    virtual std::map<std::string, ACMediaType> getSupportedExtensions(ACMediaType media_type = MEDIA_TYPE_ALL);
    virtual bool performActionOnMedia(std::string action, long int mediaId, std::vector<boost::any> arguments=std::vector<boost::any>());
    virtual std::map<std::string,ACMediaType> availableMediaActions();
    virtual std::map<std::string,ACMediaActionParameters> mediaActionsParameters();
    virtual void disable(){active = false; this->muteAll();}
    virtual void enable(){active = true;}

protected:
    void useGranulation();
    void useMotorizedFaders();

public slots:
    void muteAll();

public:
    void updateMasterVolume();
    void updatePlaybackSpeed();

//    void updateGrainVoices();
//    void updateGrainRandomness();
//    void updateGrainStretch();
//    void updateGrainParameters();

    //! Update the reverb effect mix [0 = mostly dry, 1 = mostly wet].
    void updateReverbEffectMix();

    //! Update the reverb room size (comb filter feedback gain) parameter [0,1].
    void updateReverbRoomSize();

    //! Update the reverb damping parameter [0=low damping, 1=higher damping].
    void updateReverbDamping();

    //! Update the mode [frozen = 1, unfrozen = 0].
    void updateReverbFreeze();

    //! Update the width (left-right mixing) parameter [0,1].
    void updateReverbPan();

    void updatePlaybackPan();
    void updatePlaybackVolume();
    void updateActiveTarget();
    void updateFaderEffect(int fader);
    void muteMedia(int mediaId);
    virtual int outputChannels(){return 2;}
protected:
    int availableChannels();

private:
    int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
              double streamTime, RtAudioStreamStatus status);
    static int tickWrapper(void *outputBuffer, void *inputBuffer, unsigned int nFrames,
                           double streamTime, RtAudioStreamStatus status, void *userData);

    void midiInCallback( double timeStamp, std::vector<unsigned char> *message);
    static void midiInCallbackWrapper( double timeStamp, std::vector<unsigned char> *message, void *userData);
    static void errorCallback( RtError::Type type, const std::string &errorText );

protected:
    bool openStream(unsigned int& bufferFrames);
    bool startStream();

protected:
    RtAudio* dac;
    stk::Mutex* mutex;

    std::vector<int> input_ids;// -2 input being closed, -1 slot available, >=0 media id
    std::vector<ACAudioStkFileWvIn> inputs;
    std::vector<stk::StkFloat> input_pans;
    std::vector<stk::StkFloat> input_gains;
    std::vector<int> input_current_frames;
    std::vector<stk::StkFrames*> input_frames;

    std::vector<int> loop_ids; // -2 loop being closed, -1 slot available, >=0 media id
    std::vector<ACAudioStkFileLoop*> loops;
    std::vector<stk::StkFloat> loop_pans;
    std::vector<stk::StkFloat> loop_gains;
    std::vector<int> loop_current_frames;
    std::vector<stk::StkFrames*> loop_frames;

    std::vector<int> grain_ids;// -2 grain being closed, -1 slot available, >=0 media id
    std::vector<ACAudioStkGranulate> grains;
    std::vector<stk::StkFloat> grain_pans;
    std::vector<stk::StkFloat> grain_gains;
    std::vector<int> grain_current_frames;
    std::vector<stk::StkFrames*> grain_frames;

    //std::vector<ACAudioStkFreeVerb> frevs;
    ACAudioStkFreeVerb* master_frev;

    stk::StkFloat master_volume;

    void justReadFrames(long int mediaId, int nFrames);
    void stopSource(long int mediaId);

    RtMidiIn* midi_in;
    RtMidiOut* midi_out;

    std::vector<std::string> fader_effect;

public:
    bool createGenerator(std::string action, long int mediaId, std::vector<boost::any> arguments=std::vector<boost::any>());


public:
    virtual std::vector<ACInputActionQt*> providesInputActions();

public slots:
//    void playClickedNode();
    void loopClickedNode();
//    void granulateClickedNode();
    void muteAllNodes();
//    void triggerClickedNode();

protected:
//    ACInputActionQt* playClickedNodeAction;
    ACInputActionQt* loopClickedNodeAction;
//    ACInputActionQt* granulateClickedNodeAction;
    ACInputActionQt* muteAllNodesAction;
//    ACInputActionQt* triggerClickedNodeAction;

    std::map<std::string,ACMediaActionParameters> action_parameters;
    std::vector<std::string> active_targets;
    std::string active_target;
    std::vector<std::string> midi_in_ports,midi_out_ports;
    long int current_closest_node;
    bool muting;
    bool with_granulation;
    bool with_faders;
    int active_sources;
    bool active;
    //long int sourcesGenerated;
};

#endif
