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

bool done;
static void finish(int ignore)
{
    done = true;
}

ACAudioStkEngineRendererPlugin::ACAudioStkEngineRendererPlugin() : QObject(), ACPluginQt(), ACMediaRendererPlugin(){
    this->mName = "Audio Engine (STK)";
    this->mDescription ="Plugin for playing audio files with The Synthesis ToolKit in C++ (STK)";
    this->mMediaType = MEDIA_TYPE_AUDIO;

    // Set the global sample rate before creating class instances.
    Stk::setSampleRate( (StkFloat) 44100 );

    if(this->hasCallbackNamed("Mute"))
        this->updateCallback("Mute","Mute",boost::bind(&ACAudioStkEngineRendererPlugin::muteAll,this));
    else
        this->addCallback("Mute","Mute",boost::bind(&ACAudioStkEngineRendererPlugin::muteAll,this));

    if(this->hasNumberParameterNamed("Rate"))
        this->updateNumberParameter("Rate",1.0,-10.0,10.0,0.01,"Rate",boost::bind(&ACAudioStkEngineRendererPlugin::updateRate,this));
    else
        this->addNumberParameter("Rate",1.0,-10.0,10.0,0.01,"Rate",boost::bind(&ACAudioStkEngineRendererPlugin::updateRate,this));


    /*if(this->hasNumberParameterNamed("Volume"))
        this->updateNumberParameter("Volume",100,1,100,1,"Main volume",boost::bind(&ACAudioStkEngineRendererPlugin::updateVolume,this));
    else
        this->addNumberParameter("Volume",100,1,100,1,"Main volume",boost::bind(&ACAudioStkEngineRendererPlugin::updateVolume,this));*/

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
}

