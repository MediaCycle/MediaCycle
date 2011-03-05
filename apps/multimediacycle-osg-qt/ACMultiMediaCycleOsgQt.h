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

#if defined (SUPPORT_AUDIO)
	#include <ACAudioEngine.h>
#endif //defined (SUPPORT_AUDIO)

// Dock Widgets
#include <ACDockWidgetFactoryQt.h>

#include <ACAboutDialogFactoryQt.h>

// FORWARD DECLARATIONS
QT_BEGIN_NAMESPACE
class QListWidgetItem;
QT_END_NAMESPACE

class SettingsDialog; // forward declaration; NB: SettingsDialog member has to be a pointer

class ACMultiMediaCycleOsgQt : public QMainWindow {
Q_OBJECT
	
public slots:
	// Config
	void on_actionSave_Config_File_triggered(bool checked); // settings
	void on_actionEdit_Config_File_triggered(bool checked);
	void on_actionLoad_Config_File_triggered(bool checked);
	void comboDefaultSettingsChanged(); 
	
private slots:
	// Controls
	void syncControlToggleWithDocks();
	
	// Library controls
	void on_actionLoad_ACL_triggered(bool checked);
	void on_actionSave_ACL_triggered(bool checked);		
	void on_actionLoad_XML_triggered(bool checked); // features
	void on_actionSave_XML_triggered(bool checked);	
	void on_actionLoad_Media_Directory_triggered(bool checked);
	void on_actionLoad_Media_Files_triggered(bool checked);
	void on_actionClean_triggered(bool checked);

	void on_actionHelpAbout_triggered(bool checked);
	
public:
	ACMultiMediaCycleOsgQt(QWidget *parent = 0);
	~ACMultiMediaCycleOsgQt();
	
	void updateLibrary();
	void updatePluginDock();
	
	void setBrowserMode(ACBrowserMode _mode){this->browser_mode=_mode;}
	void setMediaType(ACMediaType _mt){this->media_type = _mt;}


	// XS TODO: default values for image -- is this correct ?
	void createMediaCycle(ACMediaType _media_type = MEDIA_TYPE_IMAGE, ACBrowserMode _browser_mode = AC_MODE_CLUSTERS);
	void destroyMediaCycle();
	MediaCycle* getMediaCycle() {return media_cycle;}
	ACMediaType getMediaType() {return media_type;}
	void addPluginItem(QListWidgetItem *_item);
	void addPluginsLibrary(std::string _library);
	void removePluginsLibrary(std::string _library);
	void loadDefaultConfig(ACMediaType _media_type = MEDIA_TYPE_IMAGE, ACBrowserMode _browser_mode = AC_MODE_CLUSTERS);
	void synchronizeFeaturesWeights();
	
	// Controls
	bool addControlDock(ACAbstractDockWidgetQt* dock);
	bool addControlDock(std::string dock_type);
	
	bool addAboutDialog(ACAbstractAboutDialogQt* dock);
	bool addAboutDialog(std::string about_type);
	
	// Callback
	void mediacycleCallback(char* message);
	
	// Close Event
	void closeEvent(QCloseEvent *event);
	
	// settings and dock (XS  TODO change dock)
	void configureSettings();
	void configurePluginDock();

private:
	// variables
	Ui::ACMediaCycleOsgQt ui;
	SettingsDialog *settingsDialog;
//	QProgressBar *pb;
	bool features_known;
	bool plugins_scanned;
	MediaCycle *media_cycle;
	ACMediaType media_type;
	ACBrowserMode browser_mode;
	std::string config_file_xml;	
	std::string project_directory;	

	std::vector<std::string> plugins_libraries;
	#if defined (SUPPORT_AUDIO)
		ACAudioEngine *audio_engine;
	#endif //defined (SUPPORT_AUDIO)
	
	// Dock Widgets
	vector<int> lastDocksVisibilities; //state stored before hiding all docks with the toggle
	bool wasControlsToggleChecked;
	vector<ACAbstractDockWidgetQt*> dockWidgets;
	ACDockWidgetFactoryQt* dockWidgetFactory;
	void configureDockWidgets(ACMediaType _media_type);
	
	ACAboutDialogFactoryQt* aboutDialogFactory;
	ACAbstractAboutDialogQt* aboutDialog;
	
	// methods
	bool readQSettings();
	bool writeQSettings();
	void setDefaultQSettings();
	void clearQSettings();

	void readXMLConfig(std::string _filename="");
//	TiXmlHandle readXMLConfigHeader(std::string _filename="");
	void writeXMLConfig(std::string _filename="");
	std::string rstrip(const std::string& s);
	void showError(std::string s);
	bool hasMediaCycle();
};
#endif
