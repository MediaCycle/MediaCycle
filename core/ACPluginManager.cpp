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

using namespace std;

ACPluginManager::ACPluginManager() {
}

ACPluginManager::ACPluginManager(const ACPluginManager& orig) {
}

ACPluginManager::~ACPluginManager() {
// XS TODO: this->removeAll() ???
}

/*
 * Adds a plugin to the manager's list
 * return values:
 *   -1 if it encountered a problem loading
 *    1 if things went smoothly
 */
int ACPluginManager::add(std::string aPluginPath) {
    DynamicLibrary *lib;

    if ( !(lib = DynamicLibrary::loadLibrary(aPluginPath)) ) {
        return -1;
    }

	cout << "adding Plugin Library : " << aPluginPath << endl;
	
    ACPluginLibrary *acpl = new ACPluginLibrary(lib);
	acpl->initialize(); //useless
	acpl->setLibraryPath(aPluginPath);
	
    mPluginLibrary.push_back(acpl);

    return 1;
}

/*
 * Removes a plugin (specified by its file name) from the manager's list
 * return values:
 *   -1 if it encountered a problem (e.g., could not find the plugin library)
 *    1 if it removed the plugin library
 */

int ACPluginManager::remove(std::string _lpath) {
	vector<ACPluginLibrary *> ::iterator iter;
	bool found_library_in_path = false;
	for (iter = this->mPluginLibrary.begin(); iter != this->mPluginLibrary.end(); iter++) {
		if ((*iter)->getLibraryPath() == _lpath) {
			delete *iter;
			cout << "removing Plugin Library : " << _lpath << endl;
			found_library_in_path = true;
		}
	}
	if (!found_library_in_path){
		cout << "could not find Plugin Library in path: " << _lpath << endl;
		return -1;
	}
    return 1;
}

int ACPluginManager::removeAll() {
	vector<ACPluginLibrary *> ::iterator iter;
	for (iter = this->mPluginLibrary.begin(); iter != this->mPluginLibrary.end(); iter++) {
		delete *iter;
	}
    this->mPluginLibrary.clear();
    return 0;
}

// returns the plugin whose name matches aPluginName among all the plugin libraries
ACPlugin *ACPluginManager::getPlugin(std::string aPluginName) {
    ACPlugin *result = NULL;
    for (unsigned int k=0;k<this->mPluginLibrary.size();k++) {
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
ACPluginLibrary *ACPluginManager::getPluginLibrary(string _lpath){
	vector<ACPluginLibrary *> ::iterator iter;
	for (iter = this->mPluginLibrary.begin(); iter != this->mPluginLibrary.end(); iter++) {
		if ((*iter)->getLibraryPath() == _lpath) {
			cout << "found Plugin Library : " << _lpath << endl;
			return *iter;
		}
	}
	return NULL; 
}

void ACPluginManager::dump() {
	cout << "ACPluginManager: " << this->mPluginLibrary.size() << " plugin libraries" << endl;
    for (unsigned int k=0;k<this->mPluginLibrary.size();k++) {
		cout << " --- plugin library #" << k << endl;
		this->mPluginLibrary[k]->dump();
    }
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

    for (unsigned int i=0; i < listPlugin.size(); i++)
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

vector<ACMediaFeatures*> ACPluginLibrary::calculate(int aPluginIndex,string aFileName)
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

    for (unsigned int k=0;k<this->mPlugins.size();k++) {
        if (this->mPlugins[k]->getName() == aPluginName) {
            return this->mPlugins[k];
        }
    }

    return NULL;
}

void ACPluginLibrary::dump() {
	cout << "ACPluginLibrary: " << this->mPlugins.size() << " plugins" << endl;
    for (unsigned int k=0;k<this->mPlugins.size();k++) {
        cout << "plugin #" << k << ": " << this->mPlugins[k]->getName() << endl;
    }
}
