/*
 *  ACMediaFactory.cpp
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 11/05/09
 *  Updated by Christian Frisson since then
 *  @copyright (c) 2009 – UMONS - Numediart
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
#include "ACMediaDocument.h"
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

#include "ACMediaFactory.h"

boost::once_flag ACMediaFactory::once_flag = BOOST_ONCE_INIT;

ACMediaFactory::ACMediaFactory(){
    plugin_manager = 0;
    use_rendering = false;
}

ACMediaFactory::~ACMediaFactory(){
}

ACMedia* ACMediaFactory::create(string file_ext, ACMediaType media_type){
	boost::to_lower(file_ext);
    ACMediaReaderExtensions::iterator readable_iter = readable_extensions.find(file_ext);
    if( readable_iter == readable_extensions.end() ) {
        std::cerr << "ACMediaFactory::create: couldn't find a reader plugin for extension "<< file_ext << std::endl;
		return 0;
	}
    ACMediaReaderExtensions::iterator renderable_iter = renderable_extensions.find(file_ext);
    if( use_rendering && renderable_iter == renderable_extensions.end() ) {
        std::cerr << "ACMediaFactory::create: couldn't find a renderer plugin for extension "<< file_ext << std::endl;
        return 0;
    }
    ACMediaType m = readable_iter->second.second;
    if(!plugin_manager){
        std::cerr << "ACMediaFactory::create: plugin manager not set" << std::endl;
        return 0;
    }
    ACPlugin* plugin = plugin_manager->getPlugin(readable_iter->second.first);
    if(!plugin){
        std::cerr << "ACMediaFactory::create: plugin "<< readable_iter->second.first << " is not available" << std::endl;
        return 0;
    }
    ACMediaReaderPlugin* reader_plugin = dynamic_cast<ACMediaReaderPlugin*>(plugin);
    if(!reader_plugin){
        std::cerr << "ACMediaFactory::create: plugin "<< readable_iter->second.first << " doesn't provide any media reader" << std::endl;
        return 0;

    }
    if(media_type != MEDIA_TYPE_NONE){
        return reader_plugin->mediaFactory(media_type);
    }
    else
        return reader_plugin->mediaFactory(readable_iter->second.second);
}

ACMedia* ACMediaFactory::create(ACMediaType media_type,ACMedia* copy){
    ACMediaTypeNames::const_iterator media_type_name = media_type_names.begin();
    std::string media_type_string("");
    media_type_string = this->getNormalCaseStringFromMediaType(media_type);
    if(media_type_string==""){
        std::cerr << "ACMediaFactory::create: media type "<< media_type << " unreferenced " << std::endl;
        //return 0;
    }
    ACMediaReaderExtensions::iterator readable_iter = readable_extensions.begin();
    for(;readable_iter != readable_extensions.end(); readable_iter++){
        if(readable_iter->second.second & media_type)
            break;
    }
    if( readable_iter == readable_extensions.end() ) {
        std::cerr << "ACMediaFactory::create: could not find any media reader supporting media type "<< media_type_string << std::endl;
        return 0;
    }
    ACMediaReaderExtensions::iterator renderable_iter = renderable_extensions.begin();
    for(;renderable_iter != renderable_extensions.end(); renderable_iter++){
        if(renderable_iter->second.second & media_type)
            break;
    }
    if( renderable_iter == renderable_extensions.end() ) {
        std::cerr << "ACMediaFactory::create: could not find any media reader supporting media type "<< media_type_string << std::endl;
        return 0;
    }
    if(!plugin_manager){
        std::cerr << "ACMediaFactory::create: plugin manager not set" << std::endl;
        return 0;
    }
    ACPlugin* plugin = plugin_manager->getPlugin(readable_iter->second.first);
    if(!plugin){
        std::cerr << "ACMediaFactory::create: plugin "<< readable_iter->second.first << " not available" << std::endl;
        return 0;
    }
    ACMediaReaderPlugin* reader_plugin = dynamic_cast<ACMediaReaderPlugin*>(plugin);
    if(!reader_plugin){
        std::cerr << "ACMediaFactory::create: plugin "<< readable_iter->second.first << " doesn't provide a media reader" << std::endl;
        return 0;

    }
    ACMedia* media = reader_plugin->mediaFactory(media_type,copy);
    if(!media)
        std::cerr << "ACMediaFactory::create: plugin "<< readable_iter->second.first << " couldn't create a media of type " << media_type_string << std::endl;
    return media;
}

ACMedia* ACMediaFactory::create(ACMediaType media_type){
    return this->create(media_type,0);
}

ACMedia* ACMediaFactory::create(ACMedia* media){
    if(!media){
        std::cerr << "ACMediaFactory::create: coulnd't copy contruct given media that is null" << std::endl;
        return 0;
    }
    return this->create(media->getMediaType(),media);
}

// returns the ACMediaType corresponding to a given file extension
// or MEDIA_TYPE_NONE if the extension is unknown
ACMediaType ACMediaFactory::getMediaTypeFromExtension(std::string file_ext){
	boost::to_lower(file_ext);
    ACMediaReaderExtensions::iterator iter = readable_extensions.find(file_ext);
    if( iter == readable_extensions.end() ) {
		return MEDIA_TYPE_NONE;
	}
    ACMediaType mediaType = iter->second.second;
    return mediaType;
}

ACMediaType ACMediaFactory::guessMediaTypeFromString(std::string keyword){
    boost::to_lower(keyword);
    ACMediaType _type = MEDIA_TYPE_NONE;
    for (ACMediaTypeNames::const_iterator media_type_name = media_type_names.begin(); media_type_name != media_type_names.end(); ++media_type_name ){
        std::string _string = media_type_name->first;
        boost::to_lower(_string);
        if(_string == keyword)
            return media_type_name->second;
    }
    std::cerr << "ACMediaFactory::guessMediaTypeFromString: couldn't find media type for string '"<< keyword << "'" << std::endl;
	return _type;
}

std::string ACMediaFactory::getNormalCaseStringFromMediaType(ACMediaType _media_type){
    ACMediaTypeNames::const_iterator media_type_name = media_type_names.begin();
    std::string media_type_string("");
    for (ACMediaTypeNames::const_iterator media_type_name = media_type_names.begin(); media_type_name != media_type_names.end(); ++media_type_name ){
        //std::cout << "ACMediaFactory::getNormalCaseStringFromMediaType: name " << media_type_name->first << " type " << media_type_name->second << std::endl;
        std::string _string = media_type_name->first;
        boost::to_lower(_string);
        if(media_type_name->second == _media_type)
            return media_type_name->first;
    }
    std::cerr << "ACMediaFactory::getNormalCaseStringFromMediaType: couldn't guess from media type "<< _media_type << std::endl;
    return media_type_string;
}	

std::string ACMediaFactory::getLowCaseStringFromMediaType(ACMediaType _media_type){
    string smedia = "";
	smedia = this->getNormalCaseStringFromMediaType(_media_type);
	boost::to_lower(smedia);
	return smedia;
}	

std::vector< std::string > ACMediaFactory::listAvailableMediaTypes(){
    std::vector< std::string > _list;
    for (ACMediaTypeNames::const_iterator media_type_name = media_type_names.begin(); media_type_name != media_type_names.end(); ++media_type_name ){
        if(media_type_name->first != "" && media_type_name->first != "All")
            _list.push_back(media_type_name->first);
    }
	return _list;	
}

bool ACMediaFactory::isMediaTypeSegmentable(ACMediaType _media_type){
    if((_media_type & MEDIA_TYPE_AUDIO) || (_media_type & MEDIA_TYPE_VIDEO))
		return true;
	else
		return false;
}

std::vector<std::string> ACMediaFactory::getExtensionsFromMediaType(ACMediaType media_type){
    std::vector<std::string> mediaExt;
    ACMediaReaderExtensions::iterator iter = readable_extensions.begin();
    for(;iter!=readable_extensions.end();++iter)
        if (iter->second.second == media_type )
            mediaExt.push_back(iter->first);
    return mediaExt;
}

void ACMediaFactory::addMediaReaders(ACPluginLibrary *acpl){
    std::vector<ACPlugin *> plugins = acpl->getPlugins();
    for (std::vector<ACPlugin *>::iterator plugin = plugins.begin(); plugin!=plugins.end();plugin++){
        if((*plugin)->implementsPluginType(PLUGIN_TYPE_MEDIAREADER)){
            //checked_readers
            std::list<std::string>::iterator reader_name = std::find(checked_readers.begin(),checked_readers.end(),(*plugin)->getName());
            if (reader_name != checked_readers.end()){
                //std::cout << "ACMediaFactory::addMediaReaders: " << (*plugin)->getName() << " already checked" << std::endl;
            }
            else{
                //std::cout << "ACMediaFactory::addMediaReaders: checking " << (*plugin)->getName() << " extensions" << std::endl;
                ACMediaReaderPlugin* reader_plugin = dynamic_cast<ACMediaReaderPlugin*>(*plugin);
                if (reader_plugin){
                    std::map<std::string, ACMediaType> extensions = reader_plugin->getSupportedExtensions();
                    checked_readers.push_back( (*plugin)->getName() );

                    for(std::map<std::string, ACMediaType>::iterator extension = extensions.begin(); extension != extensions.end(); extension++){
                        readable_extensions[extension->first] = ACMediaReaderType( (*plugin)->getName(),extension->second );
                        //std::cout << "ACMediaFactory::addMediaReaders: adding extension " << extension->first << " from plugin " << (*plugin)->getName() << std::endl;
                    }
                    if(extensions.size()==0){
                        std::cout << "ACMediaFactory::addMediaRenderers: media reader plugin " << (*plugin)->getName() << " supports no extensions" << std::endl;
                    }

                }
            }

        }
        if((*plugin)->implementsPluginType(PLUGIN_TYPE_MEDIARENDERER)){
            //checked_renderers
            std::list<std::string>::iterator renderer_name = std::find(checked_renderers.begin(),checked_renderers.end(),(*plugin)->getName());
            if (renderer_name != checked_renderers.end()){
            //    std::cout << "ACMediaFactory::addMediaRenderers: " << (*plugin)->getName() << " already checked" << std::endl;
            }
            else{
                //std::cout << "ACMediaFactory::addMediaRenderers: checking " << (*plugin)->getName() << " extensions" << std::endl;
                ACMediaRendererPlugin* renderer_plugin = dynamic_cast<ACMediaRendererPlugin*>(*plugin);
                if (renderer_plugin){
                    std::map<std::string, ACMediaType> extensions = renderer_plugin->getSupportedExtensions();
                    checked_renderers.push_back( (*plugin)->getName() );

                    for(std::map<std::string, ACMediaType>::iterator extension = extensions.begin(); extension != extensions.end(); extension++){
                        renderable_extensions[extension->first] = ACMediaReaderType( (*plugin)->getName(),extension->second );
                        //std::cout << "ACMediaFactory::addMediaRenderers: adding extension " << extension->first << " from plugin " << (*plugin)->getName() << std::endl;
                    }
                    if(extensions.size()==0){
                        std::cout << "ACMediaFactory::addMediaRenderers: media renderer plugin " << (*plugin)->getName() << " supports no extensions" << std::endl;
                    }
                }
            }
        }
    }
}

void ACMediaFactory::removeMediaReaders(ACPluginLibrary *acpl){
    std::vector<ACPlugin *> plugins = acpl->getPlugins();
    for (std::vector<ACPlugin *>::iterator plugin = plugins.begin(); plugin!=plugins.end();plugin++){
        if((*plugin)->implementsPluginType(PLUGIN_TYPE_MEDIAREADER)){
            //checked_readers
            std::list<std::string>::iterator reader_name = std::find(checked_readers.begin(),checked_readers.end(),(*plugin)->getName());
            if (reader_name != checked_readers.end()){
                //std::cout << "ACMediaFactory::removeMediaReaders: removing " << (*plugin)->getName() << std::endl;
                ACMediaReaderPlugin* reader_plugin = dynamic_cast<ACMediaReaderPlugin*>(*plugin);
                if (reader_plugin){
                    std::map<std::string, ACMediaType> extensions = reader_plugin->getSupportedExtensions();
                    for(std::map<std::string, ACMediaType>::iterator extension = extensions.begin(); extension != extensions.end(); extension++){
                        ACMediaReaderExtensions::iterator readable_extension = readable_extensions.find(extension->first);
                        if(readable_extension!=readable_extensions.end()){
                            readable_extensions.erase(readable_extension);
                            //std::cout << "ACMediaFactory::removeMediaReaders: removing extension " << extension->first << " from plugin " << (*plugin)->getName() << std::endl;
                        }
                    }
                }
                checked_readers.erase(reader_name);
            }
            //else{
            //    std::cout << "ACMediaFactory::removeMediaReaders: " << (*plugin)->getName() << " wasn't loaded" << std::endl;
            //}
        }
        else if((*plugin)->implementsPluginType(PLUGIN_TYPE_MEDIARENDERER)){
            //checked_renderers
            std::list<std::string>::iterator renderer_name = std::find(checked_renderers.begin(),checked_renderers.end(),(*plugin)->getName());
            if (renderer_name != checked_renderers.end()){
                //std::cout << "ACMediaFactory::removeMediaRenderers: removing " << (*plugin)->getName() << std::endl;
                ACMediaRendererPlugin* renderer_plugin = dynamic_cast<ACMediaRendererPlugin*>(*plugin);
                if (renderer_plugin){
                    std::map<std::string, ACMediaType> extensions = renderer_plugin->getSupportedExtensions();
                    for(std::map<std::string, ACMediaType>::iterator extension = extensions.begin(); extension != extensions.end(); extension++){
                        ACMediaReaderExtensions::iterator renderable_extension = renderable_extensions.find(extension->first);
                        if(renderable_extension!=renderable_extensions.end()){
                            renderable_extensions.erase(renderable_extension);
                            //std::cout << "ACMediaFactory::removeMediaRenderers: removing extension " << extension->first << " from plugin " << (*plugin)->getName() << std::endl;
                        }
                    }
                }
                checked_renderers.erase(renderer_name);
            }
            //else{
            //    std::cout << "ACMediaFactory::removeMediaRenderers: " << (*plugin)->getName() << " wasn't loaded" << std::endl;
            //}
        }
    }
}
