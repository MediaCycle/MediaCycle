/*
 *  ACAudioCycleKIS.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 24/09/13
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

#include "ACAudioCycleKIS.h"

ACAudioCycleKIS::ACAudioCycleKIS() : ACMediaCycleOsgQt(), task_timer(0), hear_timer(0), currentId(-1) {
    count = 0;
    // delay after which we change media_files (if it's ok)
    attente = 1*60*1000; // in ms

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
}

void ACAudioCycleKIS::postLoadDefaultConfig(){

    this->parseXMLlist("KISLibraries.xml");

    task_timer = new QTimer(this);
    connect( task_timer, SIGNAL(timeout()), this, SLOT(openNextLibrary()) );
    hear_timer = new QTimer(this);
    connect( hear_timer, SIGNAL(timeout()), this, SLOT(finishedHearing()) );

    this->startLoopXML();
}

void ACAudioCycleKIS::commandLine(int argc, char *argv[]){
    for(int arg= 0;arg<argc;arg++){
        std::cout << "commandLine: arg " << arg << ": " << argv[arg] << std::endl;
    }
    if(argc >= 2){
        this->readXMLConfig(argv[1]);
    }
}


ACAudioCycleKIS::~ACAudioCycleKIS(){
    if(task_timer){
        task_timer->stop();
        delete task_timer;
    }
    if(hear_timer){
        hear_timer->stop();
        delete hear_timer;
    }
}

bool ACAudioCycleKIS::parseXMLlist(std::string filename){
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
    TiXmlHandle rootHandle = docHandle.FirstChildElement( "KIS" );
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

void ACAudioCycleKIS::startLoopXML(){
    if(task_timer) task_timer->stop();
    openNextLibrary(); // start first one before time starts so you don't wait for 30 min for the app to run
    //if(task_timer) task_timer->start(attente);
}

void ACAudioCycleKIS::openNextLibrary(){
    media_cycle->performActionOnMedia("reset",-1);
    this->getTimer()->reset();
    cout << "Opening next library: " << count << endl;
    // going through all files again
    if (count >= XMLfiles.size())
        count=0;
    cout << "closing library" << endl;
    if(task_timer) task_timer->stop();
    media_cycle->muteAllSources();
    this->clean(true);

    int a = qrand();

    int item  = a % XMLfiles.size();

    cout << "opening : '" << XMLfiles[item] << "'" << endl;

    this->readXMLConfig(XMLfiles[count]);

    std::vector<boost::any> arguments;
    arguments.push_back(XMLfiles[count]);
    media_cycle->performActionOnMedia("xml loaded",-1,arguments);

    int librarySize = this->media_cycle->getLibrarySize();
    if(librarySize< 1){
        std::cerr << "Library empty "<< std::endl;
    }

    int b = qrand();

    currentId = b % librarySize;

    media_cycle->performActionOnMedia("hear",currentId);

    if(hear_timer) hear_timer->start(3*1000);
}

void ACAudioCycleKIS::finishedHearing(){
    //media_cycle->setAutoPlay(true);
    if(hear_timer) this->hear_timer->stop();
    media_cycle->performActionOnMedia("target",currentId);
    media_cycle->muteAllSources();
    if(task_timer) task_timer->start(attente);
    this->getTimer()->start();
    count++;
}

void ACAudioCycleKIS::mediaActionPerformed(std::string action, long int mediaId, std::vector<boost::any> arguments){
    if(action == "submit"){
        std::cout << "Submitted media " << mediaId << std::endl;
        if(mediaId == currentId){
            this->getTimer()->success();
            media_cycle->performActionOnMedia("success",currentId);
        }
        else
            this->getTimer()->fail();
    }
}

void ACAudioCycleKIS::postLoadXML(){
    if(dockWidgetsManager) dockWidgetsManager->updateDocksVisibility(false);
    this->changeMenuBarVisibility(false);
	this->changeStatusBarVisibility(false);
	this->changeToolBarVisibility(false);
}
