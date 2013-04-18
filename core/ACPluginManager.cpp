/* 
 * File:   ACPluginManager.cpp
 * Author: Julien Dubois, Alexis Moinet
 * 
 * @date 23 juillet 2009
 * @copyright (c) 2009 – UMONS - Numediart
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

#include "ACPluginManager.h"
#include "ACMediaFactory.h"

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>

using namespace std;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

ACPluginManager::ACPluginManager() {
    media_cycle=0;
    mAvailableFeaturePlugins = new ACAvailableFeaturesPlugins();
    mAvailableSegmentPlugins = new ACAvailableSegmentPlugins();
    mActiveSegmentPlugins = new ACAvailableSegmentPlugins();
    mAvailableThumbnailerPlugins = new ACAvailableThumbnailerPlugins();
    mAvailableMediaReaderPlugins = new ACAvailableMediaReaderPlugins();
    mPluginLibrary.push_back(new ACDefaultPluginsLibrary());
}

ACPluginManager::ACPluginManager(const ACPluginManager& orig) {
}

ACPluginManager::~ACPluginManager() {
    this->clean();
    this->mPluginLibrary.clear();
    delete mAvailableFeaturePlugins;
    mAvailableFeaturePlugins = NULL;
    delete mAvailableSegmentPlugins;
    mAvailableSegmentPlugins = NULL;
    delete mActiveSegmentPlugins;
    mActiveSegmentPlugins = NULL;
    delete mAvailableThumbnailerPlugins;
    mAvailableThumbnailerPlugins = NULL;
    delete mAvailableMediaReaderPlugins;
    mAvailableMediaReaderPlugins = NULL;
    this->media_cycle=0;
}

void ACPluginManager::setMediaCycle(MediaCycle* _media_cycle){
    this->media_cycle=_media_cycle;
    for (vector<ACPluginLibrary *>::iterator iter = this->mPluginLibrary.begin(); iter != this->mPluginLibrary.end(); iter++){
        (*iter)->setMediaCycle(_media_cycle);
    }
}

/*
 * Adds a plugin LIBRARY to the manager's list
 * return values:
 *   -1 if it encountered a problem loading
 *    1 if things went smoothly
 */

std::vector<std::string> ACPluginManager::addLibrary(ACPluginLibrary *acpl){
    std::vector<std::string> plugins_names;

    acpl->setMediaCycle(this->media_cycle);
    acpl->initialize();
    //acpl->setLibraryPath(aPluginLibraryPath);

    std::vector<ACPlugin*> plugins = acpl->getPlugins();
    for(std::vector<ACPlugin*>::iterator plugin=plugins.begin();plugin!=plugins.end();plugin++){
        plugins_names.push_back((*plugin)->getName());
    }

    mPluginLibrary.push_back(acpl);

    //this->updateAvailablePluginLists(acpl);
    this->addLibraryToPluginLists(acpl);
    // plugins can only be added to mActiveSegmentPlugins by the user directly

    for(std::vector<ACPlugin*>::iterator plugin=plugins.begin();plugin!=plugins.end();plugin++){
        this->pluginLoaded((*plugin)->getName());
    }
    return plugins_names;
}

