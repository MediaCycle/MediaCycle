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

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>

#include "ACMediaFactory.h"

#include "ACKMeansPlugin.h"
#include "ACClusterPositionsPropellerPlugin.h"


using namespace std;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

vector<ACMediaType> mediaExtract(ACMediaType lMediaType) {
    unsigned int cpt = lMediaType;
    unsigned int currMedia = 1;
    vector<ACMediaType> desc;
    for (cpt = lMediaType; cpt > 0; cpt >>= 1, currMedia <<= 1) {
        if (lMediaType & currMedia) {
            desc.push_back(currMedia);
        }
    }
    return desc;
}

ACPluginManager::ACPluginManager() {
    media_cycle=0;
    mAvailableFeaturePlugins = new ACAvailableFeaturesPlugins();
    mAvailableSegmentPlugins = new ACAvailableSegmentPlugins();
    mActiveSegmentPlugins = new ACAvailableSegmentPlugins();
    mPluginLibrary.push_back(new ACDefaultPluginsLibrary());
}

ACPluginManager::ACPluginManager(const ACPluginManager& orig) {
}

ACPluginManager::~ACPluginManager() {
    this->clean();
    this->mPluginLibrary.clear();
    delete mAvailableFeaturePlugins;
    delete mAvailableSegmentPlugins;
    delete mActiveSegmentPlugins;
    mAvailableFeaturePlugins = NULL;
    mAvailableSegmentPlugins = NULL;
    mActiveSegmentPlugins = NULL;
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
int ACPluginManager::addLibrary(std::string aPluginLibraryPath) {
	
    for (vector<ACPluginLibrary *>::iterator it=mPluginLibrary.begin();it!=mPluginLibrary.end();it++){
		if (aPluginLibraryPath==(*it)->getLibraryPath()) {
			return 1;
		}
        }
    DynamicLibrary *lib;

    if (!(lib = DynamicLibrary::loadLibrary(aPluginLibraryPath))) {
        return -1;
    }

    cout << "adding Plugin Library : " << aPluginLibraryPath << endl;

    ACPluginLibrary *acpl = new ACPluginLibrary(lib);
    acpl->setMediaCycle(this->media_cycle);
    acpl->initialize();
    acpl->setLibraryPath(aPluginLibraryPath);

    mPluginLibrary.push_back(acpl);
    this->mAvailableFeaturePlugins->update(mPluginLibrary);
    this->mAvailableSegmentPlugins->update(mPluginLibrary);
    // plugins can only be added to mActiveSegmentPlugins by the user directly
    return 1;
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
        if ((*iter)->getLibraryPath() == _lpath) {
            delete *iter;
            cout << "removing Plugin Library : " << _lpath << endl;
            found_library_in_path = true;
        }
    }
    if (!found_library_in_path) {
        cout << "could not find Plugin Library in path: " << _lpath << endl;
        return -1;
    }
    this->mAvailableFeaturePlugins->update(mPluginLibrary);
    this->mAvailableSegmentPlugins->update(mPluginLibrary);
    this->mActiveSegmentPlugins->update(mPluginLibrary);
    return 1;
}

bool ACPluginManager::removePluginFromLibrary(std::string _plugin_name, std::string _library_path) {
    bool found_plugin = false;
    vector<ACPluginLibrary *> ::iterator iter;
    for (iter = this->mPluginLibrary.begin(); iter != this->mPluginLibrary.end(); iter++) {
        if ((*iter)->getLibraryPath() == _library_path) {
            found_plugin = (*iter)->removePlugin(_plugin_name);
        }
    }
    this->mAvailableFeaturePlugins->update(mPluginLibrary);
    this->mAvailableSegmentPlugins->update(mPluginLibrary);
    this->mActiveSegmentPlugins->update(mPluginLibrary);
    return found_plugin;
}

std::vector<std::string> ACPluginManager::getListOfPlugins() {
    std::vector<std::string> plugins_list;
    vector<ACPluginLibrary *> ::iterator lib_iter;
    vector<ACPlugin*> ::iterator plug_iter;

    for (lib_iter = this->mPluginLibrary.begin(); lib_iter != this->mPluginLibrary.end(); lib_iter++) {
        for (plug_iter = (*lib_iter)->getPlugins().begin(); plug_iter != (*lib_iter)->getPlugins().end(); plug_iter++) {
            plugins_list.push_back((*plug_iter)->getName());
        }
    }
    return plugins_list;
}

