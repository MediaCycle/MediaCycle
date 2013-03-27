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

/*bool done;
static void finish(int ignore)
{
    done = true;
}*/

ACAudioStkEngineRendererPlugin::ACAudioStkEngineRendererPlugin() : QObject(), ACPluginQt(), ACMediaRendererPlugin(){
    this->mName = "Audio Engine (STK)";
    this->mDescription ="Plugin for playing audio files with The Synthesis ToolKit in C++ (STK)";
    this->mMediaType = MEDIA_TYPE_AUDIO;

    // Set the global sample rate before creating class instances.
    Stk::setSampleRate( (StkFloat) 44100 );

    frev = new stk::FreeVerb();

    this->action_parameters["play"] = ACMediaActionParameters();
    this->action_parameters["loop"] = ACMediaActionParameters();
    this->action_parameters["granulate"] = ACMediaActionParameters();
    this->action_parameters["mute all"] = ACMediaActionParameters();

    this->addCallback("Mute","Mute",boost::bind(&ACAudioStkEngineRendererPlugin::muteAll,this));
    this->action_parameters["mute"] = ACMediaActionParameters();

    this->addNumberParameter("Playback Speed",1.0,-10.0,10.0,0.01,"Playback Speed (only for play and loop, not granulate)",boost::bind(&ACAudioStkEngineRendererPlugin::updateRate,this));
    this->action_parameters["playback speed"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Playback Speed")));

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

    //! Reverb mix [0 = mostly dry, 1 = mostly wet].
    this->addNumberParameter("Reverb Mix",0,0,1,0.01,"Reverb Mix (0 = mostly dry, 1 = mostly wet)",boost::bind(&ACAudioStkEngineRendererPlugin::updateReverbEffectMix,this));
    this->action_parameters["reverb mix"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Reverb Mix")));
    this->frev->setEffectMix( this->getNumberParameterValue("Reverb Mix"));

    //! Reverb room size (comb filter feedback gain) parameter [0,1].
    this->addNumberParameter("Reverb Room Size",1,0,1,0.01,"Reverb room size (comb filter feedback gain) parameter [0,1]",boost::bind(&ACAudioStkEngineRendererPlugin::updateReverbRoomSize,this));
    this->action_parameters["reverb room size"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Reverb Room Size")));

    //! Reverb damping parameter [0=low damping, 1=higher damping].
    this->addNumberParameter("Reverb Damping",1,0,1,0.01,"Reverb damping parameter (0=low damping, 1=higher damping)",boost::bind(&ACAudioStkEngineRendererPlugin::updateReverbDamping,this));
    this->action_parameters["reverb damping"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Reverb Damping")));

    //! Reverb mode [frozen = 1, unfrozen = 0].
    this->addNumberParameter("Reverb Freeze",0,0,1,1,"Reverb freeze (frozen = 1, unfrozen = 0)",boost::bind(&ACAudioStkEngineRendererPlugin::updateReverbFreeze,this));
    this->action_parameters["reverb freeze"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Reverb Freeze")));

    //! Reverb width (left-right mixing) parameter [0,1].
    //this->addNumberParameter("Reverb Pan",1,0,1,0.01,"Reverb width (left-right mixing) parameter [0,1]",boost::bind(&ACAudioStkEngineRendererPlugin::updateReverbPan,this));
    //this->action_parameters["reverb pan"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Reverb Pan")));

    /*this->addNumberParameter("Volume",100,1,100,1,"Main volume",boost::bind(&ACAudioStkEngineRendererPlugin::updateVolume,this));*/

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

    granulateClickedNodeAction = new ACInputActionQt(tr("Granulate clicked node"), this);
    granulateClickedNodeAction->setShortcut(Qt::Key_G);
    granulateClickedNodeAction->setKeyEventType(QEvent::KeyPress);
    granulateClickedNodeAction->setMouseEventType(QEvent::MouseButtonRelease);
    granulateClickedNodeAction->setToolTip(tr("Granulate the clicked node"));
    connect(granulateClickedNodeAction, SIGNAL(triggered()), this, SLOT(granulateClickedNode()));

    muteAllNodesAction = new ACInputActionQt(tr("Mute all"), this);
    muteAllNodesAction->setShortcut(Qt::Key_M);
    muteAllNodesAction->setKeyEventType(QEvent::KeyPress);
    //muteAllNodesAction->setMouseEventType(QEvent::MouseButtonRelease);
    muteAllNodesAction->setToolTip(tr("Mute all"));
    connect(muteAllNodesAction, SIGNAL(triggered()), this, SLOT(muteAllNodes()));
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

    action_parameters.clear();

    if(frev)
        delete frev;
    frev = 0;
}

void ACAudioStkEngineRendererPlugin::justReadFrames(long int mediaId, int nFrames){
    current_frames[mediaId] = current_frames[mediaId] + nFrames;
    if(media_cycle){
        media_cycle->setCurrentFrame(mediaId, current_frames[mediaId]);
        std::map< long int, ACAudioStkFileLoop*>::iterator loop = loops.find(mediaId);
        if( loop != loops.end()){
            //std::cout << "ACAudioStkEngineRendererPlugin: looping, current frame " << current_frames[mediaId] << " of media " << mediaId << std::endl;
            if(current_frames[mediaId] >= loop->second->getSize())
                current_frames[mediaId] = 0;
            if(current_frames[mediaId] <= 0)
                current_frames[mediaId] = loop->second->getSize();
        }
        //else
        //    std::cout << "ACAudioStkEngineRendererPlugin: playing frame " << current_frames[mediaId] << " of media " << mediaId << std::endl;
    }
}

void ACAudioStkEngineRendererPlugin::removeInput(long int mediaId){
    if(inputs.find(mediaId) != inputs.end()){
        current_frames[mediaId] = 0;
        inputs.erase(mediaId);
        if(frames[mediaId])
            delete frames[mediaId];
        frames.erase(mediaId);
        std::cout << "ACAudioStkEngineRendererPlugin::removeInput: stop playing media " << mediaId << std::endl;
        if(media_cycle)
            media_cycle->getBrowser()->toggleSourceActivity(mediaId,0);
        return;
    }
    if(loops.find(mediaId) != loops.end()){
        current_frames[mediaId] = 0;
        loops.erase(mediaId);
        if(frames[mediaId])
            delete frames[mediaId];
        frames.erase(mediaId);
        std::cout << "ACAudioStkEngineRendererPlugin::removeInput: stop looping media " << mediaId << std::endl;
        if(media_cycle)
            media_cycle->getBrowser()->toggleSourceActivity(mediaId,0);
    }
    if(grains.find(mediaId) != grains.end()){
        grains.erase(mediaId);
        std::cout << "ACAudioStkEngineRendererPlugin::removeInput: stop granularizing media " << mediaId << std::endl;
        if(media_cycle)
            media_cycle->getBrowser()->toggleSourceActivity(mediaId,0);
    }
}

int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
          double streamTime, RtAudioStreamStatus status, void *userData )
{
    std::map <long int, ACAudioStkFileWvIn*> inputs = ((ACAudioStkEngineRendererPlugin *) userData)->inputs;
    std::map <long int, ACAudioStkFileLoop*> loops = ((ACAudioStkEngineRendererPlugin *) userData)->loops;
    std::map <long int, ACAudioStkGranulate*> grains = ((ACAudioStkEngineRendererPlugin *) userData)->grains;
    stk::FreeVerb* frev = ((ACAudioStkEngineRendererPlugin *) userData)->frev;
    register StkFloat *samples = (StkFloat *) outputBuffer;
    //if(outputBuffer == 0){
    //    std::cerr << "ACAudioStkEngineRenderer: output buffer not available" << std::endl;
    //    return 0;
    //}

    if(inputs.size()==0 && loops.size()==0 && grains.size()==0){
        *samples++ = 0;
        return 1;
    }

    double rate = 1.0;
    rate = (float)((ACAudioStkEngineRendererPlugin *) userData)->getNumberParameterValue("Playback Speed");
    int outputChannels = ((ACAudioStkEngineRendererPlugin *) userData)->outputChannels();

    for(std::map <long int, ACAudioStkFileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
        if(input->second){
            if (((ACAudioStkEngineRendererPlugin *) userData)->frames[input->first]){
                input->second->tick( *(((ACAudioStkEngineRendererPlugin *) userData)->frames[input->first]) );
                ((ACAudioStkEngineRendererPlugin *) userData)->justReadFrames(input->first, (int)(rate * input->second->channelsOut() * nBufferFrames));
            }
            else{
                ((ACAudioStkEngineRendererPlugin *) userData)->removeInput(input->first);
            }
        }
    }

    for(std::map <long int, ACAudioStkFileLoop*>::iterator loop = loops.begin(); loop != loops.end(); loop++){
        if(loop->second && (((ACAudioStkEngineRendererPlugin *) userData)->frames[loop->first])){
            loop->second->tick( *(((ACAudioStkEngineRendererPlugin *) userData)->frames[loop->first]) );
            ((ACAudioStkEngineRendererPlugin *) userData)->justReadFrames(loop->first, (int)(rate * loop->second->channelsOut() * nBufferFrames));
        }
    }

    std::map <long int, const StkFrames*> lastFrames;
    for(std::map <long int, ACAudioStkGranulate*>::iterator grain = grains.begin(); grain != grains.end(); grain++){
        if(grain->second){
            lastFrames[grain->first] = &(grain->second->lastFrame());
        }
    }

    if( ((ACAudioStkEngineRendererPlugin *) userData)->frames.size()>0 || grains.size()>0 ){
        //        std::cout << "Frames /" << nBufferFrames <<":";
        //        int in = 0;
        //        for(std::map <long int, FileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
        //            std::cout << " " << in++ << " " <<  ((ACAudioStkEngineRendererPlugin *) userData)->frames[input->first]->size();
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
                        for ( unsigned int c=0; c<outputChannels; c++ ){
                            frame[c] += (*(lastFrames[grain->first]))[c%nChannels];
                        }
                    }
                }
            }

            for(std::map <long int, ACAudioStkFileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
                if( ((ACAudioStkEngineRendererPlugin *) userData)->frames[input->first]!=0 && ((ACAudioStkEngineRendererPlugin *) userData)->frames[input->first]->size()>i){
                    int fileChannels = input->second->channelsOut();
                    for ( unsigned int c=0; c<outputChannels; c++ ){
                        frame[c] += (*((ACAudioStkEngineRendererPlugin *) userData)->frames[input->first])[i*fileChannels+(c%fileChannels)];
                    }
                    count++;
                }
            }
            for(std::map <long int, ACAudioStkFileLoop*>::iterator loop = loops.begin(); loop != loops.end(); loop++){
                if( ((ACAudioStkEngineRendererPlugin *) userData)->frames[loop->first]!=0 && ((ACAudioStkEngineRendererPlugin *) userData)->frames[loop->first]->size()>i){
                    int fileChannels = loop->second->channelsOut();
                    for ( unsigned int c=0; c<outputChannels; c++ ){
                        frame[c] += (*((ACAudioStkEngineRendererPlugin *) userData)->frames[loop->first])[i*fileChannels+(c%fileChannels)];
                    }
                    count++;
                }
            }
            for ( unsigned int c=0; c<outputChannels; c++ ){
                *samples++ = frev->tick(frame[c]); // / count;
            }
        }
    }

    bool finished = true;
    for(std::map <long int, ACAudioStkFileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
        if(input->second->isFinished()){
            ((ACAudioStkEngineRendererPlugin *) userData)->removeInput(input->first);
        }
        finished = (input->second->isFinished()) && finished;
    }

    //if ( input->isFinished() ) {
    /*if ( finished ) {
    done = true;

    return 1;
  }
  else*/
    return 0;
}

