/*
 *  ACTextCycleOsgQt.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 16/02/10
 *  @author Xavier Siebert
 *
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

#include <iostream>
#include "ACTextCycleOsgQt.h"

// features_known = false : when launching the app we don't know how many features.
// plugins_scanned = false : nor which plugins are available

ACTextCycleOsgQt::ACTextCycleOsgQt(QWidget *parent)
 : QMainWindow(parent), 
  features_known(false), library_loaded(false), plugins_scanned(false)
{
	ui.setupUi(this); // first thing to do

	media_cycle = new MediaCycle(MEDIA_TYPE_TEXT,"/tmp/","mediacycle.acl");
	
	// XS TODO detect if fichier de configuration
	// if not, set default options
	media_cycle->setBrowserMode(AC_MODE_CLUSTERS);

	#if defined(__APPLE__)
		std::string build_type ("Release");
		#ifdef USE_DEBUG
			build_type = "Debug";
		#endif
		media_cycle->addPluginLibrary(QApplication::applicationDirPath().toStdString() + "/../../../plugins/text/" + build_type + "/mc_text.dylib");
	#elif defined (__WIN32__)
		media_cycle->addPluginLibrary(QApplication::applicationDirPath().toStdString() + "\..\..\plugins\text\mc_text.dll");
	#else
		media_cycle->addPluginLibrary(QApplication::applicationDirPath().toStdString() + "/../../plugins/text/mc_text.so");
	#endif
	
	std::string s_path = QApplication::applicationDirPath().toStdString();
	std::string v_plugin;
	v_plugin = s_path + "/../../../plugins/visualisation/" + build_type + "/mc_visualisation.dylib";
	std::cout<<v_plugin;
//	p_plugin = s_path + "/../../../../../../plugins/visualisation/" + build_type + "/mc_visualisation.dylib";
	media_cycle->addPluginLibrary(v_plugin);

	media_cycle->setClustersMethodPlugin("ACCosKMeans");
	media_cycle->setClustersPositionsPlugin("ACCosClustPosition");
	media_cycle->setPreProcessPlugin("TextFeatures");
	
	ui.browserOsgView->move(0,20);
	ui.browserOsgView->setMediaCycle(media_cycle);
	ui.browserOsgView->prepareFromBrowser();
	//browserOsgView->setPlaying(true);
	
	connect(ui.actionLoad_Media_Directory, SIGNAL(triggered()), this, SLOT(loadMediaDirectory()));
	connect(ui.actionLoad_Media_Files, SIGNAL(triggered()), this, SLOT(loadMediaFiles()));
	connect(ui.actionLoad_ACL, SIGNAL(triggered()), this, SLOT(on_pushButtonLaunch_clicked()));
	connect(ui.actionSave_ACL, SIGNAL(triggered()), this, SLOT(saveACLFile()));
	connect(ui.actionEdit_Config_File, SIGNAL(triggered()), this, SLOT(editConfigFile()));

	// connect spinBox and slider
	connect(ui.spinBoxClusters, SIGNAL(valueChanged(int)), this, SLOT(spinBoxClustersValueChanged(int)));
	connect(ui.sliderClusters, SIGNAL(valueChanged(int)), ui.spinBoxClusters , SIGNAL(valueChanged(int)));

	// uses another window for settings (using the class settingsDialog)
	settingsDialog = new SettingsDialog(parent);
	settingsDialog->setMediaCycleMainWindow(this);

	this->show();
	
	//ui.browserOsgView->setFocus();
}

ACTextCycleOsgQt::~ACTextCycleOsgQt()
{
	ui.browserOsgView->setMediaCycle(0);
	delete media_cycle;
}


void ACTextCycleOsgQt::updateLibrary()
{	
	media_cycle->libraryContentChanged(); 	
	media_cycle->setReferenceNode(0);
	// XSCF 250310 added these 3
	// media_cycle->pushNavigationState(); // XS 250810 removed this
	//media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
	
	// XS TODO this is sooo ugly:
	media_cycle->getBrowser()->setState(AC_CHANGING);
	
	ui.browserOsgView->prepareFromBrowser();
	//browserOsgView->setPlaying(true);
	media_cycle->setNeedsDisplay(true);
	
	// do not re-scan the directory for plugins once they have been loaded
	if (!plugins_scanned) this->configureCheckBoxes();
	
	library_loaded = true;
	ui.browserOsgView->setFocus();
}

void ACTextCycleOsgQt::on_pushButtonLaunch_clicked() {
	this->loadACLFile();
	//ui.browserOsgView->setFocus();
}

void ACTextCycleOsgQt::on_pushButtonClean_clicked()
{
	media_cycle->cleanUserLog();
	media_cycle->cleanLibrary();
	this->updateLibrary();
	library_loaded = false;
}	

void ACTextCycleOsgQt::on_pushButtonRecenter_clicked()
{
	media_cycle->setCameraRecenter();
	//ui.browserOsgView->setFocus();
}	

void ACTextCycleOsgQt::on_pushButtonBack_clicked()
{
	media_cycle->goBack();
	this->synchronizeFeaturesWeights();
//	ui.navigationLineEdit->setText(QString::number(media_cycle->getNavigationLevel()));
	
	// XS debug
	media_cycle->dumpNavigationLevel() ;
	media_cycle->dumpLoopNavigationLevels() ;

	//ui.browserOsgView->setFocus();
}

void ACTextCycleOsgQt::on_pushButtonForward_clicked()
{
	media_cycle->goForward();
	this->synchronizeFeaturesWeights();

//	ui.navigationLineEdit->setText(QString::number(media_cycle->getNavigationLevel()));
	
	// XS debug
	media_cycle->dumpNavigationLevel() ;
	media_cycle->dumpLoopNavigationLevels() ;

	//ui.browserOsgView->setFocus();
}

void ACTextCycleOsgQt::on_sliderClusters_sliderReleased(){
	// for synchronous display of values 
	ui.spinBoxClusters->setValue(ui.sliderClusters->value());
}

void ACTextCycleOsgQt::spinBoxClustersValueChanged(int _value)
{
	ui.sliderClusters->setValue(_value); 	// for synchronous display of values 

	std::cout << "ClusterNumber: " << _value << std::endl;
	if (library_loaded){
		media_cycle->setClusterNumber(_value);
		// XSCF251003 added this
		media_cycle->updateDisplay(true); //XS 250310 was: media_cycle->updateClusters(true);
		// XS 310310 removed media_cycle->setNeedsDisplay(true); // now in updateDisplay
		ui.browserOsgView->updateTransformsFromBrowser(1.0);
	}
	//ui.browserOsgView->setFocus();
}

void ACTextCycleOsgQt::loadACLFile(){
	QString fileName;
	
	QFileDialog dialog(this,"Open TextCycle Library File(s)");
	dialog.setDefaultSuffix ("acl");
	dialog.setNameFilter("TextCycle Library Files (*.acl)");
	dialog.setFileMode(QFileDialog::ExistingFiles); 
	// change to ExistingFiles for multiple file handling
	
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	
	QStringList::Iterator file = fileNames.begin();
	while(file != fileNames.end()) {
		fileName = *file;
		std::cout << "Opening ACL file: '" << fileName.toStdString() << "'" << std::endl;
		//fileName = QFileDialog::getOpenFileName(this, "~", );
		
		if (!(fileName.isEmpty())) {
			media_cycle->importACLLibrary(fileName.toStdString());
			std::cout << "File library imported" << std::endl;
		}	
		++file;
	}	
	
	// only after loading all ACL files:
	this->updateLibrary();
	
	// already done in media_cycle->importACLLibrary().
	// media_cycle->normalizeFeatures();

	// XS watch this one...
	media_cycle->storeNavigationState(); 
		
	// XS debug
	media_cycle->dumpNavigationLevel();
	media_cycle->dumpLoopNavigationLevels() ;
}

void ACTextCycleOsgQt::saveACLFile(){	
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save as MediaCycle Library"),"",tr("MediaCycle Library (*.acl)"));
	QFile file(fileName);
	
	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(this,
							 tr("File error"),
							 tr("Failed to open\n%1").arg(fileName));
	} 
	else {
		string acl_file = fileName.toStdString();
		cout << "Saving ACL file: " << acl_file << endl;
		media_cycle->saveACLLibrary(acl_file);
	}
	//ui.browserOsgView->setFocus();
}

// XS TODO: make sure it works if we add a new directory to the existing library ?
void ACTextCycleOsgQt::loadMediaDirectory(){

	QString selectDir = QFileDialog::getExistingDirectory
	(
	 this, 
	 tr("Open Directory"),
	 "",
	 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);
	
	// check if directory exists
	if (media_cycle->importDirectory(selectDir.toStdString(), 1) > 0){
		media_cycle->normalizeFeatures();
		this->updateLibrary();
	}
	//ui.browserOsgView->setFocus();
}

void ACTextCycleOsgQt::loadMediaFiles(){
	QString fileName;
	QFileDialog dialog(this,"Open MediaCycle Text File(s)");
	//CF generating supported file extensions from used media I/O libraries and current media type:
	std::vector<std::string> mediaExt = media_cycle->getExtensionsFromMediaType( media_cycle->getLibrary()->getMediaType() );
	QString mediaExts = "Supported Extensions (";
	std::vector<std::string>::iterator mediaIter = mediaExt.begin();
	for(;mediaIter!=mediaExt.end();++mediaIter){
		if (mediaIter != mediaExt.begin())
			mediaExts.append(" ");
		mediaExts.append("*");
		mediaExts.append(QString((*mediaIter).c_str()));
	}		
	mediaExts.append(")");
	//dialog.setDefaultSuffix ("png");
	//dialog.setNameFilter("Text Files (*.jpg *.png *.tif *.tiff)");
	dialog.setNameFilter(mediaExts);
	dialog.setFileMode(QFileDialog::ExistingFiles); // ExistingFile(s); "s" is for multiple file handling

	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	
	QStringList::Iterator file = fileNames.begin();
	while(file != fileNames.end()) {
		fileName = *file;
		++file;
		std::cout << "Opening: '" << fileName.toStdString() << "'" << std::endl;
		//fileName = QFileDialog::getOpenFileName(this, "~", );
	
		if (!(fileName.isEmpty())) {
			media_cycle->importDirectory(fileName.toStdString(), 0);
			//media_cycle->normalizeFeatures();
			//media_cycle->libraryContentChanged(); no, this is in updatelibrary
			std::cout <<  fileName.toStdString() << " imported" << std::endl;
		}
	}
	// XS do this only after loading all files (it was in the while loop) !
	this->updateLibrary();
}

void ACTextCycleOsgQt::editConfigFile(){
	cout << "Editing config file with GUI..." << endl;
	settingsDialog->show();
	settingsDialog->setFocus();
}

void ACTextCycleOsgQt::configureCheckBoxes(){
	// dynamic config of checkboxes
	// according to plugins found by plugin manager
	
	ACPluginManager *acpl = media_cycle->getPluginManager(); //getPlugins
	if (acpl) {
		for (int i=0;i<acpl->getSize();i++) {
			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
				if (acpl->getPluginLibrary(i)->getPlugin(j)->implementsPluginType(PLUGIN_TYPE_FEATURES) && acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType() == MEDIA_TYPE_TEXT) {
					QString s(acpl->getPluginLibrary(i)->getPlugin(j)->getName().c_str());
					QListWidgetItem * item = new QListWidgetItem(s,ui.featuresListWidget);
					item->setCheckState (Qt::Unchecked);
				}
			}
		}
	}
	
	plugins_scanned = true;
	this->synchronizeFeaturesWeights();

	connect(ui.featuresListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(modifyListItem(QListWidgetItem*)));

//	w2.show();
	
	//	QCheckBox* toto = new QCheckBox("toto",ui.groupBoxSimilarity);
	//	QCheckBox* toto2 = new QCheckBox("toto2",ui.groupBoxSimilarity);
	//	QCheckBox* toto3 = new QCheckBox("toto3",ui.groupBoxSimilarity);
	//	QCheckBox* toto4 = new QCheckBox("toto4",ui.groupBoxSimilarity);
	//	QCheckBox* toto5 = new QCheckBox("toto5",ui.groupBoxSimilarity);
	//	QCheckBox* toto6 = new QCheckBox("toto6",ui.groupBoxSimilarity);
	//	QGridLayout *lo = new QGridLayout;
	//	lo->addWidget(toto);
	//	lo->addWidget(toto2);
	//	lo->addWidget(toto3);
	//	lo->addWidget(toto4);
	//	lo->addWidget(toto5);
	//	lo->addWidget(toto6);
	//
	//	ui.groupBoxSimilarity->setLayout(lo);
	
	// XS end test -- need to delete !!!
}
void ACTextCycleOsgQt::cleanCheckBoxes(){
	// XS TODO: does this clear the list ?
	ui.featuresListWidget->clear();

	plugins_scanned = false;
}

void ACTextCycleOsgQt::modifyListItem(QListWidgetItem *item)
{
	// XS check
	cout << item->text().toStdString() << endl; // isselected...
	cout << ui.featuresListWidget->currentRow() << endl;
	// end XS check 
	
	if (library_loaded){
		float w;
		if (item->checkState() == Qt::Unchecked) w = 0.0;
		else w = 1.0 ;
		int f =  ui.featuresListWidget->currentRow(); // index of selected feature
		media_cycle->setWeight(f,w);
		media_cycle->updateDisplay(true); 
		//XS 250310 was: media_cycle->updateClusters(true);
		// XS250310 removed mediacycle->setNeedsDisplay(true); // now in updateDisplay
		ui.browserOsgView->updateTransformsFromBrowser(0.0); 
	}
}

void ACTextCycleOsgQt::synchronizeFeaturesWeights(){
	// synchronize weights with what is loaded in mediacycle
	// note: here weights are 1 or 0 (checkbox).
	// conversion: 0 remains 0, and value > 0 becomes 1.
	vector<float> w = media_cycle->getWeightVector();
	int nw = w.size();
	if (ui.featuresListWidget->count() != nw){
		cerr << "Warning: Checkboxes in GUI do not match Features in MediaCycle" << endl;
		cerr << ui.featuresListWidget->count() << "!=" << nw << endl;
		//exit(1);
	}
	else {
		for (int i=0; i< nw; i++){
			if (w[i]==0) 
				ui.featuresListWidget->item(i)->setCheckState (Qt::Unchecked);
			else
				ui.featuresListWidget->item(i)->setCheckState (Qt::Checked);		
		}
	}
}
