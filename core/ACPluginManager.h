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

#ifndef _ACPLUGINMANAGER_H
#define	_ACPLUGINMANAGER_H

#include "ACPlugin.h"
#include "DynamicLibrary.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>


class ACPluginLibrary {
public:
    ACPluginLibrary(DynamicLibrary *aLib);
    virtual ~ACPluginLibrary();
    int initialize();
    
    std::vector<ACPlugin *> getPlugins() {return this->mPlugins;};
    ACPlugin *getPlugin(int i) {return this->mPlugins[i];};
    ACPlugin *getPlugin(std::string aPluginName);
	
	bool removePlugin(std::string aPluginName);

    int getSize() {return this->mPlugins.size();};
    DynamicLibrary* getLib() { return this->mLib;};
    void freePlugins();
	void dump();
	
	// store library path, e.g. so that you can remove a whole library 
	// in the plugin manager by specifying its path
	void setLibraryPath(std::string _lpath) {library_path = _lpath;}
	std::string getLibraryPath(){return library_path;}

    //Plugins factories
    createPluginFactory* create;
    destroyPluginFactory* destroy;
    listPluginFactory* list;

private:
    DynamicLibrary *mLib;
    std::vector<ACPlugin *> mPlugins;
	std::string library_path;
};

template <typename T> 
class ACAvailablePlugins
{
public:
        ACAvailablePlugins(std::vector<ACPluginLibrary *> PluginLibrary);
        ACAvailablePlugins();
        ~ACAvailablePlugins();
	int clean();
	int remove(ACPlugin *);
	int remove(ACPluginLibrary *);
	int add(ACPlugin *);
	int add(ACPluginLibrary *);
	int update(std::vector<ACPluginLibrary *> PluginLibrary);
	std::vector<std::string> getName(ACMediaType MediaType);
        std::vector<std::string> getName();
	int getSize(ACMediaType MediaType);
	void log();
	
	
protected:	
	std::map<ACMediaType,std::vector<T *> > mCurrPlugin;
//	ACPreProcessPlugin* mCurrPreProcessPlugin;
};



class ACAvailableFeaturesPlugins:public ACAvailablePlugins<ACFeaturesPlugin>{//TR: this class doesn't allocate memory for plugins. It's just a references container.
public:
        ACAvailableFeaturesPlugins(std::vector<ACPluginLibrary *> PluginLibrary);
        ACAvailableFeaturesPlugins();
        //~ACAvailableFeaturesPlugins();
/*	int clean();
	int remove(ACPlugin *);
	int remove(ACPluginLibrary *);
	int add(ACPlugin *);
	int add(ACPluginLibrary *);
	int update(std::vector<ACPluginLibrary *> PluginLibrary);
	std::vector<std::string> getName(ACMediaType MediaType);
	int getSize(ACMediaType MediaType);
	void log();*/
	
//	std::vector<ACMediaFeatures*> calculate(std::string aFileName, bool _save_timed_feat=false );
	std::vector<ACMediaFeatures*> calculate(ACMediaData* aData, ACMedia* theMedia, bool _save_timed_feat=false);	

	// XS check this one !!
	//ACMediaTimedFeature* getTimedFeatures(ACMedia* theMedia);
	
	
protected:	
//	std::map<ACMediaType,std::vector<ACFeaturesPlugin *> > mCurrFeaturePlugin;
	//ACPreProcessPlugin* mCurrPreProcessPlugin;

};	

class ACAvailableSegmentPlugins:public ACAvailablePlugins<ACSegmentationPlugin>{//TR: this class doesn't allocate memory for plugins. It's just a references container.
public:
        ACAvailableSegmentPlugins(std::vector<ACPluginLibrary *> PluginLibrary);
        ACAvailableSegmentPlugins();
        //~ACAvailableSegmentPlugins();
/*	int clean();
	int remove(ACPlugin *);
	int remove(ACPluginLibrary *);
	int add(ACPlugin *);
	int add(ACPluginLibrary *);
	int update(std::vector<ACPluginLibrary *> PluginLibrary);
	std::vector<std::string> getName(ACMediaType MediaType);
	int getSize(ACMediaType MediaType);
	void log();*/
	
	std::vector<ACMedia*>  segment(ACMediaTimedFeature *ft,ACMedia* theMedia);
	std::vector<ACMedia*>  segment(ACMediaData* aData,ACMedia* theMedia);
	
	
protected:		
};	

class ACPluginManager {

public:
    ACPluginManager();
    ACPluginManager(const ACPluginManager& orig);
    virtual ~ACPluginManager();
    int addLibrary(std::string aPluginLibraryPath);
    int removeLibrary(std::string aPluginLibraryPath);
    bool removePluginFromLibrary(std::string _plugin_name, std::string _library_path);
    std::vector<std::string> getListOfPlugins();

    int clean();
    void dump();

    std::vector<ACPluginLibrary *> getPluginLibrary() { return this->mPluginLibrary;};
    ACPluginLibrary *getPluginLibrary(int i) { return this->mPluginLibrary[i];};
    ACPluginLibrary *getPluginLibrary(std::string _lpath) ;
    
    int getSize() { return this->mPluginLibrary.size();};
    ACPlugin *getPlugin(std::string aPluginName);

    ACAvailableFeaturesPlugins *getAvailableFeaturesPlugins(){return this->mAvailableFeaturePlugins;}// returns a container with available feature plugins reference
    int getAvailableFeaturesPluginsSize(ACMediaType MediaType);
    ACAvailableSegmentPlugins *getAvailableSegmentPlugins(){return this->mAvailableSegmentPlugins;}// returns a container with available segment plugins reference
    int getAvailableSegmentPluginsSize(ACMediaType MediaType);
    ACAvailableSegmentPlugins *getActiveSegmentPlugins(){return this->mActiveSegmentPlugins;}// returns a container with active segment plugins reference
    int getActiveSegmentPluginsSize(ACMediaType MediaType);
    std::vector<std::string> getAvailableSegmentPluginsNames(ACMediaType MediaType);
    std::vector<std::string> getActiveSegmentPluginsNames(ACMediaType MediaType);
    bool setActiveSegmentPlugin(std::string name);

    ACPreProcessPlugin* getPreProcessPlugin(ACMediaType MediaType);
	
	
private:
	
    std::vector<ACPluginLibrary *> mPluginLibrary;	
	
    ACAvailableFeaturesPlugins* mAvailableFeaturePlugins;
    ACAvailableSegmentPlugins* mAvailableSegmentPlugins;
    ACAvailableSegmentPlugins* mActiveSegmentPlugins;
};

#endif	/* _ACPLUGINMANAGER_H */