void ACAudioStkEngineRendererPlugin::muteAll(){
    //done = true;

    for(std::map <long int, ACAudioStkFileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
        if(media_cycle)
            media_cycle->getBrowser()->toggleSourceActivity(input->first,0);
        inputs.erase(input->first);
    }
    inputs.clear();
    for(std::map <long int, ACAudioStkFileLoop*>::iterator loop = loops.begin(); loop != loops.end(); loop++){
        if(media_cycle)
            media_cycle->getBrowser()->toggleSourceActivity(loop->first,0);
        loops.erase(loop->first);
    }
    loops.clear();
    for(std::map <long int, ACAudioStkGranulate*>::iterator grain = grains.begin(); grain != grains.end(); grain++){
        if(media_cycle)
            media_cycle->getBrowser()->toggleSourceActivity(grain->first,0);
        grains.erase(grain->first);
    }
    grains.clear();
    current_frames.clear();
    if(media_cycle)
        media_cycle->muteAllSources();
}

void ACAudioStkEngineRendererPlugin::updateVolume(){
    //this->getNumberParameterValue("Volume")/100.0f );
}

void ACAudioStkEngineRendererPlugin::updateRate(){
    float rate = this->getNumberParameterValue("Playback Speed");
    for(std::map <long int, ACAudioStkFileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
        if(input->second)
            input->second->setRate(rate);
    }
    for(std::map <long int, ACAudioStkFileLoop*>::iterator loop = loops.begin(); loop != loops.end(); loop++){
        if(loop->second)
            loop->second->setRate(rate);
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
    frev->setDamping( damping );
}

void ACAudioStkEngineRendererPlugin::updateReverbRoomSize(){
    float size = this->getNumberParameterValue("Reverb Room Size");
    frev->setRoomSize( size );
}

void ACAudioStkEngineRendererPlugin::updateReverbEffectMix(){
    float mix = this->getNumberParameterValue("Reverb Mix");
    frev->setEffectMix( mix );
}

void ACAudioStkEngineRendererPlugin::updateReverbFreeze(){
    bool freeze = this->getNumberParameterValue("Reverb Freeze");
    frev->setMode( freeze );
}

void ACAudioStkEngineRendererPlugin::updateReverbPan(){
    float pan = this->getNumberParameterValue("Reverb Pan");
    frev->setWidth( pan );
}

std::map<std::string,ACMediaType> ACAudioStkEngineRendererPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string,ACMediaType> extensions;
    return extensions;
}

