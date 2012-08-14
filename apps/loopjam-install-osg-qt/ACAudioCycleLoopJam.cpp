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

    XMLfiles.push_back("/Volumes/data/mc-datasets/xml/mc-LoopJam-ZeroGProPack.xml");
    //XMLfiles.push_back("/Volumes/data/mc-datasets/xml/mc-LoopJam-OLPC.xml");
    //XMLfiles.push_back("/Volumes/data/mc-datasets/xml/mc-LoopJam-MusicTechMagazine98.xml");
    XMLfiles.push_back("/Volumes/data/mc-datasets/xml/mc-LoopJam-8Bit.xml");
    XMLfiles.push_back("/Volumes/data/mc-datasets/xml/outlaw-stevetibbetts-friendlyfire.xml");
    XMLfiles.push_back("/Volumes/data/mc-datasets/xml/outlaw-sony-toyzconstructionkit.xml");
    XMLfiles.push_back("/Volumes/data/mc-datasets/xml/mc-LoopJam-BlackPaint.xml");
    //XMLfiles.push_back("/Volumes/data/mc-datasets/xml/outlaw-davidtorn-splattercell.xml");

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(openNextLibrary()) );

    QAction *actionNextLibrary = new QAction(style()->standardIcon(QStyle::SP_MediaSkipForward), tr("Next Library"), this);
    //actionNextLibrary->setShortcut(QKeySequence(Qt::Key_MediaNext)); //CF requires Qt 4.8+
    actionNextLibrary->setShortcut(QKeySequence(Qt::Key_Right )); //CF instead of Qt::Key_F1
    //actionNextLibrary->setShortcut(QKeySequence::NextChild);

    this->addAction(actionNextLibrary);
    connect(actionNextLibrary, SIGNAL(triggered()), this, SLOT(openNextLibrary()));
    this->addAction(actionNextLibrary);

    // Qt 4.8+ Enables application to receive multimedia key events (play, next, previous etc).
    //QCoreApplication::setAttribute(Qt::AA_CaptureMultimediaKeys);

    QAction *actionPreviousLibrary = new QAction(style()->standardIcon(QStyle::SP_MediaSkipBackward), tr("Previous Library"), this);
    //actionPreviousLibrary->setShortcut(QKeySequence(Qt::Key_MediaPrevious)); //CF requires Qt 4.8+
    actionPreviousLibrary->setShortcut(QKeySequence(Qt::Key_Left));
    //actionNextLibrary->setShortcut(QKeySequence::PreviousChild);
    this->addAction(actionPreviousLibrary);
    connect(actionPreviousLibrary, SIGNAL(triggered()), this, SLOT(openPreviousLibrary()));
    this->addAction(actionPreviousLibrary);
}

ACAudioCycleLoopJam::~ACAudioCycleLoopJam(){
    if(timer)
        timer->stop();
    delete timer;
}

/*void ACAudioCycleLoopJam::setDefaultWaveform(ACBrowserAudioWaveformType _type){

 compositeOsgView->getBrowserRenderer()->setAudioWaveformType(_type);
 for (int d=0;d<dockWidgets.size();d++){
//		cout << dockWidgets[d]->getClassName()<<endl;
  if (dockWidgets[d]->getClassName()==string("ACAudioControlsDockWidgetQt")){
   ((ACAudioControlsDockWidgetQt*)dockWidgets[d])->setComboBoxWaveformBrowser(_type);
   cout << "WaveformType:"<<_type<<endl;
  }
 }
}*/

void ACAudioCycleLoopJam::startLoopXML(){
        timer->stop();
        openNextLibrary(); // start first one before time starts so you don't wait for 30 min for the app to run
        timer->start(attente);
}

void ACAudioCycleLoopJam::openNextLibrary(){
    cout << "Opening next library: " << count << endl;
    // going through all files again
    if (count >= XMLfiles.size()) count=0;
    cout << "opening : " << XMLfiles[count] << endl;
    this->clean(true);
    timer->stop();
    this->readXMLConfig(XMLfiles[count]);
    //setDefaultWaveform(AC_BROWSER_AUDIO_WAVEFORM_CLASSIC);
    timer->start(attente);
    count++;
}

void ACAudioCycleLoopJam::openPreviousLibrary(){
    cout << "Opening previous library: " << count << endl;
    // going through all files again
    cout << "opening : " << XMLfiles[count] << endl;
    this->clean(true);
    timer->stop();
    this->readXMLConfig(XMLfiles[count]);
    //setDefaultWaveform(AC_BROWSER_AUDIO_WAVEFORM_CLASSIC);
    timer->start(attente);
    if (count == 0)
        count=XMLfiles.size()-1;
    else
        count--;
}
