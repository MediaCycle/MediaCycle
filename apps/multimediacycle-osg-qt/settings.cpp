/* 
 * File:   settings.cpp
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

#include "settings.h"


// This class provides general setting for the *Cycle applications
// e.g., choose audio/image/video, configure plugins, ...

// constructor : 
// parent is typically ACMultiMediaCycleOsgQt->parent
SettingsDialog::SettingsDialog(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

	this->multi_media_cycle = NULL;
	this->media_cycle = NULL;
	this->ptm = new pluginsTreeModel();
	treeViewPluginsLibrairies->setModel(this->ptm);
	for (int column = 0; column < ptm->columnCount(); ++column)
        treeViewPluginsLibrairies->resizeColumnToContents(column);

	this->media_type = comboMediaType->currentText().toStdString();
	this->browser_mode = comboBrowserMode->currentText().toStdString();
		
//    connect(buttonSaveLog, SIGNAL(clicked()), this, SLOT(saveLog()));
//    connect(pushButtonConfigureFeaturesPlugins, SIGNAL(clicked()), this, SLOT(configureFeaturesPlugins()));
//    connect(buttonSelectVisualizationPlugin, SIGNAL(clicked()), this, SLOT(selectVisualizationPlugins()));
	connect(buttonSaveConfigFile, SIGNAL(clicked()), this, SLOT(saveConfigFile()));
	connect(comboMediaType, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(comboMediaTypeValueChanged()));
	connect(comboBrowserMode, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(comboBrowserModeValueChanged()));
    connect(buttonRemovePluginsLibrary, SIGNAL(clicked()), this, SLOT(removePluginRow()));
    connect(treeViewPluginsLibrairies->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &,
                                    const QItemSelection &)),
            this, SLOT(updateActions()));
}

SettingsDialog::~SettingsDialog(){
	// this should also delete the children of the root node
	delete this->ptm;
}

void SettingsDialog::removePluginRow() {
    QModelIndex index = treeViewPluginsLibrairies->selectionModel()->currentIndex();
    QAbstractItemModel *model = treeViewPluginsLibrairies->model();
    if (model->removeRow(index.row(), index.parent())){
		cout << "removed selected row" << endl;
		updateActions();
	}
	else {
		cout << "failed removing selected row" << endl;
	}
}


// tells the settings dialog which application it will set up
// use a handy pointer directly to mediacycle too.
void SettingsDialog::setMediaCycleMainWindow(ACMultiMediaCycleOsgQt* _mcw) {
	this->multi_media_cycle = _mcw;
	this->media_cycle = _mcw->getMediaCycle();
	
	// XS  TODO : find a way to tell settings what type of media the user choose e.g. in the default config
//	if (this->media_cycle != NULL) {
//		this->browser_mode = this->media_cycle->getBrowserMode();
//		this->media_type = this->media_cycle->getMediaType();
//	}
}


void SettingsDialog::setMediaCycle(MediaCycle* _mc) {
	this->media_cycle = _mc;
}

bool SettingsDialog::setMediaType(string _mt) {
	QString _smt = QString::fromStdString(_mt);
	// test if _mt is 
	int index = comboMediaType->findText(_smt);
	if (index < 0) {
		cerr << "<SettingsDialog::setMediaType> : media type not found" << _mt << endl;
		return false;
	}
	comboMediaType->setCurrentIndex(index);
	this->media_type = _mt;
	return true;
}


// ----- SLOTS -----

bool SettingsDialog::saveConfigFile() {
	// XS TEST
	return  this->writeXMLConfigFile(lineEditXMLConfigFile->text().toStdString());
//	return (multi_media_cycle->saveConfigFile());
}

bool SettingsDialog::writeXMLConfigFile(std::string _filename) {
	configFile = doc.createElement("MediaCycleConfig"); //creation de la balise 
	doc.appendChild(configFile); //filiation de la balise
	QString qs = QString::fromStdString(_filename);
	
	file.setFileName(qs); 
	if (!file.open(QIODevice::WriteOnly)) //ouverture du fichier de sauvegarde 
		return false; //en ecriture 
	out.setDevice(&file); //association du flux au fichier 
	
	if(!(this->setXMLMediaType(comboMediaType->currentText())));
	   return false;
	if(!(this->setXMLBrowserMode(comboBrowserMode->currentText())));
		return false;
//	QString qpl = QString::fromStdString(_pl);
	
//	QDomElement mesure = doc.createElement("mesure"); 
//	configFile.appendChild(mesure); //filiation de la balise “mesure” 
//	mesure.setAttribute("numero",n); //creation de l’attribut “numero” 
//	//creation de la balise “tension” 
//	QDomElement tension = doc.createElement("tension"); 
//	mesure.appendChild(tension); //filiation de la balise “tension” 
//	//creation de la balise “frequence” 
//	QDomElement frequence = doc.createElement("frequence"); 
//	mesure.appendChild(frequence); //filiation de la balise “frequence” 
//	QDomText t1 = doc.createTextNode(t); //création de la donnée t1 
//	tension.appendChild(t1); //filiation du noeud “t1” 
//	QDomText f1 = doc.createTextNode(f); //création de la donnée f1 
//	frequence.appendChild(f1); //filiation du noeud “f1” 
	
	// finish by adding the proper XML header
	QDomNode noeud = doc.createProcessingInstruction("xml","version=\"1.0\""); 
	doc.insertBefore(noeud,doc.firstChild()); 
	//sauvegarde dans le flux (2 espaces de décalage dans l’arborescence) 
	doc.save(out,2);    
	file.close(); 
	return true;
}

bool SettingsDialog::readXMLConfigFile(std::string _filename) {
	QString qs = QString::fromStdString(_filename);
	QFile file(qs); 
	if (!file.open(QIODevice::ReadOnly)) 
		return false; 
	if (!doc.setContent(&file)) {   //établit le document XML à  
		file.close(); //partir des données du fichier (hiérarchie, etc) 
		return false; 
	} 
	file.close(); 
	
	int i=0; 
	QString affichage; 
	QDomNodeList tab; 
	QDomElement mesure; 
	QDomNode n; 
	QMessageBox a(0); 
	QDomElement racine = doc.documentElement(); //renvoie la balise racine 
	QDomNode noeud = racine.firstChild(); //renvoie la 1ère balise « mesure » 
	while(!noeud.isNull()) { 
		//convertit le nœud en élément pour utiliser les 
		//méthodes tagName() et attribute() 
		mesure = noeud.toElement();
		//vérifie la présence de la balise « mesure » 
		if (mesure.tagName() == "mesure") 
		{ 
			affichage = mesure.attribute("numero"); //récupère l’attribut 
			tab = mesure.childNodes(); //crée un tableau des enfants de « mesure » 
			for(i=0;i<tab.length();i++) 
			{ 
				//pour chaque enfant, on extrait la donnée et on concatène 
				n = tab.item(i); 
				affichage = affichage + " " + n.firstChild().toText().data(); 
			} 
			a.setText(affichage); //affichage dans un QMessageBox 
			a.exec(); 
		} 
		noeud = noeud.nextSibling(); //passe à la “mesure” suivante 
	} 
	
}

//void SettingsDialog::configureFeaturesPlugins(){
//	ACPluginManager *acpl = media_cycle->getPluginManager(); //getPlugins
//	if (acpl) {
//		for (int i=0;i<acpl->getSize();i++) {
//			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
//				if (acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_FEATURES) {
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


void SettingsDialog::comboMediaTypeValueChanged(){
	this->media_type = comboMediaType->currentText().toStdString();
//	cout << "(if you press Apply) media type will change to : " << this->media_type << endl;
}

void SettingsDialog::comboBrowserModeValueChanged(){
	this->browser_mode = comboBrowserMode->currentText().toStdString();
//	cout << "(if you press Apply) browser mode will change to : " << this->browser_mode << endl;
}

void SettingsDialog::on_buttonAddPluginsLibrary_clicked(){
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

void SettingsDialog::on_buttonApplyCurrentSettings_clicked(){
	stringToMediaTypeConverter::const_iterator iterm = stringToMediaType.find(this->media_type);
	if( iterm == stringToMediaType.end() ) {
		cout << " media type not found : " << this->media_type << endl;
		return;
	}
	ACMediaType new_media_type = iterm->second;
	cout << iterm->first << " - corresponding media type code : " << new_media_type << endl;
	
	stringToBrowserModeConverter::const_iterator iterb = stringToBrowserMode.find(this->browser_mode);
	if( iterb == stringToBrowserMode.end() ) {
		cout << " browser mode not found : " << this->browser_mode << endl;
		return;
	}
	ACBrowserMode new_browser_mode = iterb->second;
	cout << iterb->first << " - corresponding browser mode code : " << new_browser_mode << endl;
	
	if (this->media_cycle != NULL) {
		// clean the current media_cycle that was used by the ACMultiMediaCycleOsgQt app.
		this->multi_media_cycle->destroyMediaCycle();
	}
	
	this->multi_media_cycle->createMediaCycle(new_media_type, new_browser_mode);
	this->media_cycle = this->multi_media_cycle->getMediaCycle();

}

void SettingsDialog::addPluginsFromLibrary(QString _fileName){
	if (media_cycle == NULL) {
		cout << "load media_cycle first" << endl;
		return;
	}
	string plugins_library = _fileName.toStdString();
	ACPluginManager *acpl = media_cycle->getPluginManager();
	if (acpl->add(plugins_library) <=0 ){
		cerr << "Problem adding Plugins Library : " << plugins_library<< endl;
		return;
	}
	
	ACPluginLibrary* plib = acpl->getPluginLibrary(plugins_library);
	if (plib == NULL) {
		cout << "Problem loading plugins library: " << plugins_library << endl;
		return;
	}
	
	// add library item
	QVector<QVariant> libraryData;
	libraryData << _fileName << " - ";
	pluginsTreeItem* libraryItem = new pluginsTreeItem(libraryData, ptm->getRootItem());

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
		pluginsTreeItem* pluginItem = new pluginsTreeItem(pluginData, libraryItem);
		libraryItem->appendChild(pluginItem);
		p++;
//		item->setCheckState (Qt::Unchecked);
	}
	
	ptm->addRow(libraryItem);
//	for (int column = 0; column < ptm->columnCount(); ++column)
//        treeViewPluginsLibrairies->resizeColumnToContents(column);

	// XS TODO: do this at the end, after selection is made.
	// keep track of all libraries added
	this->multi_media_cycle->addPluginsLibrary(plugins_library);
}

//void SettingsDialog::on_buttonRemovePluginsLibrary_clicked(){
//	QListWidgetItem *curitem = listWidgetPluginsLibraries->currentItem();
//	
//	if (curitem) {
//		string pluginLibraryName = curitem->text().toStdString();
//		int r = listWidgetPluginsLibraries->row(curitem);
//		listWidgetPluginsLibraries->takeItem(r);
//		delete curitem;
//		this->multi_media_cycle->removePluginsLibrary(pluginLibraryName);
//	}	
//}

//void SettingsDialog::removePluginsFromLibrary(QString _fileName){
//	if (media_cycle == NULL) {
//		cout << "load media_cycle first" << endl;
//		return;
//	}
//	string plugins_library = _fileName.toStdString();
//	ACPluginManager *acpl = media_cycle->getPluginManager();
//	
//	ACPluginLibrary* plib = acpl->getPluginLibrary(plugins_library);
//	if (plib == NULL) {
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
//	this->multi_media_cycle->removePluginsLibrary(plugins_library);
//}

void SettingsDialog::on_buttonConfirmPluginsSelection_clicked(){
//	cout << "confirming plugins selection : " << listWidgetFeaturesPlugins->count()<< endl;
//	for (int i=0; i < listWidgetFeaturesPlugins->count(); i++) {
//		// XS TODO: if checkState... (so we only add the selected plugins)
//		cout << listWidgetFeaturesPlugins->item(i)->text().toStdString() << endl;
//		if (listWidgetFeaturesPlugins->item(i)->checkState() == Qt::Checked){
//			multi_media_cycle->addPluginItem(listWidgetFeaturesPlugins->item(i));
//		}
//	}
	
	// then the selected plugins get their weights synchronized with MediaCycle
	multi_media_cycle->synchronizeFeaturesWeights();
}


bool SettingsDialog::setXMLMediaType(QString _qmt){
	// XS TODO test if media type has already been added to XML
	// if yes, change it
	QDomElement e_mt = doc.createElement("MediaType"); 
	configFile.appendChild( e_mt); 
	QDomText t_mt = doc.createTextNode(_qmt); 
	e_mt.appendChild(t_mt);
	return true;
}

bool SettingsDialog::setXMLBrowserMode(QString _qbm){
	// XS TODO test if browser mode has already been added to XML
	// if yes, change it
	QDomElement e_bm = doc.createElement("BrowserMode"); 
	configFile.appendChild( e_bm); 
	QDomText t_bm = doc.createTextNode(_qbm); 
	e_bm.appendChild(t_bm);
	return true;	
}

void SettingsDialog::updateActions(){
    bool hasSelection = !treeViewPluginsLibrairies->selectionModel()->selection().isEmpty();
    buttonRemovePluginsLibrary->setEnabled(hasSelection);
}


