/*
 *  ACAudioControlsDockWidgetQt.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 8/01/11
 *  @copyright (c) 2011 – UMONS - Numediart
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

#if defined (SUPPORT_AUDIO)
//#include <ACAudioFeedback.h>
#endif// defined (SUPPORT_AUDIO)

#include "ACAudioControlsDockWidgetQt.h"

ACAudioControlsDockWidgetQt::ACAudioControlsDockWidgetQt(QWidget *parent)
: ACAbstractDockWidgetQt(parent, MEDIA_TYPE_AUDIO,"ACAudioControlsDockWidgetQt")
{
	#if defined (SUPPORT_AUDIO)
	ui.setupUi(this); // first thing to do
	
	this->initFeedbackModes();
	
        /*ui.comboBoxScaleMode->hide();
        ui.checkBoxScaleMode->hide();
        ui.labelScaleMode->hide();
        ui.comboBoxSynchroMode->hide();
        ui.checkBoxSynchroMode->hide();
        ui.labelSynchroMode->hide();
        ui.groupBoxModes->adjustSize();
        ui.dockWidgetContents->adjustSize();
        this->setMinimumHeight( ui.groupBoxFeedback->minimumHeight() + ui.groupBoxVisualization->minimumHeight() );
        this->resize(this->sizeHint());*/

        this->show();

	#endif //defined (SUPPORT_AUDIO)
}

ACAudioControlsDockWidgetQt::~ACAudioControlsDockWidgetQt(){
#if defined (SUPPORT_AUDIO)
	//delete audio_engine;
	//delete media_cycle;
#endif //defined (SUPPORT_AUDIO)
}

#if defined (SUPPORT_AUDIO)
void ACAudioControlsDockWidgetQt::initFeedbackModes(){
	ui.comboBoxSynchroMode->clear();
	#ifdef USE_DEBUG
	stringToAudioEngineSynchroModeMap::const_iterator synchro_mode = stringToAudioEngineSynchroMode.begin();
	for(;synchro_mode!=stringToAudioEngineSynchroMode.end();++synchro_mode){
		//std::cout << "Synchro mode: " << synchro_mode->first << std::endl;
		ui.comboBoxSynchroMode->addItem(QString((synchro_mode->first).c_str()));
	}
	#else
	ui.comboBoxSynchroMode->addItem(QString("None"));
	ui.comboBoxSynchroMode->addItem(QString("AutoBeat"));
	#endif
	
	ui.comboBoxScaleMode->clear();
	#ifdef USE_DEBUG
	stringToAudioEngineScaleModeMap::const_iterator scale_mode = stringToAudioEngineScaleMode.begin();
	for(;scale_mode!=stringToAudioEngineScaleMode.end();++scale_mode){
		//std::cout << "Scale mode: " << scale_mode->first << std::endl;
		ui.comboBoxScaleMode->addItem(QString((scale_mode->first).c_str()));
	}
	#else
	ui.comboBoxScaleMode->addItem(QString("None"));
	ui.comboBoxScaleMode->addItem(QString("Vocode"));
	#endif
	
	ui.comboBoxPlaybackPreset->setCurrentIndex(ui.comboBoxPlaybackPreset->findText("Sync Looping"));
	on_comboBoxPlaybackPreset_activated("Sync Looping");
}	

void ACAudioControlsDockWidgetQt::on_pushButtonMuteAll_clicked()
{
	if (media_cycle){
		media_cycle->resetPointers();//CF hack dirty
		media_cycle->muteAllSources();
	}
}

void ACAudioControlsDockWidgetQt::on_comboBoxWaveformBrowser_activated(const QString & text)
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
    std::cout << "Synchro mode: " << text.toStdString() << std::endl;
    stringToAudioEngineSynchroModeMap::const_iterator iterm = stringToAudioEngineSynchroMode.find(text.toStdString());
    if (iterm != stringToAudioEngineSynchroMode.end())
        audio_engine->setDefaultSynchroMode(iterm->second);
}

void ACAudioControlsDockWidgetQt::on_comboBoxScaleMode_activated(const QString & text)
{
    if (media_cycle == 0 && audio_engine == 0) return;
    std::cout << "Scale mode: " << text.toStdString() << std::endl;
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

                /*ui.comboBoxScaleMode->hide();
                ui.checkBoxScaleMode->hide();
                ui.labelScaleMode->hide();
                ui.comboBoxSynchroMode->hide();
                ui.checkBoxSynchroMode->hide();
                ui.labelSynchroMode->hide();
                ui.labelForce->hide();
                ui.groupBoxModes->adjustSize();
                ui.dockWidgetContents->adjustSize();*/
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

                /*ui.comboBoxScaleMode->hide();
                ui.checkBoxScaleMode->hide();
                ui.labelScaleMode->hide();
                ui.comboBoxSynchroMode->hide();
                ui.checkBoxSynchroMode->hide();
                ui.labelSynchroMode->hide();
                ui.labelForce->hide();
                ui.groupBoxModes->adjustSize();
                ui.dockWidgetContents->adjustSize();*/
	}		
	else if(text == "Custom"){
		ui.comboBoxScaleMode->setEnabled(true);
		ui.checkBoxScaleMode->setEnabled(true);
		ui.comboBoxSynchroMode->setEnabled(true);
		ui.checkBoxSynchroMode->setEnabled(true);

                /*ui.comboBoxScaleMode->show();
                ui.checkBoxScaleMode->show();
                ui.labelScaleMode->show();
                ui.comboBoxSynchroMode->show();
                ui.checkBoxSynchroMode->show();
                ui.labelSynchroMode->show();
                ui.labelForce->show();
                ui.groupBoxModes->adjustSize();
                ui.dockWidgetContents->adjustSize();*/
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


#endif //defined (SUPPORT_AUDIO)
