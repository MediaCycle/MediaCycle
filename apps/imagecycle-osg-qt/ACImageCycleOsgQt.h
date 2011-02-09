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

#include <QtGui>
#include "settings.h" // SettingsDialog

#include "ui_ACImageCycleOsgQt.h"
// XS TODO: necessary ?
#include <ACOsgBrowserViewQT.h>
#include <MediaCycle.h>

// FORWARD DECLARATIONS
QT_BEGIN_NAMESPACE
class QListWidgetItem;
QT_END_NAMESPACE

class SettingsDialog; // forward declaration; NB: SettingsDialog member has to be a pointer

class ACImageCycleOsgQt : public QMainWindow
	{
		Q_OBJECT
		
    public:
        ACImageCycleOsgQt(QWidget *parent = 0);
		~ACImageCycleOsgQt();
		void updateLibrary();
		
		// XS test
		void disBonjour(){std::cout << "bonjour" << std::endl;}
		MediaCycle* getMediaCycle() {return media_cycle;}
		
	public slots: 
		virtual void modifyListItem(QListWidgetItem *item);
		
	private slots:
        /* Ins�rez les prototypes de vos slots personnalis�s ici */
		void on_pushButtonLaunch_clicked(); // loadACLFile
		void on_pushButtonClean_clicked();
		void on_pushButtonRecenter_clicked();
		void on_pushButtonBack_clicked();
		void on_pushButtonForward_clicked();
		
		void spinBoxClustersValueChanged(int _value);
		void on_sliderClusters_sliderReleased();

		void loadACLFile();
		void saveACLFile();		
		void loadMediaDirectory();
		void loadMediaFiles();
		void editConfigFile();
		
	public:
		SettingsDialog *settingsDialog;

    private:
        Ui::ACImageCycleOsgQt ui;
		void configureCheckBoxes();
		void cleanCheckBoxes();
		void synchronizeFeaturesWeights();
		bool features_known;
		bool plugins_scanned;
		bool library_loaded;
		MediaCycle *media_cycle;

	protected:
		//ACOsgBrowserViewQT* browserOsgView;
	};
#endif