int ACAudioStkEngineRendererPlugin::outputChannels(){
    return 2; // for now
}

bool ACAudioStkEngineRendererPlugin::performActionOnMedia(std::string action, long int mediaId, std::vector<boost::any> arguments){
    //std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: action " << action << " mediaId " << mediaId << " with " << arguments.size() << " arg(s)" << std::endl;
    if(!media_cycle){
        std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: mediacycle instance not set" << std::endl;
        return false;
    }

    if(action == "mute all"){
        this->muteAll();
        return true;
    }

    if(mediaId == -1){
        std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: media id " << mediaId << " not available" << std::endl;
        return false;
    }

    if(action == "loop" || action == "play" || action == "granulate"){
        std::string filename = media_cycle->getMediaFileName(mediaId);
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
        }

        //done = false;

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
            if(!dac.isStreamOpen())
                dac.openStream( &parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &tick, (void *)this );
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
                frames[mediaId] = new StkFrames( (unsigned int)bufferFrames, (unsigned int)channels );
            }
            catch(...){
                std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: couldn't allocate frames for media " << mediaId << std::endl;
            }
        }
        try {
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
        return true;
    }
    else if(action == "mute"){
        std::map <long int, ACAudioStkFileWvIn*>::iterator input = inputs.find(mediaId);
        if(input != inputs.end()){
            media_cycle->getBrowser()->toggleSourceActivity(input->first,0);
            inputs.erase(input->first);
            //return true;
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
        }
    }
    else if(action == "playback speed"){
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
        std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: action " << action << " rate " << new_value << std::endl;
        std::map <long int, ACAudioStkFileWvIn*>::iterator input = inputs.find(mediaId);
        if(input != inputs.end()){
            float old_value = input->second->getRate();
            input->second->setRate(new_value);
        }
        std::map <long int, ACAudioStkFileLoop*>::iterator loop = loops.find(mediaId);
        if(loop != loops.end()){
            float old_value = loop->second->getRate();
            loop->second->setRate(new_value);
        }
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
                float old_value = grain->second->getVoices();
                grain->second->setVoices(new_value);
            }
            else if(action == "grain randomness"){
                float old_value = grain->second->getRandomFactor();
                grain->second->setRandomFactor(new_value);
            }
            else if(action == "grain stretch"){
                float old_value = grain->second->getStretch();
                grain->second->setStretch(new_value);
            }
            else if(action == "grain duration"){
                float old_value = grain->second->getDuration();
                grain->second->setDuration(new_value);
            }
            else if(action == "grain ramp"){
                float old_value = grain->second->getRampPercent();
                grain->second->setRampPercent(new_value);
            }
            else if(action == "grain offset"){
                float old_value = grain->second->getOffset();
                grain->second->setOffset(new_value);
            }
            else if(action == "grain delay"){
                float old_value = grain->second->getDelay();
                grain->second->setDelay(new_value);
            }
        }
    }
    return false;
}

