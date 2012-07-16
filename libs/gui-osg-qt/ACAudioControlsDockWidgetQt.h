/*
 *  ACAudioControlsDockWidgetQt.h
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

#ifndef HEADER_ACAUDIOCONTROLSDOCKWIDGETQT
#define HEADER_ACAUDIOCONTROLSDOCKWIDGETQT

#include <iostream>
#include <string.h>

#include "ACAbstractDockWidgetQt.h"

#include "ui_ACAudioControlsDockWidgetQt.h"
#include <MediaCycle.h>

class ACAudioControlsDockWidgetQt : public ACAbstractDockWidgetQt {
Q_OBJECT

//#if defined (SUPPORT_AUDIO) // don't use it!
private slots:
	// Audio controls
	void on_pushButtonMuteAll_clicked();
	
	// Audio visualization
	void on_comboBoxWaveformBrowser_activated(const QString & text);//CF or (int index);}

	// Audio playback
	void on_checkBoxSynchroMode_stateChanged(int state);
	void on_checkBoxScaleMode_stateChanged(int state);
	void on_comboBoxSynchroMode_activated(const QString & text);//CF or (int index);}
	void on_comboBoxScaleMode_activated(const QString & text);//CF or (int index);}
	void on_comboBoxPlaybackPreset_activated(const QString & text);
        void on_spinBoxBPM_valueChanged(int value);
        void on_horizontalSliderBPM_valueChanged(int value);
	
//#endif //defined (SUPPORT_AUDIO)

public:
	ACAudioControlsDockWidgetQt(QWidget *parent = 0);
	~ACAudioControlsDockWidgetQt();
        virtual bool canBeVisible(ACMediaType _media_type);

#if defined (SUPPORT_AUDIO)
	void setComboBoxWaveformBrowser(ACBrowserAudioWaveformType _type);
	void initFeedbackModes();
	void setAudioEngine(ACAudioEngine* _audio_engine);

private:
	Ui::ACAudioControlsDockWidgetQt ui;
#endif //defined (SUPPORT_AUDIO)	
};
#endif
