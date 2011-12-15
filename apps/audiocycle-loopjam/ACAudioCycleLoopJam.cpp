/*
 *  ACAudioCycleLoopJam.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 17/05/11
 *  @copyright (c) 2011 – UMONS - Numediart
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

#include "ACAudioCycleLoopJam.h"

ACAudioCycleLoopJam::ACAudioCycleLoopJam() : ACMultiMediaCycleOsgQt() {
	count = 0;
	// delay after which we change media_files (if it's ok)
	attente = 5*60*1000; // in ms

	this->useSegmentationByDefault(false);

//	XMLfiles.push_back("/Users/ravet/Desktop/MediaCycleData/AudioCycleACL/zero-g-pro-pack_a/Brass Elements/Brass 076 BPM/libxml.xml");
	XMLfiles.push_back("/Volumes/data/mc-datasets/xml/mc-LoopJam-ZeroGProPack.xml");
	XMLfiles.push_back("/Volumes/data/mc-datasets/xml/mc-LoopJam-OLPC.xml");
	XMLfiles.push_back("/Volumes/data/mc-datasets/xml/mc-LoopJam-MusicTechMagazine98.xml");

	
	timer = new QTimer(this);
	connect( timer, SIGNAL(timeout()), this, SLOT(loopXML()) );
     QAction *action = new QAction(this);  
     action->setShortcut(QKeySequence(Qt::Key_F1));  
     this->addAction(action);  
     connect(action, SIGNAL(triggered()), this, SLOT(loopXML()));  
}

ACAudioCycleLoopJam::~ACAudioCycleLoopJam(){
	delete timer;
}

void ACAudioCycleLoopJam::startLoopXML(){
	timer->stop();
	loopXML(); // start first one before time starts so you don't wait for 30 min for the app to run
	timer->start(attente);
}

void ACAudioCycleLoopJam::setDefaultWaveform(ACBrowserAudioWaveformType _type){
	
	compositeOsgView->getBrowserRenderer()->setAudioWaveformType(_type);
	for (int d=0;d<dockWidgets.size();d++){
//		cout << dockWidgets[d]->getClassName()<<endl;
		if (dockWidgets[d]->getClassName()==string("ACAudioControlsDockWidgetQt")){
			((ACAudioControlsDockWidgetQt*)dockWidgets[d])->setComboBoxWaveformBrowser(_type);
			cout << "WaveformType:"<<_type<<endl;
		}
	}
	
}

void ACAudioCycleLoopJam::loopXML(){
	cout << "Hello from LoopJam: " << count << endl;
	// going through all files again
	if (count >= XMLfiles.size()) count=0;
	cout << "opening : " << XMLfiles[count] << endl;
	this->clean(true);
	
	this->readXMLConfig(XMLfiles[count]);
    setDefaultWaveform(AC_BROWSER_AUDIO_WAVEFORM_CLASSIC);
	
	count++;
}
