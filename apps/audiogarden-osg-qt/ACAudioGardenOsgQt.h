/*
 *  ACAudioGardenOsgQt.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 27/04/10
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

#ifndef HEADER_ACAUDIOGARDENOSGQT
#define HEADER_ACAUDIOGARDENOSGQT

#include <iostream>
#include <string.h>
using namespace std;

#include <QtGui>
#include "ui_ACAudioGardenOsgQt.h"
#include <AGOsgCompositeViewQt.h>
#include <MediaCycle.h>
#include <ACAudioEngine.h>
#include <ACOscBrowser.h>
#include <ACOscFeedback.h>
#ifdef USE_APPLE_MULTITOUCH
	#include <ACAppleMultitouchTrackpadSupport.h>
#endif

class ACAudioGardenOsgQt : public QMainWindow
{
    Q_OBJECT
	
    public:
        ACAudioGardenOsgQt(QWidget *parent = 0);
		~ACAudioGardenOsgQt();
		void updateLibrary();

    private slots:
	
		// Library controls
		void on_pushButtonLaunch_clicked();
		void on_pushButtonClean_clicked();
		void loadACLFile();
		void saveACLFile();
		void loadMCSLFile();
		void saveMCSLFile();
		void loadMediaDirectory();
		void loadMediaFiles();
	
		// Browser controls
		void on_pushButtonRecenter_clicked();
		void on_pushButtonBack_clicked();
		void on_pushButtonForward_clicked();
	
		// Clustering controls
		void on_checkBoxRhythm_stateChanged(int state);
		void on_checkBoxTimbre_stateChanged(int state);
		void on_checkBoxHarmony_stateChanged(int state);
		void on_sliderClusters_sliderReleased();
		void on_comboBoxClustersMethod_activated(const QString & text);//CF or (int index);} 
		void on_comboBoxClustersPositions_activated(const QString & text);//CF or (int index);} 
	
		// Audio controls
		void on_pushButtonMuteAll_clicked();
	
		// Audio query
		void on_pushButtonQueryRecord_toggled();	
		void on_pushButtonQueryReplay_clicked();
		void on_pushButtonQueryKeep_clicked();
	
		// Audio query for compositing
		void on_pushButtonQueryPattern_clicked();
		void on_pushButtonQueryGrain_clicked();
	
		// Compositing controls
		void on_comboBoxCompositingMethod_activated(const QString & text);
		void on_comboBoxCompositingMapping_activated(const QString & text);
		void on_sliderCompositingRandom_sliderReleased();
		void on_sliderCompositingThreshold_sliderReleased();
		void on_checkBoxCompositingAuto_toggled();
		void on_pushButtonCompositingGo_clicked();
		void on_pushButtonCompositingReset_clicked();

		// OSC I/O controls
		void on_pushButtonControlStart_clicked();
		void on_pushButtonFeedbackStart_clicked();
	
	public:
		MediaCycle *media_cycle;
	
    private:
		ACAudioEngine *audio_engine;
		ACOscBrowser *osc_browser;
		ACOscFeedback *osc_feedback;
		#ifdef USE_APPLE_MULTITOUCH
			ACAppleMultitouchTrackpadSupport *multitouch_trackpad;
		#endif
        Ui::ACAudioGardenOsgQt ui;
		bool library_loaded;

	protected:
		//AGOsgCompositeViewQt* compositeOsgView;
	
	public:
		void processOscMessage(const char* tagName);
		ACOscBrowserRef mOscReceiver;
};
#endif
