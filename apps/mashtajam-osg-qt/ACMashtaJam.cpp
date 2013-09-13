/**
 * @brief The main class for MashtaJam that cycles collections periodically.
 * @author Christian Frisson
 * @date 28/08/13
 * @copyright (c) 2013 – UMONS - Numediart
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

#include "ACMashtaJam.h"

#define MashtaPI 3.14159265

ACMashtaJam::ACMashtaJam() : ACMultiMediaCycleOsgQt() {
    count = 0;
    clock_tick = 100; // in ms
    clock_freq = 0.01; // in Hz

    this->useSegmentationByDefault(false);

    //this->on_actionFullscreen_triggered(true); // to be set after the window is shown
    //this->autoConnectOSC(true); // to be set after loading the default config
    this->changeSetting(AC_SETTING_INSTALLATION);
    //this->on_actionFullscreen_triggered(true);
    //this->on_actionToggle_Controls_triggered(true);
    //this->on_actionToggle_Controls_triggered(false);
}

void ACMashtaJam::postLoadDefaultConfig(){

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(clockWisely()) );

    this->parseXMLlist("MashtaJamLibraries.xml");

    //media_cycle->setCameraPosition(0.0f,1.0f);

    this->startLoopXML();
}

ACMashtaJam::~ACMashtaJam(){
    if(timer)
        timer->stop();
    delete timer;
}

bool ACMashtaJam::parseXMLlist(std::string filename){
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
    TiXmlHandle rootHandle = docHandle.FirstChildElement( "MashtaJam" );
    TiXmlElement* clockNode=rootHandle.FirstChild( "Clock" ).Element();
    if(clockNode){
        if(clockNode->Attribute("Tick")){
            std::stringstream strm;
            strm << clockNode->Attribute("Tick");
            float _tick(1.0f);
            strm >> _tick;
            this->clock_tick = _tick;
            std::cout << "MashtaJam: clock tick " << clock_tick << " ms" << std::endl;
        }
        if(clockNode->Attribute("Frequency")){
            std::stringstream strm;
            strm << clockNode->Attribute("Frequency");
            float _freq(1.0f);
            strm >> _freq;
            this->clock_freq = _freq;
            std::cout << "MashtaJam: clock freq " << clock_freq << " Hz" << std::endl;
        }
    }
    TiXmlElement* oscNode=rootHandle.FirstChild( "OSC" ).Element();
    if(oscNode){
        TiXmlElement* oscBrowserNode=rootHandle.FirstChild( "OSC" ).FirstChild( "Browser" ).Element();
        if(oscBrowserNode){
            if(oscBrowserNode->Attribute("IP")){
                string _ip = oscBrowserNode->Attribute("IP");
                std::cout << "MashtaJam: browser IP " << _ip << " " << std::endl;
            }
            if(oscBrowserNode->Attribute("Port")){
                std::stringstream strm;
                strm << oscBrowserNode->Attribute("Port");
                int _port(0);
                strm >> _port;
                std::cout << "MashtaJam: browser port " << _port << " " << std::endl;
            }
        }
        TiXmlElement* oscFeedbackNode=rootHandle.FirstChild( "OSC" ).FirstChild( "Feedback" ).Element();
        if(oscFeedbackNode){
            if(oscFeedbackNode->Attribute("IP")){
                string _ip = oscFeedbackNode->Attribute("IP");
                std::cout << "MashtaJam: feedback IP " << _ip << " " << std::endl;
            }
            if(oscFeedbackNode->Attribute("Port")){
                std::stringstream strm;
                strm << oscFeedbackNode->Attribute("Port");
                int _port(0);
                strm >> _port;
                std::cout << "MashtaJam: feedback port " << _port << " " << std::endl;
            }
        }
    }
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

    //return true;

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

    count++;
}

void ACMashtaJam::startLoopXML(){
    timer->stop();
    clockWisely(); // start first one before time starts so you don't wait for 30 min for the app to run
    timer->start(clock_tick);
}

void ACMashtaJam::clockWisely(){

    if(!media_cycle)
        return;
    float rotation = media_cycle->getCameraRotation();
    rotation += clock_freq*clock_tick/1000.0f;
    media_cycle->setCameraRotation(rotation);
    //...
    timer->start(clock_tick);
}
