/*
 *  ACAudioCycleOsgQt.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 16/02/10
 *
 *  @copyright (c) 2010 – UMONS - Numediart
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

#ifndef HEADER_ACAUDIOCYCLEOSGQT
#define HEADER_ACAUDIOCYCLEOSGQT

#include <iostream>
#include <string.h>
using namespace std;

#include <QtGui>
#include "ui_ACAudioCycleOsgQt.h"
#include <ACOsgBrowserViewQT.h>
#include <MediaCycleLight.h>
#include <ACAudioFeedback.h>
#include <ACOscBrowser.h>
#include <ACOscFeedback.h>
#ifdef USE_APPLE_MULTITOUCH
	#include <ACAppleMultitouchTrackpadSupport.h>
#endif

class ACAudioCycleOsgQt : public QMainWindow
{
    Q_OBJECT
	
    public:
        ACAudioCycleOsgQt(QWidget *parent = 0);
		~ACAudioCycleOsgQt();
		void updateLibrary();

    private slots:
		void on_pushButtonLaunch_clicked();
		void on_pushButtonMuteAll_clicked();
		void on_pushButtonClean_clicked();
		void on_pushButtonRecenter_clicked();
		void on_pushButtonBack_clicked();
		void on_pushButtonForward_clicked();
		void on_pushButtonControlStart_clicked();
		void on_pushButtonFeedbackStart_clicked();

		void on_checkBoxRhythm_stateChanged(int state);
		void on_checkBoxTimbre_stateChanged(int state);
		void on_checkBoxHarmony_stateChanged(int state);
	
		void on_sliderClusters_sliderReleased();
		void on_sliderBPM_valueChanged(); // or sliderMoved();?
		void on_sliderKey_valueChanged(); // or sliderMoved();?

		void loadACLFile();
		void saveACLFile();
		void loadMediaDirectory();
		void loadMediaFiles();
	
	public:
		MediaCycle *media_cycle;
	
    private:
		ACAudioFeedback *audio_engine;
		ACOscBrowser *osc_browser;
		ACOscFeedback *osc_feedback;
		#ifdef USE_APPLE_MULTITOUCH
			ACAppleMultitouchTrackpadSupport *multitouch_trackpad;
		#endif
        Ui::ACAudioCycleOsgQt ui;
		bool updatedLibrary;

	protected:
		//ACOsgBrowserViewQT* browserOsgView;
	
	public:
		void processOscMessage(const char* tagName);
		ACOscBrowserRef mOscReceiver;
};
#endif
