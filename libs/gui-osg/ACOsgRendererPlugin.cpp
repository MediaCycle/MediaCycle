/*
 *  ACOsgRendererPlugin.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 1/11/2012
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

#include "ACOsgRendererPlugin.h"

ACOsgRendererPlugin::ACOsgRendererPlugin() : ACMediaRendererPlugin() {
    //this->mPluginType = mPluginType | PLUGIN_TYPE_MEDIARENDERER;
    browser = 0;
    timeline = 0;
}

/*ACOsgRendererPlugin::~ACOsgRendererPlugin(){
    browser = 0;
    timeline = 0;
}*/

void ACOsgRendererPlugin::setBrowserRenderer(ACOsgBrowserRenderer* _browser)
{
    this->browser=_browser;
}

void ACOsgRendererPlugin::setTimelineRenderer(ACOsgTimelineRenderer* _timeline)
{
    this->timeline=_timeline;
}

void ACOsgRendererPlugin::changeBrowserThumbnail(){
    std::string thumbnail_name = this->getStringParameterValue("Browser node");
    std::cout << "ACOsgRendererPlugin::changeBrowserThumbnail: " << thumbnail_name << std::endl;
    if(this->browser)
        this->browser->changeAllNodesThumbnail(thumbnail_name);
}

void ACOsgRendererPlugin::changeTimelinePlaybackThumbnail(){
    std::string thumbnail_name = this->getStringParameterValue("Timeline playback");
    std::cout << "ACOsgRendererPlugin::changeTimelinePlaybackThumbnail: " << thumbnail_name << std::endl;
    if(this->timeline)
        this->timeline->changeAllTracksPlaybackThumbnail(thumbnail_name);
}

void ACOsgRendererPlugin::changeTimelineSelectionThumbnail(){
    std::string thumbnail_name = this->getStringParameterValue("Timeline selection");
    std::cout << "ACOsgRendererPlugin::changeTimelineSelectionThumbnail: " << thumbnail_name << std::endl;
    if(this->timeline)
        this->timeline->changeAllTracksSelectionThumbnail(thumbnail_name);
}

void ACOsgRendererPlugin::changeTimelineSummaryThumbnail(){
    std::string thumbnail_name = this->getStringParameterValue("Timeline summary");
    std::cout << "ACOsgRendererPlugin::changeTimelineSummaryThumbnail: " << thumbnail_name << std::endl;
    if(this->timeline)
        this->timeline->changeAllTracksSummaryThumbnail(thumbnail_name);
}

/*void ACOsgRendererPlugin::mediaCycleSet(){

    if(this->hasCallbackNamed("Stop all"))
        this->updateCallback("Stop all","Stop all",boost::bind(&MediaCycle::muteAllSources,this->media_cycle));
    else
        this->addCallback("Stop all","Stop all",boost::bind(&MediaCycle::muteAllSources,this->media_cycle));

}*/

/*void ACOsgRendererPlugin::pluginLoaded(std::string plugin_name){
    std::cout << "ACOsgRendererPlugin::pluginLoaded "<< plugin_name << std::endl;
    if(!media_cycle)
        return;
    if(media_cycle->getPlugin(plugin_name)->implementsPluginType(PLUGIN_TYPE_THUMBNAILER)==false)
        return;
    thumbnail_descriptions = this->media_cycle->getPluginManager()->getAvailableThumbnailDescriptions(this->getMediaType());
    thumbnail_descriptions.push_back("None");

    if(!this->hasStringParameterNamed("Browser thumbnail"))
        this->addStringParameter("Browser thumbnail",thumbnail_descriptions.front(),thumbnail_descriptions,"Browser thumbnail",boost::bind(&ACOsgRendererPlugin::updateBrowserWaveformThumbnail,this));
    else
        this->updateStringParameter("Browser thumbnail",thumbnail_descriptions.front(),thumbnail_descriptions,"Browser thumbnail",boost::bind(&ACOsgRendererPlugin::updateBrowserWaveformThumbnail,this));
}*/

