/*
 *  ACOsgRendererFactory.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 2/11/2012
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

#if defined (SUPPORT_MULTIMEDIA)
#include "ACOsgMediaDocumentRenderer.h"
#endif //defined (SUPPORT_MULTIMEDIA)

#include "ACMediaTypes.h"

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include <iostream>               // for std::cout
//using boost::filesystem;          // for ease of tutorial presentation;
namespace fs = boost::filesystem;

#include <string>

using namespace std;

#include "ACOsgRendererFactory.h"
#include "ACOsgRendererPlugin.h"

boost::once_flag ACOsgRendererFactory::once_flag = BOOST_ONCE_INIT;

ACOsgRendererFactory::ACOsgRendererFactory() : ACEventListener(),media_cycle(0),browser(0),timeline(0){
}

ACOsgRendererFactory::~ACOsgRendererFactory(){
}

void ACOsgRendererFactory::setMediaCycle(MediaCycle* _media_cycle)
{
    this->media_cycle = _media_cycle;
    media_cycle->addListener(this);
}

void ACOsgRendererFactory::setBrowserRenderer(ACOsgBrowserRenderer* _browser)
{
    this->browser=_browser;
}

void ACOsgRendererFactory::setTimelineRenderer(ACOsgTimelineRenderer* _timeline)
{
    this->timeline=_timeline;
}

void ACOsgRendererFactory::pluginLoaded(std::string name){
    //std::cout << "ACOsgRendererFactory::pluginLoaded: " << name << std::endl;
    if(!(media_cycle->getPluginManager()->getPlugin(name))||!(media_cycle->getPluginManager()->getPlugin(name)->implementsPluginType(PLUGIN_TYPE_MEDIARENDERER))){
        //std::cout << "ACOsgRendererFactory::pluginLoaded: " << name << " is not a media renderer plugin" << std::endl;
        return;
    }
    ACPlugin* plugin = media_cycle->getPluginManager()->getPlugin(name);

    ACOsgRendererPlugin* osg_plugin = dynamic_cast<ACOsgRendererPlugin*>( plugin );
    if(!osg_plugin){
        //std::cout << "ACOsgRendererFactory::pluginLoaded: " << name << " is not an OSG renderer plugin" << std::endl;
        return;
    }
    std::list<std::string>::iterator renderer_name = std::find(checked_renderers.begin(),checked_renderers.end(),plugin->getName());
    if (renderer_name != checked_renderers.end()){
        std::cout << "ACOsgRendererFactory::pluginLoaded: " << plugin->getName() << " already checked" << std::endl;
    }
    else{
        std::cout << "ACOsgRendererFactory::pluginLoaded: checking " << plugin->getName() << " extensions" << std::endl;
        checked_renderers.push_back( plugin->getName() );

        if(!browser)
            std::cerr << "ACOsgRendererFactory::pluginLoaded: browser not passed to factory" << std::endl;
        if(!timeline)
            std::cerr << "ACOsgRendererFactory::pluginLoaded: timeline not passed to factory" << std::endl;
        osg_plugin->setBrowserRenderer(this->browser);
        osg_plugin->setTimelineRenderer(this->timeline);

        std::vector<ACMediaType> media_types = osg_plugin->getSupportedMediaTypes();
        for(std::vector<ACMediaType>::iterator _media_type = media_types.begin(); _media_type != media_types.end(); _media_type++){
            std::string media_type_string("");
            media_type_string = ACMediaFactory::getInstance().getNormalCaseStringFromMediaType(*_media_type);
            if(media_type_string!=""){
                renderer_types[plugin->getName()] = *_media_type;
                std::cout << "ACOsgRendererFactory::pluginLoaded: plugin " << plugin->getName() << " supports media type " << media_type_string << std::endl;
            }
            else
                std::cerr << "ACOsgRendererFactory::pluginLoaded: media type "<< *_media_type << " unreferenced " << std::endl;
        }

    }
}

std::string ACOsgRendererFactory::sharedThumbnailName(ACMediaType media_type){
    std::string media_type_string("");
    media_type_string = ACMediaFactory::getInstance().getNormalCaseStringFromMediaType(media_type);
    if(media_type_string==""){
        std::cerr << "ACOsgRendererFactory::sharedThumbnailName: media type "<< media_type << " unreferenced " << std::endl;
        return "";
    }

    if(!media_cycle){
        std::cerr << "ACOsgRendererFactory::sharedThumbnailName: mediacycle instance not set" << std::endl;
        return "";
    }

    ACOsgRendererTypes::iterator iter = renderer_types.begin();
    for(;iter != renderer_types.end();iter++){
        if(iter->second == media_type)
            break;
    }
    if( iter == renderer_types.end() ) {
        std::cerr << "ACOsgRendererFactory::sharedThumbnailName: could not find any media renderer supporting media type "<< media_type_string << std::endl;
        return "";
    }

    ACPlugin* plugin = media_cycle->getPluginManager()->getPlugin(iter->first);
    if(!plugin){
        std::cerr << "ACOsgRendererFactory::sharedThumbnailName: plugin "<< iter->first << " not available" << std::endl;
        return "";
    }

    ACOsgRendererPlugin* renderer_plugin = dynamic_cast<ACOsgRendererPlugin*>(plugin);
    if(!renderer_plugin){
        std::cerr << "ACOsgRendererFactory::sharedThumbnailName: plugin "<< iter->first << " doesn't create OSG media renderers" << std::endl;
        return "";
    }
    std::string shared_thumbnail = renderer_plugin->sharedThumbnailName(media_type);
    /*if(shared_thumbnail==""){
        std::cerr << "ACOsgRendererFactory::sharedThumbnailName: plugin "<< iter->first << " doesn't provide OSG shared thumbnails of type " << media_type_string << std::endl;
    }*/
    return shared_thumbnail;
}