std::vector<std::string> ACPluginManager::addLibrary(std::string aPluginLibraryPath) {
    std::vector<std::string> plugins_names;
    for (vector<ACPluginLibrary *>::iterator it=mPluginLibrary.begin();it!=mPluginLibrary.end();it++){
        if (aPluginLibraryPath==(*it)->getLibraryPath()) { //CF this isn't robust with "non-linear" paths (containing /../) or symlinks
            std::cout << "ACPluginManager::addLibrary: library previously loaded" << std::endl;
            /// CF There's no need to inform about plugins already loaded
//            if((*it)->list){
//                plugins_names = (*it)->list();
//            }
//            else
//                std::cerr << "ACPluginManager::addLibrary: can't access plugin list" << std::endl;
            return plugins_names;
        }
    }
    DynamicLibrary *lib;

    if (!(lib = DynamicLibrary::loadLibrary(aPluginLibraryPath))) {
        std::cerr << "ACPluginManager::addLibrary: couldn't load library from path " << aPluginLibraryPath << std::endl;
        return plugins_names;
    }

    cout << "ACPluginManager::addLibrary adding Plugin Library : " << aPluginLibraryPath << endl;

    ACPluginLibrary *acpl = new ACPluginLibrary(lib);
    acpl->setMediaCycle(this->media_cycle);
    acpl->initialize();
    acpl->setLibraryPath(aPluginLibraryPath);

    std::vector<ACPlugin*> plugins = acpl->getPlugins();
    for(std::vector<ACPlugin*>::iterator plugin=plugins.begin();plugin!=plugins.end();plugin++){
        plugins_names.push_back((*plugin)->getName());
    }

    mPluginLibrary.push_back(acpl);

    //this->updateAvailablePluginLists(acpl);
    this->addLibraryToPluginLists(acpl);
    // plugins can only be added to mActiveSegmentPlugins by the user directly

    for(std::vector<ACPlugin*>::iterator plugin=plugins.begin();plugin!=plugins.end();plugin++){
        this->pluginLoaded((*plugin)->getName());
    }

    return plugins_names;
}
/*
 * Removes a plugin LIBRARY (specified by its file name) from the manager's list
 * return values:
 *   -1 if it encountered a problem (e.g., could not find the plugin library)
 *    1 if it removed the plugin library
 */

int ACPluginManager::removeLibrary(std::string _lpath) {
    vector<ACPluginLibrary *> ::iterator iter;
    bool found_library_in_path = false;
    for (iter = this->mPluginLibrary.begin(); iter != this->mPluginLibrary.end(); iter++) {
        if(*iter){
            if ((*iter)->getLibraryPath() == _lpath) {
                //this->updateAvailablePluginLists(*iter);
                this->removeLibraryFromPluginLists(*iter);
                this->updateActivePluginLists();
                delete *iter;
                cout << "removing Plugin Library : " << _lpath << endl;
                found_library_in_path = true;
            }
        }
    }
    if (!found_library_in_path) {
        cout << "could not find Plugin Library in path: " << _lpath << endl;
        return -1;
    }
    return 1;
}

bool ACPluginManager::removePluginFromLibrary(std::string _plugin_name, std::string _library_path) {
    bool found_plugin = false;
    vector<ACPluginLibrary *> ::iterator iter;
    for (iter = this->mPluginLibrary.begin(); iter != this->mPluginLibrary.end(); iter++) {
        if(*iter){
            if ((*iter)->getLibraryPath() == _library_path) {
                std::cerr << "ACPluginManager::removePluginFromLibrary: not bound to ACMediaFactory" << std::endl;
                found_plugin = (*iter)->removePlugin(_plugin_name);
            }
        }
    }
    std::cerr << "ACPluginManager::removePluginFromLibrary: media factory issue" << std::endl;
    //this->updateAvailablePluginLists();
    this->updateActivePluginLists();
    return found_plugin;
}

std::vector<std::string> ACPluginManager::getListOfPlugins() {
    std::vector<std::string> plugins_list;
    vector<ACPluginLibrary *> ::iterator lib_iter;
    vector<ACPlugin*> ::iterator plug_iter;

    for (lib_iter = this->mPluginLibrary.begin(); lib_iter != this->mPluginLibrary.end(); lib_iter++) {
        std::vector<ACPlugin *> plugins = (*lib_iter)->getPlugins();
        //std::cout << "ACPluginManager::getListOfPlugins(): plugin library: " << (*lib_iter)->getName() << " with " << (*lib_iter)->getSize() << " plugin(s)" << std::endl;
        for (plug_iter = plugins.begin(); plug_iter != plugins.end(); plug_iter++) {
            if(*plug_iter){
                //std::cout << "ACPluginManager::getListOfPlugins(): plugin: " << (*plug_iter)->getName() << std::endl;
                plugins_list.push_back((*plug_iter)->getName());
            }
            //else
            //    std::cout << "ACPluginManager::getListOfPlugins(): ghost plugin " << std::endl;

        }
    }
    return plugins_list;
}

