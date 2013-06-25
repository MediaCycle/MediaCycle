/**
 * @brief Plugin for contesting in the video browser showdown
 * @author Christian Frisson
 * @date 2/01/2013
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

#include "ACUsabilityKnownItemSearchPlugin.h"
#include<MediaCycle.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>

// Headers needed for sleep()
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <curl/curl.h>

using namespace std;

ACUsabilityKnownItemSearchPlugin::ACUsabilityKnownItemSearchPlugin() : QObject(), ACPluginQt(), ACClientPlugin(){
    this->mName = "Known-Item Search Usability";
    this->mDescription ="Plugin for undertaking usability tests with known-item search tasks";
    this->mMediaType = MEDIA_TYPE_ALL;

    url = "localhost";
    port = 4040;
    team = 1;

    submitAction = new ACInputActionQt(tr("Submit the file"), this);
    submitAction->setToolTip(tr("Submit the file"));
    submitAction->setShortcut(Qt::Key_K);
    submitAction->setKeyEventType(QEvent::KeyPress);
    submitAction->setMouseEventType(QEvent::MouseButtonRelease);
    //submitAction->setDeviceName(device);
    //submitAction->setDeviceEvent("button 09 pressed");
    connect(submitAction, SIGNAL(triggered()), this, SLOT(submitCallback()));

    triggerMediaHoverAction = new ACInputActionQt(tr("Trigger Media Hover"), this);
    triggerMediaHoverAction->setShortcut(Qt::Key_L);
    triggerMediaHoverAction->setKeyEventType(QEvent::KeyPress);
    //triggerMediaHoverAction->setAutoRepeat(false); // works on OSX but not Ubuntu (10.04)
    triggerMediaHoverAction->setToolTip(tr("Trigger Media Hover (faster browsing with playback and magnification)"));
    connect(triggerMediaHoverAction, SIGNAL(triggered(bool)), this, SLOT(triggerMediaHover(bool)));

    this->parseXmlConfig("KnownItemSearch.xml");

    this->addStringParameter("Server",this->url,std::vector<std::string>(),"Server",boost::bind(&ACUsabilityKnownItemSearchPlugin::changeServer,this));
    this->addNumberParameter("Port",this->port,1,65535,1,"Port",boost::bind(&ACUsabilityKnownItemSearchPlugin::changePort,this));
    this->addNumberParameter("Team",this->team,0,16,1,"Team",boost::bind(&ACUsabilityKnownItemSearchPlugin::changeTeam,this));
    this->addCallback("Submit","Submit the clicked or clostest node",boost::bind(&ACUsabilityKnownItemSearchPlugin::submitCallback,this));
}

void ACUsabilityKnownItemSearchPlugin::changeServer(){
    this->url = this->getStringParameterValue("Server");
    std::cout << "ACUsabilityKnownItemSearchPlugin::changeServer: " << this->url << std::endl;
}

void ACUsabilityKnownItemSearchPlugin::changePort(){
    this->port = this->getNumberParameterValue("Port");
    std::cout << "ACUsabilityKnownItemSearchPlugin::changePort: " << this->port << std::endl;
}

void ACUsabilityKnownItemSearchPlugin::changeTeam(){
    this->team = this->getNumberParameterValue("Team");
    std::cout << "ACUsabilityKnownItemSearchPlugin::changeTeam: " << this->team << std::endl;
}

ACUsabilityKnownItemSearchPlugin::~ACUsabilityKnownItemSearchPlugin(){

}

void ACUsabilityKnownItemSearchPlugin::submitCallback(){
    std::cout << "ACUsabilityKnownItemSearchPlugin::submitCallback" << std::endl;
    if(!media_cycle){
        std::cerr << "ACUsabilityKnownItemSearchPlugin::performActionOnMedia: mediacycle instance not set" << std::endl;
        return;
    }

    int id = -1;
    if(media_cycle->getClickedNode() != -1)
        id = media_cycle->getClickedNode();
    else if(media_cycle->getClosestNode() != -1)
        id = media_cycle->getClosestNode();
    else
        return;

    media_cycle->performActionOnMedia("submit",id);

}

void ACUsabilityKnownItemSearchPlugin::triggerMediaHover(bool trigger){
    if (media_cycle == 0) return;
    //std::cout << "Trigger Media hover " << trigger << std::endl;
    media_cycle->setAutoPlay(trigger);
}

bool ACUsabilityKnownItemSearchPlugin::performActionOnMedia(std::string action, long int mediaId, std::vector<boost::any> arguments){
    //std::cout << "ACUsabilityKnownItemSearchPlugin::performActionOnMedia: action " << action << " mediaId " << mediaId << std::endl;
    if(!media_cycle){
        std::cerr << "ACUsabilityKnownItemSearchPlugin::performActionOnMedia: mediacycle instance not set" << std::endl;
        return false;
    }

    if(action == "submit"){

        std::stringstream command;
        std::string file("");

        if(mediaId != -1)
            file = media_cycle->getMediaFileName(mediaId);

        command << url << ":" << port << "/team=" << team << "/file=" << file;

        std::cout << "ACUsabilityKnownItemSearchPlugin::submitCallback: sending HTTP request: '" << command.str() << "'"<< std::endl;

        // Put this inside a thread, blocks the plugin if the server can't be reached
        CURL *curl;
        CURLcode res;

        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, command.str().c_str());

            /* Perform the request, res will get the return code */
            res = curl_easy_perform(curl);
            /* Check for errors */
            if(res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));

            /* always cleanup */
            curl_easy_cleanup(curl);
            return true;
        }
    }
    return false;
}

