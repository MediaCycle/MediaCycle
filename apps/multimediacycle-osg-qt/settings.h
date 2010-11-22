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

#include <map>
#include <string>
using std::string;

// forward declaration
class ACMultiMediaCycleOsgQt;

// conversion between MediaTypes and text string (e.g., to be used in the labels for the gui)
typedef std::map<std::string, ACMediaType> stringToMediaTypeConverter;
typedef std::map<std::string, ACBrowserMode > stringToBrowserModeConverter;

class SettingsDialog : public QMainWindow, private Ui::SettingsDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = 0);
	virtual ~SettingsDialog(){};
	void setMediaCycleMainWindow(ACMultiMediaCycleOsgQt* _mc); 

private slots:
	void on_buttonApplyCurrentSettings_clicked();
	void on_buttonBrowsePluginsLibrary_clicked();
	void on_buttonAddLibrary_clicked();
	void on_buttonRemoveLibrary_clicked();

//    void loadFromSettings();

 //   void saveLog();
 //   void selectVisualizationPlugins();
 //   void saveCurrentSettings();
	void selectSaveConfigFile();
	void configureFeaturesPlugins();
	void comboMediaTypeValueChanged(); 
	void comboBrowserModeValueChanged(); 

private:
	static const stringToMediaTypeConverter stringToMediaType;
	static const stringToBrowserModeConverter stringToBrowserMode;
//	Ui::SettingsDialog ui;
	string config_file;	
	ACMultiMediaCycleOsgQt* multi_media_cycle;
	MediaCycle* media_cycle;
	
	// parameters set by the comboBoxes in GUI
	string media_type;
	string browser_mode;
	string plugins_library;
};

#endif // SETTINGS_H

