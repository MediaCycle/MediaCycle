/* 
 * File:   settings.h
 * Author: xavier
 * inspired from Qt's "browser" example
 *
 * @date 31/03/10
 * @copyright (c) 2010 – UMONS - Numediart
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


#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QtGui>

#include "pluginsTreeItem.h"
#include "pluginsTreeModel.h"

#include "ui_settings.h"

#include "ACMultiMediaCycleOsgQt.h"
#include "MediaCycle.h"
#include "ACMediaTypes.h" // for ACMediaType
#include "ACMediaBrowser.h" // for ACBrowserMode

#include <map>
#include <string>

// forward declaration
class ACMultiMediaCycleOsgQt;

class SettingsDialog : public QMainWindow, private Ui::SettingsDialog
{
    Q_OBJECT

public:
    SettingsDialog(ACMultiMediaCycleOsgQt* _mc);
	virtual ~SettingsDialog();
	void setMediaCycleMainWindow(ACMultiMediaCycleOsgQt* _mc); 
	void setMediaCycle(MediaCycle* _mc);
	bool setMediaType(std::string _mt);
	
	// Close Event define what to do when window is closed
	void closeEvent(QCloseEvent *event);

private:
	void addPluginsFromLibrary(QString _fileName);	
	void readSettings();
	void writeSettings();
	void applyCurrentSettings();

public slots:
    void updateActions();
	
private slots:
	void on_buttonAddPluginsLibrary_clicked();
	void on_buttonProjectDirectory_clicked();

 //   void selectVisualizationPlugins();
//	void configureFeaturesPlugins();
	void comboMediaTypeValueChanged(); 
	void comboBrowserModeValueChanged(); 
	void removePluginRow(); 

private:
	ACMultiMediaCycleOsgQt* multi_media_cycle;
	MediaCycle* media_cycle;
	
	// parameters set by the comboBoxes in GUI
	std::string media_type;
	std::string browser_mode;
	std::string project_directory;
	std::string xml_config_file;
	
//	std::string plugins_library;
		
	// for plugins tree view
	pluginsTreeModel* ptm;
};

#endif // SETTINGS_H

