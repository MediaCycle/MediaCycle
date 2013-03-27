/*
 *  ACAudioEngineRendererPlugin.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 14/10/12
 *  @copyright (c) 2012 – UMONS - Numediart
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

#include "ACAudioEngineRendererPlugin.h"
#include<MediaCycle.h>

using namespace std;

ACAudioEngineRendererPlugin::ACAudioEngineRendererPlugin() : QObject(), ACPluginQt(), ACMediaRendererPlugin(){
    this->mName = "Audio Engine";
    this->mDescription ="Plugin for playing audio files with OpenAL or PortAudio";
    this->mMediaType = MEDIA_TYPE_AUDIO;

    audio_engine = 0;

#ifdef USE_DEBUG
    stringToAudioEngineSynchroModeMap::const_iterator synchro_mode = stringToAudioEngineSynchroMode.begin();
    for(;synchro_mode!=stringToAudioEngineSynchroMode.end();++synchro_mode){
        //std::cout << "Synchro Mode: " << synchro_mode->first << std::endl;
        synchro_modes.push_back(synchro_mode->first);
    }
#else
    synchro_modes.push_back("None");
    synchro_modes.push_back("AutoBeat");
#endif

#ifdef USE_DEBUG
    stringToAudioEngineScaleModeMap::const_iterator scale_mode = stringToAudioEngineScaleMode.begin();
    for(;scale_mode!=stringToAudioEngineScaleMode.end();++scale_mode){
        //std::cout << "Scale Mode: " << scale_mode->first << std::endl;
        scale_modes.push_back(scale_mode->first);
    }
#else
    scale_modes.push_back("None");
    scale_modes.push_back("Vocode");
#endif

    presets.push_back("High-Fidelity");
    presets.push_back("Sync Looping");

    //this->addStringParameter("Preset");

    this->action_parameters["play"] = ACMediaActionParameters();
    this->action_parameters["loop"] = ACMediaActionParameters();
    this->action_parameters["mute all"] = ACMediaActionParameters();

    this->addNumberParameter("Volume",100,1,100,1,"Main volume",boost::bind(&ACAudioEngineRendererPlugin::updateVolume,this));
    this->action_parameters["gain"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Volume")));

    this->addNumberParameter("BPM",100,1,240,1,"BPM",boost::bind(&ACAudioEngineRendererPlugin::updateBPM,this));
    this->action_parameters["pitch"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("BPM")));

    this->addStringParameter("Scale Mode","None",scale_modes,"Scale Mode",boost::bind(&ACAudioEngineRendererPlugin::updateScaleMode,this));
    this->action_parameters["scale mode"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Scale Mode")));

    this->addStringParameter("Synchro Mode","None",synchro_modes,"Synchro Mode",boost::bind(&ACAudioEngineRendererPlugin::updateSynchroMode,this));
    this->action_parameters["synchro mode"] = ACMediaActionParameters(1,ACMediaActionParameter(AC_TYPE_FLOAT,this->getParameter("Synchro Mode")));

    loopClickedNodeAction = new ACInputActionQt(tr("Loop clicked node"), this);
    loopClickedNodeAction->setShortcut(Qt::Key_L);
    loopClickedNodeAction->setKeyEventType(QEvent::KeyPress);
    loopClickedNodeAction->setMouseEventType(QEvent::MouseButtonRelease);
    loopClickedNodeAction->setToolTip(tr("Loop the clicked node"));
    connect(loopClickedNodeAction, SIGNAL(triggered()), this, SLOT(loopClickedNode()));

    /*playClickedNodeAction = new ACInputActionQt(tr("Play clicked node"), this);
    //playClickedNodeAction->setShortcut(Qt::Key_P);
    //playClickedNodeAction->setKeyEventType(QEvent::KeyPress);
    playClickedNodeAction->setMouseEventType(QEvent::MouseButtonRelease);
    playClickedNodeAction->setToolTip(tr("Play the clicked node"));
    connect(playClickedNodeAction, SIGNAL(triggered()), this, SLOT(playClickedNode()));*/

    muteAllNodesAction = new ACInputActionQt(tr("Mute all"), this);
    muteAllNodesAction->setShortcut(Qt::Key_M);
    muteAllNodesAction->setKeyEventType(QEvent::KeyPress);
    //muteAllNodesAction->setMouseEventType(QEvent::MouseButtonRelease);
    muteAllNodesAction->setToolTip(tr("Mute all"));
    connect(muteAllNodesAction, SIGNAL(triggered()), this, SLOT(muteAllNodes()));
}

