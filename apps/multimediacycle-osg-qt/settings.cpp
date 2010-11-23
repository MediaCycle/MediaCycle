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
#include "ACMediaTypes.h"

// initialize static class variable
const stringToMediaTypeConverter::value_type _initm[] = {
stringToMediaTypeConverter::value_type("Audio", MEDIA_TYPE_AUDIO), \
stringToMediaTypeConverter::value_type("Image", MEDIA_TYPE_IMAGE), \
stringToMediaTypeConverter::value_type("Video", MEDIA_TYPE_VIDEO), \
stringToMediaTypeConverter::value_type("3DModel",MEDIA_TYPE_3DMODEL), \
stringToMediaTypeConverter::value_type("Text",MEDIA_TYPE_TEXT), \
stringToMediaTypeConverter::value_type("Mixed", MEDIA_TYPE_MIXED), \
stringToMediaTypeConverter::value_type("All", MEDIA_TYPE_ALL)
};

const stringToBrowserModeConverter::value_type _initb[] = {
stringToBrowserModeConverter::value_type("Clusters", AC_MODE_CLUSTERS), \
stringToBrowserModeConverter::value_type("Neighbors", AC_MODE_NEIGHBORS)
};

const stringToMediaTypeConverter SettingsDialog::stringToMediaType(_initm, _initm + sizeof _initm / sizeof *_initm);
const stringToBrowserModeConverter SettingsDialog::stringToBrowserMode(_initb, _initb + sizeof _initb / sizeof *_initb);

// This class provides general setting for the *Cycle applications
// e.g., choose audio/image/video, configure plugins, ...

// constructor : 
// parent is typically ACMultiMediaCycleOsgQt->parent
SettingsDialog::SettingsDialog(QWidget *parent) : QMainWindow(parent) {
	this->multi_media_cycle = NULL;
	this->media_cycle = NULL;

    setupUi(this);
	
	this->media_type = comboMediaType->currentText().toStdString();
	this->browser_mode = comboBrowserMode->currentText().toStdString();
	
	this->media_type = this->browser_mode = this->plugins_library = "";
	
//    connect(buttonSaveLog, SIGNAL(clicked()), this, SLOT(saveLog()));
//    connect(pushButtonConfigureFeaturesPlugins, SIGNAL(clicked()), this, SLOT(configureFeaturesPlugins()));
//    connect(buttonSelectVisualizationPlugin, SIGNAL(clicked()), this, SLOT(selectVisualizationPlugins()));
//	connect(buttonSaveCurrentSettings, SIGNAL(clicked()), this, SLOT(saveCurrentSettings()));
	connect(buttonSelectSaveConfigFile, SIGNAL(clicked()), this, SLOT(selectSaveConfigFile()));
	connect(comboMediaType, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(comboMediaTypeValueChanged()));
	connect(comboBrowserMode, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(comboBrowserModeValueChanged()));
}

// tells the settings dialog which application it will set up
// use a handy pointer directly to mediacycle too.
void SettingsDialog::setMediaCycleMainWindow(ACMultiMediaCycleOsgQt* _mc) {
	this->multi_media_cycle = _mc;
	this->media_cycle = _mc->getMediaCycle();
}

// ----- SLOTS -----

void SettingsDialog::selectSaveConfigFile() {
	QString _configFile = QFileDialog::getOpenFileName();
	// TODO: try/catch
	config_file = _configFile.toStdString();
	lineEditSaveConfigFile->setText(_configFile);
	lineEditSaveConfigFile->update();
}

void SettingsDialog::configureFeaturesPlugins(){
	ACPluginManager *acpl = media_cycle->getPluginManager(); //getPlugins
	if (acpl) {
		for (int i=0;i<acpl->getSize();i++) {
			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
				if (acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_FEATURES) {
					QString s(acpl->getPluginLibrary(i)->getPlugin(j)->getName().c_str());
					QListWidgetItem * item = new QListWidgetItem(s,listWidgetFeaturesPlugins);
					item->setCheckState (Qt::Unchecked);
				}
			}
		}
	}
	// TODO
	//	connect(listWidgetFeaturesPlugins, SIGNAL(itemClicked(QListWidgetItem*)),
	//			this, SLOT(modifyListItem(QListWidgetItem*)));
}


