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

#include "ACPluginLibrary.h"

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>

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

/*
 * ACPluginLibrary implementation
 */
ACPluginLibrary::ACPluginLibrary(DynamicLibrary* aLib) {
    this->library_path = "";
    this->mLib = aLib;
    this->create = (createPluginFactory*) aLib->getProcAddress("create");
    this->destroy = (destroyPluginFactory*) aLib->getProcAddress("destroy");
    this->list = (listPluginFactory*) aLib->getProcAddress("list");
    this->dependencies = (listThirdPartyMetadata*) aLib->getProcAddress("dependencies");
    this->media_cycle=0;
}

ACPluginLibrary::ACPluginLibrary() {
    this->library_path = "";
    this->mLib = 0;
    this->create = 0;
    this->destroy = 0;
    this->list = 0;
    this->dependencies = 0;
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

    if(!list || !create || !destroy){ // || !dependencies not mandatory
        std::cerr << "ACPluginLibrary::initialize: invalid plugin library" << std::endl;
        return -1;
    }

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

std::vector<ACThirdPartyMetadata> ACPluginLibrary::getThirdPartyMetadata(){
    std::vector<ACThirdPartyMetadata> thirdPartyMetadata;
    if(this->dependencies)
        thirdPartyMetadata = this->dependencies();
    return thirdPartyMetadata;
}

std::string ACPluginLibrary::getName(){
    std::string name("");
    if(this->library_path != "")
        name = fs::basename(library_path);
    return name;
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

ACDefaultPluginsLibrary::ACDefaultPluginsLibrary() : ACPluginLibrary(){
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