std::map<std::string,ACMediaType> ACUsabilityKnownItemSearchPlugin::availableMediaActions(){
    std::map<std::string,ACMediaType> media_actions;
    media_actions["submit"] = MEDIA_TYPE_ALL;
    return media_actions;
}

std::vector<ACInputActionQt*> ACUsabilityKnownItemSearchPlugin::providesInputActions()
{
    std::vector<ACInputActionQt*> actions;
    actions.push_back(submitAction);
    actions.push_back(triggerMediaHoverAction);
    return actions;
}


bool ACUsabilityKnownItemSearchPlugin::parseXmlConfig(std::string filename){
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
    std::cout << "ACUsabilityKnownItemSearchPlugin::parseXmlConfig: XML config file: " << filelist << std::endl;
    boost::filesystem::path b_path( filelist );

    if(boost::filesystem::exists( b_path) == false){
        std::cerr << "ACUsabilityKnownItemSearchPlugin::parseXmlConfig: couldn't load XML config "<< filelist << std::endl;
        return false;
    }

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
    TiXmlHandle rootHandle = docHandle.FirstChildElement( "KnownItemSearch" );
    TiXmlElement* serverNode=rootHandle.FirstChild( "Server" ).Element();
    if(!serverNode){
        std::cerr << "ACUsabilityKnownItemSearchPlugin::parseXmlConfig: couldn't access server parameters " << std::endl;
        return false;
    }
    try {
        string pIP ="";
        if(serverNode->Attribute("IP"))
            pIP = serverNode->Attribute("IP");
        if (pIP == "")
            throw runtime_error("corrupted XML file, wrong ip");

        int pPort=-1;
        serverNode->QueryIntAttribute("Port", &pPort); // If this fails, original value is left as-is
        if (pPort < 0)
            throw runtime_error("corrupted XML file, wrong port");

        int pTeam=-1;
        serverNode->QueryIntAttribute("Team", &pTeam); // If this fails, original value is left as-is
        if (pTeam < 0)
            throw runtime_error("corrupted XML file, wrong team");

        // Only change default settings if all parameters are properly read from the XML file
        this->url = pIP;
        this->port = pPort;
        this->team = pTeam;
    }
    catch (/*const exception& e*/...) {
        //cout << e.what( ) << endl;
        return false;//EXIT_FAILURE;
    }


    return true;
}

