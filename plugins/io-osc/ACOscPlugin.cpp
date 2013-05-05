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
    this->browser_port = 3333;
    this->feedback_port = 3334;

    this->addStringParameter("Browser IP",browser_ip,std::vector<std::string>(),"Browser IP",boost::bind(&ACOscPlugin::changeBrowserIP,this));
    this->addNumberParameter("Browser Port",browser_port,1,65555,1,"Browser Port",boost::bind(&ACOscPlugin::changeBrowserPort,this));
    this->addStringParameter("Feedback IP",feedback_ip,std::vector<std::string>(),"Feedback IP",boost::bind(&ACOscPlugin::changeFeedbackIP,this));
    this->addNumberParameter("Feedback Port",feedback_port,1,65555,1,"Feedback Port",boost::bind(&ACOscPlugin::changeFeedbackPort,this));
    this->addNumberParameter("Browser Status",0,0,1,1,"Toggle Browser",boost::bind(&ACOscPlugin::toggleBrowser,this));
    this->addNumberParameter("Feedback Status",0,0,1,1,"Toggle Feedback",boost::bind(&ACOscPlugin::toggleFeedback,this));
}

void ACOscPlugin::changeBrowserIP(){
    std::cout << "ACOscPlugin::changeBrowserIP" << std::endl;
    std::string _ip = this->getStringParameterValue("Browser IP");
    if(osc_browser->isActive()){
        this->toggleBrowser(); // off
        this->toggleBrowser(); // on
    }
}

void ACOscPlugin::changeBrowserPort(){
    std::cout << "ACOscPlugin::changeBrowserPort" << std::endl;
    int _port = this->getNumberParameterValue("Browser Port");
    if(osc_browser->isActive()){
        this->toggleBrowser(); // off
        this->toggleBrowser(); // on
    }
}

void ACOscPlugin::changeFeedbackIP(){
    std::cout << "ACOscPlugin::changeFeedbackIP" << std::endl;
    std::string _ip = this->getStringParameterValue("Feedback IP");
    if(osc_feedback->isActive()){
        this->toggleFeedback(); // off
        this->toggleFeedback(); // on
    }
}

void ACOscPlugin::changeFeedbackPort(){
    std::cout << "ACOscPlugin::changeFeedbackPort" << std::endl;
    int _port = this->getNumberParameterValue("Feedback Port");
    if(osc_feedback->isActive()){
        this->toggleFeedback(); // off
        this->toggleFeedback(); // on
    }
}

void ACOscPlugin::toggleBrowser(){
    if(!osc_browser->isActive()){
        std::string _ip = this->getStringParameterValue("Browser IP");
        int _port = this->getNumberParameterValue("Browser Port");
        osc_browser->create(_ip.c_str(),_port);
        osc_browser->setMediaCycle(media_cycle);
        osc_browser->start();
        //this->setNumberParameterValue("Browser Status",1);
        std::cout << "ACOscPlugin::toggleBrowser: on" << std::endl;
    }
    else{
        osc_browser->stop();
        osc_browser->release();
        //this->setNumberParameterValue("Browser Status",0);
        std::cout << "ACOscPlugin::toggleBrowser: off" << std::endl;
    }


}

