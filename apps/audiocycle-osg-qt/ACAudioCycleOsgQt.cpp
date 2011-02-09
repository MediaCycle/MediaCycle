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
		if ( vizplugloaded == 1 )
		{
			//CF this should be on a separate function or even on a mediacycle-(osg-)qt class
			ACPluginManager *acpl = media_cycle->getPluginManager();
			if (acpl) {
				for (int i=0;i<acpl->getSize();i++) {
					for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
						//if (acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType() == media_cycle->getLibrary()->getMediaType()) {
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
		//media_cycle->addPlugin("../../../plugins/segmentation/" + build_type + "/mc_segmentation.dylib");	
	#endif

	audio_engine = new ACAudioEngine();
	audio_engine->setMediaCycle(media_cycle);
	audio_engine->printDeviceList();
	audio_engine->printCaptureDeviceList();
	
	osc_feedback = NULL;
	osc_browser = NULL;
	
	//ui.compositeOsgView->move(0,20);
	ui.compositeOsgView->setMediaCycle(media_cycle);
	ui.compositeOsgView->setAudioEngine(audio_engine);
	ui.compositeOsgView->prepareFromBrowser();
	ui.compositeOsgView->prepareFromTimeline();
	//compositeOsgView->setPlaying(true);
	
	connect(ui.actionLoad_Media_Directory, SIGNAL(triggered()), this, SLOT(loadMediaDirectory()));
	connect(ui.actionLoad_Media_Files, SIGNAL(triggered()), this, SLOT(loadMediaFiles()));
	connect(ui.actionLoad_ACL, SIGNAL(triggered()), this, SLOT(on_pushButtonLaunch_clicked()));
	connect(ui.actionSave_ACL, SIGNAL(triggered()), this, SLOT(saveACLFile()));
	
	this->show();
	
	#ifdef USE_APPLE_MULTITOUCH
	/*
		multitouch_trackpad = new ACAppleMultitouchTrackpadSupport();
		multitouch_trackpad->setMediaCycle(media_cycle);
		multitouch_trackpad->start();
	*/ 
	#endif
	//ui.compositeOsgView->setFocus();
}

ACAudioCycleOsgQt::~ACAudioCycleOsgQt()
{
	//compositeOsgView->setPlaying(false);
	if (osc_browser) {
		osc_browser->stop(mOscReceiver);		
		//osc_browser->release(mOscReceiver);//should be in destructor ?
	}

	#ifdef USE_APPLE_MULTITOUCH
		//multitouch_trackpad->stop();
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
	// media_cycle->pushNavigationState();  XS 250810 removed
	//media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
	media_cycle->getBrowser()->setState(AC_CHANGING);
	media_cycle->getBrowser()->updateNextPositions();
	ui.compositeOsgView->prepareFromBrowser();
	ui.compositeOsgView->prepareFromTimeline();
	//ui.compositeOsgView->setPlaying(true);
	media_cycle->setNeedsDisplay(true);
	library_loaded = true;
	
	//XS new, use this carefully 
	this->configureCheckBoxes();//CF debug, might call prepareNodes twice
	
	ui.compositeOsgView->setFocus();
}

void ACAudioCycleOsgQt::on_pushButtonLaunch_clicked()
{
	this->loadACLFile();
	//ui.compositeOsgView->setFocus();
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
	
	QFileDialog dialog(this,"Open MediaCycle Library File(s)");
	dialog.setDefaultSuffix ("acl");
	dialog.setNameFilter("MediaCycle Library Files (*.acl)");
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
		media_cycle->importACLLibrary(fileName.toStdString());//(char*) fileName.toStdString().c_str());
		media_cycle->normalizeFeatures();
		media_cycle->libraryContentChanged();
		std::cout << "File library imported" << std::endl;
		this->updateLibrary();
	}
	//ui.compositeOsgView->setFocus();
}

void ACAudioCycleOsgQt::saveACLFile(){	
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
	//ui.compositeOsgView->setFocus();
}

// XS TODO: make sure it works if we add a new directory to the existing library ?
void ACAudioCycleOsgQt::loadMediaDirectory(){
	
	QString selectDir = QFileDialog::getExistingDirectory
	(
	 this, 
	 tr("Open Directory"),
	 "",
	 QFileDialog::DontResolveSymlinks
	 );
	
	// XS TODO : check if directory exists
	
	media_cycle->importDirectory(selectDir.toStdString(), 1);
	media_cycle->normalizeFeatures();
	media_cycle->libraryContentChanged(); 
	this->updateLibrary();
	//ui.compositeOsgView->setFocus();
}

void ACAudioCycleOsgQt::loadMediaFiles(){
	QString fileName;
	QFileDialog dialog(this,"Open MediaCycle Media File(s)");
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
	//dialog.setDefaultSuffix ("wav");
	//dialog.setNameFilter("Supported Audio Files (*.wav *.aif)");
	dialog.setNameFilter(mediaExts);
	dialog.setFileMode(QFileDialog::ExistingFiles); // ExistingFile(s); "s" is for multiple file handling
	

	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	
	QStringList::Iterator file = fileNames.begin();
	while(file != fileNames.end()) {
		//std::cout << "File library: '" << (*file).toStdString() << "'" << std::endl;
		fileName = *file;
		++file;
	//std::cout << "Will open: '" << fileName.toStdString() << "'" << std::endl;
	//fileName = QFileDialog::getOpenFileName(this, "~", );
	
		if (!(fileName.isEmpty())) {
			
			media_cycle->importDirectory((char*) fileName.toStdString().c_str(), 0);
			//media_cycle->normalizeFeatures();
			//media_cycle->libraryContentChanged();
			std::cout << "File library imported" << std::endl;
			// XS do this only after loading all files (it was in the while loop) !
			// XS for CF: in ImageCycle I put "libraryContentChanged" inside updateLibrary
			media_cycle->libraryContentChanged();
			this->updateLibrary();
		}	
	}
}

void ACAudioCycleOsgQt::configureCheckBoxes(){
	// dynamic config of checkboxes
	// according to plugins found by plugin manager
	ACPluginManager *acpl = media_cycle->getPluginManager(); //getPlugins
	if (acpl) {
		for (int i=0;i<acpl->getSize();i++) {
			for (int j=0;j<acpl->getPluginLibrary(i)->getSize();j++) {
				if (acpl->getPluginLibrary(i)->getPlugin(j)->getPluginType() == PLUGIN_TYPE_FEATURES && acpl->getPluginLibrary(i)->getPlugin(j)->getMediaType() == media_cycle->getLibrary()->getMediaType()) {
					QString s(acpl->getPluginLibrary(i)->getPlugin(j)->getName().c_str());
					QListWidgetItem * item = new QListWidgetItem(s,ui.featuresListWidget);
					item->setCheckState (Qt::Unchecked);
					std::cout << "Using feature extraction plugin: " << acpl->getPluginLibrary(i)->getPlugin(j)->getName() << std::endl;
				}
			}
		}
	}
	
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
void ACAudioCycleOsgQt::cleanCheckBoxes(){
}

// NEW SLOTS
void ACAudioCycleOsgQt::modifyListItem(QListWidgetItem *item)
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
		ui.compositeOsgView->updateTransformsFromBrowser(0.0); 
	}
}