ACMediaThumbnail* ACOsgRendererFactory::createSharedThumbnail(ACMedia* media){
    if(!media){
        std::cerr << "ACOsgRendererFactory::createSharedThumbnail: media empty" << std::endl;
        return 0;
    }
    ACMediaType media_type = media->getType();
    std::string media_type_string("");
    media_type_string = ACMediaFactory::getInstance().getNormalCaseStringFromMediaType(media_type);
    if(media_type_string==""){
        std::cerr << "ACOsgRendererFactory::createSharedThumbnail: media type "<< media_type << " unreferenced " << std::endl;
        return 0;
    }

    if(!media_cycle){
        std::cerr << "ACOsgRendererFactory::createSharedThumbnail: mediacycle instance not set" << std::endl;
        return 0;
    }

    ACOsgRendererTypes::iterator iter = renderer_types.begin();
    for(;iter != renderer_types.end();iter++){
        if(iter->second == media_type)
            break;
    }
    if( iter == renderer_types.end() ) {
        std::cerr << "ACOsgRendererFactory::createSharedThumbnail: could not find any media renderer supporting media type "<< media_type_string << std::endl;
        return 0;
    }

    ACPlugin* plugin = media_cycle->getPluginManager()->getPlugin(iter->first);
    if(!plugin){
        std::cerr << "ACOsgRendererFactory::createSharedThumbnail: plugin "<< iter->first << " not available" << std::endl;
        return 0;
    }

    ACOsgRendererPlugin* renderer_plugin = dynamic_cast<ACOsgRendererPlugin*>(plugin);
    if(!renderer_plugin){
        std::cerr << "ACOsgRendererFactory::createSharedThumbnail: plugin "<< iter->first << " doesn't create OSG media renderers" << std::endl;
        return 0;
    }
    ACMediaThumbnail* shared_thumbnail = renderer_plugin->createSharedThumbnail(media);
    if(!shared_thumbnail){
        std::cerr << "ACOsgRendererFactory::createSharedThumbnail: plugin "<< iter->first << " couldn't create an OSG shared thumbnail of type " << media_type_string << std::endl;
    }
    return shared_thumbnail;
}