void ACOscPlugin::toggleFeedback(){

    if(!osc_feedback->isActive()){
        std::string _ip = this->getStringParameterValue("Feedback IP");
        int _port = this->getNumberParameterValue("Feedback Port");
        osc_feedback->create(_ip.c_str(),_port);
        osc_browser->setFeedback(osc_feedback);
        osc_feedback->messageBegin("test mc send osc");
        std::cout << "sending test messages to "<< _ip << " on port " << _port << endl;
        osc_feedback->messageSend();
        //this->setNumberParameterValue("Feedback Status",1);
        std::cout << "ACOscPlugin::toggleFeedback: on" << std::endl;
    }
    else{
        osc_feedback->release();
        //this->setNumberParameterValue("Feedback Status",0);
        std::cout << "ACOscPlugin::toggleFeedback: off" << std::endl;
    }
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
    if(!osc_browser){
        std::cerr << "ACOscPlugin::mediaCycleSet: OSC browser instance not available" << std::endl;
        return;
    }

    osc_browser->setMediaCycle(media_cycle);

    if(!media_cycle->getCurrentConfig())
        return;
    if(!media_cycle->getCurrentConfig()->useOSC())
        return;
    if(media_cycle->getCurrentConfig()->connectOSC()){
        if(!osc_browser->isActive()){
            //this->toggleBrowser(); // on
            this->setNumberParameterValue("Browser Status",1);
        }
        if(!osc_feedback->isActive()){
            //this->toggleFeedback(); // on
            this->setNumberParameterValue("Feedback Status",1);
        }
    }

    osc_browser->clearMediaActions();

    ACMediaType _media_type = media_cycle->getMediaType();

    std::vector<std::string> renderer_plugins = media_cycle->getPluginManager()->getAvailablePluginsNames(PLUGIN_TYPE_MEDIARENDERER, media_cycle->getMediaType());
    for(std::vector<std::string>::iterator renderer_plugin = renderer_plugins.begin();renderer_plugin!=renderer_plugins.end();renderer_plugin++){
        ACMediaRendererPlugin* plugin = dynamic_cast<ACMediaRendererPlugin*>(media_cycle->getPluginManager()->getPlugin(*renderer_plugin));
        if(plugin){
            std::map<std::string,ACMediaType> _media_actions = plugin->availableMediaActions();
            for(std::map<std::string,ACMediaType>::iterator _media_action=_media_actions.begin();_media_action!=_media_actions.end();_media_action++){
                if(_media_action->second == _media_type){
                    osc_browser->addMediaAction(_media_action->first);
                    std::cout << "ACOscPlugin::mediaCycleSet: adding action '" << _media_action->first << "' from plugin '" << *renderer_plugin << "'" << std::endl;
                }
            }
            std::map<std::string,ACMediaActionParameters> _actions_parameters  = plugin->mediaActionsParameters();
            for(std::map<std::string,ACMediaActionParameters>::iterator _action_parameters=_actions_parameters.begin();_action_parameters!=_actions_parameters.end();_action_parameters++){
                osc_browser->addActionParameters(_action_parameters->first,_action_parameters->second);
                std::cout << "ACOscPlugin::mediaCycleSet: adding action '" << _action_parameters->first << "' parameters from plugin '" << *renderer_plugin << "'" << std::endl;
            }
        }
    }
}

void ACOscPlugin::pluginLoaded(std::string plugin_name){
    if(!media_cycle)
        return;
    if(!osc_browser){
        std::cerr << "ACOscPlugin::pluginLoaded: OSC browser instance not available" << std::endl;
        return;
    }
    ACPlugin* _plugin = media_cycle->getPlugin(plugin_name);
    if(!_plugin)
        return;

    ACMediaType _media_type = media_cycle->getMediaType();

    if(_plugin->implementsPluginType(PLUGIN_TYPE_MEDIARENDERER)){
        ACMediaRendererPlugin* plugin = dynamic_cast<ACMediaRendererPlugin*>(_plugin);
        if(plugin){
            std::map<std::string,ACMediaType> _media_actions = plugin->availableMediaActions();
            for(std::map<std::string,ACMediaType>::iterator _media_action=_media_actions.begin();_media_action!=_media_actions.end();_media_action++){
                if(_media_action->second == _media_type){
                    osc_browser->addMediaAction(_media_action->first);
                    std::cout << "ACOscPlugin::pluginLoaded: adding action '" << _media_action->first << "' from plugin '" << plugin_name << "'" << std::endl;
                }
            }
            std::map<std::string,ACMediaActionParameters> _actions_parameters  = plugin->mediaActionsParameters();
            for(std::map<std::string,ACMediaActionParameters>::iterator _action_parameters=_actions_parameters.begin();_action_parameters!=_actions_parameters.end();_action_parameters++){
                osc_browser->addActionParameters(_action_parameters->first,_action_parameters->second);
                std::cout << "ACOscPlugin::pluginLoaded: adding action '" << _action_parameters->first << "' parameters from plugin '" << plugin_name << "'" << std::endl;
            }
        }
    }
}
