/*
 *  ACImageCycleOsgQt.h
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

#ifndef HEADER_ACIMAGECYCLEOSGQT
#define HEADER_ACIMAGECYCLEOSGQT

#include <iostream>
#include <string.h>
using namespace std;

#include <QtGui>
#include "ui_ACImageCycleOsgQt.h"
#include <ACOsgBrowserViewQT.h>
#include <MediaCycle.h>

class ACImageCycleOsgQt : public QMainWindow
	{
		Q_OBJECT
		
    public:
        ACImageCycleOsgQt(QWidget *parent = 0);
		~ACImageCycleOsgQt();
		void updateLibrary();
		
		private slots:
        /* Insérez les prototypes de vos slots personnalisés ici */
		void on_pushButtonLaunch_clicked(); // loadACLFile
		void on_pushButtonClean_clicked();
		void on_pushButtonRecenter_clicked();
		void on_pushButtonBack_clicked();
		void on_pushButtonForward_clicked();
		
		void on_checkBoxFeat1_stateChanged(int state);
		void on_checkBoxFeat2_stateChanged(int state);
		void on_checkBoxFeat3_stateChanged(int state);
		
		void on_sliderClusters_sliderReleased();
		
		void loadACLFile();
		void saveACLFile();
		void loadMediaDirectory();
		void loadMediaFiles();
		
	public:
		MediaCycle *media_cycle;
		
    private:
        Ui::ACImageCycleOsgQt ui;
		bool updatedLibrary;
		
	protected:
		//ACOsgBrowserViewQT* browserOsgView;
	};
#endif