ACOsgMediaRenderer* ACOsgRendererFactory::createMediaRenderer(ACMediaType media_type){
    std::string media_type_string("");
    media_type_string = ACMediaFactory::getInstance().getNormalCaseStringFromMediaType(media_type);
    if(media_type_string==""){
        std::cerr << "ACOsgRendererFactory::createMediaRenderer: media type "<< media_type << " unreferenced " << std::endl;
        return 0;
    }

    if(!media_cycle){
        std::cerr << "ACOsgRendererFactory::createMediaRenderer: mediacycle instance not set" << std::endl;
        return 0;
    }

    #if defined (SUPPORT_MULTIMEDIA)
    if(media_type == MEDIA_TYPE_MIXED){
        ACOsgMediaRenderer* renderer = new ACOsgMediaDocumentRenderer();
        return renderer;
    }
    #endif

    ACOsgRendererTypes::iterator iter = renderer_types.begin();
    for(;iter != renderer_types.end();iter++){
        if(iter->second == media_type)
            break;
    }
    if( iter == renderer_types.end() ) {
        std::cerr << "ACOsgRendererFactory::createMediaRenderer: could not find any media renderer supporting media type "<< media_type_string << std::endl;
        return 0;
    }

    ACPlugin* plugin = media_cycle->getPluginManager()->getPlugin(iter->first);
    if(!plugin){
        std::cerr << "ACOsgRendererFactory::createMediaRenderer: plugin "<< iter->first << " not available" << std::endl;
        return 0;
    }

    ACOsgRendererPlugin* renderer_plugin = dynamic_cast<ACOsgRendererPlugin*>(plugin);
    if(!renderer_plugin){
        std::cerr << "ACOsgRendererFactory::createMediaRenderer: plugin "<< iter->first << " doesn't create OSG media renderers" << std::endl;
        return 0;

    }
    ACOsgMediaRenderer* renderer = renderer_plugin->createMediaRenderer(media_type);
    if(!renderer){
        std::cerr << "ACOsgRendererFactory::createMediaRenderer: plugin "<< iter->first << " couldn't create an OSG media renderer of type " << media_type_string << std::endl;
    }
    return renderer;
}

ACOsgTrackRenderer* ACOsgRendererFactory::createTrackRenderer(ACMediaType media_type){
    std::string media_type_string("");
    media_type_string = ACMediaFactory::getInstance().getNormalCaseStringFromMediaType(media_type);
    if(media_type_string==""){
        std::cerr << "ACOsgRendererFactory::createTrackRenderer: media type "<< media_type << " unreferenced " << std::endl;
        return 0;
    }

    ACOsgRendererTypes::iterator iter = renderer_types.begin();
    for(;iter != renderer_types.end();iter++){
        if(iter->second == media_type)
            break;
    }
    if( iter == renderer_types.end() ) {
        std::cerr << "ACOsgRendererFactory::createTrackRenderer: could not find any media reader supporting media type "<< media_type_string << std::endl;
        return 0;
    }

    if(!media_cycle){
        std::cerr << "ACOsgRendererFactory::createTrackRenderer: mediacycle instance not set" << std::endl;
        return 0;
    }

    ACPlugin* plugin = media_cycle->getPluginManager()->getPlugin(iter->first);
    if(!plugin){
        std::cerr << "ACOsgRendererFactory::createTrackRenderer: plugin "<< iter->first << " not available" << std::endl;
        return 0;
    }

    ACOsgRendererPlugin* renderer_plugin = dynamic_cast<ACOsgRendererPlugin*>(plugin);
    if(!renderer_plugin){
        std::cerr << "ACOsgRendererFactory::createTrackRenderer: plugin "<< iter->first << " doesn't create OSG track renderers" << std::endl;
        return 0;

    }
    ACOsgTrackRenderer* track = renderer_plugin->createTrackRenderer(media_type);
    if(!track){
        std::cerr << "ACOsgRendererFactory::createTrackRenderer: plugin "<< iter->first << " couldn't create an OSG track renderer of type " << media_type_string << std::endl;
    }
    return track;
}
