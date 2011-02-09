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
    SettingsDialog(QWidget *parent = 0);
	virtual ~SettingsDialog(){};
	void setMediaCycleMainWindow(ACMultiMediaCycleOsgQt* _mc); 
	void setMediaCycle(MediaCycle* _mc);
	bool setMediaType(std::string _mt);

private slots:
	void on_buttonApplyCurrentSettings_clicked();
	void on_buttonBrowsePluginsLibrary_clicked();
	void on_buttonAddPluginsLibrary_clicked();
	void on_buttonRemovePluginsLibrary_clicked();
	void on_buttonConfirmPluginsSelection_clicked();

 //   void selectVisualizationPlugins();
	bool saveConfigFile();
	void configureFeaturesPlugins();
	void comboMediaTypeValueChanged(); 
	void comboBrowserModeValueChanged(); 

// suggestion: may be add a way to save settings (like sliders positions, ...) ?
	//   void loadSettings();
	//   void saveSettings();

private:
//	Ui::SettingsDialog ui;
	ACMultiMediaCycleOsgQt* multi_media_cycle;
	MediaCycle* media_cycle;
	
	// parameters set by the comboBoxes in GUI
	std::string media_type;
	std::string browser_mode;
	std::string plugins_library;
};

#endif // SETTINGS_H

