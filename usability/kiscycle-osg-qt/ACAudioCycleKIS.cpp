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

ACAudioCycleKIS::ACAudioCycleKIS() : ACMediaCycleOsgQt(), task_timer(0), hear_timer(0), currentId(-1), sequence(0) {
    count = 0;
    // delay after which we change media_files (if it's ok)
    attente = 2*60*1000; // in ms
    grace = 10*1000; // in ms

    this->useSegmentationByDefault(false);

    QAction *actionNextLibrary = new QAction(style()->standardIcon(QStyle::SP_MediaSkipForward), tr("Next Library"), this);
    //actionNextLibrary->setShortcut(QKeySequence(Qt::Key_MediaNext)); //CF requires Qt 4.8+
    actionNextLibrary->setShortcut(QKeySequence(Qt::Key_Right )); //CF instead of Qt::Key_F1
    //actionNextLibrary->setShortcut(QKeySequence::NextChild);

    this->addAction(actionNextLibrary);
    connect(actionNextLibrary, SIGNAL(triggered()), this, SLOT(openNextLibrary()));
    
    
    // Qt 4.8+ Enables application to receive multimedia key events (play, next, previous etc).
    //QCoreApplication::setAttribute(Qt::AA_CaptureMultimediaKeys);
    this->on_actionFullscreen_triggered(true);
}

void ACAudioCycleKIS::postLoadDefaultConfig(){

    if(!this->parseXMLlist("KISLibraries.xml"))
        return;
    
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
    TiXmlElement* randomizedNode=rootHandle.FirstChild( "Random" ).Element();
    if(randomizedNode){
    try {
        for( randomizedNode; randomizedNode; randomizedNode=randomizedNode->NextSiblingElement()) {
            TiXmlText* fileName = randomizedNode->FirstChild()->ToText();
            std::string file("");
            file = fileName->ValueStr();
            std::cout << "File '" << file << "'" << std::endl;
            //if(!file.empty())
            randomizedFiles.push_back(file);
            if(randomizedNode->Attribute("Target")){
                std::stringstream strm;
                strm << randomizedNode->Attribute("Target");
                int _target(-1);
                strm >> _target;
                randomizedFilesTargets.push_back(_target);
            }
            else
                randomizedFilesTargets.push_back(-1);
        }
    }
    catch (const exception& e) {
        cout << e.what( ) << endl;
        return false;//EXIT_FAILURE;
    }
    }
    TiXmlElement* sequentialNode=rootHandle.FirstChild( "Sequential" ).Element();
    if(sequentialNode){
    try {
        for( sequentialNode; sequentialNode; sequentialNode=sequentialNode->NextSiblingElement()) {
            TiXmlText* fileName = sequentialNode->FirstChild()->ToText();
            std::string file("");
            file = fileName->ValueStr();
            std::cout << "File '" << file << "'" << std::endl;
            //if(!file.empty())
            sequentialFiles.push_back(file);
            if(sequentialNode->Attribute("Target")){
                std::stringstream strm;
                strm << sequentialNode->Attribute("Target");
                int _target(-1);
                strm >> _target;
                sequentialFilesTargets.push_back(_target);
            }
            else
                sequentialFilesTargets.push_back(-1);
            sequence++;
        }
    }
    catch (const exception& e) {
        cout << e.what( ) << endl;
        return false;//EXIT_FAILURE;
    }
    }
    if(randomizedFiles.size()==0 && sequentialFiles.size()==0)
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

    cout << "closing library" << endl;
    if(task_timer) task_timer->stop();

    int item = 0;
    int target = -1;
    std::string filename("");
    if(sequence>0){
        item = sequentialFiles.size()-sequence;
        filename = sequentialFiles[item];
        target = sequentialFilesTargets[item];
        sequence--;
    }
    else if(randomizedFiles.size()>0){
        int a = qrand();
        item  = a % randomizedFiles.size();
        filename = randomizedFiles[item];
        target = randomizedFilesTargets[item];
    }
    else{
        exit(1);
    }

    media_cycle->muteAllSources();
    this->clean(true);

    this->readXMLConfig(filename);

    std::vector<boost::any> arguments;
    arguments.push_back(filename);
    media_cycle->performActionOnMedia("xml loaded",-1,arguments);

    int librarySize = this->media_cycle->getLibrarySize();
    if(librarySize< 1){
        std::cerr << "Library empty "<< std::endl;
    }

    if(target == -1){
        int b = qrand();
        currentId = b % librarySize;
    }
    else
        currentId = target;

    QCursor *cur = new QCursor;
    cur->setPos(40,450);

    media_cycle->performActionOnMedia("hear",currentId);

    if(hear_timer) hear_timer->start(3*1000);
}

void ACAudioCycleKIS::finishedHearing(){
    //media_cycle->setAutoPlay(true);
    if(hear_timer) this->hear_timer->stop();
    media_cycle->performActionOnMedia("target",currentId);
    media_cycle->muteAllSources();

    QPixmap originalPixmap = QPixmap::grabWindow(QApplication::desktop()->winId()); //sQPixmap::grabWidget(this);//

    QString format = "jpg";
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString timetag = time.toString("hh-mm-ss");
    QString datetag = date.toString("yy-MM-dd");
    QString fileName = QString(getExecutablePath().c_str()) + "/" + "MediaCycleScreenshot-" + datetag +"-"+ timetag + "." + format;
    originalPixmap.save(fileName, format.toAscii());

    if(task_timer) task_timer->start(attente);
    this->getTimer()->start();
    count++;
}

void ACAudioCycleKIS::mediaActionPerformed(std::string action, long int mediaId, std::vector<boost::any> arguments){
    if(currentId == -1)
        return;
    if(action == "submit"){
        std::cout << "Submitted media " << mediaId << std::endl;
        if(mediaId == currentId){
            this->getTimer()->success();
            media_cycle->performActionOnMedia("success",currentId);
            //std::cout << "Current " << this->getScore()->value() << " attente "<< attente<< " timer "<< this->getTimer()->value() <<std::endl;
            this->getScore()->display( this->getScore()->value() + this->getTimer()->value() );
            this->currentId = -1; // disable submissions
        }
        else{
            this->getTimer()->fail();
            this->getScore()->display( this->getScore()->value() - 10 );
        }
    }
    else if(action == "replay"){
        media_cycle->performActionOnMedia("hear",currentId);
        if(this->getTimer()->value() < 0.001f*(attente - grace)){
            this->getScore()->display( this->getScore()->value() - 10 );
        }
    }
}

void ACAudioCycleKIS::postLoadXML(){
    if(dockWidgetsManager) dockWidgetsManager->updateDocksVisibility(false);
    this->changeMenuBarVisibility(false);
    this->changeStatusBarVisibility(false);
    this->changeToolBarVisibility(false);
}
