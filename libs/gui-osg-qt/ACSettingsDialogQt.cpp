/* 
 * File:   ACSettingsDialogQt.cpp
 * Author: xavier
 * Inspiration sources :
 *   - Qt's "browser" example
 *   - Qt's "Editable Tree Model" example
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

#include "ACSettingsDialogQt.h"


// This class provides general setting for the *Cycle applications
// e.g., choose audio/image/video, configure plugins, ...

// constructor : 
// parent is typically ACMultiMediaCycleOsgQt->parent
ACSettingsDialogQt::ACSettingsDialogQt(ACMultiMediaCycleOsgQt* _mc) : QMainWindow() {
    setupUi(this);
	
	if (_mc ==0){
		cerr << "undefined multimediacycle application" << endl;
		return;
	}
		
	this->multi_media_cycle = _mc;
	
    this->ptm = new ACPluginsTreeModelQt();
	this->project_directory="/tmp";
	this->xml_config_file="MCconfig.xml";

	treeViewPluginsLibrairies->setModel(this->ptm);

	// resize first column to fit the plugin's path
	// XS TODO: shorten the path by stripping directory ?
	for (int column = 0; column < ptm->columnCount(); ++column)
        treeViewPluginsLibrairies->resizeColumnToContents(column);
	
	// XS TODO: non, get it from _mc
//	this->media_type = comboMediaType->currentText().toStdString();
//	this->browser_mode = comboBrowserMode->currentText().toStdString();
		
	lineEditProjectDirectory->setText(QString::fromStdString(this->project_directory));
	lineEditXMLConfigFile ->setText(QString::fromStdString(this->xml_config_file));

//    connect(buttonSelectVisualizationPlugin, SIGNAL(clicked()), this, SLOT(selectVisualizationPlugins()));
	connect(comboMediaType, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(comboMediaTypeValueChanged()));
	connect(comboBrowserMode, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(comboBrowserModeValueChanged()));
    connect(treeViewPluginsLibrairies->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &,
                                    const QItemSelection &)),
            this, SLOT(updateActions()));
	this->activateWindow();
	this->readSettings();
	// keep the settings window on top of the app's window
	//this->setWindowFlags(Qt::WindowStaysOnTopHint);
}

ACSettingsDialogQt::~ACSettingsDialogQt(){
	// this should also delete the children of the root node
	delete this->ptm;
}

// tells the settings dialog which application it will set up
// use a handy pointer directly to mediacycle too.
void ACSettingsDialogQt::setMediaCycleMainWindow(ACMultiMediaCycleOsgQt* _mcw) {
	this->multi_media_cycle = _mcw;
	
	// XS  TODO : find a way to tell settings what type of media the user choose e.g. in the default config
//	if (this->media_cycle != 0) {
//		this->browser_mode = this->media_cycle->getBrowserMode();
//		this->media_type = this->media_cycle->getMediaType();
//	}
}


MediaCycle* ACSettingsDialogQt::getMediaCycle() {
	MediaCycle* _mc = 0;
	if (!this->multi_media_cycle)
		// XS TODO add error message ?
		_mc = 0;
	else 
		_mc = this->multi_media_cycle->getMediaCycle();
	return _mc;
}

bool ACSettingsDialogQt::changeMediaType(ACMediaType _mt){
	MediaCycle* _mc = this->getMediaCycle();
	bool ok = false;
	if (!_mc) 
		ok = false;
	else
		ok = _mc->changeMediaType(_mt);
	return ok;
}

bool ACSettingsDialogQt::changeBrowserMode(ACBrowserMode _bm){
	MediaCycle* _mc = this->getMediaCycle();
	bool ok = false;
	if (!_mc) 
		ok = false;
	else
		ok = _mc->changeBrowserMode(_bm);
	return ok;
}

//bool ACSettingsDialogQt::setMediaType(string _mt) {
//	QString _smt = QString::fromStdString(_mt);
//	// test if _mt is among the options
//	int index = comboMediaType->findText(_smt);
//	if (index < 0) {
//		cerr << "<ACSettingsDialogQt::setMediaType> : media type not found" << _mt << endl;
//		return false;
//	}
//	comboMediaType->setCurrentIndex(index);
//	this->media_type = _mt;
//	return true;
//}


// ----- SLOTS -----

//void ACSettingsDialogQt::configureFeaturesPlugins(){
//	ACPluginManager *acpl = media_cycle->getPluginManager(); //getPlugins
//	if (acpl) {
//		for (int i=0;i<acpl->getSize();i++) {
//			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
//				if (acpl->getPluginLibrary(i)->getPlugin(j)->implementsPluginType(PLUGIN_TYPE_FEATURES)) {
//					QString s(acpl->getPluginLibrary(i)->getPlugin(j)->getName().c_str());
//					QListWidgetItem * item = new QListWidgetItem(s,listWidgetFeaturesPlugins);
//					item->setCheckState (Qt::Unchecked);
//				}
//			}
//		}
//	}
//	// TODO
//	//	connect(listWidgetFeaturesPlugins, SIGNAL(itemClicked(QListWidgetItem*)),
//	//			this, SLOT(modifyListItem(QListWidgetItem*)));
//}


// changes will take effect when we close the settings window
void ACSettingsDialogQt::comboMediaTypeValueChanged(){
	string s_media_type = comboMediaType->currentText().toStdString() ;
    ACMediaTypeNames::const_iterator iterm = media_type_names.find(s_media_type);
    if( iterm == media_type_names.end() ) {
        cout << " media type not found : " << s_media_type << endl;
		return;
	}
	else {
		this->media_type = iterm->second;
//		cout << iterm->first << " - corresponding media type code : " << new_media_type << endl;
	}
}

// changes will take effect when we close the settings window
void ACSettingsDialogQt::comboBrowserModeValueChanged(){
	string s_browser_mode = comboBrowserMode->currentText().toStdString();
	stringToBrowserModeConverter::const_iterator iterb = stringToBrowserMode.find(s_browser_mode);
	if( iterb == stringToBrowserMode.end() ) {
		cout << " browser mode not found : " << s_browser_mode << endl;
		return;
	}
	else {
		this->browser_mode = iterb->second;
//		cout << iterb->first << " - corresponding browser mode code : " << new_browser_mode << endl;
	}
}

void ACSettingsDialogQt::on_buttonAddPluginLibrary_clicked(){
	// if media_cycle has not been instantiated yet, 
	// it creates one (if media_type and browser_mode have been defined)
	try{	
		if (this->getMediaCycle() == 0) {
			if (this->getMediaType() > 0 && this->getBrowserMode() > 0) 
				this->multi_media_cycle->createMediaCycle(this->getMediaType(), this->getBrowserMode());
			else 
				throw runtime_error("define media type and browser mode first");
		}
	}catch (const exception& e) {
		this->showError(e);
		return;
	}	
	
	QString pluginsLibraryFileName;
	QFileDialog	dialog(this, tr("Browse Plugins Library"),"",tr("Plugins Library (*.*)"));
	dialog.setFileMode(QFileDialog::ExistingFile);
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	
	if (fileNames.size() > 0){
		// for the moment: only one file
		QStringList::Iterator file = fileNames.begin();
		QString fileName  = *file;
		std::cout << "Opening: " << fileName.toStdString() << std::endl;
		this->addPluginsFromLibrary(fileName);
	}
}

// removes any row of plugins in the "table" (list of tree items)
void ACSettingsDialogQt::on_buttonRemovePluginLibrary_clicked() {
    const QModelIndex index = treeViewPluginsLibrairies->selectionModel()->currentIndex();
    QAbstractItemModel *model = treeViewPluginsLibrairies->model();
	
	// remove associated plugin or plugin library...
	if (index.parent().row() == -1){ 
		// then it's a whole library
		string pl = ptm->data(index, Qt::DisplayRole).toString().toStdString();
		cout << "removing plugin library : "<< pl << endl;
		this->multi_media_cycle->removePluginLibrary(pl);
	}
	else{ 
		// then it's a single plugin, not a whole library
		string pl = ptm->data(index, Qt::DisplayRole).toString().toStdString();
		cout << "removing plugin : "<< pl << endl;
		string pll = ptm->data(index.parent(), Qt::DisplayRole).toString().toStdString();
		cout << "from library : "<< pll << endl;
		this->multi_media_cycle->removePluginFromLibrary(pl,pll);
	}
	
    if (model->removeRow(index.row(), index.parent())){
		cout << "removed selected row" << endl;
		updateActions();
	}
	else {
		cout << "failed removing selected row" << endl;
	}
}

void ACSettingsDialogQt::applyCurrentSettings(){
	this->changeMediaType(this->media_type);
	this->changeBrowserMode(this->browser_mode);
}

void ACSettingsDialogQt::on_buttonProjectDirectory_clicked(){
	QString default_dir = QString::fromStdString(this->project_directory);
	QString select_dir = QFileDialog::getExistingDirectory
	(
	 this, 
	 tr("Open Directory"),
	 default_dir,
	 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	 );
	
	if (select_dir=="") return; // e.g. the user pressed "cancel"
	else {
		lineEditProjectDirectory->setText(select_dir);
		this->project_directory=select_dir.toStdString();
	}
}

void ACSettingsDialogQt::on_buttonXMLConfigFile_clicked(){
	QString default_file = QString::fromStdString(this->xml_config_file);
	QString select_file = QFileDialog::getSaveFileName(this, tr("Open File"),
													   QString::fromStdString(this->project_directory),
													   tr("xml config file (*.xml)"));
	if (select_file=="") return; // e.g. the user pressed "cancel"
	else {
		lineEditXMLConfigFile->setText(select_file);
		this->xml_config_file=select_file.toStdString();
	}
}


void ACSettingsDialogQt::addPluginsFromLibrary(QString _fileName){
	// if media_cycle has not been instantiated yet, shows error
	// should have been done before calling this method
	try{	
		if (this->getMediaCycle() == 0) {
			throw runtime_error("missing MediaCycle");
		}
	} catch (const exception& e) {
		this->showError(e);
		return;
	}	
	
	string plugins_library = _fileName.toStdString();
	try {
		this->multi_media_cycle->addPluginLibrary(plugins_library);
	} catch (const exception& e) {
		this->showError(e);
		return;
	}
		
	ACPluginLibrary* plib = this->multi_media_cycle->getPluginLibrary(plugins_library);
	if (plib == 0) {
		// XS TODO showerror
		cout << "Problem loading plugins library: " << plugins_library << endl;
		return;
	}
	
	// add library item
	QVector<QVariant> libraryData;
	libraryData << _fileName << " - ";
    ACPluginsTreeItemQt* libraryItem = new ACPluginsTreeItemQt(libraryData, ptm->getRootItem());

	// add plugins from this library as children
	
	vector<ACPlugin *> ::iterator iter;
	vector<ACPlugin *> plug = plib->getPlugins();
	
	QStringList plugin_list;
	
	// gives a new slider ID to each feature.
	// then the user can manually adapt it
	// XS TODO: add tests for this, the user can't put any number of sliders!
	int p=ptm->rowCount();
	for (iter = plug.begin(); iter != plug.end(); iter++) {
		string s = (*iter)->getName().c_str();
		QString ss = QString::fromStdString( s );
		QString sid = QString::number(p);
		QVector<QVariant> pluginData;
		pluginData << ss << p;
        ACPluginsTreeItemQt* pluginItem = new ACPluginsTreeItemQt(pluginData, libraryItem);
		libraryItem->appendChild(pluginItem);
		p++;
//		item->setCheckState (Qt::Unchecked);
	}
	
	ptm->addRow(libraryItem);
	treeViewPluginsLibrairies->resizeColumnToContents(0);
	treeViewPluginsLibrairies->expandAll();

}

//void ACSettingsDialogQt::on_buttonRemovePluginLibrary_clicked(){
//	QListWidgetItem *curitem = listWidgetPluginsLibraries->currentItem();
//	
//	if (curitem) {
//		string pluginLibraryName = curitem->text().toStdString();
//		int r = listWidgetPluginsLibraries->row(curitem);
//		listWidgetPluginsLibraries->takeItem(r);
//		delete curitem;
//		this->multi_media_cycle->removePluginLibrary(pluginLibraryName);
//	}	
//}

//void ACSettingsDialogQt::removePluginsFromLibrary(QString _fileName){
//	if (media_cycle == 0) {
//		cout << "load media_cycle first" << endl;
//		return;
//	}
//	string plugins_library = _fileName.toStdString();
//	ACPluginManager *acpl = media_cycle->getPluginManager();
//	
//	ACPluginLibrary* plib = acpl->getPluginLibrary(plugins_library);
//	if (plib == 0) {
//		cout << "Problem finding plugins from library: " << plugins_library << endl;
//		return;
//	}
//	
//	vector<ACPlugin *> ::iterator iter;
//	vector<ACPlugin *> plug = plib->getPlugins();
//	
//	for (iter = plug.begin(); iter != plug.end(); iter++) {
//		QString s((*iter)->getName().c_str());
//		int r = listWidgetPluginsLibraries->row(s);
//		listWidgetPluginsLibraries->takeItem(r);
//		delete (*iter);
//	}
//	
//	if (acpl->remove(plugins_library) <=0 ){
//		cerr << "Problem removgin Plugins Library : " << plugins_library<< endl;
//		return;
//	}
//	
//	// keep track of all libraries added
//	this->multi_media_cycle->removePluginLibrary(plugins_library);
//}

void ACSettingsDialogQt::updateActions(){
    bool hasSelection = !treeViewPluginsLibrairies->selectionModel()->selection().isEmpty();
    buttonRemovePluginLibrary->setEnabled(hasSelection);
}

void ACSettingsDialogQt::closeEvent(QCloseEvent *event) {
	// MediaCycle settins (important !)
	this->applyCurrentSettings();
	// GUI-related settings ("cosmetic")
	this->writeSettings(); 
	QMainWindow::closeEvent(event);		
	cout << "closed settings window properly" << endl;
}

void ACSettingsDialogQt::readSettings() {
	QSettings settings(QSettings::UserScope, "numediart", "MediaCycleSettings");
	
	
	QPoint pos = settings.value("pos").toPoint(); //, QPoint(200, 200)).toPoint();
	QSize size = settings.value("size").toSize(); //, QSize(400, 400)).toSize();
	this->resize(size);
	this->move(pos);	
	this->restoreState(settings.value("windowState").toByteArray());
}

void ACSettingsDialogQt::writeSettings() {
	//QSettings settings(QApplication::applicationDirPath().append(QDir::separator()).append("settings.ini"),
	//				   QSettings::NativeFormat);
	
	QSettings settings(QSettings::UserScope, "numediart", "MediaCycleSettings");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
	//settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState());
}

void ACSettingsDialogQt::showError(std::string s){
	int warn_button;
	const QString qs = QString::fromStdString(s);
	warn_button = QMessageBox::warning(this, "Error", qs);
	cerr << s << endl;
}

void ACSettingsDialogQt::showError(const exception& e) {
	this->showError(e.what());
}