std::string ACPluginManager::getLibraryPathFromPlugin(std::string name) {
    std::string path("");

    vector<ACPluginLibrary *> ::iterator lib_iter;
    vector<ACPlugin*> ::iterator plug_iter;

    for (lib_iter = this->mPluginLibrary.begin(); lib_iter != this->mPluginLibrary.end(); lib_iter++) {
        std::vector<ACPlugin *> plugins = (*lib_iter)->getPlugins();
        //std::cout << "ACPluginManager::getListOfPlugins(): plugin library: " << (*lib_iter)->getName() << " with " << (*lib_iter)->getSize() << " plugin(s)" << std::endl;
        for (plug_iter = plugins.begin(); plug_iter != plugins.end(); plug_iter++) {
            if(*plug_iter){
                //std::cout << "ACPluginManager::getListOfPlugins(): plugin: " << (*plug_iter)->getName() << std::endl;
                if((*plug_iter)->getName() == name){
                    boost::filesystem::path s_path( (*lib_iter)->getLibraryPath() );
                    return s_path.parent_path().string();
                }
            }
            //else
            //    std::cout << "ACPluginManager::getListOfPlugins(): ghost plugin " << std::endl;

        }
    }
    return path;
}

int ACPluginManager::clean() {
    this->cleanPluginLists();
    vector<ACPluginLibrary *> ::iterator iter;
    for (iter = this->mPluginLibrary.begin(); iter != this->mPluginLibrary.end(); iter++) {
        delete *iter; // this will delete plugins from each library too
        *iter = 0;
    }
    this->mPluginLibrary.clear();
    //this->mPluginLibrary.push_back(new ACDefaultPluginsLibrary());
    return 0;
}

// returns the plugin whose name matches aPluginName among all the plugin libraries
ACPlugin *ACPluginManager::getPlugin(std::string aPluginName) {
    ACPlugin *result = 0;
    for (unsigned int k = 0; k<this->mPluginLibrary.size(); k++) {
        result = this->mPluginLibrary[k]->getPlugin(aPluginName);
        if (result) {
            return result;
        }
    }
    return result;
}



// returns the plugin library whose name matches _lpath
// NB: plugins are identified by names
//     plugins libraries are identified by path
ACPluginLibrary *ACPluginManager::getPluginLibrary(string _lpath) {
    vector<ACPluginLibrary *> ::iterator iter;
    for (iter = this->mPluginLibrary.begin(); iter != this->mPluginLibrary.end(); iter++) {
        if ((*iter)->getLibraryPath() == _lpath) {
            cout << "found Plugin Library : " << _lpath << endl;
            return *iter;
        }
    }
    return 0;
}

void ACPluginManager::dump() {
    cout << "ACPluginManager: " << this->mPluginLibrary.size() << " plugin libraries" << endl;
    for (unsigned int k = 0; k<this->mPluginLibrary.size(); k++) {
        cout << " --- plugin library #" << k << endl;
        this->mPluginLibrary[k]->dump();
    }
}

void ACPluginManager::cleanPluginLists(){
    this->mAvailableFeaturePlugins->clean();
    this->mAvailableSegmentPlugins->clean();
    this->mActiveSegmentPlugins->clean();
    this->mAvailableThumbnailerPlugins->clean();
    this->mAvailableMediaReaderPlugins->clean();
}

void ACPluginManager::updateAvailablePluginLists(ACPluginLibrary *acpl){
    this->mAvailableFeaturePlugins->update(mPluginLibrary);
    this->mAvailableSegmentPlugins->update(mPluginLibrary);
    this->mAvailableThumbnailerPlugins->update(mPluginLibrary);
    this->mAvailableMediaReaderPlugins->update(mPluginLibrary);
    //ACMediaFactory::getInstance().updateMediaReaders(acpl);
}

void ACPluginManager::addLibraryToPluginLists(ACPluginLibrary *acpl){
    this->mAvailableFeaturePlugins->add(acpl);
    this->mAvailableSegmentPlugins->add(acpl);
    this->mAvailableThumbnailerPlugins->add(acpl);
    this->mAvailableMediaReaderPlugins->add(acpl);
    ACMediaFactory::getInstance().addMediaReaders(acpl);
}

void ACPluginManager::removeLibraryFromPluginLists(ACPluginLibrary *acpl){
    this->mAvailableFeaturePlugins->remove(acpl);
    this->mAvailableSegmentPlugins->remove(acpl);
    this->mAvailableThumbnailerPlugins->remove(acpl);
    this->mAvailableMediaReaderPlugins->remove(acpl);
    //ACMediaFactory::getInstance().removeMediaReaders(acpl);
}

