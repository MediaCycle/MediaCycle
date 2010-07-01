/*
 *  ACAudioCycleOsgQt.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 16/02/10
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
#include "ACAudioCycleOsgQt.h"

static void osc_callback(ACOscBrowserRef, const char *tagName, void *userData)
{
	ACAudioCycleOsgQt *window = (ACAudioCycleOsgQt*)userData;
	//printf("osc received tag: %s\n", tagName);
	//std::cout << "tagName: " << tagName << std::endl;
	window->processOscMessage(tagName);
}

ACAudioCycleOsgQt::ACAudioCycleOsgQt(QWidget *parent)
: QMainWindow(parent), library_loaded(false)
{
	ui.setupUi(this); // first thing to do
	media_cycle = new MediaCycle(MEDIA_TYPE_AUDIO,"/tmp/","mediacycle.acl");
	
	// XS TODO fichier de configuration
	#if defined(__APPLE__)
		std::string build_type ("Release");
		#ifdef USE_DEBUG
			build_type = "Debug";
		#endif
		int vizplugloaded = media_cycle->addPlugin("../../../plugins/visualisation/" + build_type + "/mc_visualisation.dylib");
		if ( vizplugloaded == 0 )
		{
			//CF this should be on a separate function or even on a mediacycle-(osg-)qt class
			ACPluginManager *acpl = media_cycle->getPluginManager();
			if (acpl) {
				for (int i=0;i<acpl->getSize();i++) {
					for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
						//if (acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType() == MEDIA_TYPE_AUDIO) {
						if (acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_CLUSTERS_METHOD) {
							//CF on the first detected Clusters Method plugin
							if (ui.comboBoxClustersMethod->count() == 1 && ui.comboBoxClustersMethod->currentText().toStdString() == "KMeans (default)") {
								ui.comboBoxClustersMethod->setEnabled(true);
								//CF default settings: no Clusters Method plugin, use KMeans 
							}
							ui.comboBoxClustersMethod->addItem(QString(acpl->getPluginLibrary(i)->getPlugin(j)->getName().c_str()));
							
						}
						else if (acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_CLUSTERS_POSITIONS) {
							//CF on the first detected Clusters Positions plugin
							if (ui.comboBoxClustersPositions->count() == 1 && ui.comboBoxClustersPositions->currentText().toStdString() == "Propeller (default)") {
								ui.comboBoxClustersPositions->setEnabled(true);
								//CF default settings: no Clusters Positions plugin, use Propeller 
							}	
							ui.comboBoxClustersPositions->addItem(QString(acpl->getPluginLibrary(i)->getPlugin(j)->getName().c_str()));
							
						}
						else if (acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_NEIGHBORS_METHOD) {
							//CF on the first detected Neighbors Method plugin
							if (ui.comboBoxNeighborsMethod->count() == 1 && ui.comboBoxNeighborsMethod->currentText().toStdString() == "None available") {
								ui.comboBoxNeighborsMethod->setEnabled(true);
								ui.comboBoxNeighborsMethod->clear();
								//CF default settings: no factory method available, use the first detected Neighbors Method plugin
								media_cycle->setNeighborsMethodPlugin(acpl->getPluginLibrary(i)->getPlugin(j)->getName());
							}	
							ui.comboBoxNeighborsMethod->addItem(QString(acpl->getPluginLibrary(i)->getPlugin(j)->getName().c_str()));
						}
						else if (acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_NEIGHBORS_POSITIONS) {
							//CF on the first detected Neighbors Positions plugin
							if (ui.comboBoxNeighborsPositions->count() == 1 && ui.comboBoxNeighborsPositions->currentText().toStdString() == "None available") {
								ui.comboBoxNeighborsPositions->setEnabled(true);
								ui.comboBoxNeighborsPositions->clear();
								//CF default settings: no factory method available, use the first detected Neighbors Positions plugin
								media_cycle->setNeighborsPositionsPlugin(acpl->getPluginLibrary(i)->getPlugin(j)->getName());
							}	
							ui.comboBoxNeighborsPositions->addItem(QString(acpl->getPluginLibrary(i)->getPlugin(j)->getName().c_str()));
						}
						//CF we don't yet deal with Visualisation Plugins (combining Methods and Positions for Clusters and/or Neighborhoods)
					}
				}
			}
		}
		media_cycle->addPlugin("../../../plugins/audio/" + build_type + "/mc_audio.dylib");	
		media_cycle->addPlugin("../../../plugins/segmentation/" + build_type + "/mc_segmentation.dylib");	
	#endif

	audio_engine = new ACAudioEngine();
	audio_engine->setMediaCycle(media_cycle);
	audio_engine->printDeviceList();
	audio_engine->printCaptureDeviceList();
	
	osc_feedback = NULL;
	osc_browser = NULL;
	
	ui.browserOsgView->move(0,20);
	ui.browserOsgView->setMediaCycle(media_cycle);
	ui.browserOsgView->prepareFromBrowser();
	//browserOsgView->setPlaying(true);
	
	connect(ui.actionLoad_Media_Directory, SIGNAL(triggered()), this, SLOT(loadMediaDirectory()));
	connect(ui.actionLoad_Media_Files, SIGNAL(triggered()), this, SLOT(loadMediaFiles()));
	connect(ui.actionLoad_ACL, SIGNAL(triggered()), this, SLOT(on_pushButtonLaunch_clicked()));
	connect(ui.actionSave_ACL, SIGNAL(triggered()), this, SLOT(saveACLFile()));
	
	this->show();
	
	#ifdef USE_APPLE_MULTITOUCH
		multitouch_trackpad = new ACAppleMultitouchTrackpadSupport();
		multitouch_trackpad->setMediaCycle(media_cycle);
		multitouch_trackpad->start();
	#endif
	//ui.browserOsgView->setFocus();
}

ACAudioCycleOsgQt::~ACAudioCycleOsgQt()
{
	//browserOsgView->setPlaying(false);
	if (osc_browser) {
		osc_browser->stop(mOscReceiver);		
		//osc_browser->release(mOscReceiver);//should be in destructor ?
	}

	#ifdef USE_APPLE_MULTITOUCH
		multitouch_trackpad->stop();
	#endif
	
	delete osc_browser;
	delete osc_feedback;//osc_feedback destructor calls ACOscFeedback::release()
	delete audio_engine;
	delete media_cycle;
	//delete mOscReceiver;
	//delete mOscFeeder;
}

void ACAudioCycleOsgQt::updateLibrary()
{
	if (!library_loaded) {
		// set to 0 the first time a library is loaded
		// XSCF do we always want this to be 0 ?
		media_cycle->setReferenceNode(0);
	}
	// XSCF 250310 added these 3
	media_cycle->pushNavigationState();
	//media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
	media_cycle->getBrowser()->setState(AC_CHANGING);
	
	ui.browserOsgView->prepareFromBrowser();
	//ui.browserOsgView->setPlaying(true);
	media_cycle->setNeedsDisplay(true);
	library_loaded = true;
	ui.browserOsgView->setFocus();
}

void ACAudioCycleOsgQt::on_pushButtonLaunch_clicked()
{
	this->loadACLFile();
	//ui.browserOsgView->setFocus();
}

void ACAudioCycleOsgQt::on_pushButtonClean_clicked()
{
	media_cycle->cleanLibrary();
	media_cycle->cleanUserLog();
	media_cycle->libraryContentChanged();
	this->updateLibrary();
	library_loaded = false;
}	

void ACAudioCycleOsgQt::loadACLFile(){
	QString fileName;
	
	QFileDialog dialog(this,"Open AudioCycle Library File(s)");
	dialog.setDefaultSuffix ("acl");
	dialog.setNameFilter("AudioCycle Library Files (*.acl)");
	dialog.setFileMode(QFileDialog::ExistingFile); // change to ExistingFiles for multiple file handling
	
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	
	QStringList::Iterator file = fileNames.begin();
	while(file != fileNames.end()) {
		//std::cout << "File library: '" << (*file).toStdString() << "'" << std::endl;
		fileName = *file;
		++file;
	}
	std::cout << "Will open: '" << fileName.toStdString() << "'" << std::endl;
	//fileName = QFileDialog::getOpenFileName(this, "~", );
	
	if (!(fileName.isEmpty())) {
		media_cycle->importACLLibrary(fileName.toStdString());//(char*) fileName.toStdString().c_str());
		//media_cycle->setNeedsDisplay(true);//CF
		//ui.browserOsgView->updateTransformsFromBrowser(1.0);//CF
		media_cycle->normalizeFeatures();
		media_cycle->libraryContentChanged();
		std::cout << "File library imported" << std::endl;
		this->updateLibrary();
	}
	//ui.browserOsgView->setFocus();
}

void ACAudioCycleOsgQt::saveACLFile(){
	cout << "Saving ACL File..." << endl;
	
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save as MediaCycle Library"),"",tr("MediaCycle Library (*.acl)"));
	QFile file(fileName);
	
	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(this,
							 tr("File error"),
							 tr("Failed to open\n%1").arg(fileName));
	} 
	else {
		string acl_file = fileName.toStdString();
		cout << "saving ACL file: " << acl_file << endl;
		media_cycle->saveACLLibrary(acl_file);
	}
	//ui.browserOsgView->setFocus();
}

void ACAudioCycleOsgQt::loadMediaDirectory(){
	
	QString selectDir = QFileDialog::getExistingDirectory
	(
	 this, 
	 tr("Open Directory"),
	 "",
	 QFileDialog::DontResolveSymlinks
	 );
	
	// XS TODO : check if directory exists
	// XS : do not separate directory and files in Qt and let MediaCycle handle it
	
	media_cycle->importDirectory(selectDir.toStdString(), 1, 0);
	// with this function call here, do not import twice!!!
	// XS TODO: what if we add a new directory to the existing library ?
	media_cycle->normalizeFeatures();
	media_cycle->libraryContentChanged(); 
	this->updateLibrary();
	//ui.browserOsgView->setFocus();
	
	//	QStringList listFilter;
	//	listFilter << "*.png";
	//	listFilter << "*.jpg";
	//	
	//	QDirIterator dirIterator(selectDir, listFilter ,QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
	//	
	//	// Variable qui contiendra tous les fichiers correspondant à notre recherche
	//	QStringList fileList; 
	//	// Tant qu'on n'est pas arrivé à la fin de l'arborescence...
	//	while(dirIterator.hasNext()) 
	//	{   
	//		// ...on va au prochain fichier correspondant à notre filtre
	//		fileList << dirIterator.next(); 
	//	}
	//	for ( QStringList::Iterator it = fileList.begin(); it != fileList.end(); ++it ) {
	//		cout << (*it).toStdString() << endl;
	//	}	
}

void ACAudioCycleOsgQt::loadMediaFiles(){
	QString fileName;
	
	QFileDialog dialog(this,"Open AudioCycle Media File(s)");
	dialog.setDefaultSuffix ("wav");
	dialog.setNameFilter("Media Files (*.wav)");
	dialog.setFileMode(QFileDialog::ExistingFile); // change to ExistingFiles for multiple file handling
	
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	
	QStringList::Iterator file = fileNames.begin();
	while(file != fileNames.end()) {
		//std::cout << "File library: '" << (*file).toStdString() << "'" << std::endl;
		fileName = *file;
		++file;
	}
	//std::cout << "Will open: '" << fileName.toStdString() << "'" << std::endl;
	//fileName = QFileDialog::getOpenFileName(this, "~", );
	
	if (!(fileName.isEmpty())) {
		
		media_cycle->importDirectory((char*) fileName.toStdString().c_str(),0, 0);
		media_cycle->normalizeFeatures();
		media_cycle->libraryContentChanged();
		std::cout << "File library imported" << std::endl;
		this->updateLibrary();
	}	
	
}

void ACAudioCycleOsgQt::on_pushButtonRecenter_clicked()
{
	media_cycle->setCameraRecenter();
	//ui.browserOsgView->setFocus();
}

void ACAudioCycleOsgQt::on_pushButtonBack_clicked()
{
	media_cycle->goBack();
	//ui.browserOsgView->setFocus();
}

void ACAudioCycleOsgQt::on_pushButtonForward_clicked()
{
	media_cycle->goForward();
	//ui.browserOsgView->setFocus();
}

void ACAudioCycleOsgQt::on_radioButtonClusters_toggled()
{
	//std::cout << ui.radioButtonClusters->isChecked() << std::endl;
	ACBrowserMode mode = (ui.radioButtonClusters->isChecked() ? AC_MODE_CLUSTERS : AC_MODE_NEIGHBORS);
	if (media_cycle && media_cycle->hasBrowser()) //CF and test on library loaded?
		media_cycle->getBrowser()->switchMode( mode );
}	

void ACAudioCycleOsgQt::on_checkBoxRhythm_stateChanged(int state)
{
	media_cycle->setWeight(0,state/2.0f);
	if (library_loaded)
		media_cycle->updateDisplay(true);
}

void ACAudioCycleOsgQt::on_checkBoxTimbre_stateChanged(int state)
{
	media_cycle->setWeight(1,state/2.0f);
	if (library_loaded)
		media_cycle->updateDisplay(true);
}

void ACAudioCycleOsgQt::on_checkBoxHarmony_stateChanged(int state)
{
	media_cycle->setWeight(2,state/2.0f);
	if (library_loaded)
		media_cycle->updateDisplay(true);
}	

void ACAudioCycleOsgQt::on_sliderClusters_sliderReleased()
{
	std::cout << "ClusterNumber: " << ui.sliderClusters->value() << std::endl;
	if (library_loaded){
		media_cycle->setClusterNumber(ui.sliderClusters->value());
		// XSCF251003 added this
		media_cycle->updateDisplay(true); //XS 250310 was: media_cycle->updateClusters(true);
		// XS250310 removed mediacycle->setNeedsDisplay(true); // now in updateDisplay
		//ui.browserOsgView->updateTransformsFromBrowser(1.0);
	}
	//ui.browserOsgView->setFocus();
}

void ACAudioCycleOsgQt::on_comboBoxClustersMethod_activated(const QString & text) 
{
	std::cout << "Clusters Method: " << text.toStdString() << std::endl;
	media_cycle->changeClustersMethodPlugin(text.toStdString());
}

void ACAudioCycleOsgQt::on_comboBoxClustersPositions_activated(const QString & text) 
{
	std::cout << "Clusters Positions: " << text.toStdString() << std::endl;	
	media_cycle->changeClustersPositionsPlugin(text.toStdString());
}


void ACAudioCycleOsgQt::on_comboBoxNeighborsMethod_activated(const QString & text) 
{
	std::cout << "Neighbors Method: " << text.toStdString() << std::endl;
	media_cycle->changeNeighborsMethodPlugin(text.toStdString());
}


void ACAudioCycleOsgQt::on_comboBoxNeighborsPositions_activated(const QString & text) 
{
	std::cout << "Neighbors Method: " << text.toStdString() << std::endl;
	media_cycle->changeNeighborsPositionsPlugin(text.toStdString());	
}

void ACAudioCycleOsgQt::on_sliderBPM_valueChanged() //[0;220]
{
	std::cout << "BPM: " << ui.sliderBPM->value() << std::endl;
	//if (library_loaded){
	int node = media_cycle->getClickedNode();
	if (node > -1)
	{
		audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeAutoBeat);
		audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);
		audio_engine->setBPM(ui.sliderBPM->value());
	}
	//}
	//ui.browserOsgView->setFocus();
}

void ACAudioCycleOsgQt::on_sliderPitch_valueChanged() // [50;200]
{
	std::cout << "Pitch: " << (float) ui.sliderPitch->value()/100.0f << std::endl;
	//if (library_loaded){
	int node = media_cycle->getClickedNode();
	if (node > -1)
	{
		audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeAutoBeat);
		audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);
		audio_engine->setSourcePitch(node, (float) ui.sliderPitch->value()/100.0f); 
	}
	//}
	//ui.browserOsgView->setFocus();
}

void ACAudioCycleOsgQt::on_pushButtonMuteAll_clicked()
{
	media_cycle->muteAllSources();
	//ui.browserOsgView->setFocus();
}

void ACAudioCycleOsgQt::on_pushButtonQueryRecord_toggled()
{
	if (ui.pushButtonQueryRecord->isChecked() == 1)
	{
		if (audio_engine->isCaptureAvailable())
		{
			ui.pushButtonQueryReplay->setEnabled(false);
			ui.pushButtonQueryKeep->setEnabled(false);
			ui.pushButtonQueryReferent->setEnabled(false);
			std::cout <<"Recording..."<<std::endl;
			// CF There is a delay before the recording actually starts: work around with a countdown on a modal window?
			audio_engine->startCapture();
		}
		else
			ui.pushButtonQueryRecord->setChecked(true);
	}
	else
	{
		if (audio_engine->isCaptureAvailable())
		{	
			audio_engine->stopCapture();	
			std::cout <<"Recording done."<<std::endl;
			ui.pushButtonQueryReplay->setEnabled(true);
			ui.pushButtonQueryKeep->setEnabled(true);
			ui.pushButtonQueryReferent->setEnabled(true);
		}	
	}	
}	

void ACAudioCycleOsgQt::on_pushButtonQueryReplay_clicked()
{
	
}

// Pops up a modal window for saving the query as wavefile and add it to the library
void ACAudioCycleOsgQt::on_pushButtonQueryKeep_clicked()
{
	//...
	
	//CF then only re-recording can re-enable the keeping
	ui.pushButtonQueryKeep->setEnabled(false);
}

void ACAudioCycleOsgQt::on_pushButtonQueryReferent_clicked()
{

}

void ACAudioCycleOsgQt::on_pushButtonControlStart_clicked()
{
	std::cout << "IP: " << ui.lineEditControlIP->text().toStdString() << std::endl;
	std::cout << "Port: " << ui.lineEditControlPort->text().toInt() << std::endl;
	if ( ui.pushButtonControlStart->text().toStdString() == "Start")
	{	
		osc_browser = new ACOscBrowser();
		mOscReceiver = osc_browser->create(ui.lineEditControlIP->text().toStdString().c_str(), ui.lineEditControlPort->text().toInt());
		osc_browser->setUserData(mOscReceiver, this);
		osc_browser->setCallback(mOscReceiver, osc_callback);
		osc_browser->start(mOscReceiver);
		ui.pushButtonControlStart->setText("Stop");
		
	}	
	else if ( ui.pushButtonControlStart->text().toStdString() == "Stop")
	{	
		osc_browser->stop(mOscReceiver);
		ui.pushButtonControlStart->setText("Start");
	}
	//ui.browserOsgView->setFocus();
}	

void ACAudioCycleOsgQt::on_pushButtonFeedbackStart_clicked()
{
	std::cout << "IP: " << ui.lineEditFeedbackIP->text().toStdString() << std::endl;
	std::cout << "Port: " << ui.lineEditFeedbackPort->text().toInt() << std::endl;
	if ( ui.pushButtonFeedbackStart->text().toStdString() == "Start")
	{	
		osc_feedback = new ACOscFeedback();
		osc_feedback->create(ui.lineEditFeedbackIP->text().toStdString().c_str(), ui.lineEditFeedbackPort->text().toInt());
		ui.pushButtonFeedbackStart->setText("Stop");
	}	
	else if ( ui.pushButtonFeedbackStart->text().toStdString() == "Stop")
	{	
		osc_feedback->release();//mOscFeeder);
		ui.pushButtonFeedbackStart->setText("Start");
	}
	//ui.browserOsgView->setFocus();
}	

void ACAudioCycleOsgQt::processOscMessage(const char* tagName)
{	
	if(strcasecmp(tagName, "/audiocycle/test") == 0)
	{
		std::cout << "OSC communication established" << std::endl;

		if (osc_feedback)
		{
			osc_feedback->messageBegin("/audiocycle/received");
			osc_feedback->messageEnd();
			osc_feedback->messageSend();
		}
	}
	else if(strcasecmp(tagName, "/audiocycle/fullscreen") == 0)
	{
		int fullscreen = 0;
		osc_browser->readInt(mOscReceiver, &fullscreen);
		if (fullscreen == 1)
			ui.groupControls->hide();
		else
			ui.groupControls->show();		
	}	
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/xy") == 0)
	{
		float x = 0.0, y = 0.0;
		media_cycle->getCameraPosition(x,y);
		osc_browser->readFloat(mOscReceiver, &x);
		osc_browser->readFloat(mOscReceiver, &y);
		
		float zoom = media_cycle->getCameraZoom();
		float angle = media_cycle->getCameraRotation();
		float xmove = x*cos(-angle)-y*sin(-angle);
		float ymove = y*cos(-angle)+x*sin(-angle);
		media_cycle->setCameraPosition(xmove/2/zoom , ymove/2/zoom); // norm [-1;1] = 2 (instead of 100 from mediacycle-osg)
		media_cycle->setNeedsDisplay(true);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/hover/xy") == 0)
	{
		float x = 0.0, y = 0.0;
		osc_browser->readFloat(mOscReceiver, &x);
		osc_browser->readFloat(mOscReceiver, &y);
		
		media_cycle->hoverCallback(x,y);
		int closest_node = media_cycle->getClosestNode();
		float distance = ui.browserOsgView->getRenderer()->getDistanceMouse()[closest_node];
		if (osc_feedback)
		{
			osc_feedback->messageBegin("/audiocycle/closest_node_at");
			osc_feedback->messageAppendFloat(distance);
			osc_feedback->messageEnd();
			osc_feedback->messageSend();
		}	
		media_cycle->setNeedsDisplay(true);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/zoom") == 0)
	{
		float zoom;//, refzoom = media_cycle->getCameraZoom();
		osc_browser->readFloat(mOscReceiver, &zoom);
		//zoom = zoom*600/50; // refzoom +
		media_cycle->setCameraZoom((float)zoom);
		media_cycle->setNeedsDisplay(true);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/angle") == 0)
	{
		float angle;//, refangle = media_cycle->getCameraRotation();
		osc_browser->readFloat(mOscReceiver, &angle);
		media_cycle->setCameraRotation((float)angle);
		media_cycle->setNeedsDisplay(true);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/library/load") == 0)
	{
		char *lib_path = NULL;
		lib_path = new char[500]; // wrong magic number!
		osc_browser->readString(mOscReceiver, lib_path, 500); // wrong magic number!
		std::cout << "Importing file library '" << lib_path << "'..." << std::endl;
		media_cycle->importACLLibrary(lib_path); // XS instead of getImageLibrary CHECK THIS
		//updateLibrary();
		std::cout << "File library imported" << std::endl;
		media_cycle->setReferenceNode(0);
		// XSCF 250310 added these 3
		media_cycle->pushNavigationState();
		media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
		media_cycle->getBrowser()->setState(AC_CHANGING);
		
		ui.browserOsgView->prepareFromBrowser();
		media_cycle->setNeedsDisplay(true);
		library_loaded = true;
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/library/clear") == 0)
	{
		media_cycle->cleanLibrary(); // XS instead of getImageLibrary CHECK THIS
		media_cycle->libraryContentChanged();
		media_cycle->setReferenceNode(0);
		// XSCF 250310 added these 3
		media_cycle->pushNavigationState();
		media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
		media_cycle->getBrowser()->setState(AC_CHANGING);
		
		ui.browserOsgView->prepareFromBrowser();
		media_cycle->setNeedsDisplay(true);
		library_loaded = true;
	}	
	else if(strcasecmp(tagName, "/audiocycle/1/browser/recenter") == 0)
	{
		media_cycle->setCameraRecenter();
	}
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/playclosestloop") == 0)
	{	
		media_cycle->pickedObjectCallback(-1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/muteall") == 0)
	{	
		media_cycle->muteAllSources();
	}
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/bpm") == 0)
	{
		float bpm;
		osc_browser->readFloat(mOscReceiver, &bpm);
		//int node = media_cycle->getClickedNode();
		int node = media_cycle->getClosestNode();
		if (node > -1)
		{
			audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeAutoBeat);
			audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);
			audio_engine->setBPM((float)bpm);
		}
	}	
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/scrub") == 0)
	{
		float scrub;
		osc_browser->readFloat(mOscReceiver, &scrub);
		
		//int node = media_cycle->getClickedNode();
		int node = media_cycle->getClosestNode();
		if (node > -1)
		{
			//media_cycle->pickedObjectCallback(-1);
			audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeManual);
			audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);//ACAudioEngineScaleModeVocode
			audio_engine->setScrub((float)scrub*10000); // temporary hack to scrub between 0 an 1
		}
	}
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/pitch") == 0)
	{
		float pitch;
		osc_browser->readFloat(mOscReceiver, &pitch);
		
		//int node = media_cycle->getClickedNode();
		int node = media_cycle->getClosestNode();
		if (node > -1)
		{
			/*
			 if (!is_pitching)
			 {	
			 is_pitching = true;
			 is_scrubing = false;
			 */ 
			//media_cycle->pickedObjectCallback(-1);
			audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeAutoBeat);
			audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);
			//}
			audio_engine->setSourcePitch(node, (float) pitch); 
		}
		
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/recluster") == 0)
	{		
		//int node = media_cycle->getClickedNode();
		int node = media_cycle->getClosestNode();
		if (library_loaded && media_cycle->getBrowser()->getMode() == AC_MODE_CLUSTERS && node > -1)
		{
			media_cycle->setReferenceNode(node);
			media_cycle->pushNavigationState();
			media_cycle->updateDisplay(true);
		}
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/back") == 0)
	{		
		media_cycle->goBack();
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/forward") == 0)
	{		
		media_cycle->goForward();
	}
	//std::cout << "End of OSC process messages" << std::endl;
}