int ACPluginManager::clean() {
    this->mAvailableFeaturePlugins->clean();
    this->mAvailableSegmentPlugins->clean();
    this->mActiveSegmentPlugins->clean();
    vector<ACPluginLibrary *> ::iterator iter;
    for (iter = this->mPluginLibrary.begin(); iter != this->mPluginLibrary.end(); iter++) {
        delete *iter; // this will delete plugins from each library too
        *iter = 0;
    }
    this->mPluginLibrary.clear();
    this->mPluginLibrary.push_back(new ACDefaultPluginsLibrary());
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

/*int ACPluginManager::getAvailableFeaturesPluginsSize(ACMediaType MediaType) {
    return this->mAvailableFeaturePlugins->getSize(MediaType);
}

int ACPluginManager::getAvailableSegmentPluginsSize(ACMediaType MediaType) {
    return this->mAvailableSegmentPlugins->getSize(MediaType);
}*/

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


/*
 * ACPluginLibrary implementation
 */
ACPluginLibrary::ACPluginLibrary(DynamicLibrary* aLib) {
    this->library_path = "";
    this->mLib = aLib;
    this->create = (createPluginFactory*) aLib->getProcAddress("create");
    this->destroy = (destroyPluginFactory*) aLib->getProcAddress("destroy");
    this->list = (listPluginFactory*) aLib->getProcAddress("list");
    this->media_cycle=0;
}

ACPluginLibrary::ACPluginLibrary() {
    this->library_path = "";
    this->mLib = 0;
    this->create = 0;
    this->destroy = 0;
    this->list = 0;
    this->media_cycle=0;
}

ACPluginLibrary::~ACPluginLibrary() {
    freePlugins();
    delete mLib;
    this->media_cycle=0;
}

void ACPluginLibrary::setMediaCycle(MediaCycle *_media_cycle)
{
    this->media_cycle=_media_cycle;
    for (vector<ACPlugin*>::iterator iter = this->mPlugins.begin(); iter != this->mPlugins.end(); iter++) {
        (*iter)->setMediaCycle(_media_cycle);
    }
}

int ACPluginLibrary::initialize() {
    freePlugins();
    vector<std::string> listPlugin = list();

    //TODO: faire une boucle for sur chaque plugin et faire un getParameterDescriptors()
    //ainsi l'hote est mis au courant des parametres et peut faire un setParam()
    //exemple : AmplitudeFollower.cpp (see vamp-plugin-sdk-2.0/examples)

    for (unsigned int i = 0; i < listPlugin.size(); i++) {
        //std::cout<<listPlugin[i]<<endl;
        ACPlugin* plugin = create(listPlugin[i]);
        if (plugin) {
            this->mPlugins.push_back(plugin);
            plugin->setMediaCycle(this->media_cycle);
        } else {
            return -1;
        }
        //operations on the plugin
        //string pluginID = plugin->getIdentifier();
        //plugin->initialize();
        //destroy(plugin);
    }
    return 0;
}

// clean
void ACPluginLibrary::freePlugins() {
    //TODO ? replace with iterator
    for (int i = this->mPlugins.size() - 1; i >= 0; i--) {
        destroy(this->mPlugins[i]);
        this->mPlugins.pop_back();
    }
}

ACPlugin *ACPluginLibrary::getPlugin(std::string aPluginName) {
    for (unsigned int k = 0; k<this->mPlugins.size(); k++) {
        if (this->mPlugins[k]->getName() == aPluginName) {
            return this->mPlugins[k];
        }
    }
    return 0;
}

bool ACPluginLibrary::removePlugin(std::string aPluginName) {
    bool found_plugin = false;
    vector<ACPlugin*> ::iterator iter;

    for (iter = this->mPlugins.begin(); iter != this->mPlugins.end(); iter++) {
        if ((*iter)->getName() == aPluginName) {
            this->mPlugins.erase(iter);
            destroy(*iter);
            found_plugin = true;
            break;
        }
    }
    return found_plugin;
}

void ACPluginLibrary::dump() {
    cout << "ACPluginLibrary: " << this->mPlugins.size() << " plugins" << endl;
    for (unsigned int k = 0; k<this->mPlugins.size(); k++) {
        cout << "plugin #" << k << ": " << this->mPlugins[k]->getName() << endl;
    }
}

//typedef ACPlugin* createPluginFactory(std::string);
//typedef void destroyPluginFactory(ACPlugin*);
//typedef std::vector<std::string> listPluginFactory();

ACDefaultPluginsLibrary::ACDefaultPluginsLibrary(){
    this->library_path = "";
    this->mLib = 0;
    mPlugins.push_back(new ACKMeansPlugin());
    mPlugins.push_back(new ACClusterPositionsPropellerPlugin());
}

ACDefaultPluginsLibrary::~ACDefaultPluginsLibrary(){
    for (std::vector<ACPlugin *>::iterator iter = this->mPlugins.begin(); iter != this->mPlugins.end(); iter++) {
        delete(*iter);
        *iter = 0;
    }
    mPlugins.clear();
}

int ACDefaultPluginsLibrary::initialize() {
    for (std::vector<ACPlugin *>::iterator iter = this->mPlugins.begin(); iter != this->mPlugins.end(); iter++) {
        (*iter)->setMediaCycle(this->media_cycle);
    }
    return 1;
}

//ACAvailableFeaturesPlugins implementation

ACAvailableFeaturesPlugins::ACAvailableFeaturesPlugins() : ACAvailablePlugins<ACFeaturesPlugin>::ACAvailablePlugins() {

}

ACAvailableFeaturesPlugins::ACAvailableFeaturesPlugins(vector<ACPluginLibrary *> PluginLibrary) : ACAvailablePlugins<ACFeaturesPlugin>::ACAvailablePlugins(PluginLibrary) {
    //this->update(PluginLibrary);
}

//ACAvailableFeaturesPlugins::~ACAvailableFeaturesPlugins(){
//	this->clean();
//}
/*
int ACAvailableFeaturesPlugins::clean(){
	for (map<ACMediaType,std::vector<ACFeaturesPlugin *> > ::iterator iter_map = this->mCurrFeaturePlugin.begin(); iter_map != this->mCurrFeaturePlugin.end(); iter_map++) {
		for (vector<ACFeaturesPlugin *> ::iterator iter_vec = iter_map->second.begin(); iter_vec != (*iter_map).second.end(); iter_vec++) {
			*iter_vec=NULL; // this does'nt delete plugins from each library too. We just delete the reference			
		}
		iter_map->second.clear();		//to clean
	}
	this->mCurrFeaturePlugin.clear(); // to clean
	
	
}
int ACAvailableFeaturesPlugins::remove(ACPlugin *pPlugin){
	ACMediaType mediaType =pPlugin->getMediaType();
	
	for (vector<ACFeaturesPlugin *> ::iterator iter_vec = mCurrFeaturePlugin[mediaType].begin(); iter_vec != mCurrFeaturePlugin[mediaType].end(); iter_vec++) {
		if ( (*iter_vec)==pPlugin)
		{
			mCurrFeaturePlugin[mediaType].erase(iter_vec);
			if (mCurrFeaturePlugin[mediaType].empty())
				mCurrFeaturePlugin.erase(mediaType);
			return 1;
		}
	}
	return 0;
	
}

int ACAvailableFeaturesPlugins::remove(ACPluginLibrary *PluginLibrary){
	
    std::vector<ACPlugin *> plugins= PluginLibrary->getPlugins();
	for (std::vector<ACPlugin *>::iterator iter=plugins.begin();iter!= plugins.end();iter++)
		this->remove(*iter);
	return 1;
}
//impossible to add a plugin that is already referenced in the map
int ACAvailableFeaturesPlugins::add(ACPlugin *pPlugin){
	if (pPlugin->implementsPluginType(PLUGIN_TYPE_FEATURES)==false)
		return 0;
	ACMediaType mediaType =pPlugin->getMediaType();
	for (vector<ACFeaturesPlugin *> ::iterator iter_vec = mCurrFeaturePlugin[mediaType].begin(); iter_vec != mCurrFeaturePlugin[mediaType].end(); iter_vec++) {
		if ( (*iter_vec)==pPlugin)
		{
			//this plugin is already referenced in the container
			return 0;
		}		
	}
	mCurrFeaturePlugin[mediaType].push_back(dynamic_cast<ACFeaturesPlugin*> (pPlugin) );
	return 1;
}

int ACAvailableFeaturesPlugins::add(ACPluginLibrary *PluginLibrary){
    vector<ACPlugin *> plugins= PluginLibrary->getPlugins();
	for (std::vector<ACPlugin *>::iterator iter=plugins.begin();iter!= plugins.end();iter++)
		this->add(*iter);
	return 1;
	
}

int ACAvailableFeaturesPlugins::update(vector<ACPluginLibrary *> PluginLibrary){
	
	this->clean();
	for (vector<ACPluginLibrary *>::iterator iter =PluginLibrary.begin();iter!=PluginLibrary.end();iter++)
		this->add(*iter);
	this->log();
	return 1;
}
 
int ACAvailableFeaturesPlugins::getSize(ACMediaType MediaType){
	return mCurrFeaturePlugin[MediaType].size();
}

vector<string> ACAvailableFeaturesPlugins::getName(ACMediaType MediaType)
{
	vector<string> names;
	for (vector<ACFeaturesPlugin *> ::iterator iter_vec = mCurrFeaturePlugin[MediaType].begin(); iter_vec != mCurrFeaturePlugin[MediaType].end(); iter_vec++) {
		names.push_back((*iter_vec)->getName());
	}
	
}
void ACAvailableFeaturesPlugins::log()
{
	for (map<ACMediaType,std::vector<ACFeaturesPlugin *> > ::iterator iter_map = this->mCurrFeaturePlugin.begin(); iter_map != this->mCurrFeaturePlugin.end(); iter_map++) {
		for (vector<ACFeaturesPlugin *> ::iterator iter_vec = iter_map->second.begin(); iter_vec != (*iter_map).second.end(); iter_vec++) {
                        cout << "available plugin for type media" <<"\t"<<(*iter_vec)->getName() <<"\n";
		}
	}
	
} 
 */

//vector<ACMediaFeatures*> ACAvailableFeaturesPlugins::calculate(std::string aFileName, bool _save_timed_feat){
//	//first step identify the mediatype
//	string extension = fs::extension(aFileName);
//	ACMediaType fileMediaType = ACMediaFactory::getInstance().getMediaTypeFromExtension(extension);
//	vector<ACMediaFeatures*> output;
//	for (vector<ACFeaturesPlugin *> ::iterator iter_vec = mCurrPlugin[fileMediaType].begin(); iter_vec != mCurrPlugin[fileMediaType].end(); iter_vec++) {
//		ACFeaturesPlugin* localPlugin=(*iter_vec);
//		vector<ACMediaFeatures*> afv;
//		if (localPlugin!=NULL)
//			afv =localPlugin->calculate(aFileName, _save_timed_feat);
//		else {
//			cerr << "<ACAvailableFeaturesPlugins::calculate> failed plugin access failed "<< localPlugin->getName() << endl;
//		}
//		
//		if (afv.size()==0){
//			cerr << "<ACAvailableFeaturesPlugins::calculate> failed computing feature from plugin: " << localPlugin->getName() << endl;
//		}
//		else {
//			for (unsigned int Iafv=0; Iafv< afv.size() ; Iafv++)
//				output.push_back(afv[Iafv]);
//		}
//	}
//	return output;
//	
//}

vector<ACMediaFeatures*> ACAvailableFeaturesPlugins::calculate(ACMediaData* aData, ACMedia* theMedia, bool _save_timed_feat) {
    ACMediaType mediaType = theMedia->getMediaType();
    vector<ACMediaFeatures*> output,empty;
    for (vector<ACFeaturesPlugin *> ::iterator iter_vec = mCurrPlugin[mediaType].begin(); iter_vec != mCurrPlugin[mediaType].end(); iter_vec++) {
        ACFeaturesPlugin* localPlugin = (*iter_vec);
        vector<ACMediaFeatures*> afv;
        if (localPlugin != NULL)
            afv = localPlugin->calculate(aData, theMedia, _save_timed_feat);
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

// XS TODO check with TR


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

ACAvailableSegmentPlugins::ACAvailableSegmentPlugins(std::vector<ACPluginLibrary *> PluginLibrary) : ACAvailablePlugins<ACSegmentationPlugin>::ACAvailablePlugins(PluginLibrary) {
    //this->update(PluginLibrary);
}

ACAvailableSegmentPlugins::ACAvailableSegmentPlugins() : ACAvailablePlugins<ACSegmentationPlugin>::ACAvailablePlugins() {
}

std::vector<ACMedia*> ACAvailableSegmentPlugins::segment(ACMediaTimedFeature *ft, ACMedia* theMedia) {
    std::vector<ACMedia*> segments;
    ACMediaType mediaType = theMedia->getMediaType();
	vector<string> timedFileNames=theMedia->getTimedFileNames();
    for (vector<ACSegmentationPlugin *> ::iterator iter_vec = mCurrPlugin[mediaType].begin(); iter_vec != mCurrPlugin[mediaType].end(); iter_vec++) {
        ACSegmentationPlugin* localPlugin = (*iter_vec);
        vector<ACMedia*> afv = localPlugin->segment(ft, theMedia);
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

std::vector<ACMedia*> ACAvailableSegmentPlugins::segment(ACMediaData* aData, ACMedia* theMedia) {
    std::vector<ACMedia*> segments;
    ACMediaType mediaType = theMedia->getMediaType();
    for (vector<ACSegmentationPlugin *> ::iterator iter_vec = mCurrPlugin[mediaType].begin(); iter_vec != mCurrPlugin[mediaType].end(); iter_vec++) {
        ACSegmentationPlugin* localPlugin = (*iter_vec);
        vector<ACMedia*> afv = localPlugin->segment(aData, theMedia);
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