void ACPluginManager::updateActivePluginLists(){
    this->mActiveSegmentPlugins->update(mPluginLibrary);
}

/*int ACPluginManager::getAvailableFeaturesPluginsSize(ACMediaType MediaType) {
    return this->mAvailableFeaturePlugins->getSize(MediaType);
}

int ACPluginManager::getAvailableSegmentPluginsSize(ACMediaType MediaType) {
    return this->mAvailableSegmentPlugins->getSize(MediaType);
}*/

int ACPluginManager::getAvailableSegmentPluginsSize(ACMediaType MediaType) {
    return this->mAvailableSegmentPlugins->getSize(MediaType);
}

int ACPluginManager::getActiveSegmentPluginsSize(ACMediaType MediaType) {
    return this->mActiveSegmentPlugins->getSize(MediaType);
}

/*std::vector<std::string> ACPluginManager::getAvailableSegmentPluginsNames(ACMediaType MediaType) {
    return this->mAvailableSegmentPlugins->getName(MediaType);
}*/

int ACPluginManager::getAvailablePluginsSize(ACPluginType PluginType, ACMediaType MediaType){
    int number = 0;
    for(std::vector<ACPluginLibrary *>::iterator pluginLibrary = mPluginLibrary.begin();pluginLibrary != mPluginLibrary.end();pluginLibrary++){
        std::vector<ACPlugin *> plugins = (*pluginLibrary)->getPlugins();
        for(std::vector<ACPlugin *>::iterator plugin = plugins.begin();plugin != plugins.end();plugin++){
            if( (*plugin)->implementsPluginType(PluginType) && (*plugin)->mediaTypeSuitable(MediaType) )
                number++;
        }
    }
    return number;
}

std::vector<std::string> ACPluginManager::getAvailablePluginsNames(ACPluginType PluginType, ACMediaType MediaType){
    std::vector<std::string> names;
    for(std::vector<ACPluginLibrary *>::iterator pluginLibrary = mPluginLibrary.begin();pluginLibrary != mPluginLibrary.end();pluginLibrary++){
        std::vector<ACPlugin *> plugins = (*pluginLibrary)->getPlugins();
        for(std::vector<ACPlugin *>::iterator plugin = plugins.begin();plugin != plugins.end();plugin++){
            if(*plugin){
                if( (*plugin)->implementsPluginType(PluginType) && (*plugin)->mediaTypeSuitable(MediaType) )
                    names.push_back( (*plugin)->getName() );
            }
        }
    }
    return names;
}

std::vector<std::string> ACPluginManager::getAvailableFeaturesPluginsNames(ACMediaType MediaType) {
    return this->mAvailableFeaturePlugins->getName(MediaType);
}

std::vector<std::string> ACPluginManager::getActiveSegmentPluginsNames(ACMediaType MediaType) {
    return this->mActiveSegmentPlugins->getName(MediaType);
}

bool ACPluginManager::setActiveSegmentPlugin(std::string _name){
    // Check if the plugin exists in the loaded plugin libraries
    if(this->getPlugin(_name) == NULL){
        std::cerr << "ACPluginManager::setActiveSegmentPlugin: plugin '" << _name << "' doesn't exist in the library" << std::endl;
        return false;
    }

    // Check if the plugin is listed in the available segmentation plugins:
    std::vector<std::string> available_names = this->mAvailableSegmentPlugins->getName();
    std::vector<std::string>::iterator available_name = std::find(available_names.begin(), available_names.end(), _name);
    if(available_name == available_names.end()){
        std::cerr << "ACPluginManager::setActiveSegmentPlugin: plugin '" << _name << "' isn't a segmentation plugin" << std::endl;
        return false;
    }

    // Check if the plugin is already listed in the active segmentation plugins:
    std::vector<std::string> active_names = this->mActiveSegmentPlugins->getName();
    std::vector<std::string>::iterator active_name = std::find(active_names.begin(), active_names.end(), _name);
    if(active_name != active_names.end()){
        std::cerr << "ACPluginManager::setActiveSegmentPlugin: plugin '" << _name << "' is already active" << std::endl;
        return false;
    }

    std::cout << "ACPluginManager::setActiveSegmentPlugin: plugin '" << _name << "'" << std::endl;

    // For now we allow only one segmentation plugin at a time
    this->mActiveSegmentPlugins->clean();

    // Add it as active segmentation plugin:
    return this->mActiveSegmentPlugins->add(this->getPlugin(_name));
}

