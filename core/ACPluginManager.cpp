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

ACPluginManager::ACPluginManager() {
}

ACPluginManager::ACPluginManager(const ACPluginManager& orig) {
}

ACPluginManager::~ACPluginManager() {

}

/*
 * Adds a plugin to the manager's list
 */
int ACPluginManager::add(std::string aPluginPath) {
    DynamicLibrary *lib;

    if ( !(lib = DynamicLibrary::loadLibrary(aPluginPath)) ) {
        return -1;
    }

    ACPluginLibrary *acpl = new ACPluginLibrary(lib);
    acpl->initialize();

    this->mPluginLibrary.push_back(acpl);

    return 0;
}

int ACPluginManager::remove(std::string aPluginPath) {
    //TODO find aPluginPath
    return 0;
}

int ACPluginManager::removeAll() {
    //TODO iterator through mLibrary
    for (int i=0;i<this->mPluginLibrary.size();i++)
        delete(this->mPluginLibrary[i]);
    this->mPluginLibrary.clear();
    return 0;
}

ACPlugin *ACPluginManager::getPlugin(std::string aPluginName) {
    ACPlugin *result = NULL;
    for (int k=0;k<this->mPluginLibrary.size();k++) {
        result = this->mPluginLibrary[k]->getPlugin(aPluginName);
        if (result) {
            return result;
        }
    }

    return result;
}

/*
 * ACPluginLibrary implementation
 */
ACPluginLibrary::ACPluginLibrary(DynamicLibrary* aLib)
{
    this->mLib = aLib;
    this->create = (createPluginFactory*) aLib->getProcAddress("create");
    this->destroy = (destroyPluginFactory*) aLib->getProcAddress("destroy");
    this->list = (listPluginFactory*) aLib->getProcAddress("list");
}

ACPluginLibrary::~ACPluginLibrary()
{
    freePlugins();
    delete mLib;
}

int ACPluginLibrary::initialize()
{
    freePlugins();
    
    vector<std::string> listPlugin = list();

    //faire une boucle for sur chaque plugin et faire un getParameterDescriptors()
    //ainsi l'hote est mis au courant des parametres et peut faire un setParam()
    //exemple : AmplitudeFollower.cpp (see vamp-plugin-sdk-2.0/examples)

    for (int i=0; i < listPlugin.size(); i++)
    {
        std::cout<<listPlugin[i]<<endl;
        ACPlugin* plugin = create(listPlugin[i]);
        if (plugin) {
            this->mPlugins.push_back(plugin);
        } else {
            return -1;
        }
//         string dataFile = "/Users/dtardieu/data/DANCERS/Video/Front/Bru_102#2.mov";
//         //plugin->calculate(dataFile);
//         ACMediaFeatures *mediaFeatures = plugin->calculate(dataFile);
        /*
        //operations on the plugin
        string pluginID = plugin->getIdentifier();
        plugin->initialize();
        ACMediaFeatures *af = plugin->calculate("/home/alexis/Programmation/TiCore-app/Applications/Numediart/MediaCycle/src/plugins/eyesweb/BruT_108#2-test.ew.txt");
        //af->dump();
        //plugin->calculate();

        destroy(plugin);
        */
    }
    return 0;
}

ACMediaFeatures *ACPluginLibrary::calculate(int aPluginIndex,string aFileName)
{
    return this->mPlugins[aPluginIndex]->calculate(aFileName);
    //return getPlugin(aPluginIndex)->calculate(aFileName);
}

void ACPluginLibrary::freePlugins() {
    //TODO ? replace with iterator
    for (int i=this->mPlugins.size()-1;i>=0;i--) {
        destroy(this->mPlugins[i]);
        this->mPlugins.pop_back();
    }
}

ACPlugin *ACPluginLibrary::getPlugin(std::string aPluginName) {

    for (int k=0;k<this->mPlugins.size();k++) {
        if (this->mPlugins[k]->getName() == aPluginName) {
            return this->mPlugins[k];
        }
    }

    return NULL;
}