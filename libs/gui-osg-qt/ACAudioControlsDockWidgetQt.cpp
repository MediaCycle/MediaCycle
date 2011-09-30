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

#include "ACAudioControlsDockWidgetQt.h"

ACAudioControlsDockWidgetQt::ACAudioControlsDockWidgetQt(QWidget *parent)
: ACAbstractDockWidgetQt(parent, MEDIA_TYPE_AUDIO,"ACAudioControlsDockWidgetQt")
{
	#if defined (SUPPORT_AUDIO)
	ui.setupUi(this); // first thing to do
	this->show();
	#ifndef USE_DEBUG
	ui.groupBoxQuery->hide();
	#endif
	#endif //defined (SUPPORT_AUDIO)
}

ACAudioControlsDockWidgetQt::~ACAudioControlsDockWidgetQt(){
	#if defined (SUPPORT_AUDIO)
	//delete audio_engine;
	//delete media_cycle;
	#endif //defined (SUPPORT_AUDIO)
}

#if defined (SUPPORT_AUDIO)
void ACAudioControlsDockWidgetQt::on_pushButtonMuteAll_clicked()
{
	if (media_cycle){
		media_cycle->resetPointers();//CF hack dirty
		media_cycle->muteAllSources();
	}
}

void ACAudioControlsDockWidgetQt::on_pushButtonQueryRecord_toggled()
{
	if (audio_engine){
		if (ui.pushButtonQueryRecord->isChecked() == 1)
		{
			if (audio_engine->isCaptureAvailable())
			{
				ui.pushButtonQueryReplay->setEnabled(false);
				ui.pushButtonQueryKeep->setEnabled(false);
				ui.pushButtonQueryReferent->setEnabled(false);
				std::cout <<"Recording..."<<std::endl;
				// CF There is a delay before the recording actually starts: work around with a countdown on a modal window?
				audio_engine->startCapture();
			}
			else
				ui.pushButtonQueryRecord->setChecked(false);
		}
		else
		{
			if (audio_engine->isCaptureAvailable())
			{
				audio_engine->stopCapture();
				std::cout <<"Recording done."<<std::endl;
				ui.pushButtonQueryReplay->setEnabled(true);
				ui.pushButtonQueryKeep->setEnabled(true);
				ui.pushButtonQueryReferent->setEnabled(true);
			}
		}
	}
}

void ACAudioControlsDockWidgetQt::on_pushButtonQueryReplay_clicked()
{

}

// Pops up a modal window for saving the query as wavefile and add it to the library
void ACAudioControlsDockWidgetQt::on_pushButtonQueryKeep_clicked()
{
	//...

	//CF then only re-recording can re-enable the keeping
	ui.pushButtonQueryKeep->setEnabled(false);
}

void ACAudioControlsDockWidgetQt::on_pushButtonQueryReferent_clicked()
{

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