ACPreProcessPlugin* ACPluginManager::getPreProcessPlugin(ACMediaType MediaType){
	for (unsigned int k = 0; k<this->mPluginLibrary.size(); k++) {
		std::vector<ACPlugin *> pluginsList=this->mPluginLibrary[k]->getPlugins();
		std::vector<ACPlugin *>::iterator iter;
		
		for (iter=pluginsList.begin(); iter!=pluginsList.end(); iter++){
			if ((*iter)->implementsPluginType(PLUGIN_TYPE_PREPROCESS)&&(*iter)->mediaTypeSuitable(MediaType))
			{
				ACPlugin *locPlugin= *iter;
				ACPreProcessPlugin* ret=dynamic_cast<ACPreProcessPlugin*> (locPlugin);
				return ret;
			}
		}
    }
	return NULL;

}

void ACPluginManager::pluginLoaded(std::string plugin_name)
{
    for(std::vector<ACPluginLibrary *>::iterator plugin_library = mPluginLibrary.begin(); plugin_library != mPluginLibrary.end(); ++plugin_library){
        std::vector<ACPlugin*> plugins = (*plugin_library)->getPlugins();
        for(std::vector<ACPlugin*>::iterator plugin = plugins.begin(); plugin != plugins.end(); ++plugin){
            (*plugin)->pluginLoaded(plugin_name);
        }
    }
}

//ACAvailableFeaturesPlugins implementation

ACAvailableFeaturesPlugins::ACAvailableFeaturesPlugins() : ACAvailablePlugins<ACFeaturesPlugin>::ACAvailablePlugins() {

}

ACAvailableFeaturesPlugins::ACAvailableFeaturesPlugins(vector<ACPluginLibrary *> PluginLibrary) : ACAvailablePlugins<ACFeaturesPlugin>::ACAvailablePlugins(PluginLibrary) {
    //this->update(PluginLibrary);
}

vector<ACMediaFeatures*> ACAvailableFeaturesPlugins::calculate(ACMedia* theMedia, bool _save_timed_feat) {
    ACMediaType mediaType = theMedia->getMediaType();
    vector<ACMediaFeatures*> output,empty;
    for (vector<ACFeaturesPlugin *> ::iterator iter_vec = mCurrPlugin[mediaType].begin(); iter_vec != mCurrPlugin[mediaType].end(); iter_vec++) {
        ACFeaturesPlugin* localPlugin = (*iter_vec);
        vector<ACMediaFeatures*> afv;
        if (localPlugin != NULL)
            afv = localPlugin->calculate(theMedia, _save_timed_feat);
        else {
            cerr << "<ACAvailableFeaturesPlugins::calculate> failed plugin access failed " << localPlugin->getName() << endl;
        }

        if (afv.size() == 0) {
            cerr << "<ACAvailableFeaturesPlugins::calculate> failed computing feature from plugin: " << localPlugin->getName() << endl;
            return empty;
        } else {
            for (unsigned int Iafv = 0; Iafv < afv.size(); Iafv++)
                output.push_back(afv[Iafv]);
        }
    }
    return output;
}
ACAvailableSegmentPlugins::ACAvailableSegmentPlugins(std::vector<ACPluginLibrary *> PluginLibrary) : ACAvailablePlugins<ACSegmentationPlugin>::ACAvailablePlugins(PluginLibrary) {
    //this->update(PluginLibrary);
}

ACAvailableSegmentPlugins::ACAvailableSegmentPlugins() : ACAvailablePlugins<ACSegmentationPlugin>::ACAvailablePlugins() {
}