void SettingsDialog::comboMediaTypeValueChanged(){
	this->media_type = comboMediaType->currentText().toStdString();
	cout << "(if you press Apply) media type will change to : " << this->media_type << endl;
}

void SettingsDialog::comboBrowserModeValueChanged(){
	this->browser_mode = comboBrowserMode->currentText().toStdString();
	cout << "(if you press Apply) browser mode will change to : " << this->browser_mode << endl;
}

void SettingsDialog::on_buttonBrowsePluginsLibrary_clicked(){
	QString pluginsLibraryFileName;
	QFileDialog	dialog(this, tr("Browse Plugins Library"),"",tr("Plugins Library (*.*)"));
	dialog.setFileMode(QFileDialog::ExistingFile);
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	
	QStringList::Iterator file = fileNames.begin();
	QString fileName  = *file;
	this->plugins_library = fileName.toStdString();
	std::cout << "Opening: " << this->plugins_library << std::endl;
	lineEditPluginsLibrary->setText(fileName);
}

void SettingsDialog::on_buttonApplyCurrentSettings_clicked(){
	stringToMediaTypeConverter::const_iterator iterm = stringToMediaType.find(this->media_type);
	if( iterm == stringToMediaType.end() ) {
		cout << " media type not found : " << iterm->first << endl;
		return;
	}
	ACMediaType new_media_type = iterm->second;
	cout << "corresponding media type code : " << new_media_type << endl;
	
	stringToBrowserModeConverter::const_iterator iterb = stringToBrowserMode.find(this->browser_mode);
	if( iterb == stringToBrowserMode.end() ) {
		cout << " browser mode not found : " << iterb->first << endl;
		return;
	}
	ACBrowserMode new_browser_mode = iterb->second;
	cout << "corresponding browser mode code : " << new_browser_mode << endl;
	
	if (this->media_cycle != NULL) {
		// clean the current media_cycle that was used by the ACMultiMediaCycleOsgQt app.
		this->multi_media_cycle->destroyMediaCycle();
	}
	
	this->multi_media_cycle->createMediaCycle(new_media_type, new_browser_mode);
	this->media_cycle = this->multi_media_cycle->getMediaCycle();

}

void SettingsDialog::on_buttonAddLibrary_clicked(){
	if (media_cycle == NULL) {
		cout << "load media_cycle first" << endl;
		return;
	}
	
	ACPluginManager *acpl = media_cycle->getPluginManager();
	if (acpl->add(this->plugins_library) <=0 ){
		cerr << "Problem adding Plugins Library : " << this->plugins_library<< endl;
		return;
	}
	
	ACPluginLibrary* plib = acpl->getPluginLibrary(this->plugins_library);
	if (plib == NULL) {
		cout << "Problem loading plugins library: " << this->plugins_library << endl;
		return;
	}
	
	vector<ACPlugin *> ::iterator iter;
	vector<ACPlugin *> plug = plib->getPlugins();
	
	for (iter = plug.begin(); iter != plug.end(); iter++) {
		
		QString s((*iter)->getName().c_str());
		QListWidgetItem * item = new QListWidgetItem(s,listWidgetFeaturesPlugins);
		item->setCheckState (Qt::Unchecked);
	}
	
	
}

void SettingsDialog::on_buttonRemoveLibrary_clicked(){
//	
}

void SettingsDialog::on_buttonConfirmPluginsSelection_clicked(){
	cout << "confirming plugins selection : " << listWidgetFeaturesPlugins->count()<< endl;
	for (int i=0; i < listWidgetFeaturesPlugins->count(); i++) {
		// XS TODO: if checkState... (so we only add the selected plugins)
		cout << listWidgetFeaturesPlugins->item(i)->text().toStdString() << endl;
		multi_media_cycle->addPluginItem(listWidgetFeaturesPlugins->item(i));
	}
	
	// then the selected plugins get their weights synchronized with MediaCycle
	multi_media_cycle->synchronizeFeaturesWeights();
}