void ACAudioStkEngineRendererPlugin::justReadFrames(long int mediaId, int nFrames){
    current_frames[mediaId] = current_frames[mediaId] + nFrames;
    if(media_cycle){
        media_cycle->setCurrentFrame(mediaId, current_frames[mediaId]);
        std::map< long int, stk::FileLoop*>::iterator loop = loops.find(mediaId);
        if( loop != loops.end()){
            //std::cout << "ACAudioStkEngineRendererPlugin: looping, current frame " << current_frames[mediaId] << " of media " << mediaId << std::endl;
            if(current_frames[mediaId] >= loop->second->getSize())
                current_frames[mediaId] = 0;
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
}

int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
          double streamTime, RtAudioStreamStatus status, void *userData )
{
    std::map <long int, FileWvIn*> inputs = ((ACAudioStkEngineRendererPlugin *) userData)->inputs;
    std::map <long int, FileLoop*> loops = ((ACAudioStkEngineRendererPlugin *) userData)->loops;
    register StkFloat *samples = (StkFloat *) outputBuffer;
    //if(outputBuffer == 0){
    //    std::cerr << "ACAudioStkEngineRenderer: output buffer not available" << std::endl;
    //    return 0;
    //}

    if(inputs.size()==0 && loops.size()==0){
        *samples++ = 0;
        return 1;
    }

    double rate = 1.0;
    rate = (float)((ACAudioStkEngineRendererPlugin *) userData)->getNumberParameterValue("Rate");
    int outputChannels = ((ACAudioStkEngineRendererPlugin *) userData)->outputChannels();

    for(std::map <long int, FileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
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

    for(std::map <long int, FileLoop*>::iterator loop = loops.begin(); loop != loops.end(); loop++){
        if(loop->second && (((ACAudioStkEngineRendererPlugin *) userData)->frames[loop->first])){
            loop->second->tick( *(((ACAudioStkEngineRendererPlugin *) userData)->frames[loop->first]) );
            ((ACAudioStkEngineRendererPlugin *) userData)->justReadFrames(loop->first, (int)(rate * loop->second->channelsOut() * nBufferFrames));
        }
    }

    if( ((ACAudioStkEngineRendererPlugin *) userData)->frames.size()>0){
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
            for(std::map <long int, FileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
                if( ((ACAudioStkEngineRendererPlugin *) userData)->frames[input->first]!=0 && ((ACAudioStkEngineRendererPlugin *) userData)->frames[input->first]->size()>i){
                    int fileChannels = input->second->channelsOut();
                    for ( unsigned int c=0; c<outputChannels; c++ ){
                        frame[c] += (*((ACAudioStkEngineRendererPlugin *) userData)->frames[input->first])[i*fileChannels+(c%fileChannels)];
                    }
                    count++;
                }
            }
            for(std::map <long int, FileLoop*>::iterator loop = loops.begin(); loop != loops.end(); loop++){
                if( ((ACAudioStkEngineRendererPlugin *) userData)->frames[loop->first]!=0 && ((ACAudioStkEngineRendererPlugin *) userData)->frames[loop->first]->size()>i){
                    int fileChannels = loop->second->channelsOut();
                    for ( unsigned int c=0; c<outputChannels; c++ ){
                        frame[c] += (*((ACAudioStkEngineRendererPlugin *) userData)->frames[loop->first])[i*fileChannels+(c%fileChannels)];
                    }
                    count++;
                }
            }
            for ( unsigned int c=0; c<outputChannels; c++ ){
                *samples++ = frame[c]; // / count;
            }
        }
    }
    bool finished = true;
    for(std::map <long int, FileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
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
    done = true;

    for(std::map <long int, FileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
        if(media_cycle)
            media_cycle->getBrowser()->toggleSourceActivity(input->first,0);
        inputs.erase(input->first);
    }
    inputs.clear();
    for(std::map <long int, FileLoop*>::iterator loop = loops.begin(); loop != loops.end(); loop++){
        if(media_cycle)
            media_cycle->getBrowser()->toggleSourceActivity(loop->first,0);
        loops.erase(loop->first);

    }
    loops.clear();
    current_frames.clear();
    if(this->media_cycle)
        media_cycle->muteAllSources();
}

void ACAudioStkEngineRendererPlugin::updateVolume(){
    //this->getNumberParameterValue("Volume")/100.0f );
}

void ACAudioStkEngineRendererPlugin::updateRate(){
    float rate = this->getNumberParameterValue("Rate");
    for(std::map <long int, FileWvIn*>::iterator input = inputs.begin(); input != inputs.end(); input++){
        if(input->second)
            input->second->setRate(rate);
    }
    for(std::map <long int, FileLoop*>::iterator loop = loops.begin(); loop != loops.end(); loop++){
        if(loop->second)
            loop->second->setRate(rate);
    }
}

std::map<std::string,ACMediaType> ACAudioStkEngineRendererPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string,ACMediaType> extensions;
    return extensions;
}

int ACAudioStkEngineRendererPlugin::outputChannels(){
    return 2; // for now
}

bool ACAudioStkEngineRendererPlugin::performActionOnMedia(std::string action, long int mediaId, std::string value){
    std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: action " << action << " mediaId " << mediaId << " value " << value << std::endl;
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

    if(action == "loop" || action == "play"){
        std::string filename = media_cycle->getMediaFileName(mediaId);
        int channels = 0;
        // Try to load the soundfile.
        if(action == "play"){
            FileWvIn* input = 0;
            try {
                input = new FileWvIn();
                input->openFile( filename.c_str() );
                //input->ignoreSampleRateChange();
            }
            catch ( StkError & ) {
                std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: play: couldn't load media " << mediaId << " file " << filename << std::endl;
                return false;
            }

            // Set input read rate based on the default STK sample rate.
            double rate = 1.0;
            rate = this->getNumberParameterValue("Rate");//input->getFileRate() / Stk::sampleRate();
            //if ( argc == 4 ) rate *= atof( argv[3] );
            input->setRate( rate );

            // Find out how many channels we have.
            channels = input->channelsOut();
            std::cout << "ACAudioStkEngineRendererPlugin::performActionOnMedia: play: file has " << input->channelsOut() << " channels " << std::endl;

            inputs[mediaId] = input;
        }
        else if(action == "loop"){

            FileLoop* input = 0;
            try {
                input = new FileLoop();
                input->openFile( filename.c_str() );
            }
            catch ( StkError & ) {
                std::cerr << "ACAudioStkEngineRendererPlugin::performActionOnMedia: loop: couldn't load media " << mediaId << " file " << filename << std::endl;
                return false;
            }

            // Set input read rate based on the default STK sample rate.
            double rate = 1.0;
            rate = this->getNumberParameterValue("Rate");//input->getFileRate() / Stk::sampleRate();
            //if ( argc == 4 ) rate *= atof( argv[3] );
            input->setRate( rate );

            // Find out how many channels we have.
            channels = input->channelsOut();

            loops[mediaId] = input;
        }

        done = false;

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
        if(frames.find(mediaId)!=frames.end()){
            if(frames[mediaId]!=0)
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
    return false;
}

std::map<std::string,ACMediaType> ACAudioStkEngineRendererPlugin::availableMediaActions(){
    std::map<std::string,ACMediaType> media_actions;
    media_actions["play"] = MEDIA_TYPE_AUDIO;
    media_actions["loop"] = MEDIA_TYPE_AUDIO;
    media_actions["mute all"] = MEDIA_TYPE_AUDIO;
    return media_actions;
}

void ACAudioStkEngineRendererPlugin::playClickedNode(){
    if(!media_cycle)
        return;
    int media_id = media_cycle->getClickedNode();
    media_cycle->performActionOnMedia("play", media_id,"");
}

void ACAudioStkEngineRendererPlugin::loopClickedNode(){
    if(!media_cycle)
        return;
    int media_id = media_cycle->getClickedNode();
    media_cycle->performActionOnMedia("loop", media_id,"");
}

void ACAudioStkEngineRendererPlugin::muteAllNodes(){
    if(!media_cycle)
        return;
    media_cycle->performActionOnMedia("mute all", -1,"");
}


std::vector<ACInputActionQt*> ACAudioStkEngineRendererPlugin::providesInputActions(){
    std::vector<ACInputActionQt*> inputActions;
    inputActions.push_back(playClickedNodeAction);
    inputActions.push_back(loopClickedNodeAction);
    inputActions.push_back(muteAllNodesAction);
    return inputActions;
}

