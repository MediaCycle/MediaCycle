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

#include "ui_ACMultiMediaCycleOsgQt.h"

#include <MediaCycle.h>
#if defined (SUPPORT_AUDIO)
#include <ACAudioEngine.h>
#endif //defined (SUPPORT_AUDIO)
#include <ACOsgCompositeViewQt.h>

#include "ACSettingsDialogQt.h" // SettingsDialog
#include <ACDockWidgetsManagerQt.h>
#include <ACAboutDialogFactoryQt.h>
#include <ACInputControlsDialogQt.h>

#include "ACMediaLibraryMetadataQt.h"
#include "ACUserProfileQt.h"
#include "ACSegmentationControlsDialogQt.h"

#include "ACEventListener.h"

// FORWARD DECLARATIONS
QT_BEGIN_NAMESPACE
class QListWidgetItem;
QT_END_NAMESPACE

class ACSettingsDialogQt; // forward declaration; NB: SettingsDialog member has to be a pointer

class ACDetachedMediaBrowserOsgQt : public QMainWindow {
Q_OBJECT
public:
	ACDetachedMediaBrowserOsgQt(QWidget *parent = 0): QMainWindow(parent){};
};

class ACQProgressBar : public QProgressBar {
Q_OBJECT
public:
	ACQProgressBar(QWidget *parent = 0);
	~ACQProgressBar(){};
	
	public slots:
		void loading_started();
		void loading_finished();
		void loading_file(int media_id, int dir_size);
};

class ACMultiMediaCycleOsgQt : public QMainWindow,public ACEventListener {
Q_OBJECT
	
public slots:
	// Config
	void on_actionEdit_Config_File_triggered(bool checked);
    void changeLibraryMediaType(QString media); // media type of the library
    #ifdef SUPPORT_MULTIMEDIA
    void changeActiveMediaType(QString media); // active media type for documents
    #endif
    void on_actionEdit_Input_Controls_triggered(bool checked);
	//SENEFFE ?
	virtual void loopXML(){};
	
public slots:	
	// Library controls
	void on_actionLoad_XML_triggered(bool checked); // features
	void on_actionSave_XML_triggered(bool checked);	
	void on_actionLoad_Media_Directory_triggered(bool checked);
	void on_actionLoad_Media_Files_triggered(bool checked);
	void on_actionClean_triggered(bool checked);
	void on_actionHelpAbout_triggered(bool checked);
	void on_actionDetachBrowser_triggered(bool checked);
	void on_actionFullscreen_triggered(bool checked);
    void on_actionToggle_Controls_triggered(bool checked);
    void on_actionEdit_Library_Metadata_triggered(bool checked);
    void on_actionEdit_Profile_triggered(bool checked);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

public:
	ACMultiMediaCycleOsgQt(QWidget *parent = 0);
	~ACMultiMediaCycleOsgQt();
	
	void updateLibrary();
	
	void setBrowserMode(ACBrowserMode _mode){this->browser_mode=_mode;}
    void setMediaType(ACMediaType _mt);

	// XS TODO: default values for image -- is this correct ?
	void createMediaCycle(ACMediaType _media_type = MEDIA_TYPE_IMAGE, ACBrowserMode _browser_mode = AC_MODE_CLUSTERS);
	void destroyMediaCycle();
	MediaCycle* getMediaCycle() {return media_cycle;}
	ACMediaType getMediaType() {return media_type;}
	bool addPluginLibrary(std::string _library);
	bool removePluginLibrary(std::string _library);
	ACPluginLibrary* getPluginLibrary (std::string _library);

	bool removePluginFromLibrary(std::string _plugin_name, std::string _library_path);

    std::string getPluginPathFromBaseName(std::string basename);
    int loadPluginFromBaseName(std::string basename);
    int tryLoadFeaturePluginFromBaseName(std::string basename);
	void loadDefaultConfig(ACMediaType _media_type = MEDIA_TYPE_IMAGE, ACBrowserMode _browser_mode = AC_MODE_CLUSTERS);
	
	void loadMediaDocumentConfig(std::string _name);
	// Controls
	bool addControlDock(std::string dock_type);
	
	bool addAboutDialog(ACAbstractAboutDialogQt* dock);
	bool addAboutDialog(std::string about_type);
	
	// Callback
	void mediaImported(int n,int nTot);
	
	// Close Event
	void closeEvent(QCloseEvent *event);
	
	// settings and dock (XS  TODO change dock)
    void configureSettings();
    //void configurePluginDock();
	bool readXMLConfig(std::string _filename="");
	void clean(bool _updategl=false);
	void setDefaultQSettings();
	
	void useSegmentationByDefault(bool _status);// derived apps can disable segmentation by default on their main.cpp
	void switchSegmentation(bool _status);// disable/enable segmentation when (failing) loading segmentation plugins and changing media types
	void switchFeatureExtraction(bool _status);// disable/enable feature extraction when (failing) loading feature extraction plugins and changing media types
	void switchPluginVisualizations(bool _status);// disable/enable visualization from plugins when (failing) loading visualization plugins and changing media types
	
	void autoConnectOSC(bool _status = true);
        void changeSetting(ACSettingType _setting);

signals:	
	void mediacycle_message_changed(QString mess);
	void loading_started();
	void loading_finished();
	void loading_file(int media_id, int dir_size);

private:
	// variables
	Ui::ACMediaCycleOsgQt ui;
    ACSettingsDialogQt *settingsDialog;
//	QProgressBar *pb;
	bool features_known;
	ACMediaType media_type;
	ACBrowserMode browser_mode;
	std::string config_file_xml;	
	std::string project_directory;	

	std::vector<std::string> plugins_libraries;
	#if defined (SUPPORT_AUDIO)
    ACAudioEngine *audio_engine;
	#endif //defined (SUPPORT_AUDIO)
	
    ACDockWidgetsManagerQt* dockWidgetsManager;
	ACAboutDialogFactoryQt* aboutDialogFactory;
	ACAbstractAboutDialogQt* aboutDialog;
    ACInputControlsDialogQt* controlsDialog;
    ACSegmentationControlsDialogQt* segmentationDialog;
	QMainWindow* detachedBrowser;
	
	// methods
	bool readQSettings();
	bool writeQSettings();
	void clearQSettings();
	bool doSegments();
	
//	TiXmlHandle readXMLConfigHeader(std::string _filename="");
	void writeXMLConfig(std::string _filename="");
	std::string rstrip(const std::string& s);
	void showError(std::string s);
	void showError(const exception& e);
	bool hasMediaCycle();
	void changeMediaType(ACMediaType _mt);

	void importDirectoriesThreaded(std::vector<std::string> directories);

	QDockWidget* osgViewDock;
	QWidget* osgViewDockWidget;
	QVBoxLayout *osgViewDockLayout;
	QWidget* osgViewDockTitleBar;
	QRect osgViewDockNormalSize;
	//QProgressBar* progressBar;
	ACQProgressBar* progressBar;
	
	bool use_segmentation_current,use_segmentation_default,use_feature_extraction,use_visualization_plugins;
	bool auto_connect_osc;

        ACMediaLibraryMetadataQt* metadataWindow;
        ACUserProfileQt* userProfileWindow;
	
protected:
	ACOsgCompositeViewQt* compositeOsgView;
	MediaCycle *media_cycle;
        ACSettingType setting;
};
#endif
