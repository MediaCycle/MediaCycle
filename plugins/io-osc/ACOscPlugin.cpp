/*
 *  ACOscPlugin.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 22/11/2012
 *  @copyright (c) 2012 – UMONS - Numediart
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

#include "ACOscPlugin.h"
#include <MediaCycle.h>

using namespace std;

ACOscPlugin::ACOscPlugin() : ACClientPlugin(){
    this->mName = "OpenSoundControl (OSC)";
    this->mDescription ="Plugin for controlling MediaCycle with OpenSoundControl (OSC)";
    this->mMediaType = MEDIA_TYPE_ALL;
    this->osc_browser = 0;
    this->osc_feedback = 0;
    this->osc_browser = new ACOscBrowser();
    this->osc_feedback = new ACOscFeedback();
    this->browser_ip = "localhost";
    this->feedback_ip = "localhost";
    this->browser_port = 12345;
    this->feedback_port = 12346;

    this->addStringParameter("Browser IP","localhost",std::vector<std::string>(),"Browser IP",boost::bind(&ACOscPlugin::changeBrowserIP,this));
    this->addNumberParameter("Browser Port",12345,1,65555,1,"Browser Port",boost::bind(&ACOscPlugin::changeBrowserPort,this));
    this->addStringParameter("Feedback IP","localhost",std::vector<std::string>(),"Feedback IP",boost::bind(&ACOscPlugin::changeFeedbackIP,this));
    this->addNumberParameter("Feedback Port",12346,1,65555,1,"Feedback Port",boost::bind(&ACOscPlugin::changeFeedbackPort,this));
    this->addNumberParameter("Toggle Browser",0,0,1,1,"Toggle Browser",boost::bind(&ACOscPlugin::toggleBrowser,this));
    this->addNumberParameter("Toggle Feedback",0,0,1,1,"Toggle Feedback",boost::bind(&ACOscPlugin::toggleFeedback,this));
}

void ACOscPlugin::changeBrowserIP(){
    std::cout << "ACOscPlugin::changeBrowserIP" << std::endl;
    std::string _ip = this->getStringParameterValue("Browser IP");
}

void ACOscPlugin::changeBrowserPort(){
    std::cout << "ACOscPlugin::changeBrowserPort" << std::endl;
    int _port = this->getNumberParameterValue("Browser Port");
}

void ACOscPlugin::changeFeedbackIP(){
    std::cout << "ACOscPlugin::changeFeedbackIP" << std::endl;
    std::string _ip = this->getStringParameterValue("Feedback IP");
}

void ACOscPlugin::changeFeedbackPort(){
    std::cout << "ACOscPlugin::changeFeedbackPort" << std::endl;
    int _port = this->getNumberParameterValue("Feedback Port");
}

void ACOscPlugin::toggleBrowser(){
    std::cout << "ACOscPlugin::toggleBrowser" << std::endl;
}

void ACOscPlugin::toggleFeedback(){
    std::cout << "ACOscPlugin::toggleFeedback" << std::endl;
}

ACOscPlugin::~ACOscPlugin(){
    if (osc_browser) {
        osc_browser->release();
        delete osc_browser;
        osc_browser = 0;
    }
    if (osc_feedback) {
        osc_feedback->release();
        delete osc_feedback;
        osc_feedback = 0;
    }
}

void ACOscPlugin::mediaCycleSet(){
    osc_browser->setMediaCycle(media_cycle);

    if(!media_cycle->getCurrentConfig())
        return;
    if(!media_cycle->getCurrentConfig()->useOSC())
        return;
    if(media_cycle->getCurrentConfig()->connectOSC()){
        if(!osc_browser->isActive()){
            osc_browser->create("localhost",12345);
            osc_browser->setMediaCycle(media_cycle);
            osc_browser->start();
        }
        if(!osc_feedback->isActive()){
            osc_feedback->create("localhost",12346);
            osc_browser->setFeedback(osc_feedback);
            osc_feedback->messageBegin("test mc send osc");
            std::cout << "sending test messages to localhost on port 12346" << endl;
            osc_feedback->messageSend();
        }
    }

}
