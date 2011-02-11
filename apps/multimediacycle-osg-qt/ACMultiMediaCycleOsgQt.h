/*
 *  ACMultiMediaCycleOsgQt.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 16/11/10
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

// one GUI for all media GUIs in Qt 
// replaces: ACAudioCycleOsgQt, ACImageCycleOsgQt, ACVideoCycleOsgQt, ...
// media-specific : needs configuration file (that can be generated via the GUI)

#ifndef HEADER_ACMULTIMEDIACYCLEOSGQT
#define HEADER_ACMULTIMEDIACYCLEOSGQT

#include <iostream>
#include <string.h>

#include <QtGui>
#include "settings.h" // SettingsDialog

#include "ui_ACMultiMediaCycleOsgQt.h"
#include <ACOsgCompositeViewQt.h>
#include <MediaCycle.h>
#include <ACAudioEngine.h>

// Dock Widgets
/*#include <ACMediaConfigDockWidgetQt.h>
#include <ACBrowserControlsClustersNeighborsDockWidgetQt.h>*/
#include <ACAudioControlsDockWidgetQt.h>
#include <ACVideoControlsDockWidgetQt.h>

// FORWARD DECLARATIONS
QT_BEGIN_NAMESPACE
class QListWidgetItem;
QT_END_NAMESPACE

class SettingsDialog; // forward declaration; NB: SettingsDialog member has to be a pointer

class ACMultiMediaCycleOsgQt : public QMainWindow {
Q_OBJECT
	
public slots:
	// Config
	virtual void modifyListItem(QListWidgetItem *item); // XS TODO why virtual again ?
	bool saveConfigFile();
	void editConfigFile();
	void loadConfigFile();
	void comboDefaultSettingsChanged(); 
	
private slots:
	// Controls
	void syncControlToggleWithDocks();

	// Library controls
	void loadACLFile();
	void saveACLFile();		
	void loadXMLFile();
	void saveXMLFile();		
	void loadMediaDirectory();
	void loadMediaFiles();
	//void on_pushButtonLaunch_clicked();
	void on_pushButtonClean_clicked();
	
	// Browser controls
	void on_pushButtonRecenter_clicked();
	void on_pushButtonBack_clicked();
	void on_pushButtonForward_clicked();
	//void on_radioButtonClusters_toggled();

	// Clustering controls
	void spinBoxClustersValueChanged(int _value);
	void on_sliderClusters_sliderReleased();
	//void on_comboBoxClustersMethod_activated(const QString & text);//CF or (int index);} 
	//void on_comboBoxClustersPositions_activated(const QString & text);//CF or (int index);} 

	// Neighborhoods controls
	//void on_comboBoxNeighborsMethod_activated(const QString & text);//CF or (int index);} 
	//void on_comboBoxNeighborsPositions_activated(const QString & text);//CF or (int index);} 
	
public:
	ACMultiMediaCycleOsgQt(QWidget *parent = 0);
	~ACMultiMediaCycleOsgQt();
	void updateLibrary();
	
	// XS TODO: default values for image -- is this correct ?
	void createMediaCycle(ACMediaType _media_type = MEDIA_TYPE_IMAGE, ACBrowserMode _browser_mode = AC_MODE_CLUSTERS);
	MediaCycle* getMediaCycle() {return media_cycle;}
	void destroyMediaCycle();
	void addPluginItem(QListWidgetItem *_item);
	void synchronizeFeaturesWeights();
	
	void addPluginsLibrary(std::string _library);
	void loadDefaultConfig(ACMediaType _media_type = MEDIA_TYPE_IMAGE, ACBrowserMode _browser_mode = AC_MODE_CLUSTERS);

private:
	// variables
	
	Ui::ACMediaCycleOsgQt ui;
	SettingsDialog *settingsDialog;
//	QProgressBar *pb;
	bool features_known;
	bool plugins_scanned;
	bool library_loaded;
	MediaCycle *media_cycle;
	ACMediaType media_type;
	ACBrowserMode browser_mode;
	std::string config_file;	
	std::vector<std::string> plugins_libraries;
	
	ACAudioEngine *audio_engine;
	
	// Dock Widgets
	/*ACMediaConfigDockWidgetQt* mediaConfig;
	ACBrowserControlsClustersNeighborsDockWidgetQt browserControls;*/
	ACAudioControlsDockWidgetQt* audioControls;
	ACVideoControlsDockWidgetQt* videoControls;
	vector<int> lastDocksVisibilities; //state stored before hiding all docks with the toggle
	bool wasControlsToggleChecked;
	
	// methods
	void configureCheckBoxes();
	void cleanCheckBoxes();
	bool saveFile(const QString& _filename);
	std::string rstrip(const std::string& s);
	void showError(std::string s);
	bool hasMediaCycle();
};
#endif