std::vector<ACMedia*> ACAvailableSegmentPlugins::segment(ACMediaTimedFeature *ft, ACMedia* theMedia) {
    std::vector<ACMedia*> segments;
    ACMediaType mediaType = theMedia->getMediaType();
    vector<string> timedFileNames=theMedia->getTimedFileNames();
    if(mCurrPlugin[mediaType].size()==0){
        std::cerr << "<ACMedia::segment> no segmentation plugin set, can't segment" << std::endl;
        return segments;
    }
    for (vector<ACSegmentationPlugin *> ::iterator iter_vec = mCurrPlugin[mediaType].begin(); iter_vec != mCurrPlugin[mediaType].end(); iter_vec++) {
        ACSegmentationPlugin* localPlugin = (*iter_vec);
        vector<ACMedia*> afv;
        try{
            afv = localPlugin->segment(ft, theMedia);
        }
        catch(const exception& e){
            cerr << "<ACMedia::segment> failed segmenting with plugin: " << localPlugin->getName() << " due to " << e.what() << endl;
        }
        if (afv.size() == 0) {
            cerr << "<ACMedia::segment> failed importing segments from plugin: " << localPlugin->getName() << endl;
        } else {
            for (unsigned int Iafv = 0; Iafv < afv.size(); Iafv++) {
                for (unsigned int j=0;j<timedFileNames.size();j++)
                    afv[Iafv]->addTimedFileNames(timedFileNames[j]);
                segments.push_back(afv[Iafv]);
            }
        }
    }
    return segments;
}

std::vector<ACMedia*> ACAvailableSegmentPlugins::segment(ACMedia* theMedia) {
    std::vector<ACMedia*> segments;
    ACMediaType mediaType = theMedia->getMediaType();
    if(mCurrPlugin[mediaType].size()==0){
        std::cerr << "<ACMedia::segment> no segmentation plugin set, can't segment" << std::endl;
        return segments;
    }
    for (vector<ACSegmentationPlugin *> ::iterator iter_vec = mCurrPlugin[mediaType].begin(); iter_vec != mCurrPlugin[mediaType].end(); iter_vec++) {
        ACSegmentationPlugin* localPlugin = (*iter_vec);
        vector<ACMedia*> afv = localPlugin->segment(theMedia);
        if (afv.size() == 0) {
            cerr << "<ACMedia::segment> failed importing segments from plugin: " << localPlugin->getName() << endl;
        } else {
            for (unsigned int Iafv = 0; Iafv < afv.size(); Iafv++) {
                segments.push_back(afv[Iafv]);
            }
        }
    }
    return segments;
}

ACAvailableThumbnailerPlugins::ACAvailableThumbnailerPlugins() : ACAvailablePlugins<ACThumbnailerPlugin>::ACAvailablePlugins() {
}

ACAvailableThumbnailerPlugins::ACAvailableThumbnailerPlugins(vector<ACPluginLibrary *> PluginLibrary) : ACAvailablePlugins<ACThumbnailerPlugin>::ACAvailablePlugins(PluginLibrary) {
    //this->update(PluginLibrary);
}

vector<ACMediaThumbnail*> ACAvailableThumbnailerPlugins::summarize(ACMedia* theMedia, bool feature_extracted, bool segmentation_done) {
    ACMediaType mediaType = theMedia->getMediaType();
    vector<ACMediaThumbnail*> output,empty;
    for (vector<ACThumbnailerPlugin *> ::iterator iter_vec = mCurrPlugin[mediaType].begin(); iter_vec != mCurrPlugin[mediaType].end(); iter_vec++) {
        ACThumbnailerPlugin* localPlugin = (*iter_vec);
        vector<ACMediaThumbnail*> afv;
        if (localPlugin != NULL)
            if( (localPlugin->requiresFeaturesPlugins().size()>0)==feature_extracted && (localPlugin->requiresSegmentationPlugins().size()>0)==segmentation_done ){
                std::cout << "Summarizing with plugin '" << localPlugin->getName() << "'" << std::endl;
                afv = localPlugin->summarize(theMedia);

                if (afv.size() == 0) {
                    cerr << "<ACAvailableThumbnailerPlugins::summarize> failed computing thumbnail from plugin: " << localPlugin->getName() << endl;
                    //return empty;
                } else {
                    for (unsigned int Iafv = 0; Iafv < afv.size(); Iafv++)
                        output.push_back(afv[Iafv]);
                }
            }
            else{
                std::cout << "Skipping plugin '" << localPlugin->getName() << "'";
                std::cout << ", scheduled " << (localPlugin->requiresFeaturesPlugins().size()>0 ? "after" : "before") << " feature extraction";
                std::cout << " and " << (localPlugin->requiresSegmentationPlugins().size()>0 ? "after" : "before") << " segmentation";
                std::cout  << std::endl;
            }

        else {
            cerr << "<ACAvailableThumbnailerPlugins::summarize> failed plugin access failed " << localPlugin->getName() << endl;
        }
    }
    return output;
}

