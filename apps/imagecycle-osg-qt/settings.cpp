/* 
 * File:   settings.cpp
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

#include "settings.h"

SettingsDialog::SettingsDialog(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);
//    connect(buttonSaveLog, SIGNAL(clicked()), this, SLOT(saveLog()));
//    connect(buttonSelectFeaturesPlugin, SIGNAL(clicked()), this, SLOT(selectFeaturesPlugins()));
//    connect(buttonSelectVisualizationPlugin, SIGNAL(clicked()), this, SLOT(selectVisualizationPlugins()));
//	connect(buttonSaveCurrentSettings, SIGNAL(clicked()), this, SLOT(saveCurrentSettings()));
	connect(buttonSelectSaveConfigFile, SIGNAL(clicked()), this, SLOT(selectSaveConfigFile()));
}


void SettingsDialog::selectSaveConfigFile() {
	QString _configFile = QFileDialog::getOpenFileName();
	// TODO: try/catch
	config_file = _configFile.toStdString();
	lineEditSaveConfigFile->setText(_configFile);
	lineEditSaveConfigFile->update();
}

void SettingsDialog::setMediaCycleMainWindow(ACImageCycleOsgQt* _mc) {
	//XS test
	_mc->disBonjour() ;
}
