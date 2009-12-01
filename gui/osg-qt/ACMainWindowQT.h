/**
 * @brief ACMainWindowQT.h
 * @author Christian Frisson
 * @date 01/12/2009
 * @copyright (c) 2009 – UMONS - Numediart
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

#ifndef HEADER_ACMAINWINDOWQT
#define HEADER_ACMAINWINDOWQT

#include <string.h>
using namespace std;

#include <QtGui>
#include "ui_ACMainWindowQT.h"
#include "ACOsgBrowserViewQT.h"
#include <MediaCycleLight.h>
#include <Io/TiOscReceiver.h>

class ACMainWindowQT : public QMainWindow
{
    Q_OBJECT
	
    public:
        ACMainWindowQT(QWidget *parent = 0);
		~ACMainWindowQT();
		//ACOsgBrowserViewQT* getViewerWindow(){return browserOsgView;};
		//bool isLibraryUpToDate(){return updatedLibrary;};
		//void updateLibrary(bool update){updatedLibrary = update;};
		void updateLibrary();

    private slots:
        /* Insérez les prototypes de vos slots personnalisés ici */
		void on_pushButtonLaunch_clicked();
		void on_horizontalSliderRhythm_sliderReleased();
		void on_horizontalSliderTimbre_sliderReleased();
		void on_horizontalSliderHarmony_sliderReleased();
		void on_horizontalSliderClusters_sliderReleased();
		void on_horizontalSliderBPM_valueChanged(); // or sliderMoved();?
		void on_horizontalSliderKey_valueChanged(); // or sliderMoved();?

    private:
		MediaCycle *media_cycle;
        Ui::ACMainWindowQT ui;
		bool updatedLibrary;

	protected:
		ACOsgBrowserViewQT* browserOsgView;
		//TiOscReceiverRef oscReceiver;
    //public:
		//TiOscReceiverRef oscReceiver;
		//void processOscMessage(const char* tagName);
};
#endif