std::vector<std::string> ACAvailableThumbnailerPlugins::getThumbnailNames(ACMediaType mediaType){
    std::vector<std::string> descriptions;
    for (vector<ACThumbnailerPlugin *> ::iterator iter_vec = mCurrPlugin[mediaType].begin(); iter_vec != mCurrPlugin[mediaType].end(); iter_vec++) {
        std::vector<std::string> desc = (*iter_vec)->getThumbnailNames();
        for(std::vector<std::string>::iterator description = desc.begin(); description != desc.end(); description++)
            descriptions.push_back(*description);
    }
    return descriptions;
}

ACAvailableMediaReaderPlugins::ACAvailableMediaReaderPlugins() : ACAvailablePlugins<ACMediaReaderPlugin>::ACAvailablePlugins() {
}

ACAvailableMediaReaderPlugins::ACAvailableMediaReaderPlugins(vector<ACPluginLibrary *> PluginLibrary) : ACAvailablePlugins<ACMediaReaderPlugin>::ACAvailablePlugins(PluginLibrary) {
    //this->update(PluginLibrary);
}

std::map<std::string,std::string> ACAvailableMediaReaderPlugins::getExtensionsFromMediaType(ACMediaType mediaType){
    map<std::string,std::string> output,empty;
    for (vector<ACMediaReaderPlugin *> ::iterator iter_vec = mCurrPlugin[mediaType].begin(); iter_vec != mCurrPlugin[mediaType].end(); iter_vec++) {
        ACMediaReaderPlugin* localPlugin = (*iter_vec);
        std::map<std::string, ACMediaType> afv;
        if (localPlugin != NULL)
            afv = localPlugin->getSupportedExtensions();
        else {
            cerr << "<ACAvailableMediaReaderPlugins::getExtensionsFromMediaType> failed plugin access failed " << localPlugin->getName() << endl;
        }

        if (afv.size() == 0) {
            cerr << "<ACAvailableMediaReaderPlugins::getExtensionsFromMediaType> failed computing feature from plugin: " << localPlugin->getName() << endl;
            return empty;
        } else {
            for (std::map<std::string, ACMediaType>::iterator ext = afv.begin(); ext != afv.end(); ext++)
                if( ext->second == mediaType )
                    output[ext->first] = localPlugin->getName();
        }
    }
    return output;
}

//ACAvailablePlugins implementation

template <typename T>
ACAvailablePlugins<T>::ACAvailablePlugins() {

}

template <typename T>
ACAvailablePlugins<T>::ACAvailablePlugins(vector<ACPluginLibrary *> PluginLibrary) {
    this->update(PluginLibrary);
}

template <typename T>
ACAvailablePlugins<T>::~ACAvailablePlugins() {
    this->clean();
}

template <typename T>
int ACAvailablePlugins<T>::clean() {
    for (typename map<ACMediaType, std::vector<T*> >::iterator iter_map = this->mCurrPlugin.begin(); iter_map != this->mCurrPlugin.end(); iter_map++) {
        for (typename vector<T *> ::iterator iter_vec = iter_map->second.begin(); iter_vec != (*iter_map).second.end(); iter_vec++) {
            *iter_vec = NULL; // this does'nt delete plugins from each library too. We just delete the reference
        }
        iter_map->second.clear(); //to clean
    }
    this->mCurrPlugin.clear(); // to clean
}

template <typename T>
int ACAvailablePlugins<T>::remove(ACPlugin *pPlugin) {
    vector<ACMediaType> mediaTypes = mediaExtract(pPlugin->getMediaType());
    for (vector<ACMediaType>::iterator mediaType = mediaTypes.begin(); mediaType != mediaTypes.end(); mediaType++) {
        for (typename vector<T *> ::iterator iter_vec = mCurrPlugin[*mediaType].begin(); iter_vec != mCurrPlugin[*mediaType].end(); iter_vec++) {
            if ((*iter_vec) == pPlugin) {
                mCurrPlugin[*mediaType].erase(iter_vec);
                if (mCurrPlugin[*mediaType].empty())
                    mCurrPlugin.erase(*mediaType);
                //return 1;
            }
        }
    }
    return 1;
}

