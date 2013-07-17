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

#include "ACAudioStkEngineRendererPlugin.h"
#include<MediaCycle.h>

#include <signal.h>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace stk;

ACAudioStkEngineRendererPlugin::ACAudioStkEngineRendererPlugin() : QObject(), ACPluginQt(), ACMediaRendererPlugin(){
    this->mName = "Audio Engine (STK)";
    this->mDescription ="Plugin for playing audio files with The Synthesis ToolKit in C++ (STK)";
    this->mMediaType = MEDIA_TYPE_AUDIO;
    this->master_frev = 0;

    this->with_granulation = false;
    this->with_faders = false;

    current_closest_node = -1;

    // Set the global sample rate before creating class instances.
    Stk::setSampleRate( (StkFloat) 44100 );

    this->action_parameters["play"] = ACMediaActionParameters();
    this->action_parameters["loop"] = ACMediaActionParameters();
    this->action_parameters["granulate"] = ACMediaActionParameters();
    this->action_parameters["mute all"] = ACMediaActionParameters();
    this->action_parameters["trigger"] = ACMediaActionParameters();

    // Only for the GUI sliders
    this->active_target = "All nodes";
    this->active_targets.push_back("All nodes");
    this->active_targets.push_back("Closest node");
    //this->addStringParameter("Effect target",active_target,active_targets,"Choose which node(s) to change effect parameters on",boost::bind(&ACAudioStkEngineRendererPlugin::updateActiveTarget,this));

    // Master/playback effects

    this->addCallback("Mute","Mute",boost::bind(&ACAudioStkEngineRendererPlugin::muteAll,this));
    this->action_parameters["mute"] = ACMediaActionParameters();

    this->master_volume = 1.0f;
    this->addNumberParameter("Master Volume",this->master_volume,0,1,0.01,"Master Volume",boost::bind(&ACAudioStkEngineRendererPlugin::updateMasterVolume,this));
    this->action_parameters["master volume"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Master Volume")));

    this->addNumberParameter("Playback Volume",1.0,0.0,1.0,0.01,"Playback Volume",boost::bind(&ACAudioStkEngineRendererPlugin::updatePlaybackVolume,this));
    this->action_parameters["playback volume"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Playback Volume")));

    this->addNumberParameter("Playback Speed",1.0,-1.0,1.0,0.01,"Playback Speed (only for play and loop, not granulate)",boost::bind(&ACAudioStkEngineRendererPlugin::updatePlaybackSpeed,this));
    this->action_parameters["playback speed"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Playback Speed")));

    this->addNumberParameter("Playback Pan",0.0,-1.0,1.0,0.01,"Playback Pan (-1 left, 0 center, 1 right)",boost::bind(&ACAudioStkEngineRendererPlugin::updatePlaybackPan,this));
    this->action_parameters["playback pan"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Playback Pan")));

    // Reverb

    master_frev = new ACAudioStkFreeVerb();

    //! Reverb mix [0 = mostly dry, 1 = mostly wet].
    this->addNumberParameter("Reverb Mix",0,0,1,0.01,"Reverb Mix (0 = mostly dry, 1 = mostly wet)",boost::bind(&ACAudioStkEngineRendererPlugin::updateReverbEffectMix,this));
    //if(!master_frev)
    this->action_parameters["reverb mix"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Reverb Mix")));
    if(master_frev)
        master_frev->setEffectMix( this->getNumberParameterValue("Reverb Mix") );

    //! Reverb room size (comb filter feedback gain) parameter [0,1].
    this->addNumberParameter("Reverb Room Size",1,0,1,0.01,"Reverb room size (comb filter feedback gain) parameter [0,1]",boost::bind(&ACAudioStkEngineRendererPlugin::updateReverbRoomSize,this));
    //if(!master_frev)
    this->action_parameters["reverb room size"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Reverb Room Size")));

    //! Reverb damping parameter [0=low damping, 1=higher damping].
    this->addNumberParameter("Reverb Damping",1,0,1,0.01,"Reverb damping parameter (0=low damping, 1=higher damping)",boost::bind(&ACAudioStkEngineRendererPlugin::updateReverbDamping,this));
    //if(!master_frev)
    this->action_parameters["reverb damping"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Reverb Damping")));

    //! Reverb mode [frozen = 1, unfrozen = 0].
    this->addNumberParameter("Reverb Freeze",0,0,1,1,"Reverb freeze (frozen = 1, unfrozen = 0)",boost::bind(&ACAudioStkEngineRendererPlugin::updateReverbFreeze,this));
    //if(!master_frev)
    this->action_parameters["reverb freeze"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Reverb Freeze")));

    //! Reverb width (left-right mixing) parameter [0,1].
    //this->addNumberParameter("Reverb Pan",1,0,1,0.01,"Reverb width (left-right mixing) parameter [0,1]",boost::bind(&ACAudioStkEngineRendererPlugin::updateReverbPan,this));
    //if(!master_frev)
    //this->action_parameters["reverb pan"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Reverb Pan")));



    //midi_in->openVirtualPort("MashtaCycle In");
    //midi_out->openVirtualPort("MashtaCycle Out");

    loopClickedNodeAction = new ACInputActionQt(tr("Loop clicked node"), this);
    loopClickedNodeAction->setShortcut(Qt::Key_L);
    loopClickedNodeAction->setKeyEventType(QEvent::KeyPress);
    loopClickedNodeAction->setMouseEventType(QEvent::MouseButtonRelease);
    loopClickedNodeAction->setToolTip(tr("Loop the clicked node"));
    connect(loopClickedNodeAction, SIGNAL(triggered()), this, SLOT(loopClickedNode()));

    playClickedNodeAction = new ACInputActionQt(tr("Play clicked node"), this);
    //playClickedNodeAction->setShortcut(Qt::Key_P);
    //playClickedNodeAction->setKeyEventType(QEvent::KeyPress);
    playClickedNodeAction->setMouseEventType(QEvent::MouseButtonRelease);
    playClickedNodeAction->setToolTip(tr("Play the clicked node"));
    connect(playClickedNodeAction, SIGNAL(triggered()), this, SLOT(playClickedNode()));

    triggerClickedNodeAction = new ACInputActionQt(tr("Trigger clicked node"), this);
    triggerClickedNodeAction->setShortcut(Qt::Key_T);
    triggerClickedNodeAction->setKeyEventType(QEvent::KeyPress);
    triggerClickedNodeAction->setMouseEventType(QEvent::MouseButtonRelease);
    triggerClickedNodeAction->setToolTip(tr("Trigger the clicked node"));
    connect(triggerClickedNodeAction, SIGNAL(triggered()), this, SLOT(triggerClickedNode()));

    muteAllNodesAction = new ACInputActionQt(tr("Mute all"), this);
    muteAllNodesAction->setShortcut(Qt::Key_M);
    muteAllNodesAction->setKeyEventType(QEvent::KeyPress);
    //muteAllNodesAction->setMouseEventType(QEvent::MouseButtonRelease);
    muteAllNodesAction->setToolTip(tr("Mute all"));
    connect(muteAllNodesAction, SIGNAL(triggered()), this, SLOT(muteAllNodes()));

    muting = false;
}

void ACAudioStkEngineRendererPlugin::useGranulation(){

    // make this function callable once

    this->with_granulation = true;

    //Granulation

    this->addNumberParameter("Grain Voices",1,0,25,1,"Grain Voices",boost::bind(&ACAudioStkEngineRendererPlugin::updateGrainVoices,this));
    this->action_parameters["grain voices"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Grain Voices")));

    // factor between 0 - 1.0
    this->addNumberParameter("Grain Randomness",0,0,1,0.1,"Grain Randomness",boost::bind(&ACAudioStkEngineRendererPlugin::updateGrainRandomness,this));
    this->action_parameters["grain randomness"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Grain Randomness")));

    // factor (1-1000)
    this->addNumberParameter("Grain Stretch",1,1,1000,1,"Grain Stretch",boost::bind(&ACAudioStkEngineRendererPlugin::updateGrainStretch,this));
    this->action_parameters["grain stretch"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Grain Stretch")));

    //duration (ms)
    this->addNumberParameter("Grain Duration",1000,0,10000,1,"Grain Duration (ms)",boost::bind(&ACAudioStkEngineRendererPlugin::updateGrainParameters,this));
    this->action_parameters["grain duration"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Grain Duration")));

    //ramp: envelope percent (0-100)
    this->addNumberParameter("Grain Ramp",100,0,100,1,"Grain Ramp (envelope percent)",boost::bind(&ACAudioStkEngineRendererPlugin::updateGrainParameters,this));
    this->action_parameters["grain ramp"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Grain Ramp")));

    //offset: hop time between grains (ms)
    this->addNumberParameter("Grain Offset",100,0,1000,1,"Grain Offset (hop time between grains in ms)",boost::bind(&ACAudioStkEngineRendererPlugin::updateGrainParameters,this));
    this->action_parameters["grain offset"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Grain Offset")));

    //delay: pause time between grains (ms)
    this->addNumberParameter("Grain Delay",100,0,10000,1,"Grain Delay (pause time between grains in ms)",boost::bind(&ACAudioStkEngineRendererPlugin::updateGrainParameters,this));
    this->action_parameters["grain delay"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Grain Delay")));

    granulateClickedNodeAction = new ACInputActionQt(tr("Granulate clicked node"), this);
    granulateClickedNodeAction->setShortcut(Qt::Key_G);
    granulateClickedNodeAction->setKeyEventType(QEvent::KeyPress);
    granulateClickedNodeAction->setMouseEventType(QEvent::MouseButtonRelease);
    granulateClickedNodeAction->setToolTip(tr("Granulate the clicked node"));
    connect(granulateClickedNodeAction, SIGNAL(triggered()), this, SLOT(granulateClickedNode()));
}

void ACAudioStkEngineRendererPlugin::useMotorizedFaders(){

    // make this function callable once

    this->with_faders = true;

    // to replace by a callback probing for the device

    // Behringer BCF 2000 motorized faders assignation

    fader_effect.push_back("reverb mix");
    fader_effect.push_back("reverb room size");
    fader_effect.push_back("reverb damping");
    fader_effect.push_back("grain voices");
    fader_effect.push_back("playback speed");
    fader_effect.push_back("playback pan");
    fader_effect.push_back("playback volume");
    fader_effect.push_back("master volume");

    this->midi_in = new RtMidiIn();
    int in_port_count = this->midi_in->getPortCount();
    bool in_port_found = false;
    midi_in_ports.push_back("None");
    for (int port = 0; port < in_port_count; port++){
        std::cout << "ACAudioStkEngineRendererPlugin::ACAudioStkEngineRendererPlugin(): midi in port " << port << " name " << this->midi_in->getPortName(port) << " is available " << std::endl;
        midi_in_ports.push_back(midi_in->getPortName(port));
        if(midi_in->getPortName(port) == "BCF2000 Port 1"){
            midi_in->openPort(port);
            midi_in->setCallback(&ACAudioStkEngineRendererPlugin::midiInCallbackWrapper,this);
            in_port_found = true;
        }
    }
    if(!in_port_found){
        delete midi_in;
        midi_in = 0;
    }

    this->midi_out = new RtMidiOut();
    int out_port_count = this->midi_out->getPortCount();
    bool out_port_found = false;
    midi_out_ports.push_back("None");
    for (int port = 0; port < out_port_count; port++){
        std::cout << "ACAudioStkEngineRendererPlugin::ACAudioStkEngineRendererPlugin(): midi out port " << port << " name " << this->midi_out->getPortName(port) << " is available " << std::endl;
        midi_out_ports.push_back(midi_out->getPortName(port));
        if(midi_out->getPortName(port) == "BCF2000 Port 1"){
            midi_out->openPort(port);
            out_port_found = true;
        }
    }
    if(!out_port_found){
        delete midi_out;
        midi_out = 0;
    }


    if(with_faders){
        std::vector<std::string> available_effects;
        for(std::map<std::string,ACMediaActionParameters>::iterator action = action_parameters.begin(); action != action_parameters.end(); action++){
            if(action->second.size()>0){
                std::cout << "Adding action " << action->first << std::endl;
                available_effects.push_back(action->first);
            }

        }
        for(int fader = 1; fader<=8; fader++){
            std::stringstream _fader;
            _fader << fader;
            this->addStringParameter("Fader " + _fader.str(),fader_effect[fader-1],available_effects,"Assign fader " + _fader.str() + " to an effect parameter " ,boost::bind(&ACAudioStkEngineRendererPlugin::updateFaderEffect,this,fader));
        }
    }

    pthread_mutexattr_init(&delete_mutex_attr);
    pthread_mutex_init(&delete_mutex, &delete_mutex_attr);
    pthread_mutexattr_destroy(&delete_mutex_attr);
}

ACAudioStkEngineRendererPlugin::~ACAudioStkEngineRendererPlugin(){

    if(dac.isStreamRunning())
        try {
        dac.abortStream();
    }
    catch ( RtError &error ) {
        error.printMessage();
        //return false;
    }
    if(dac.isStreamOpen()){
        try {
            dac.closeStream();
        }
        catch ( RtError &error ) {
            error.printMessage();
            //return false;
        }
    }
    for(std::map< long int, stk::StkFrames*>::iterator frame = frames.begin(); frame != frames.end(); frame++)
        delete frame->second;
    frames.clear();

    if(master_frev)
        delete master_frev;
    master_frev = 0;

    action_parameters.clear();

    if(midi_in)
        delete midi_in;
    midi_in = 0;

    pthread_mutex_destroy(&delete_mutex);
}

void ACAudioStkEngineRendererPlugin::justReadFrames(long int mediaId, int nFrames){

    if(media_cycle){
        std::map< long int, ACAudioStkFileLoop*>::iterator loop = loops.find(mediaId);
        if( loop != loops.end()){
            current_frames[mediaId] += nFrames /* * loop->second->channelsOut()*/;
            //std::cout << "ACAudioStkEngineRendererPlugin: looping, current frame " << current_frames[mediaId] << " of media " << mediaId << std::endl;
            if(current_frames[mediaId] > loop->second->getSize() /* * loop->second->channelsOut()*/)
                current_frames[mediaId] -= loop->second->getSize() /* * loop->second->channelsOut()*/;
            if(current_frames[mediaId] < 0)
                current_frames[mediaId] += loop->second->getSize() /* * loop->second->channelsOut()*/;
            media_cycle->setCurrentFrame(mediaId, current_frames[mediaId]);
            //std::cout << "ACAudioStkEngineRendererPlugin::justReadFrames " << mediaId << " " << current_frames[mediaId] << "(" << loop->second->channelsOut() <<" channels)"<< std::endl;
        }

        //else
        //    std::cout << "ACAudioStkEngineRendererPlugin: playing frame " << current_frames[mediaId] << " of media " << mediaId << std::endl;
    }
}

void ACAudioStkEngineRendererPlugin::stopSource(long int mediaId){
    if(gains.find(mediaId) != gains.end()){
        gains[mediaId] = 0.0f;
    }
    if(playback_types.find(mediaId) != playback_types.end()){
        playback_types[mediaId] = PLAYBACK_MUTE;
    }
    if(inputs.find(mediaId) != inputs.end()){
        if(inputs[mediaId]){
            //if(inputs[mediaId]->isOpen())
            inputs[mediaId]->closeFile();
        }
        std::cout << "ACAudioStkEngineRendererPlugin::stopSource: stop playing media " << mediaId << std::endl;
        return;
    }
    if(loops.find(mediaId) != loops.end()){
        if(loops[mediaId]){
            //if(loops[mediaId]->isOpen())
            loops[mediaId]->closeFile();
        }
        std::cout << "ACAudioStkEngineRendererPlugin::stopSource: stop looping media " << mediaId << std::endl;
        return;
    }
    if(grains.find(mediaId) != grains.end()){
        if(grains[mediaId]){
            //if(grains[mediaId]->isOpen())
            grains[mediaId]->closeFile();
        }
        std::cout << "ACAudioStkEngineRendererPlugin::stopSource: stop granularizing media " << mediaId << std::endl;
        return;
    }
}

void ACAudioStkEngineRendererPlugin::deleteSource(long int mediaId){
    if(playback_types.find(mediaId) != playback_types.end()){
        playback_types[mediaId] = PLAYBACK_OFF;
    }
    if(inputs.find(mediaId) != inputs.end()){
        if(inputs[mediaId]){
            //if(inputs[mediaId]->isOpen())
            inputs[mediaId]->closeFile();
            delete inputs[mediaId];
        }
        inputs.erase(mediaId);
        current_frames[mediaId] = 0;
        if(frames[mediaId])
            delete frames[mediaId];
        frames.erase(mediaId);
        std::cout << "ACAudioStkEngineRendererPlugin::deleteSource: stop playing media " << mediaId << std::endl;
        if(media_cycle)
            media_cycle->getBrowser()->toggleSourceActivity(mediaId,0);
    }
    if(loops.find(mediaId) != loops.end()){
        if(loops[mediaId]){
            //if(loops[mediaId]->isOpen())
            loops[mediaId]->closeFile();
            delete loops[mediaId];
        }
        loops.erase(mediaId);
        current_frames[mediaId] = 0;
        if(frames[mediaId])
            delete frames[mediaId];
        frames.erase(mediaId);
        std::cout << "ACAudioStkEngineRendererPlugin::deleteSource: stop looping media " << mediaId << std::endl;
        if(media_cycle)
            media_cycle->getBrowser()->toggleSourceActivity(mediaId,0);
    }
    if(grains.find(mediaId) != grains.end()){
        if(grains[mediaId]){
            //if(grains[mediaId]->isOpen())
            grains[mediaId]->closeFile();
            delete grains[mediaId];
        }
        grains.erase(mediaId);
        std::cout << "ACAudioStkEngineRendererPlugin::deleteSource: stop granularizing media " << mediaId << std::endl;
        if(media_cycle)
            media_cycle->getBrowser()->toggleSourceActivity(mediaId,0);
    }
    if(frevs.find(mediaId) != frevs.end()){
        //delete frevs[mediaId];
        frevs.erase(mediaId);
    }
    if(gains.find(mediaId) != gains.end()){
        gains.erase(mediaId);
    }
    if(pans.find(mediaId) != pans.end()){
        pans.erase(mediaId);
    }
}

int ACAudioStkEngineRendererPlugin::tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                                          double streamTime, RtAudioStreamStatus status )
{
    if(status == RTAUDIO_INPUT_OVERFLOW)
        std::cerr << "ACAudioStkEngineRendererPlugin:tick input overflow" << std::endl;
    if(status == RTAUDIO_OUTPUT_UNDERFLOW)
        std::cerr << "ACAudioStkEngineRendererPlugin:tick output underflow" << std::endl;

    //pthread_mutex_lock(&(this->delete_mutex));

    register StkFloat *samples = (StkFloat *) outputBuffer;
    //if(outputBuffer == 0){
    //    std::cerr << "ACAudioStkEngineRenderer: output buffer not available" << std::endl;
    //    return 0;
    //}

    if(inputs.size()==0 && loops.size()==0 && grains.size()==0){
        *samples++ = 0;
        pthread_mutex_unlock(&(this->delete_mutex));
        return 1;
    }

    double rate = 1.0;
    rate = (float)this->getNumberParameterValue("Playback Speed");
    int outputChannels = this->outputChannels();

    for(std::map <long int, ACAudioStkFileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
        if(input->second){
            if (this->frames[input->first]){
                input->second->tick( *(this->frames[input->first]) );
                this->justReadFrames(input->first, (int)(rate * input->second->channelsOut() * nBufferFrames));
            }
            else{
                this->stopSource(input->first);
            }
        }
    }

    std::cout << "ACAudioStkEngineRenderer: number of mapped loops "<< loops.size() << std::endl;

    for(std::map <long int, ACAudioStkFileLoop*>::iterator loop = loops.begin(); loop != loops.end(); loop++){
        if(loop->second){
            if(this->frames[loop->first]){
                loop->second->tick( *(this->frames[loop->first]) );
                this->justReadFrames(loop->first, (int)(rate * loop->second->channelsOut() * nBufferFrames));
            }
            else if(loop->second->isFinished()){
                this->deleteSource(loop->first);
            }
        }
        else{
            this->deleteSource(loop->first);
        }

    }

    std::map <long int, const StkFrames*> lastFrames;
    for(std::map <long int, ACAudioStkGranulate*>::iterator grain = grains.begin(); grain != grains.end(); grain++){
        if(grain->second){
            lastFrames[grain->first] = &(grain->second->lastFrame());
        }
    }

    if( this->frames.size()>0 || grains.size()>0 ){
        //        std::cout << "Frames /" << nBufferFrames <<":";
        //        int in = 0;
        //        for(std::map <long int, FileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
        //            std::cout << " " << in++ << " " <<  this->frames[input->first]->size();
        //        }
        //        std::cout << std::endl;
        for ( unsigned int i=0; i<nBufferFrames; i++ ){
            StkFloat frame[outputChannels];
            for ( unsigned int c=0; c<outputChannels; c++ ){
                frame[c] = 0;
            }

            int count = 0;

            for(std::map <long int, ACAudioStkGranulate*>::iterator grain = grains.begin(); grain != grains.end(); grain++){
                if(grain->second){
                    grain->second->tick();
                    if(lastFrames[grain->first]){
                        unsigned int nChannels = lastFrames[grain->first]->channels();
                        if(frevs.find(grain->first) != frevs.end()){
                            for ( unsigned int c=0; c<outputChannels; c++ ){
                                StkFloat pan = 0.5f*(1 + (c==0?-1:1)* pans[grain->first]);
                                if(nChannels!=0)
                                    frame[c] += frevs[grain->first]->tick(  (*(lastFrames[grain->first]))[c%nChannels] ) * gains[grain->first] * pan;
                            }
                        }
                        else{
                            for ( unsigned int c=0; c<outputChannels; c++ ){
                                StkFloat pan = 0.5f*(1 + (c==0?-1:1)* pans[grain->first]);
                                if(nChannels!=0)
                                    frame[c] += (*(lastFrames[grain->first]))[c%nChannels]  * gains[grain->first] * pan;
                            }
                        }
                    }
                }
            }

            for(std::map <long int, ACAudioStkFileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
                if( this->frames[input->first]!=0 && this->frames[input->first]->size()>i){
                    int fileChannels = input->second->channelsOut();
                    if(frevs.find(input->first) != frevs.end()){
                        for ( unsigned int c=0; c<outputChannels; c++ ){
                            StkFloat pan = 0.5f*(1 + (c==0?-1:1)* pans[input->first]);
                            if(fileChannels!=0)
                                frame[c] += frevs[input->first]->tick( (*this->frames[input->first])[i*fileChannels+(c%fileChannels)] ) * gains[input->first] * pan;
                        }
                    }
                    else{
                        for ( unsigned int c=0; c<outputChannels; c++ ){
                            StkFloat pan = 0.5f*(1 + (c==0?-1:1)* pans[input->first]);
                            if(fileChannels!=0)
                                frame[c] += (*this->frames[input->first])[i*fileChannels+(c%fileChannels)] * gains[input->first] * pan;
                        }
                    }
                    count++;
                }
            }
            for(std::map <long int, ACAudioStkFileLoop*>::iterator loop = loops.begin(); loop != loops.end(); loop++){
                if( this->frames[loop->first]!=0 && this->frames[loop->first]->size()>i){
                    int fileChannels = loop->second->channelsOut();
                    if(frevs.find(loop->first) != frevs.end()){
                        for ( unsigned int c=0; c<outputChannels; c++ ){
                            StkFloat pan = 0.5f*(1 + (c==0?-1:1)* pans[loop->first]);
                            if(fileChannels!=0)
                                frame[c] += frevs[loop->first]->tick( (*this->frames[loop->first])[i*fileChannels+(c%fileChannels)] ) * gains[loop->first] * pan;
                        }
                    }
                    else{
                        for ( unsigned int c=0; c<outputChannels; c++ ){
                            StkFloat pan = 0.5f*(1 + (c==0?-1:1)* pans[loop->first]);
                            if(fileChannels!=0)
                                frame[c] += (*this->frames[loop->first])[i*fileChannels+(c%fileChannels)] * gains[loop->first] * pan;
                        }
                    }

                    count++;
                }
            }
            if(master_frev){
                for ( unsigned int c=0; c<outputChannels; c++ ){
                    *samples++ = master_frev->tick(frame[c]) * master_volume; // / count;
                }
            }
            else{
                for ( unsigned int c=0; c<outputChannels; c++ ){
                    *samples++ = frame[c] * master_volume; // / count;
                }
            }
        }
    }

    /*bool finished = true;
    for(std::map<long int, ACAudioStkFileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
        finished = (input->second->isFinished()) && finished;
        if(input->second->isFinished()){
            this->deleteSource(input->first);
        }

    }
    for(std::map<long int, ACAudioStkGranulate*>::iterator grain = grains.begin(); grain != grains.end(); grain++){
        finished = (grain->second->isFinished()) && finished;
        if(grain->second->isFinished()){
            this->deleteSource(grain->first);
        }
    }

    for(std::map<long int, ACAudioStkFileLoop*>::iterator loop = loops.begin(); loop != loops.end(); loop++){
        finished = (loop->second->isFinished()) && finished;
        if(loop->second->isFinished()){
            this->deleteSource(loop->first);
            std::cout << "ACAudioStkEngineRendererPlugin::tick: deleted source " << loop->first << std::endl;
        }
    }*/

    //pthread_mutex_unlock(&(this->delete_mutex));
    return 0;
}

int ACAudioStkEngineRendererPlugin::tickWrapper(void *outputBuffer, void *inputBuffer,
                                                unsigned int nFrames, double streamTime,
                                                RtAudioStreamStatus status, void *userData)
{
    return static_cast<ACAudioStkEngineRendererPlugin*>(userData)->tick(outputBuffer,inputBuffer,
                                                                        nFrames,
                                                                        streamTime, status);
}

void ACAudioStkEngineRendererPlugin::midiInCallback( double timeStamp, std::vector<unsigned char> *message){
    /*std::cout << "Time " << timeStamp;//<< std::endl;
    for(std::vector<unsigned char>::iterator word = message->begin(); word != message->end(); word++){
        std::cout << " '" << (int)(*word) << "'";
    }
    std::cout << std::endl;*/
    if(!message)
        return;
    if(message->size()!=3)
        return;

    int typechan = (int)((*message)[0]);
    int number = (int)((*message)[1]);
    int value = (int)((*message)[2]);
    std::string type("");
    int chan = 0;

    if(typechan >= 176 && typechan < 192){
        type = "control change";
        chan = typechan - 176;
    }

    std::cout << "ACAudioStkEngineRendererPlugin midi in @ " << timeStamp << " type " << type << /*" typechan " << typechan <<*/ " channel " << chan << " number " << number << " value " << value << std::endl;

    //std::vector<std::string> fader_effect = this->fader_effect;
    //MediaCycle* media_cycle = this->getMediaCycle();
    if(!media_cycle){
        std::cerr << "ACAudioStkEngineRendererPlugin: midi in: can't access media cycle" << std::endl;
        return;
    }
    int mediaId = media_cycle->getClosestNode(); // warning, mono-pointer

    // The following is specific to the BFC2000
    if(fader_effect.size()!=8){
        std::cerr << "ACAudioStkEngineRendererPlugin: wrong BFC2000 mapping, size doesn't match" << std::endl;
        return;
    }
    if(number >= 81 && number <= 88){
        int fader = number - 81;
        std::string parameter = fader_effect[fader];
        float min(0.0f), max(1.0f);
        /*if(parameter == "master volume"){
            value = this->master_volume;
        }
        else{*/
        min = this->getNumberParameterMin(parameter);
        max = this->getNumberParameterMax(parameter);
        //}
        std::cout << "ACAudioStkEngineRendererPlugin::midiInCallback: " << parameter << " on media Id " << mediaId << " value " << min + (float)(value)/127.0f * (max-min) << std::endl;
        media_cycle->performActionOnMedia(parameter,mediaId,min + (float)(value)/127.0f * (max-min)); // through media cycle for logging
    }
}

void ACAudioStkEngineRendererPlugin::midiInCallbackWrapper( double timeStamp, std::vector<unsigned char> *message, void *userData){
    return static_cast<ACAudioStkEngineRendererPlugin*>(userData)->midiInCallback( timeStamp, message);
}

void ACAudioStkEngineRendererPlugin::muteAll(){
    //pthread_mutex_lock(&delete_mutex);

    if(!muting){
        muting = true;

        if(dac.isStreamRunning())
            try {
            dac.abortStream();
        }
        catch ( RtError &error ) {
            error.printMessage();
            //return false;
        }
        if(dac.isStreamOpen()){
            try {
                dac.closeStream();
            }
            catch ( RtError &error ) {
                error.printMessage();
                //return false;
            }
        }

        for(std::map< long int, ACAudioStkPlaybackType>::iterator playback_type = playback_types.begin(); playback_type != playback_types.end(); playback_type++){
            playback_type->second = PLAYBACK_OFF;
        }

        for(std::map <long int, ACAudioStkFileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
            long int mediaId = input->first;
            if(input->second){
                //if(inputs[mediaId]->isOpen())
                //input->second->closeFile();
                //CF crash: delete input->second;
            }
            inputs.erase(mediaId);
            current_frames[mediaId] = 0;
            //if(frames[mediaId])
            //    delete frames[mediaId];
            //frames.erase(mediaId);
            std::cout << "ACAudioStkEngineRendererPlugin::muteAll: stop playing media " << mediaId << std::endl;
            if(media_cycle)
                media_cycle->getBrowser()->toggleSourceActivity(mediaId,0);
        }
        inputs.clear();

        for(std::map <long int, ACAudioStkFileLoop*>::iterator loop = loops.begin(); loop != loops.end(); loop++){
            long int mediaId = loop->first;
            if(loop->second){
                //if(loops[mediaId]->isOpen())
                loop->second->closeFile();
                //CF crash: delete loop->second;
            }
            loops.erase(mediaId);
            current_frames[mediaId] = 0;
            //if(frames[mediaId])
            //    delete frames[mediaId];
            //frames.erase(mediaId);
            std::cout << "ACAudioStkEngineRendererPlugin::muteAll: stop looping media " << mediaId << std::endl;
            if(media_cycle)
                media_cycle->getBrowser()->toggleSourceActivity(mediaId,0);

        }
        loops.clear();

        for(std::map <long int, ACAudioStkGranulate*>::iterator grain = grains.begin(); grain != grains.end(); grain++){
            long int mediaId = grain->first;
            if(grain->second){
                //if(grains[mediaId]->isOpen())
                grain->second->closeFile();
                //CF crash: delete grain->second;
            }
            grains.erase(mediaId);
            std::cout << "ACAudioStkEngineRendererPlugin::muteAll: stop granularizing media " << mediaId << std::endl;
            if(media_cycle)
                media_cycle->getBrowser()->toggleSourceActivity(mediaId,0);

        }
        grains.clear();

        //if(media_cycle)
        //    media_cycle->muteAllSources();
        gains.clear();
        pans.clear();
        for(std::map <long int, ACAudioStkFreeVerb*>::iterator frev = frevs.begin(); frev != frevs.end(); frev++){
            //delete frev->second;
            frevs.erase(frev->first);
        }
        frevs.clear();

        current_frames.clear();
        frevs.clear();
        gains.clear();
        pans.clear();

        //pthread_mutex_unlock(&delete_mutex);
        muting = false;
    }
}

void ACAudioStkEngineRendererPlugin::updateMasterVolume(){
    this->master_volume = this->getNumberParameterValue("Master Volume");
}

void ACAudioStkEngineRendererPlugin::updatePlaybackSpeed(){
    float rate = this->getNumberParameterValue("Playback Speed");
    if(active_target == "All nodes"){
        for(std::map <long int, ACAudioStkFileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
            if(input->second)
                input->second->setRate(rate);
        }
        for(std::map <long int, ACAudioStkFileLoop*>::iterator loop = loops.begin(); loop != loops.end(); loop++){
            if(loop->second)
                loop->second->setRate(rate);
        }
    }
}

void ACAudioStkEngineRendererPlugin::updateGrainVoices(){
    float voices = this->getNumberParameterValue("Grain Voices");
    for(std::map <long int, ACAudioStkGranulate*>::iterator grain = grains.begin(); grain != grains.end(); grain++){
        if(grain->second)
            grain->second->setVoices(voices);
    }
}

void ACAudioStkEngineRendererPlugin::updateGrainRandomness(){
    float randomness = this->getNumberParameterValue("Grain Randomness");
    for(std::map <long int, ACAudioStkGranulate*>::iterator grain = grains.begin(); grain != grains.end(); grain++){
        if(grain->second)
            grain->second->setRandomFactor( randomness ); // factor between 0 - 1.0
    }
}

void ACAudioStkEngineRendererPlugin::updateGrainStretch(){
    float stretch = this->getNumberParameterValue("Grain Stretch");
    for(std::map <long int, ACAudioStkGranulate*>::iterator grain = grains.begin(); grain != grains.end(); grain++){
        if(grain->second)
            grain->second->setStretch( stretch ); // factor (1-1000)
    }
}

void ACAudioStkEngineRendererPlugin::updateGrainParameters(){
    float duration = this->getNumberParameterValue("Grain Duration");
    float ramp = this->getNumberParameterValue("Grain Ramp");
    float offset = this->getNumberParameterValue("Grain Offset");
    float delay = this->getNumberParameterValue("Grain Delay");
    for(std::map <long int, ACAudioStkGranulate*>::iterator grain = grains.begin(); grain != grains.end(); grain++){
        if(grain->second)
            grain->second->setGrainParameters(
                        /*duration (ms)*/ duration,
                        /*ramp: envelope percent (0-100)*/ ramp,
                        /*offset: hop time between grains (ms)*/ offset,
                        /*delay: pause time between grains (ms)*/ delay );
    }
}

void ACAudioStkEngineRendererPlugin::updateReverbDamping(){
    float damping = this->getNumberParameterValue("Reverb Damping");
    if(master_frev){
        master_frev->setDamping( damping );
    }
    for(std::map <long int, ACAudioStkFreeVerb*>::iterator frev = frevs.begin(); frev != frevs.end(); frev++)
        if(frev->second)
            frev->second->setDamping( damping );
}

void ACAudioStkEngineRendererPlugin::updateReverbRoomSize(){
    float size = this->getNumberParameterValue("Reverb Room Size");
    if(master_frev)
        master_frev->setRoomSize( size );
    for(std::map <long int, ACAudioStkFreeVerb*>::iterator frev = frevs.begin(); frev != frevs.end(); frev++)
        if(frev->second)
            frev->second->setRoomSize( size );
}

void ACAudioStkEngineRendererPlugin::updateReverbEffectMix(){
    float mix = this->getNumberParameterValue("Reverb Mix");
    if(master_frev)
        master_frev->setEffectMix( mix );
    for(std::map <long int, ACAudioStkFreeVerb*>::iterator frev = frevs.begin(); frev != frevs.end(); frev++)
        if(frev->second)
            frev->second->setEffectMix( mix );
}

void ACAudioStkEngineRendererPlugin::updateReverbFreeze(){
    bool freeze = this->getNumberParameterValue("Reverb Freeze");
    if(master_frev)
        master_frev->setMode( freeze );
    for(std::map <long int, ACAudioStkFreeVerb*>::iterator frev = frevs.begin(); frev != frevs.end(); frev++)
        if(frev->second)
            frev->second->setMode( freeze );
}

void ACAudioStkEngineRendererPlugin::updateReverbPan(){
    float pan = this->getNumberParameterValue("Reverb Pan");
    if(master_frev)
        master_frev->setWidth( pan );
    for(std::map <long int, ACAudioStkFreeVerb*>::iterator frev = frevs.begin(); frev != frevs.end(); frev++)
        if(frev->second)
            frev->second->setWidth( pan );
}

void ACAudioStkEngineRendererPlugin::updatePlaybackPan(){
    float value = this->getNumberParameterValue("Playback Pan");
    for(std::map <long int, stk::StkFloat>::iterator pan = pans.begin(); pan != pans.end(); pan++)
        pan->second = value;
}

void ACAudioStkEngineRendererPlugin::updatePlaybackVolume(){
    float value = this->getNumberParameterValue("Playback Volume");
    for(std::map <long int, stk::StkFloat>::iterator gain = gains.begin(); gain != gains.end(); gain++)
        gain->second = value;
}

void ACAudioStkEngineRendererPlugin::updateActiveTarget(){
    std::string target = this->getStringParameterValue("Effect target");
    if(std::find(active_targets.begin(),active_targets.end(),target) == active_targets.end()){
        std::cerr << "ACAudioStkEngineRendererPlugin::updateActiveTarget: unknown target " << target << std::endl;
        return;
    }
    this->active_target = target;
}

void ACAudioStkEngineRendererPlugin::updateFaderEffect(int fader){
    if(fader_effect.size() > 0 && fader-1 < fader_effect.size()){
        std::stringstream _fader;
        _fader << fader;
        std::cout << "Fader " << fader << " assigned to " << this->getStringParameterValue("Fader " + _fader.str()) << std::endl;
        fader_effect[fader-1] = this->getStringParameterValue("Fader " + _fader.str());
    }
}

std::map<std::string,ACMediaType> ACAudioStkEngineRendererPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string,ACMediaType> extensions;
    return extensions;
}

int ACAudioStkEngineRendererPlugin::outputChannels(){
    return 2; // for now
}

bool ACAudioStkEngineRendererPlugin::performActionOnMedia(std::string action, long int mediaId, std::vector<boost::any> arguments){
    std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: requesting action " << action << " mediaId " << mediaId << " with " << arguments.size() << " arg(s)" << " @ " << getTime() << std::endl;
    if(!media_cycle){
        std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: mediacycle instance not set" << std::endl;
        return false;
    }

    if(action == "mute all"){
        this->muteAll();
        return true;
    }

    if(mediaId == -1){
        std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: action " << action << " media id " << mediaId << " not available" << std::endl;
        return false;
    }
    bool allMedia = (mediaId == -2);

    if(action == "loop" || action == "play" || action == "granulate"){
        return this->createGenerator(action, mediaId, arguments);
        std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: generated sound generator @ " << getTime() << std::endl;
    }
    else if(action == "mute"){
        if(allMedia)
            this->muteAll();
        else
            this->muteMedia(mediaId);
        return true;
    }
    else if(action == "trigger"){
        if(allMedia)
            this->muteAll();
        else
            this->muteMedia(mediaId);
        return true;
    }
    else if(action == "trigger"){
        if(allMedia){
            for(std::map< long int, ACAudioStkFileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
                if(input->second)
                    input->second->reset();
            }
            for(std::map< long int, ACAudioStkFileLoop*>::iterator loop = loops.begin(); loop != loops.end(); loop++){
                if(loop->second)
                    loop->second->reset();
            }
            for(std::map< long int, ACAudioStkGranulate*>::iterator grain = grains.begin(); grain != grains.end(); grain++){
                if(grain->second)
                    grain->second->reset();
            }
            std::cout <<"ACAudioStkEngineRendererPlugin::performActionOnMedia: triggered all playing/looping/granulating media " << std::endl;
        }
        else{
            std::map< long int, ACAudioStkFileWvIn*>::iterator input = inputs.find(mediaId);
            if(input != inputs.end()){
                std::cout <<"ACAudioStkEngineRendererPlugin::performActionOnMedia: trigger, media " << mediaId << " playing" << std::endl;
                if(input->second)
                    input->second->reset();
                return true;
            }
            std::map< long int, ACAudioStkFileLoop*>::iterator loop = loops.find(mediaId);
            if(loop != loops.end()){
                std::cout <<"ACAudioStkEngineRendererPlugin::performActionOnMedia: trigger, media " << mediaId << " playing" << std::endl;
                if(loop->second)
                    loop->second->reset();
                return true;
            }
            std::map< long int, ACAudioStkGranulate*>::iterator grain = grains.find(mediaId);
            if(grain != grains.end()){
                std::cout <<"ACAudioStkEngineRendererPlugin::performActionOnMedia: trigger, media " << mediaId << " granulating" << std::endl;
                if(grain->second)
                    grain->second->reset();
                return true;
            }
        }
        return false;
    }
    else if (action.find("playback", 0) != string::npos){
        if(arguments.size() !=1){
            std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: couldn't convert parameter to float for action " << action << " , aborting..."<< std::endl;
            return false;
        }
        float new_value = 0.0f;
        try{
            new_value = boost::any_cast<float>(arguments[0]);
        }
        catch(const boost::bad_any_cast &){
            std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: couldn't convert to float, aborting..."<< std::endl;
            return false;
        }

        std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: action " << action << " value " << new_value << std::endl;

        if(action == "playback speed"){
            //float old_value = player->getRate();
            std::map <long int, ACAudioStkFileWvIn*>::iterator input = inputs.find(mediaId);
            if(input != inputs.end()){
                //float old_value = input->second->getRate();
                input->second->setRate(new_value);
                return true;
            }
            std::map <long int, ACAudioStkFileLoop*>::iterator loop = loops.find(mediaId);
            if(loop != loops.end()){
                //float old_value = loop->second->getRate();
                loop->second->setRate(new_value);
                return true;
            }
            return false;
        }
        else if(action == "playback volume"){
            std::map <long int, StkFloat>::iterator gain = gains.find(mediaId);
            if(gain != gains.end()){
                gain->second = new_value;
                return true;
            }
            return false;
        }
        else if(action == "playback pan"){
            std::map <long int, StkFloat>::iterator pan = pans.find(mediaId);
            if(pan != pans.end()){
                pan->second = new_value;
                return true;
            }
            return false;
        }
        return false;
    }
    else if(action == "master volume"){
        if(arguments.size() !=1){
            std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: couldn't convert parameter to float for action " << action << " , aborting..."<< std::endl;
            return false;
        }
        float new_value = 0.0f;
        try{
            new_value = boost::any_cast<float>(arguments[0]);
        }
        catch(const boost::bad_any_cast &){
            std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: couldn't convert to float, aborting..."<< std::endl;
            return false;
        }
        if(new_value > 1.0f)
            new_value = 1.0f;
        if(new_value < 0.0f)
            new_value = 0.0f;
        this->setNumberParameterValue("Master Volume",new_value);
        master_volume = new_value;
    }
    else if (action.find("grain", 0) != string::npos){
        std::map <long int, ACAudioStkGranulate*>::iterator grain = grains.find(mediaId);
        if(grain != grains.end()){
            if(arguments.size() !=1){
                std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: action " << action << " requires 1 float argument" << std::endl;
                return false;
            }
            float new_value = 0.0f;
            try{
                new_value = boost::any_cast<float>(arguments[0]);
            }
            catch(const boost::bad_any_cast &){
                std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: couldn't convert parameter to float for action " << action << " , aborting..."<< std::endl;
                return false;
            }

            if(action == "grain voices"){
                //float old_value = grain->second->getVoices();
                grain->second->setVoices(new_value);
            }
            else if(action == "grain randomness"){
                //float old_value = grain->second->getRandomFactor();
                grain->second->setRandomFactor(new_value);
            }
            else if(action == "grain stretch"){
                //float old_value = grain->second->getStretch();
                grain->second->setStretch(new_value);
            }
            else if(action == "grain duration"){
                //float old_value = grain->second->getDuration();
                grain->second->setDuration(new_value);
            }
            else if(action == "grain ramp"){
                //float old_value = grain->second->getRampPercent();
                grain->second->setRampPercent(new_value);
            }
            else if(action == "grain offset"){
                //float old_value = grain->second->getOffset();
                grain->second->setOffset(new_value);
            }
            else if(action == "grain delay"){
                //float old_value = grain->second->getDelay();
                grain->second->setDelay(new_value);
            }
        }
    }
    else if (action.find("reverb", 0) != string::npos){
        /*if(!allMedia){
            std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: action " << action << " for now for all media" << std::endl;
            return false;
        }*/

        if(arguments.size() !=1){
            std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: action " << action << " requires 1 float argument" << std::endl;
            return false;
        }
        float new_value = 0.0f;
        try{
            new_value = boost::any_cast<float>(arguments[0]);
        }
        catch(const boost::bad_any_cast &){
            std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: couldn't convert parameter to float for action " << action << " , aborting..."<< std::endl;
            return false;
        }

        if(action == "reverb mix"){
            this->setNumberParameterValue("Reverb Mix",new_value);
        }
        else if(action == "reverb room size"){
            this->setNumberParameterValue("Reverb Room Size",new_value);
        }
        else if(action == "reverb damping"){
            this->setNumberParameterValue("Reverb Damping",new_value);
        }
        else if(action == "reverb freeze"){
            this->setNumberParameterValue("Reverb Freeze",new_value);
        }
    }
    else if (action.find("hover", 0) != string::npos){

        ACMediaNode* node = media_cycle->getMediaNode(mediaId);
        if(!node){
            std::cerr <<"ACAudioStkEngineRendererPlugin::performActionOnMedia: audio hover, couldn't access node " << mediaId << " aborting" << std::endl;
            return 1;
        }

        int activity = 0;
        activity = node->getActivity();

        if(action == "hover closest node"){
            /*std::list<int> pointerIds = media_cycle->getPointerIds();
            if(pointerIds.size() > 0){
                std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: active pointer(s): ";
                for(std::list<int>::iterator pointerId = pointerIds.begin(); pointerId != pointerIds.end(); pointerId++){
                    std::cout << "id " << *pointerId;
                    ACPointer* pointer = media_cycle->getPointerFromId(*pointerId);
                    if(pointer){
                        std::cout << " (";
                        if(pointer->getType() == AC_POINTER_MOUSE)
                            std::cout << "mouse";
                        else
                            std::cout << "other";
                        std::cout << " @";
                        ACPoint coord = pointer->getCurrentPosition();
                        std::cout << coord.x << " " << coord.y;
                        std::cout << ")";
                    }
                    std::cout << " ";
                }
                std::cout << std::endl;
            }*/

            if(media_cycle->getAutoPlay() &&  inputs.find(mediaId) == inputs.end() && grains.find(mediaId) == grains.end()){

                //std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: audio hover, current activity "<< activity << std::endl;

                media_cycle->performActionOnMedia("loop", mediaId);
                if(node){
                    if(activity != 1){
                        if(node){
                            std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: audio hover, forcing activity to 2" << std::endl;
                            node->setActivity(2);
                        }
                    }
                }
            }

            // The following is specific to the BFC2000
            if(midi_out && current_closest_node != mediaId && midi_out->getPortName() == "BCF2000 Port 1" && fader_effect.size()==8){
                int fader = 0;
                current_closest_node = mediaId;
                for(std::vector<std::string>::iterator effect = fader_effect.begin(); effect != fader_effect.end(); effect++){
                    std::string parameter = *effect;
                    float min = this->getNumberParameterMin(parameter);
                    float max = this->getNumberParameterMax(parameter);
                    float value = 0.0f;
                    if(parameter == "master volume"){
                        value = master_volume;
                    }
                    else if(parameter == "playback speed"){
                        if(inputs.find(mediaId)!=inputs.end())
                            value = inputs[mediaId]->getRate();
                        else if(loops.find(mediaId)!=loops.end())
                            value = loops[mediaId]->getRate();
                    }
                    else if(parameter == "playback volume"){
                        value = gains[mediaId];
                    }
                    else if(parameter == "playback pan"){
                        value = pans[mediaId];
                    }
                    else if(parameter == "grain voices"){
                        if(grains.find(mediaId)!=grains.end())
                            value = grains[mediaId]->getVoices();
                    }
                    else if(parameter == "grain randomness"){
                        if(grains.find(mediaId)!=grains.end())
                            value = grains[mediaId]->getRandomFactor();
                    }
                    else if(parameter == "grain stretch"){
                        if(grains.find(mediaId)!=grains.end())
                            value = grains[mediaId]->getStretch();
                    }
                    else if(parameter == "grain duration"){
                        if(grains.find(mediaId)!=grains.end())
                            value = grains[mediaId]->getDuration();
                    }
                    else if(parameter == "grain ramp"){
                        if(grains.find(mediaId)!=grains.end())
                            value = grains[mediaId]->getRampPercent();
                    }
                    else if(parameter == "grain offset"){
                        if(grains.find(mediaId)!=grains.end())
                            value = grains[mediaId]->getOffset();
                    }
                    else if(parameter == "grain delay"){
                        if(grains.find(mediaId)!=grains.end())
                            value = grains[mediaId]->getDelay();
                    }
                    else if(parameter == "reverb mix"){
                        if(master_frev){
                            value = master_frev->getEffectMix();
                            this->setNumberParameterValue("Reverb Mix",value);
                        }
                    }
                    else if(parameter == "reverb room size"){
                        if(master_frev){
                            value = master_frev->getRoomSize();
                            this->setNumberParameterValue("Reverb Room Size",value);
                        }
                    }
                    else if(parameter == "reverb damping"){
                        if(master_frev){
                            value = master_frev->getDamping();
                            this->setNumberParameterValue("Reverb Damping",value);
                        }
                    }
                    else
                        value = min;
                    std::vector<unsigned char> message;
                    message.push_back((unsigned char)(176)); // typechan
                    message.push_back((unsigned char)(81+fader)); // cc number
                    message.push_back((unsigned char)( (value-min)/(max-min) * 127) ); // value
                    midi_out->sendMessage(&message);
                    std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: audio hover midi out parameter " << parameter << " cc " << 81+fader << " value " << (value-min)/(max-min) * 127 << std::endl;
                    fader++;
                }
            }
            current_closest_node = mediaId;
        }
        else if(action == "hover off node"){
            std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: hover off node " << mediaId << std::endl;
            //if(activity == 2)
            media_cycle->performActionOnMedia("mute", mediaId);
        }
    }
    return false;
}

bool ACAudioStkEngineRendererPlugin::createGenerator(std::string action, long int mediaId, std::vector<boost::any> arguments){
    std::map <long int, ACAudioStkFileWvIn*>::iterator _input = inputs.find(mediaId);
    std::map <long int, ACAudioStkFileLoop*>::iterator _loop = loops.find(mediaId);
    std::map <long int, ACAudioStkGranulate*>::iterator _grain = grains.find(mediaId);

    ACAudioStkPlaybackType playback_type = PLAYBACK_OFF;
    if(playback_types.find(mediaId) != playback_types.end()){
        playback_type = playback_types[mediaId];
    }

    bool playing = playback_type&PLAYBACK_PLAY;//(_input != inputs.end());
    bool looping = playback_type&PLAYBACK_LOOP;//(_loop != loops.end());
    bool granulating = playback_type&PLAYBACK_GRANULATE;//(_grain != grains.end());
    bool active = playing || looping || granulating;

    if(active){
        if(action == "play" && playing){
            std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: action " << action << " media id " << mediaId << " already " << action << "ing" << std::endl;
            return false;
        }
        else if(action == "loop" && looping){
            std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: action " << action << " media id " << mediaId << " already " << action << "ing, forcing activity to 1" << std::endl;
            ACMediaNode* node = media_cycle->getMediaNode(mediaId);
            if(node)
                node->setActivity(1);
            return false;
        }
        else if(action == "granulate" && granulating){
            std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: action " << action << " media id " << mediaId << " already " << action << "ing" << std::endl;
            return false;
        }
        else{
            std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: action " << action << " requested on media id " << mediaId << " already ";
            if(playing)
                std::cout << "playing";
            else if(looping)
                std::cout << "looping";
            else if(granulating)
                std::cout << "granulating";
            std::cout << ", removing first" << std::endl;
            this->stopSource(mediaId);
        }
    }

    bool allMedia = (mediaId == -2);
    if(allMedia) // not playing all media, crash prone
        return false;
    std::string filename = media_cycle->getMediaFileName(mediaId);
    if(filename == "")
        return false;
    int channels = 0;
    // Try to load the soundfile.
    if(action == "play"){
        ACAudioStkFileWvIn* input = 0;
        try {
            input = new ACAudioStkFileWvIn();
            input->openFile( filename.c_str() );
            //input->ignoreSampleRateChange();
        }
        catch ( StkError & ) {
            std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: play: couldn't load media " << mediaId << " file " << filename << std::endl;
            return false;
        }

        // Set input read rate based on the default STK sample rate.
        double rate = 1.0;
        rate = this->getNumberParameterValue("Playback Speed");//input->getFileRate() / Stk::sampleRate();
        //if ( argc == 4 ) rate *= atof( argv[3] );
        input->setRate( rate );

        // Find out how many channels we have.
        channels = input->channelsOut();
        std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: play: file has " << input->channelsOut() << " channels " << std::endl;

        inputs[mediaId] = input;
        playback_types[mediaId] = PLAYBACK_PLAY;
        ACMediaNode* node = media_cycle->getMediaNode(mediaId);
        if(node)
            node->setActivity(1);
        std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: action " << action << " media id " << mediaId << " done" << std::endl;
    }
    else if(action == "loop"){
        ACAudioStkFileLoop* input = 0;
        try {
            input = new ACAudioStkFileLoop();
            input->openFile( filename.c_str() );
        }
        catch ( StkError & ) {
            std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: loop: couldn't load media " << mediaId << " file " << filename << std::endl;
            return false;
        }

        // Set input read rate based on the default STK sample rate.
        double rate = 1.0;
        rate = this->getNumberParameterValue("Playback Speed");//input->getFileRate() / Stk::sampleRate();
        //if ( argc == 4 ) rate *= atof( argv[3] );
        input->setRate( rate );

        // Find out how many channels we have.
        channels = input->channelsOut();

        loops[mediaId] = input;
        playback_types[mediaId] = PLAYBACK_LOOP;
        ACMediaNode* node = media_cycle->getMediaNode(mediaId);
        if(node)
            node->setActivity(1);

        std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: action " << action << " media id " << mediaId << " done" << std::endl;
    }
    else if(action == "granulate"){
        ACAudioStkGranulate* grain = 0;
        try {
            grain = new ACAudioStkGranulate();
            grain->setVoices( this->getNumberParameterValue("Grain Voices") );
            grain->setRandomFactor( this->getNumberParameterValue("Grain Randomness") ); // factor between 0 - 1.0
            grain->setStretch( this->getNumberParameterValue("Grain Stretch") ); // factor (1-1000)
            grain->setGrainParameters(
                        /*duration (ms)*/ this->getNumberParameterValue("Grain Duration"),
                        /*ramp: envelope percent (0-100)*/ this->getNumberParameterValue("Grain Ramp"),
                        /*offset: hop time between grains (ms)*/ this->getNumberParameterValue("Grain Offset"),
                        /*delay: pause time between grains (ms)*/ this->getNumberParameterValue("Grain Delay") );
            grain->openFile( filename.c_str() );
        }
        catch ( StkError & ) {
            std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: granulate: couldn't load media " << mediaId << " file " << filename << std::endl;
            return false;
        }
        grains[mediaId] = grain;
        playback_types[mediaId] = PLAYBACK_GRANULATE;
        ACMediaNode* node = media_cycle->getMediaNode(mediaId);
        if(node)
            node->setActivity(1);

        std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: action " << action << " media id " << mediaId << " done" << std::endl;
    }

    /*ACAudioStkFreeVerb* frev = 0;
    try {
        frev = new ACAudioStkFreeVerb();
        frev->setEffectMix( this->getNumberParameterValue("Reverb Mix"));
    }catch ( StkError & ) {
        std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: couldn't create reverb for media " << mediaId << " file " << filename << ", bypassing." << std::endl;
    }
    if(frev)
        frevs[mediaId] = frev;*/
    gains[mediaId] = 1.0f;
    pans[mediaId] = 0.0f;

    // Figure out how many bytes in an StkFloat and setup the RtAudio stream.
    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac.getDefaultOutputDevice();
    parameters.nChannels = this->outputChannels();//channels;
    RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
    unsigned int bufferFrames = RT_BUFFER_SIZE;

    /*try {
      dac.closeStream();
    }
    catch ( RtError &error ) {
      error.printMessage();
      //return false;
    }*/

    try {
        if(!dac.isStreamOpen()){
            dac.showWarnings();
            RtAudio::StreamOptions *options = new RtAudio::StreamOptions;
            options->flags = RTAUDIO_SCHEDULE_REALTIME /* | RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_HOG_DEVICE*/;
            if(options->flags & RTAUDIO_MINIMIZE_LATENCY)
                std::cout << "ACAudioStkEngineRendererPlugin: with latency minimizing" << std::endl;
            if(options->flags & RTAUDIO_HOG_DEVICE)
                std::cout << "ACAudioStkEngineRendererPlugin: with device hogging" << std::endl;
            if(options->flags & RTAUDIO_SCHEDULE_REALTIME)
                std::cout << "ACAudioStkEngineRendererPlugin: with realtime scheduling" << std::endl;
            dac.openStream( &parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &ACAudioStkEngineRendererPlugin::tickWrapper, this, options );
        }
    }
    catch ( RtError &error ) {
        error.printMessage();
        return false;
    }

    // Install an interrupt handler function.
    //(void) signal(SIGINT, finish);

    // Resize the StkFrames object appropriately.
    if(frames.find(mediaId)!=frames.end() && frames[mediaId]!=0){
        frames[mediaId]->resize( bufferFrames, channels );
    }
    else{
        try{
            StkFrames* _frames = new StkFrames( (unsigned int)bufferFrames, (unsigned int)channels );
            frames[mediaId] = _frames;
        }
        catch(...){
            std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: couldn't allocate frames for media " << mediaId << std::endl;
        }
    }
    try {
        if(!dac.isStreamRunning())
            dac.startStream();
    }
    catch ( RtError &error ) {
        error.printMessage();
        return false;
    }

    // Block waiting until callback signals done.
    /*while ( !done )
      Stk::sleep( 100 );

    // By returning a non-zero value in the callback above, the stream
    // is automatically stopped.  But we should still close it.
    try {
      dac.closeStream();
    }
    catch ( RtError &error ) {
      error.printMessage();
      return false;
    }*/
}

void ACAudioStkEngineRendererPlugin::muteMedia(int mediaId){
    this->stopSource(mediaId);
    /*std::map <long int, ACAudioStkFileWvIn*>::iterator input = inputs.find(mediaId);
    if(input != inputs.end()){
        media_cycle->getBrowser()->toggleSourceActivity(input->first,0);
        inputs.erase(input->first);
    }

    std::map <long int, ACAudioStkFileLoop*>::iterator loop = loops.find(mediaId);
    if(loop != loops.end()){
        media_cycle->getBrowser()->toggleSourceActivity(loop->first,0);
        loops.erase(loop->first);
    }

    std::map <long int, ACAudioStkGranulate*>::iterator grain = grains.find(mediaId);
    if(grain != grains.end()){
        media_cycle->getBrowser()->toggleSourceActivity(grain->first,0);
        grains.erase(grain->first);
    }

    std::map< long int, int>::iterator current_frame = current_frames.find(mediaId);
    if(current_frame != current_frames.end()){
        media_cycle->getBrowser()->toggleSourceActivity(current_frame->first,0);
        current_frames.erase(current_frame->first);
    }*/
}

std::map<std::string,ACMediaType> ACAudioStkEngineRendererPlugin::availableMediaActions(){
    std::map<std::string,ACMediaType> media_actions;
    media_actions["play"] = MEDIA_TYPE_AUDIO;
    media_actions["loop"] = MEDIA_TYPE_AUDIO;
    if(with_granulation){
        media_actions["granulate"] = MEDIA_TYPE_AUDIO;
    }
    media_actions["mute"] = MEDIA_TYPE_AUDIO;
    media_actions["mute all"] = MEDIA_TYPE_AUDIO;
    media_actions["trigger"] = MEDIA_TYPE_AUDIO;
    media_actions["master volume"] = MEDIA_TYPE_AUDIO;
    media_actions["playback speed"] = MEDIA_TYPE_AUDIO;
    media_actions["playback volume"] = MEDIA_TYPE_AUDIO;
    media_actions["playback pan"] = MEDIA_TYPE_AUDIO;
    if(with_granulation){
        media_actions["grain voices"] = MEDIA_TYPE_AUDIO;
        media_actions["grain randomness"] = MEDIA_TYPE_AUDIO;
        media_actions["grain stretch"] = MEDIA_TYPE_AUDIO;
        media_actions["grain duration"] = MEDIA_TYPE_AUDIO;
        media_actions["grain ramp"] = MEDIA_TYPE_AUDIO;
        media_actions["grain offset"] = MEDIA_TYPE_AUDIO;
        media_actions["grain delay"] = MEDIA_TYPE_AUDIO;
    }
    media_actions["reverb mix"] = MEDIA_TYPE_AUDIO;
    media_actions["reverb room size"] = MEDIA_TYPE_AUDIO;
    media_actions["reverb damping"] = MEDIA_TYPE_AUDIO;
    media_actions["reverb freeze"] = MEDIA_TYPE_AUDIO;
    return media_actions;
}

std::map<std::string,ACMediaActionParameters> ACAudioStkEngineRendererPlugin::mediaActionsParameters(){
    return this->action_parameters;
}

void ACAudioStkEngineRendererPlugin::playClickedNode(){
    if(!media_cycle)
        return;
    int media_id = media_cycle->getClickedNode();
    media_cycle->performActionOnMedia("play", media_id);
}

void ACAudioStkEngineRendererPlugin::loopClickedNode(){
    if(!media_cycle)
        return;
    int media_id = media_cycle->getClickedNode();
    media_cycle->performActionOnMedia("loop", media_id);
}

void ACAudioStkEngineRendererPlugin::granulateClickedNode(){
    if(!media_cycle)
        return;
    int media_id = media_cycle->getClickedNode();
    media_cycle->performActionOnMedia("granulate", media_id);
}

void ACAudioStkEngineRendererPlugin::muteAllNodes(){
    if(!media_cycle)
        return;
    media_cycle->performActionOnMedia("mute all", -1);
}

void ACAudioStkEngineRendererPlugin::triggerClickedNode(){
    if(!media_cycle)
        return;
    int media_id = media_cycle->getClickedNode();
    media_cycle->performActionOnMedia("trigger", media_id);
}

std::vector<ACInputActionQt*> ACAudioStkEngineRendererPlugin::providesInputActions(){
    std::vector<ACInputActionQt*> inputActions;
    inputActions.push_back(playClickedNodeAction);
    inputActions.push_back(loopClickedNodeAction);
    if(with_granulation){
        inputActions.push_back(granulateClickedNodeAction);
    }
    inputActions.push_back(muteAllNodesAction);
    return inputActions;
}

