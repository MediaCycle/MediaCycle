/* 
 * File:   ACPluginManager.h
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

#ifndef _ACPLUGINLIBRARY_H
#define	_ACPLUGINLIBRARY_H

#include "ACPlugin.h"
#include "DynamicLibrary.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

extern std::vector<ACMediaType> mediaExtract(ACMediaType lMediaType);

class MediaCycle;

class ACPluginLibrary {
public:
    ACPluginLibrary(DynamicLibrary *aLib);
    ACPluginLibrary();
    virtual ~ACPluginLibrary();
    void setMediaCycle(MediaCycle* _media_cycle);
    virtual int initialize();

    std::vector<ACPlugin *> getPlugins() {return this->mPlugins;}
    ACPlugin *getPlugin(int i) {return this->mPlugins[i];}
    ACPlugin *getPlugin(std::string aPluginName);

    bool removePlugin(std::string aPluginName);

    int getSize() {return this->mPlugins.size();}
    DynamicLibrary* getLib() { return this->mLib;}
    void freePlugins();
    std::vector<ACThirdPartyMetadata> getThirdPartyMetadata();
    void dump();

    // store library path, e.g. so that you can remove a whole library
    // in the plugin manager by specifying its path
    void setLibraryPath(std::string _lpath) {library_path = _lpath;}
    std::string getLibraryPath(){return library_path;}
    virtual std::string getName();

    //Plugins factories
    createPluginFactory* create;
    destroyPluginFactory* destroy;
    listPluginFactory* list;
    listThirdPartyMetadata* dependencies;

protected:
    DynamicLibrary *mLib;
    std::string library_path;
    std::vector<ACPlugin *> mPlugins;
    MediaCycle* media_cycle;
};

class ACDefaultPluginsLibrary : public ACPluginLibrary{
public:
    ACDefaultPluginsLibrary();
    ~ACDefaultPluginsLibrary();
    virtual int initialize();
    virtual std::string getName(){return "default-plugins";}
};

#endif	/* _ACPLUGINLIBRARY_H */