void ACAudioCycleOsgQt::synchronizeFeaturesWeights(){
	// synchronize weights with what is loaded in mediacycle
	// note: here weights are 1 or 0 (checkbox).
	// conversion: 0 remains 0, and value > 0 becomes 1.
	vector<float> w = media_cycle->getWeightVector();
	int nw = w.size();
	/*
	if (ui.featuresListWidget->count() != nw){
		cerr << "Checkboxes in GUI do not match Features in MediaCycle" << endl;
		cerr << ui.featuresListWidget->count() << "!=" << nw << endl;
		exit(1);
	}
	else {*/
		for (int i=0; i< ui.featuresListWidget->count(); i++){ //for (int i=0; i< nw; i++){
			if (w[i]==0) 
				ui.featuresListWidget->item(i)->setCheckState (Qt::Unchecked);
			else
				ui.featuresListWidget->item(i)->setCheckState (Qt::Checked);		
		}
	//}
}


void ACAudioCycleOsgQt::on_pushButtonRecenter_clicked()
{
	media_cycle->setCameraRecenter();
	//ui.compositeOsgView->setFocus();
}

void ACAudioCycleOsgQt::on_pushButtonBack_clicked()
{
	media_cycle->goBack();
	//ui.compositeOsgView->setFocus();
}