void ACAudioEngineRendererPlugin::mediaCycleSet(){

    if (audio_engine) {
        audio_engine->stopAudioEngine();
        delete audio_engine;
    }
    audio_engine = new ACAudioEngine();
    audio_engine->setMediaCycle(media_cycle);

    if(this->hasCallbackNamed("Mute"))
        this->updateCallback("Mute","Mute",boost::bind(&MediaCycle::muteAllSources,this->media_cycle));
    else
        this->addCallback("Mute","Mute",boost::bind(&MediaCycle::muteAllSources,this->media_cycle));

    //media_cycle->setAutoPlay(1);//Seneffe
    audio_engine->startAudioEngine();
}

ACAudioEngineRendererPlugin::~ACAudioEngineRendererPlugin(){
    if (audio_engine) {
        audio_engine->stopAudioEngine();
        usleep(10000);
        delete audio_engine;
    }
    audio_engine = 0;
}

void ACAudioEngineRendererPlugin::updateBPM(){
    if(audio_engine)
        audio_engine->setBPM( this->getNumberParameterValue("BPM") );
}

void ACAudioEngineRendererPlugin::updateVolume(){
    if(audio_engine)
        audio_engine->setGain( this->getNumberParameterValue("Volume")/100.0f );
}

void ACAudioEngineRendererPlugin::updateSynchroMode(){
    if(!audio_engine)
        return;
    stringToAudioEngineSynchroModeMap::const_iterator iterm = stringToAudioEngineSynchroMode.find(this->getStringParameterValue("Synchro Mode"));
    if(iterm==stringToAudioEngineSynchroMode.end())
        return;
    audio_engine->updateSynchroMode( iterm->second );
}

void ACAudioEngineRendererPlugin::updateScaleMode(){
    if(!audio_engine)
        return;
    stringToAudioEngineScaleModeMap::const_iterator iterm = stringToAudioEngineScaleMode.find(this->getStringParameterValue("Scale Mode"));
    if(iterm==stringToAudioEngineScaleMode.end())
        return;
    audio_engine->updateScaleMode( iterm->second );
}

void ACAudioEngineRendererPlugin::updatePreset(){

}

std::map<std::string,ACMediaType> ACAudioEngineRendererPlugin::getSupportedExtensions(ACMediaType media_type){
    std::map<std::string,ACMediaType> extensions;
    return extensions;
}