std::map<std::string,ACMediaType> ACAudioStkEngineRendererPlugin::availableMediaActions(){
    std::map<std::string,ACMediaType> media_actions;
    media_actions["play"] = MEDIA_TYPE_AUDIO;
    media_actions["loop"] = MEDIA_TYPE_AUDIO;
    media_actions["granulate"] = MEDIA_TYPE_AUDIO;
    media_actions["mute"] = MEDIA_TYPE_AUDIO;
    media_actions["mute all"] = MEDIA_TYPE_AUDIO;
    media_actions["playback speed"] = MEDIA_TYPE_AUDIO;
    media_actions["grain voices"] = MEDIA_TYPE_AUDIO;
    media_actions["grain randomness"] = MEDIA_TYPE_AUDIO;
    media_actions["grain stretch"] = MEDIA_TYPE_AUDIO;
    media_actions["grain duration"] = MEDIA_TYPE_AUDIO;
    media_actions["grain ramp"] = MEDIA_TYPE_AUDIO;
    media_actions["grain offset"] = MEDIA_TYPE_AUDIO;
    media_actions["grain delay"] = MEDIA_TYPE_AUDIO;
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


std::vector<ACInputActionQt*> ACAudioStkEngineRendererPlugin::providesInputActions(){
    std::vector<ACInputActionQt*> inputActions;
    inputActions.push_back(playClickedNodeAction);
    inputActions.push_back(loopClickedNodeAction);
    inputActions.push_back(granulateClickedNodeAction);
    inputActions.push_back(muteAllNodesAction);
    return inputActions;
}