template <typename T>
int ACAvailablePlugins<T>::remove(ACPluginLibrary *PluginLibrary) {
    std::vector<ACPlugin *> plugins = PluginLibrary->getPlugins();
    for (std::vector<ACPlugin *>::iterator iter = plugins.begin(); iter != plugins.end(); iter++)
        this->remove(*iter);
    return 1;
}

//impossible to add a plugin that is already referenced in the map
template <typename T>
int ACAvailablePlugins<T>::add(ACPlugin *pPlugin) {
    T *lPlugin = dynamic_cast<T*> (pPlugin);
    if (lPlugin == NULL)
        return 0;
    vector<ACMediaType> mediaTypes = mediaExtract(pPlugin->getMediaType());
    for (vector<ACMediaType>::iterator mediaType = mediaTypes.begin(); mediaType != mediaTypes.end(); mediaType++) {

        if (pPlugin->mediaTypeSuitable(*mediaType)) {
            for (typename vector<T *> ::iterator iter_vec = mCurrPlugin[*mediaType].begin(); iter_vec != mCurrPlugin[*mediaType].end(); iter_vec++) {
                if ((*iter_vec) == lPlugin) {
                    //this plugin is already referenced in the container
                    return 0;
                }
            }
            mCurrPlugin[*mediaType].push_back(lPlugin);
        }
    }
    return 1;
}

template <typename T>
int ACAvailablePlugins<T>::add(ACPluginLibrary *PluginLibrary) {
    vector<ACPlugin *> plugins = PluginLibrary->getPlugins();
    for (std::vector<ACPlugin *>::iterator iter = plugins.begin(); iter != plugins.end(); iter++)
        this->add(*iter);
    return 1;

}

template <typename T>
int ACAvailablePlugins<T>::update(vector<ACPluginLibrary *> PluginLibrary) {

    this->clean();
    for (vector<ACPluginLibrary *>::iterator iter = PluginLibrary.begin(); iter != PluginLibrary.end(); iter++)
        this->add(*iter);
    //this->log();
    return 1;
}

template <typename T>
int ACAvailablePlugins<T>::getSize(ACMediaType MediaType) {
    return mCurrPlugin[MediaType].size();
}

template <typename T>
vector<string> ACAvailablePlugins<T>::getName(ACMediaType MediaType) {
    vector<string> names;
    for (typename vector<T *> ::iterator iter_vec = mCurrPlugin[MediaType].begin(); iter_vec != mCurrPlugin[MediaType].end(); iter_vec++) {
        names.push_back(((ACPlugin*) (*iter_vec))->getName());
    }
    return names;
}

template <typename T>
vector<string> ACAvailablePlugins<T>::getName() {
    vector<string> names;
    for (typename map<ACMediaType,vector<T *> > ::iterator iter_map = mCurrPlugin.begin(); iter_map != mCurrPlugin.end(); iter_map++) {
        for (typename vector<T *> ::iterator iter_vec = (*iter_map).second.begin(); iter_vec != (*iter_map).second.end(); iter_vec++) {
            names.push_back(((ACPlugin*) (*iter_vec)) ->getName());
        }
    }
    return names;
}

template <typename T>
void ACAvailablePlugins<T>::log() {
    for (typename map<ACMediaType, std::vector<T *> > ::iterator iter_map = this->mCurrPlugin.begin(); iter_map != this->mCurrPlugin.end(); iter_map++) {
        for (typename vector<T *> ::iterator iter_vec = iter_map->second.begin(); iter_vec != (*iter_map).second.end(); iter_vec++) {
            cout << "available plugin for type media " << iter_map->first << "\t" << ((ACPlugin*) (*iter_vec))->getName() << "\n";
        }
    }
}

/*template <typename T>
std::vector<T *> ACAvailablePlugins<T>::getPlugins(ACMediaType MediaType) {
    std::vector< T *> plugins;
    for (typename vector< T *> ::iterator iter_vec = mCurrPlugin[MediaType].begin(); iter_vec != mCurrPlugin[MediaType].end(); iter_vec++) {
        plugins.push_back(((T *) (*iter_vec)));
    }
    return plugins;
}*/
