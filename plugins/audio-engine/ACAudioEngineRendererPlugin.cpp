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

ACAudioEngineRendererPlugin::ACAudioEngineRendererPlugin() : ACMediaRendererPlugin(){
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

}

ACAudioEngineRendererPlugin::~ACAudioEngineRendererPlugin(){
    if (audio_engine) {
        audio_engine->stopAudioEngine();
        usleep(10000);
        delete audio_engine;
    }
    audio_engine = 0;
}

void ACAudioEngineRendererPlugin::setMediaCycle(MediaCycle* _media_cycle){
    this->media_cycle=_media_cycle;

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

    if(this->hasNumberParameterNamed("Volume"))
        this->updateNumberParameter("Volume",100,1,100,1,"Main volume",boost::bind(&ACAudioEngineRendererPlugin::updateVolume,this));
    else
        this->addNumberParameter("Volume",100,1,100,1,"Main volume",boost::bind(&ACAudioEngineRendererPlugin::updateVolume,this));

    if(this->hasNumberParameterNamed("BPM"))
        this->updateNumberParameter("BPM",100,1,240,1,"BPM",boost::bind(&ACAudioEngineRendererPlugin::updateBPM,this));
    else
        this->addNumberParameter("BPM",100,1,240,1,"BPM",boost::bind(&ACAudioEngineRendererPlugin::updateBPM,this));

    if(!this->hasStringParameterNamed("Scale Mode"))
        this->addStringParameter("Scale Mode","None",scale_modes,"Scale Mode",boost::bind(&ACAudioEngineRendererPlugin::updateScaleMode,this));
    if(!this->hasStringParameterNamed("Synchro Mode"))
        this->addStringParameter("Synchro Mode","None",synchro_modes,"Synchro Mode",boost::bind(&ACAudioEngineRendererPlugin::updateSynchroMode,this));

    //this->addStringParameter("Preset");

    //media_cycle->setAutoPlay(1);//Seneffe
    audio_engine->startAudioEngine();
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

bool ACAudioEngineRendererPlugin::performActionOnMedia(std::string action, long int mediaId, std::string value){
    std::cout << "ACAudioEngineRendererPlugin::performActionOnMedia: action " << action << " mediaId " << mediaId << " value " << value << std::endl;
    if(!media_cycle){
        std::cerr << "ACAudioEngineRendererPlugin::performActionOnMedia: mediacycle instance not set" << std::endl;
        return false;
    }

    if(!audio_engine){
        std::cerr << "ACAudioEngineRendererPlugin::performActionOnMedia: audioengine not created" << std::endl;
        return false;
    }
    /*if(mediaId == -1){
        std::cerr << "ACAudioEngineRendererPlugin::performActionOnMedia: media id " << mediaId << " not available" << std::endl;
        return false;
    }*/
    //mediaId = media_cycle->getClickedNode();

    if(action == "scale mode"){
        if(!audio_engine)
            return false;
        stringToAudioEngineScaleModeMap::const_iterator iterm = stringToAudioEngineScaleMode.find(value);
        if(iterm==stringToAudioEngineScaleMode.end())
            return false;
        audio_engine->setLoopScaleMode(mediaId, iterm->second);
    }
    else if(action == "synchro mode"){
        if(!audio_engine)
            return false;
        stringToAudioEngineSynchroModeMap::const_iterator iterm = stringToAudioEngineSynchroMode.find(value);
        if(iterm==stringToAudioEngineSynchroMode.end())
            return false;
        audio_engine->setLoopSynchroMode(mediaId, iterm->second);
    }
    else if(action == "scrub"){
        double scrub = atof(value.c_str());
        audio_engine->setScrub(scrub);
    }
    else if(action == "pitch"){
        double pitch = atof(value.c_str());
        audio_engine->setSourcePitch(mediaId, pitch);
    }
    else if(action == "gain"){
        double gain = atof(value.c_str());
        audio_engine->setSourceGain(mediaId, gain);
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
    return media_actions;
}

/*void ACAudioControlsDockWidgetQt::on_pushButtonMuteAll_clicked()
{
    if (media_cycle){
        media_cycle->resetPointers();//CF hack dirty
        media_cycle->muteAllSources();
    }
}*/

/*void ACAudioControlsDockWidgetQt::on_comboBoxWaveformBrowser_activated(const QString & text)
{
    if (media_cycle == 0) return;
    std::cout << "Browser waveform type: " << text.toStdString() << std::endl;
    if (text == "None")
        osg_view->getBrowserRenderer()->setAudioWaveformType(AC_BROWSER_AUDIO_WAVEFORM_NONE);
    else if (text == "Classic")
        osg_view->getBrowserRenderer()->setAudioWaveformType(AC_BROWSER_AUDIO_WAVEFORM_CLASSIC);
    osg_view->setFocus();
}

void ACAudioControlsDockWidgetQt::setComboBoxWaveformBrowser(ACBrowserAudioWaveformType _type)
{
    if (_type==AC_BROWSER_AUDIO_WAVEFORM_NONE){
        ui.comboBoxWaveformBrowser->setCurrentIndex(0);
        on_comboBoxWaveformBrowser_activated("None");
    }
    else {
        ui.comboBoxWaveformBrowser->setCurrentIndex(1);
        on_comboBoxWaveformBrowser_activated("Classic");
    }

}

void ACAudioControlsDockWidgetQt::on_checkBoxSynchroMode_stateChanged(int state)
{
    if (media_cycle == 0 && audio_engine == 0) return;
    audio_engine->forceDefaultSynchroMode((bool)(state));
}

void ACAudioControlsDockWidgetQt::on_checkBoxScaleMode_stateChanged(int state)
{
    if (media_cycle == 0 && audio_engine == 0) return;
    audio_engine->forceDefaultScaleMode((bool)(state));
}

void ACAudioControlsDockWidgetQt::on_comboBoxSynchroMode_activated(const QString & text)
{
    if (media_cycle == 0 && audio_engine == 0) return;
    std::cout << "Synchro Mode: " << text.toStdString() << std::endl;
    stringToAudioEngineSynchroModeMap::const_iterator iterm = stringToAudioEngineSynchroMode.find(text.toStdString());
    if (iterm != stringToAudioEngineSynchroMode.end())
        audio_engine->setDefaultSynchroMode(iterm->second);
}

void ACAudioControlsDockWidgetQt::on_comboBoxScaleMode_activated(const QString & text)
{
    if (media_cycle == 0 && audio_engine == 0) return;
    std::cout << "Scale Mode: " << text.toStdString() << std::endl;
    stringToAudioEngineScaleModeMap::const_iterator iterm = stringToAudioEngineScaleMode.find(text.toStdString());
    if (iterm != stringToAudioEngineScaleMode.end())
        audio_engine->setDefaultScaleMode(iterm->second);
}

void ACAudioControlsDockWidgetQt::on_comboBoxPlaybackPreset_activated(const QString & text)
{
    if(text == "Sync Looping"){
        on_comboBoxSynchroMode_activated("AutoBeat");
        on_comboBoxScaleMode_activated("Vocode");
        ui.comboBoxScaleMode->setCurrentIndex(ui.comboBoxScaleMode->findText("Vocode"));
        ui.comboBoxSynchroMode->setCurrentIndex(ui.comboBoxSynchroMode->findText("AutoBeat"));
        on_checkBoxSynchroMode_stateChanged(0);
        on_checkBoxScaleMode_stateChanged(0);
        ui.checkBoxSynchroMode->setChecked(false);
        ui.checkBoxScaleMode->setChecked(false);
        ui.comboBoxScaleMode->setEnabled(false);
        ui.checkBoxScaleMode->setEnabled(false);
        ui.comboBoxSynchroMode->setEnabled(false);
        ui.checkBoxSynchroMode->setEnabled(false);
        ui.horizontalSliderBPM->setEnabled(true);
        ui.spinBoxBPM->setEnabled(true);

        ui.comboBoxScaleMode->hide();
                ui.checkBoxScaleMode->hide();
                ui.labelScaleMode->hide();
                ui.comboBoxSynchroMode->hide();
                ui.checkBoxSynchroMode->hide();
                ui.labelSynchroMode->hide();
                ui.labelForce->hide();
                ui.groupBoxModes->adjustSize();
                ui.dockWidgetContents->adjustSize();
    }
    else if(text == "High-Fidelity"){
        on_comboBoxSynchroMode_activated("None");
        on_comboBoxScaleMode_activated("None");
        ui.comboBoxScaleMode->setCurrentIndex(ui.comboBoxScaleMode->findText("None"));
        ui.comboBoxSynchroMode->setCurrentIndex(ui.comboBoxSynchroMode->findText("None"));
        on_checkBoxSynchroMode_stateChanged(1);
        on_checkBoxScaleMode_stateChanged(1);
        ui.checkBoxSynchroMode->setChecked(true);
        ui.checkBoxScaleMode->setChecked(true);
        ui.comboBoxScaleMode->setEnabled(false);
        ui.checkBoxScaleMode->setEnabled(false);
        ui.comboBoxSynchroMode->setEnabled(false);
        ui.checkBoxSynchroMode->setEnabled(false);
        ui.horizontalSliderBPM->setEnabled(false);
        ui.spinBoxBPM->setEnabled(false);

//        ui.comboBoxScaleMode->hide();
//        ui.checkBoxScaleMode->hide();
//        ui.labelScaleMode->hide();
//        ui.comboBoxSynchroMode->hide();
//        ui.checkBoxSynchroMode->hide();
//        ui.labelSynchroMode->hide();
//        ui.labelForce->hide();
//        ui.groupBoxModes->adjustSize();
//        ui.dockWidgetContents->adjustSize();
    }
    else if(text == "Custom"){
        ui.comboBoxScaleMode->setEnabled(true);
        ui.checkBoxScaleMode->setEnabled(true);
        ui.comboBoxSynchroMode->setEnabled(true);
        ui.checkBoxSynchroMode->setEnabled(true);
        ui.horizontalSliderBPM->setEnabled(true);
        ui.spinBoxBPM->setEnabled(true);

//        ui.comboBoxScaleMode->show();
//        ui.checkBoxScaleMode->show();
//        ui.labelScaleMode->show();
//        ui.comboBoxSynchroMode->show();
//        ui.checkBoxSynchroMode->show();
//        ui.labelSynchroMode->show();
//        ui.labelForce->show();
//        ui.groupBoxModes->adjustSize();
//        ui.dockWidgetContents->adjustSize();
    }
}

void ACAudioControlsDockWidgetQt::setAudioEngine(ACAudioEngine* _audio_engine)
{ 
    audio_engine = _audio_engine;
    if(audio_engine){
        audio_engine->forceDefaultSynchroMode(ui.checkBoxSynchroMode->isChecked());
        audio_engine->forceDefaultScaleMode(ui.checkBoxScaleMode->isChecked());
        stringToAudioEngineSynchroModeMap::const_iterator synchro_mode = stringToAudioEngineSynchroMode.find(ui.comboBoxSynchroMode->currentText().toStdString());
        if (synchro_mode != stringToAudioEngineSynchroMode.end())
            audio_engine->setDefaultSynchroMode(synchro_mode->second);
        stringToAudioEngineScaleModeMap::const_iterator scale_mode = stringToAudioEngineScaleMode.find(ui.comboBoxScaleMode->currentText().toStdString());
        if (scale_mode != stringToAudioEngineScaleMode.end())
            audio_engine->setDefaultScaleMode(scale_mode->second);
    }
}

// XS TODO : recuperate these from Audiocycle ?

//void ACAudioCycleOsgQt::on_sliderBPM_valueChanged() //[0;220]
//{
//	std::cout << "BPM: " << ui.sliderBPM->value() << std::endl;
//	//if (library_loaded){
//	int node = media_cycle->getClickedNode();
//	if (node > -1)
//	{
//		audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeAutoBeat);
//		audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);
//		audio_engine->setBPM(ui.sliderBPM->value());
//	}
//	//}
//	//ui.compositeOsgView->setFocus();
//}
//
//void ACAudioCycleOsgQt::on_sliderPitch_valueChanged() // [50;200]
//{
//	std::cout << "Pitch: " << (float) ui.sliderPitch->value()/100.0f << std::endl;
//	//if (library_loaded){
//	int node = media_cycle->getClickedNode();
//	if (node > -1)
//	{
//		audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeAutoBeat);
//		audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);
//		audio_engine->setSourcePitch(node, (float) ui.sliderPitch->value()/100.0f);
//	}
//	//}
//	//ui.compositeOsgView->setFocus();
//}
//void ACAudioCycleOsgQt::on_pushButtonQueryRecord_toggled()
//{
//	if (ui.pushButtonQueryRecord->isChecked() == 1)
//	{
//		if (audio_engine->isCaptureAvailable())
//		{
//			ui.pushButtonQueryReplay->setEnabled(false);
//			ui.pushButtonQueryKeep->setEnabled(false);
//			ui.pushButtonQueryReferent->setEnabled(false);
//			std::cout <<"Recording..."<<std::endl;
//			// CF There is a delay before the recording actually starts: work around with a countdown on a modal window?
//			audio_engine->startCapture();
//		}
//		else
//			ui.pushButtonQueryRecord->setChecked(true);
//	}
//	else
//	{
//		if (audio_engine->isCaptureAvailable())
//		{
//			audio_engine->stopCapture();
//			std::cout <<"Recording done."<<std::endl;
//			ui.pushButtonQueryReplay->setEnabled(true);
//			ui.pushButtonQueryKeep->setEnabled(true);
//			ui.pushButtonQueryReferent->setEnabled(true);
//		}
//	}
//}
*/
