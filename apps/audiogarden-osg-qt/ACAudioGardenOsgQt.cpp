/*
 *  ACAudioGardenOsgQt.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 27/04/10
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
#include "ACAudioGardenOsgQt.h"

using namespace std;

static void osc_callback(ACOscBrowserRef, const char *tagName, void *userData)
{
	ACAudioGardenOsgQt *window = (ACAudioGardenOsgQt*)userData;
	//printf("osc received tag: %s\n", tagName);
	//std::cout << "tagName: " << tagName << std::endl;
	window->processOscMessage(tagName);
}

ACAudioGardenOsgQt::ACAudioGardenOsgQt(QWidget *parent)
 : QMainWindow(parent), 
 updatedLibrary(false)
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
			//media_cycle->getBrowser()->setMode(AC_MODE_NEIGHBORS);// set this if using NeighborhoodsPlugins
			media_cycle->setVisualisationPlugin("VisAudiogarden");
		}
		media_cycle->addPlugin("../../../plugins/vamp/" + build_type + "/mc_vamp.dylib");	
	#endif
	
	audio_engine = new ACAudioFeedback();
	audio_engine->setMediaCycle(media_cycle);
	audio_engine->printDeviceList();

	osc_feedback = NULL;
	osc_browser = NULL;
	
	ui.compositeOsgView->move(0,-20);
	ui.compositeOsgView->setMediaCycle(media_cycle);
	ui.compositeOsgView->prepareFromBrowser();
	ui.compositeOsgView->prepareFromTimeline();
	//ui.compositeOsgView->setPlaying(true);

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
	//ui.compositeOsgView->setFocus();
}

ACAudioGardenOsgQt::~ACAudioGardenOsgQt()
{
	//compositeOsgView->setPlaying(false);
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

void ACAudioGardenOsgQt::updateLibrary()
{
	if (!updatedLibrary) {
		// set to 0 the first time a library is loaded
		// XSCF do we always want this to be 0 ?
		media_cycle->setReferenceNode(0);
	}
	// XSCF 250310 added these 3
	media_cycle->pushNavigationState();
	media_cycle->getBrowser()->setState(AC_CHANGING);
	media_cycle->getBrowser()->updateNextPositions(); // TODO is it required ?? .. hehehe

	ui.compositeOsgView->prepareFromBrowser();
	ui.compositeOsgView->prepareFromTimeline();
	//ui.compositeOsgView->setPlaying(true);
	media_cycle->setNeedsDisplay(true);
	updatedLibrary = true;
	ui.compositeOsgView->setFocus();
}

void ACAudioGardenOsgQt::on_pushButtonLaunch_clicked()
{
	this->loadACLFile();
	//ui.compositeOsgView->setFocus();
}

void ACAudioGardenOsgQt::on_pushButtonMuteAll_clicked()
{
	media_cycle->muteAllSources();
	//ui.compositeOsgView->setFocus();
}

void ACAudioGardenOsgQt::on_pushButtonClean_clicked()
{
	media_cycle->cleanUserLog();
	media_cycle->cleanLibrary();
	media_cycle->libraryContentChanged();
	this->updateLibrary();
}	

void ACAudioGardenOsgQt::on_pushButtonRecenter_clicked()
{
	media_cycle->setCameraRecenter();
	//ui.compositeOsgView->setFocus();
}

void ACAudioGardenOsgQt::on_pushButtonBack_clicked()
{
	media_cycle->goBack();
	//ui.compositeOsgView->setFocus();
}

void ACAudioGardenOsgQt::on_pushButtonForward_clicked()
{
	media_cycle->goForward();
	//ui.compositeOsgView->setFocus();
}

void ACAudioGardenOsgQt::on_pushButtonControlStart_clicked()
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

void ACAudioGardenOsgQt::on_pushButtonFeedbackStart_clicked()
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

void ACAudioGardenOsgQt::on_checkBoxRhythm_stateChanged(int state)
{
	if (updatedLibrary)
	{
		media_cycle->setWeight(0,state/2.0f);
// XS CF TODO check if this works
		media_cycle->updateDisplay(true); 
		//		media_cycle->updateClusters(true); 
		//media_cycle->setNeedsDisplay(true);

		ui.compositeOsgView->updateTransformsFromBrowser(0.0); 
	}
	//ui.compositeOsgView->setFocus();
}

void ACAudioGardenOsgQt::on_checkBoxTimbre_stateChanged(int state)
{
	if (updatedLibrary)
	{
		media_cycle->setWeight(1,state/2.0f);
		// XS CF TODO check if this works
		media_cycle->updateDisplay(true); 
//		media_cycle->updateClusters(true); 
//		media_cycle->setNeedsDisplay(true);
		ui.compositeOsgView->updateTransformsFromBrowser(1.0); 
	}
	//ui.compositeOsgView->setFocus();
}

void ACAudioGardenOsgQt::on_checkBoxHarmony_stateChanged(int state)
{
	if (updatedLibrary)
	{
		media_cycle->setWeight(2,state/2.0f);
		// XS CF TODO check if this works
		media_cycle->updateDisplay(true); 
		//		media_cycle->updateClusters(true); 
		//		media_cycle->setNeedsDisplay(true);
		ui.compositeOsgView->updateTransformsFromBrowser(1.0); 
	}
	//ui.compositeOsgView->setFocus();
}

void ACAudioGardenOsgQt::on_sliderClusters_sliderReleased()
{
	std::cout << "ClusterNumber: " << ui.sliderClusters->value() << std::endl;
	if (updatedLibrary){
		media_cycle->setClusterNumber(ui.sliderClusters->value());
		// XS CF TODO check if this works
		media_cycle->updateDisplay(true); 
		//		media_cycle->updateClusters(true); 
		//		media_cycle->setNeedsDisplay(true);
		ui.compositeOsgView->updateTransformsFromBrowser(1.0);
	}
	//ui.compositeOsgView->setFocus();
}

void ACAudioGardenOsgQt::loadACLFile(){
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
	//std::cout << "Will open: '" << fileName.toStdString() << "'" << std::endl;
	//fileName = QFileDialog::getOpenFileName(this, "~", );

	if (!(fileName.isEmpty())) {
		media_cycle->importACLLibrary((char*) fileName.toStdString().c_str());
		media_cycle->normalizeFeatures();
		media_cycle->libraryContentChanged();
		std::cout << "File library imported" << std::endl;
		this->updateLibrary();
	}
	//ui.compositeOsgView->setFocus();
}

void ACAudioGardenOsgQt::saveACLFile(){
	cout << "Saving ACL File..." << endl;
	
	QString fileName = QFileDialog::getSaveFileName(this);
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
	//ui.compositeOsgView->setFocus();
}

void ACAudioGardenOsgQt::loadMediaDirectory(){
	
	QString selectDir = QFileDialog::getExistingDirectory
	(
	 this, 
	 tr("Open Directory"),
	 "",
	 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	 );
	
	// XS TODO : check if directory exists
	// XS : do not separate directory and files in Qt and let MediaCycle handle it
	
	media_cycle->importDirectory(selectDir.toStdString(), 1);
	// with this function call here, do not import twice!!!
	// XS TODO: what if we add a new directory to the existing library ?
	media_cycle->normalizeFeatures();
	media_cycle->libraryContentChanged(); 
	this->updateLibrary();
	
	
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

void ACAudioGardenOsgQt::loadMediaFiles(){
}

void ACAudioGardenOsgQt::on_sliderBPM_valueChanged() // [0;220]
{
	std::cout << "BPM: " << ui.sliderBPM->value() << std::endl;
	//if (updatedLibrary){
		int clicked_node = media_cycle->getClickedNode();
		if (clicked_node > -1)
		{
			audio_engine->setLoopSynchroMode(clicked_node, ACAudioEngineSynchroModeAutoBeat);
			audio_engine->setLoopScaleMode(clicked_node, ACAudioEngineScaleModeResample);
			audio_engine->setBPM(ui.sliderBPM->value());
		}
	//}
	//ui.compositeOsgView->setFocus();
}

void ACAudioGardenOsgQt::on_sliderPitch_valueChanged() // [50;200]
{
	std::cout << "Pitch: " << (float) ui.sliderPitch->value()/100.0f << std::endl;
	//if (updatedLibrary){
		int clicked_node = media_cycle->getClickedNode();
		if (clicked_node > -1)
		{
			audio_engine->setLoopSynchroMode(clicked_node, ACAudioEngineSynchroModeAutoBeat);
			audio_engine->setLoopScaleMode(clicked_node, ACAudioEngineScaleModeResample);
			audio_engine->setSourcePitch(clicked_node, (float) ui.sliderPitch->value()/100.0f); 
		}
	//}
	//ui.compositeOsgView->setFocus();
}

void ACAudioGardenOsgQt::processOscMessage(const char* tagName)
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
		media_cycle->setNeedsDisplay(1);
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
		//media_cycle->setNeedsDisplay(1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/zoom") == 0)
	{
		float zoom;
		osc_browser->readFloat(mOscReceiver, &zoom);
		//zoom = zoom*600/50; // refzoom +
		media_cycle->setCameraZoom((float)zoom);
		media_cycle->setNeedsDisplay(1);
	}
	else if(strcasecmp(tagName, "/audiocycle/1/browser/1/move/angle") == 0)
	{
		float angle;//, refangle = media_cycle->getCameraRotation();
		osc_browser->readFloat(mOscReceiver, &angle);
		media_cycle->setCameraRotation((float)angle);
		media_cycle->setNeedsDisplay(1);
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
		
		ui.compositeOsgView->prepareFromBrowser();
		media_cycle->setNeedsDisplay(true);
		updatedLibrary = true;
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
		
		ui.compositeOsgView->prepareFromBrowser();
		media_cycle->setNeedsDisplay(true);
		updatedLibrary = true;
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
		int clicked_node = media_cycle->getClickedNode();
		if (clicked_node > -1)
		{
			audio_engine->setLoopSynchroMode(clicked_node, ACAudioEngineSynchroModeAutoBeat);
			audio_engine->setLoopScaleMode(clicked_node, ACAudioEngineScaleModeResample);
			audio_engine->setBPM((float)bpm);
		}
	}	
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/scrub") == 0)
	{
		float scrub;
		osc_browser->readFloat(mOscReceiver, &scrub);
		
		int clicked_node = media_cycle->getClickedNode();
		if (clicked_node > -1)
		{
			//media_cycle->pickedObjectCallback(-1);
			audio_engine->setLoopSynchroMode(clicked_node, ACAudioEngineSynchroModeManual);
			audio_engine->setLoopScaleMode(clicked_node, ACAudioEngineScaleModeVocode);
			audio_engine->setScrub((float)scrub*10000); // temporary hack to scrub between 0 an 1
		}
	}
	else if(strcasecmp(tagName, "/audiocycle/1/player/1/pitch") == 0)
	{
		float pitch;
		osc_browser->readFloat(mOscReceiver, &pitch);
		
		int clicked_node = media_cycle->getClickedNode();
		if (clicked_node > -1)
		{
			/*
			 if (!is_pitching)
			 {	
			 is_pitching = true;
			 is_scrubing = false;
			 */ 
			//media_cycle->pickedObjectCallback(-1);
			audio_engine->setLoopSynchroMode(clicked_node, ACAudioEngineSynchroModeAutoBeat);
			audio_engine->setLoopScaleMode(clicked_node, ACAudioEngineScaleModeResample);
			//}
			audio_engine->setSourcePitch(clicked_node, (float) pitch); 
		}
		
	}
	//std::cout << "End of OSC process messages" << std::endl;
}