void ACAudioCycleOsgQt::on_pushButtonForward_clicked()
{
	media_cycle->goForward();
	//ui.compositeOsgView->setFocus();
}

void ACAudioCycleOsgQt::on_radioButtonClusters_toggled()
{
	//std::cout << ui.radioButtonClusters->isChecked() << std::endl;
	ACBrowserMode mode = (ui.radioButtonClusters->isChecked() ? AC_MODE_CLUSTERS : AC_MODE_NEIGHBORS);
	if (media_cycle && media_cycle->hasBrowser()) //CF and test on library loaded?
		media_cycle->getBrowser()->switchMode( mode );
}	

void ACAudioCycleOsgQt::on_sliderClusters_sliderReleased()
{
	std::cout << "ClusterNumber: " << ui.sliderClusters->value() << std::endl;
	if (library_loaded){
		media_cycle->setClusterNumber(ui.sliderClusters->value());
		// XSCF251003 added this
		media_cycle->updateDisplay(true); //XS 250310 was: media_cycle->updateClusters(true);
		// XS250310 removed mediacycle->setNeedsDisplay(true); // now in updateDisplay
		//ui.compositeOsgView->updateTransformsFromBrowser(1.0);
	}
	//ui.compositeOsgView->setFocus();
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
	//ui.compositeOsgView->setFocus();
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
	//ui.compositeOsgView->setFocus();
}

void ACAudioCycleOsgQt::on_pushButtonMuteAll_clicked()
{
	media_cycle->muteAllSources();
	//ui.compositeOsgView->setFocus();
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
	//ui.compositeOsgView->setFocus();
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
	//ui.compositeOsgView->setFocus();
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
		float distance = ui.compositeOsgView->getBrowserRenderer()->getDistanceMouse()[closest_node];
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
		media_cycle->normalizeFeatures();
		media_cycle->libraryContentChanged();
		std::cout << "File library imported" << std::endl;
		this->updateLibrary();
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/library/clear") == 0)
	{
		media_cycle->cleanLibrary(); // XS instead of getImageLibrary CHECK THIS
		media_cycle->libraryContentChanged();
		media_cycle->setReferenceNode(0);
		// XSCF 250310 added these 3
		// media_cycle->pushNavigationState();  XS 250810 removed
		media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe
		media_cycle->getBrowser()->setState(AC_CHANGING);
		
		ui.compositeOsgView->prepareFromBrowser();
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
		//int node = media_cycle->getClosestNode();
		int node = media_cycle->getLastSelectedNode();
		
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
		//int node = media_cycle->getClosestNode();
		int node = media_cycle->getLastSelectedNode();
		
		if (node > -1)
		{
			//media_cycle->pickedObjectCallback(-1);
			audio_engine->setLoopSynchroMode(node, ACAudioEngineSynchroModeManual);
			audio_engine->setLoopScaleMode(node, ACAudioEngineScaleModeResample);//ACAudioEngineScaleModeVocode
			audio_engine->setScrub((float)scrub*100); // temporary hack to scrub between 0 an 1
		}
	}
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/pitch") == 0)
	{
		float pitch;
		osc_browser->readFloat(mOscReceiver, &pitch);
		
		//int node = media_cycle->getClickedNode();
		//int node = media_cycle->getClosestNode();
		int node = media_cycle->getLastSelectedNode();
		
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
			// media_cycle->pushNavigationState(); XS 250810 removed
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
