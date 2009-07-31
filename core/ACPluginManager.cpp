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

ACPluginManager::ACPluginManager(std::string aPluginPath) {
    DynamicLibrary *libTemp = new DynamicLibrary();
    DynamicLibrary *lib;
    lib = libTemp->loadLibrary(aPluginPath);

    //TODO: gestionnaire de Registry pour parser les répertoire à la recherche de DLLs

    createFactory* create = (createFactory*) lib->getProcAddress("create");
    destroyFactory* destroy = (destroyFactory*) lib->getProcAddress("destroy");
    listFactory* list = (listFactory*) lib->getProcAddress("list");

    vector<std::string> listPlugin = list();

    //faire une boucle for sur chaque plugin et faire un getParameterDescriptors()
    //ainsi l'hote est mis au courant des parametres et peut faire un setParam()
    //exemple : AmplitudeFollower.cpp (see vamp-plugin-sdk-2.0/examples)

    for (int i=0; i < listPlugin.size(); i++)
    {
        std::cout<<listPlugin[i]<<endl;
        ACPlugin* plugin = create(listPlugin[i]);

        //operations on the plugin
        string pluginID = plugin->getIdentifier();
        plugin->initialize();
        ACMediaFeatures *af = plugin->calculate("/home/alexis/Programmation/TiCore-app/Applications/Numediart/MediaCycle/src/plugins/eyesweb/BruT_108#2-test.ew.txt");
        //af->dump();
        //plugin->calculate();

        destroy(plugin);
    }
}

ACPluginManager::ACPluginManager(const ACPluginManager& orig) {
}

ACPluginManager::~ACPluginManager() {
}