bool ACAudioEngineRendererPlugin::performActionOnMedia(std::string action, long int mediaId, std::vector<boost::any> arguments){
    //std::cout << "ACAudioEngineRendererPlugin::performActionOnMedia: action " << action << " mediaId " << mediaId << std::endl;
    if(!media_cycle){
        std::cerr << "ACAudioEngineRendererPlugin::performActionOnMedia: mediacycle instance not set" << std::endl;
        return false;
    }

    if(!audio_engine){
        std::cerr << "ACAudioEngineRendererPlugin::performActionOnMedia: audioengine not created" << std::endl;
        return false;
    }

    if(action == "mute all"){
        this->muteAll();
        return true;
    }

    if(mediaId == -1){
        std::cerr << "ACAudioEngineRendererPlugin::performActionOnMedia: media id " << mediaId << " not available" << std::endl;
        return false;
    }

    if(action.find("mode", 0) != string::npos){

        if(arguments.size() !=1){
            std::cerr << "ACAudioEngineRendererPlugin::performActionOnMedia: action " << action << " requires 1 string argument" << std::endl;
            return false;
        }
        std::string new_value("");
        try{
            new_value = boost::any_cast<std::string>(arguments[0]);
        }
        catch(const boost::bad_any_cast &){
            std::cerr << "ACAudioEngineRendererPlugin::performActionOnMedia: couldn't convert parameter to std::string for action " << action << " , aborting..."<< std::endl;
            return false;
        }

        if(action == "scale mode"){
            if(!audio_engine)
                return false;
            stringToAudioEngineScaleModeMap::const_iterator iterm = stringToAudioEngineScaleMode.find(new_value);
            if(iterm==stringToAudioEngineScaleMode.end())
                return false;
            audio_engine->setLoopScaleMode(mediaId, iterm->second);
        }
        else if(action == "synchro mode"){
            if(!audio_engine)
                return false;
            stringToAudioEngineSynchroModeMap::const_iterator iterm = stringToAudioEngineSynchroMode.find(new_value);
            if(iterm==stringToAudioEngineSynchroMode.end())
                return false;
            audio_engine->setLoopSynchroMode(mediaId, iterm->second);
        }
    }
    else{

        if(arguments.size() !=1){
            std::cerr << "ACAudioEngineRendererPlugin::performActionOnMedia: action " << action << " requires 1 float argument" << std::endl;
            return false;
        }
        float new_value = 0.0f;
        try{
            new_value = boost::any_cast<float>(arguments[0]);
        }
        catch(const boost::bad_any_cast &){
            std::cerr << "ACAudioEngineRendererPlugin::performActionOnMedia: couldn't convert parameter to float for action " << action << " , aborting..."<< std::endl;
            return false;
        }

        if(action == "scrub"){
            audio_engine->setScrub(new_value);
        }
        else if(action == "pitch"){
            audio_engine->setSourcePitch(mediaId, new_value);
        }
        else if(action == "gain"){
            audio_engine->setSourceGain(mediaId, new_value);
        }
        /*else if(action == "loop" || action == "play"){
        media_cycle->setNeedsActivityUpdateMedia();
    }*/
    }
    return true;
}

std::map<std::string,ACMediaType> ACAudioEngineRendererPlugin::availableMediaActions(){
    std::map<std::string,ACMediaType> media_actions;
    media_actions["scale mode"] = MEDIA_TYPE_AUDIO;
    media_actions["synchro mode"] = MEDIA_TYPE_AUDIO;
    media_actions["scrub"] = MEDIA_TYPE_AUDIO;
    media_actions["pitch"] = MEDIA_TYPE_AUDIO;
    media_actions["gain"] = MEDIA_TYPE_AUDIO;
    media_actions["play"] = MEDIA_TYPE_AUDIO;
    media_actions["loop"] = MEDIA_TYPE_AUDIO;
    media_actions["mute all"] = MEDIA_TYPE_AUDIO;
    return media_actions;
}

std::map<std::string,ACMediaActionParameters> ACAudioEngineRendererPlugin::mediaActionsParameters(){
    return this->action_parameters;
}

void ACAudioEngineRendererPlugin::playClickedNode(){
    if(!media_cycle)
        return;
    int media_id = media_cycle->getClickedNode();
    media_cycle->performActionOnMedia("play", media_id);
}

void ACAudioEngineRendererPlugin::loopClickedNode(){
    if(!media_cycle)
        return;
    int media_id = media_cycle->getClickedNode();
    media_cycle->performActionOnMedia("loop", media_id);
}

void ACAudioEngineRendererPlugin::muteAllNodes(){
    if(!media_cycle)
        return;
    media_cycle->performActionOnMedia("mute all", -1);
    media_cycle->muteAllSources();
}


std::vector<ACInputActionQt*> ACAudioEngineRendererPlugin::providesInputActions(){
    std::vector<ACInputActionQt*> inputActions;
    //inputActions.push_back(playClickedNodeAction);
    //inputActions.push_back(loopClickedNodeAction);
    inputActions.push_back(muteAllNodesAction);
    return inputActions;
}

void ACAudioEngineRendererPlugin::muteAll(){
    if(this->media_cycle)
        media_cycle->muteAllSources();

}
