/*
 *  ACAudioCycleListeningRoom.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 12/09/13
 *  @copyright (c) 2013 – UMONS - Numediart
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

#include "ACAudioCycleListeningRoom.h"

ACAudioCycleListeningRoom::ACAudioCycleListeningRoom() : ACMultiMediaCycleOsgQt() {
    count = 0;
    // delay after which we change media_files (if it's ok)
    attente = 5*60*1000; // in ms

    this->useSegmentationByDefault(false);

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

    this->changeSetting(AC_SETTING_INSTALLATION);
    this->on_actionFullscreen_triggered(true); // to be set after the window is shown
    //this->on_actionFullscreen_triggered(true); // to be set after the window is shown
    //this->autoConnectOSC(true); // to be set after loading the default config
    //this->dockWidgetsManager->updateDocksVisibility(false);

}

void ACAudioCycleListeningRoom::postLoadDefaultConfig(){

    this->parseXMLlist("ListeningRoomLibraries.xml");

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(openNextLibrary()) );

    this->startLoopXML();
}

ACAudioCycleListeningRoom::~ACAudioCycleListeningRoom(){
    if(timer)
        timer->stop();
    delete timer;
}

bool ACAudioCycleListeningRoom::parseXMLlist(std::string filename){
    std::string filelist("");
#ifdef USE_DEBUG
    boost::filesystem::path s_path( __FILE__ );
    //std::cout << "Current source path " << s_path.parent_path().string() << std::endl;
    filelist += s_path.parent_path().string() + "/" + filename;
#else
#ifdef __APPLE__
    filelist = getExecutablePath() + "/" + filename;
#elif __WIN32__
    filelist = filename;
#else
    filelist = "~/" + filename;
#endif
#endif
    std::cout << "Filelist " << filelist << std::endl;

    TiXmlDocument doc( filelist.c_str() );
    try {
        if (!doc.LoadFile( TIXML_ENCODING_UTF8 ))
            throw runtime_error("bad parse");
        //		doc.Print( stdout );
    } catch (const exception& e) {
        cout << e.what( ) << endl;
        return false;//EXIT_FAILURE;
    }

    TiXmlHandle docHandle(&doc);
    TiXmlHandle rootHandle = docHandle.FirstChildElement( "ListeningRoom" );
    TiXmlElement* fileNode=rootHandle.FirstChild( "File" ).Element();
    std::cout << "fileNode " << fileNode << std::endl;
    try {
        for( fileNode; fileNode; fileNode=fileNode->NextSiblingElement()) {
            TiXmlText* fileName = fileNode->FirstChild()->ToText();
            std::string file("");
            file = fileName->ValueStr();
            std::cout << "File '" << file << "'" << std::endl;
            //if(!file.empty())
            XMLfiles.push_back(file);
        }
    }
    catch (const exception& e) {
        cout << e.what( ) << endl;
        return false;//EXIT_FAILURE;
    }

    std::cout << "Filelist size "  << XMLfiles.size() << std::endl;

    if(XMLfiles.empty())
        return false;

    return true;
}

void ACAudioCycleListeningRoom::startLoopXML(){
    timer->stop();
    openNextLibrary(); // start first one before time starts so you don't wait for 30 min for the app to run
    timer->start(attente);
}

void ACAudioCycleListeningRoom::openNextLibrary(){
    cout << "Opening next library: " << count << endl;
    // going through all files again
    if (count >= XMLfiles.size())
        count=0;
    cout << "closing library" << endl;
    timer->stop();
    media_cycle->muteAllSources();
    this->clean(true);
    cout << "opening : '" << XMLfiles[count] << "'" << endl;
    this->readXMLConfig(XMLfiles[count]);
    //setDefaultWaveform(AC_BROWSER_AUDIO_WAVEFORM_CLASSIC);
    timer->start(attente);
    count++;
}

void ACAudioCycleListeningRoom::openPreviousLibrary(){
    cout << "Opening previous library: " << count << endl;
    // going through all files again
    cout << "opening : " << XMLfiles[count] << endl;
    media_cycle->muteAllSources();
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
