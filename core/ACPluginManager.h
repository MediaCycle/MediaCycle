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

#include "ACPluginLibrary.h"

class MediaCycle;

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
    std::vector<ACMediaFeatures*> calculate(ACMedia* theMedia, bool _save_timed_feat=false);
};	

class ACAvailableSegmentPlugins:public ACAvailablePlugins<ACSegmentationPlugin>{//TR: this class doesn't allocate memory for plugins. It's just a references container.
public:
    ACAvailableSegmentPlugins(std::vector<ACPluginLibrary *> PluginLibrary);
    ACAvailableSegmentPlugins();
    std::vector<ACMedia*> segment(ACMediaTimedFeature *ft,ACMedia* theMedia);
    std::vector<ACMedia*> segment(ACMedia* theMedia);
};

class ACAvailableThumbnailerPlugins:public ACAvailablePlugins<ACThumbnailerPlugin>{//TR: this class doesn't allocate memory for plugins. It's just a references container.
public:
    ACAvailableThumbnailerPlugins(std::vector<ACPluginLibrary *> PluginLibrary);
    ACAvailableThumbnailerPlugins();
    std::vector<ACMediaThumbnail*> summarize(ACMedia* theMedia, bool feature_extracted, bool segmentation_done);
    std::vector<std::string> getThumbnailNames(ACMediaType mediaType);
};


class ACAvailableMediaReaderPlugins:public ACAvailablePlugins<ACMediaReaderPlugin>{//TR: this class doesn't allocate memory for plugins. It's just a references container.
public:
    ACAvailableMediaReaderPlugins(std::vector<ACPluginLibrary *> PluginLibrary);
    ACAvailableMediaReaderPlugins();
    std::map<std::string,std::string> getExtensionsFromMediaType(ACMediaType media_type); // returns a map of extensions (key) and plugin names (warning: the last loaded plugin supersedes the others for common extensions)
};

class ACAvailableMediaRendererPlugins:public ACAvailablePlugins<ACMediaRendererPlugin>{//TR: this class doesn't allocate memory for plugins. It's just a references container.
public:
    ACAvailableMediaRendererPlugins(std::vector<ACPluginLibrary *> PluginLibrary);
    ACAvailableMediaRendererPlugins();
    bool performActionOnMedia(std::string action, long int mediaId, std::string value="");
};

/*class ACAvailableClusterMethodPlugins:public ACAvailablePlugins<ACClusterMethodPlugin>{//TR: this class doesn't allocate memory for plugins. It's just a references container.
public:
    ACAvailableClusterMethodPlugins(std::vector<ACPluginLibrary *> PluginLibrary);
    ACAvailableClusterMethodPlugins();
    //~ACAvailableClusterMethodPlugins();
    //void updateClusters(ACMediaBrowser* mediaBrowser ,bool needsCluster=true);
};

class ACAvailableClusterPositionsPlugins:public ACAvailablePlugins<ACClusterPositionsPlugin>{//TR: this class doesn't allocate memory for plugins. It's just a references container.
public:
    ACAvailableClusterPositionsPlugins(std::vector<ACPluginLibrary *> PluginLibrary);
    ACAvailableClusterPositionsPlugins();
    //~ACAvailableClusterPositionsPlugins();
    //void updateNextPositions(ACMediaBrowser* );
};

class ACAvailableNeighborMethodPlugins:public ACAvailablePlugins<ACNeighborMethodPlugin>{//TR: this class doesn't allocate memory for plugins. It's just a references container.
public:
    ACAvailableNeighborMethodPlugins(std::vector<ACPluginLibrary *> PluginLibrary);
    ACAvailableNeighborMethodPlugins();
    //~ACAvailableNeighborMethodPlugins();
    //void updateNeighborhoods(ACMediaBrowser* mediaBrowser);
};

class ACAvailableNeighborPositionsPlugins:public ACAvailablePlugins<ACNeighborPositionsPlugin>{//TR: this class doesn't allocate memory for plugins. It's just a references container.
public:
    ACAvailableNeighborPositionsPlugins(std::vector<ACPluginLibrary *> PluginLibrary);
    ACAvailableNeighborPositionsPlugins();
    //~ACAvailableNeighborPositionsPlugins();
    //void updateNextPositions(ACMediaBrowser* );
};*/

class ACPluginManager {

public:
    ACPluginManager();
    ACPluginManager(const ACPluginManager& orig);
    virtual ~ACPluginManager();
    void setMediaCycle(MediaCycle* _media_cycle);
    std::vector<std::string> addLibrary(std::string aPluginLibraryPath);
    int removeLibrary(std::string aPluginLibraryPath);
    bool removePluginFromLibrary(std::string _plugin_name, std::string _library_path);
    std::vector<std::string> getListOfPlugins();
    std::string getLibraryPathFromPlugin(std::string name);

    int clean();
    void dump();

    std::vector<ACPluginLibrary *> getPluginLibrary() { return this->mPluginLibrary;}
    ACPluginLibrary *getPluginLibrary(int i) { return this->mPluginLibrary[i];}
    ACPluginLibrary *getPluginLibrary(std::string _lpath) ;
    
    int getSize() { return this->mPluginLibrary.size();}
    ACPlugin *getPlugin(std::string aPluginName);

    ACAvailableFeaturesPlugins *getAvailableFeaturesPlugins(){return this->mAvailableFeaturePlugins;}// returns a container with available feature plugins reference
    std::vector<std::string> getAvailableFeaturesPluginsNames(ACMediaType MediaType);
    ACAvailableSegmentPlugins *getAvailableSegmentPlugins(){return this->mAvailableSegmentPlugins;}// returns a container with available segment plugins reference
    int getAvailableSegmentPluginsSize(ACMediaType MediaType);
    ACAvailableSegmentPlugins *getActiveSegmentPlugins(){return this->mActiveSegmentPlugins;}// returns a container with active segment plugins reference
    int getActiveSegmentPluginsSize(ACMediaType MediaType);
    std::vector<std::string> getActiveSegmentPluginsNames(ACMediaType MediaType);
    bool setActiveSegmentPlugin(std::string name);

    ACAvailableThumbnailerPlugins *getAvailableThumbnailerPlugins(){return this->mAvailableThumbnailerPlugins;}// returns a container with available thumbnailer plugins reference
    std::vector<std::string> getAvailableThumbnailNames(ACMediaType mediaType){return this->mAvailableThumbnailerPlugins->getThumbnailNames(mediaType);}

    ACAvailableMediaReaderPlugins *getAvailableMediaReaderPlugins(){return this->mAvailableMediaReaderPlugins;}// returns a container with available thumbnailer plugins reference
    ACAvailableMediaRendererPlugins *getAvailableMediaRendererPlugins(){return this->mAvailableMediaRendererPlugins;}// returns a container with available thumbnailer plugins reference

    int getAvailablePluginsSize(ACPluginType PluginType, ACMediaType MediaType);
    std::vector<std::string> getAvailablePluginsNames(ACPluginType PluginType, ACMediaType MediaType);

    ACPreProcessPlugin* getPreProcessPlugin(ACMediaType MediaType);

    void pluginLoaded(std::string plugin_name);

protected:
    void cleanPluginLists();
    void updateAvailablePluginLists(ACPluginLibrary *acpl);
    void addLibraryToPluginLists(ACPluginLibrary *acpl);
    void removeLibraryFromPluginLists(ACPluginLibrary *acpl);
    void updateActivePluginLists();

private:
    std::vector<ACPluginLibrary *> mPluginLibrary;	
    ACAvailableFeaturesPlugins* mAvailableFeaturePlugins;
    ACAvailableSegmentPlugins* mAvailableSegmentPlugins;
    ACAvailableSegmentPlugins* mActiveSegmentPlugins;
    ACAvailableThumbnailerPlugins* mAvailableThumbnailerPlugins;
    ACAvailableMediaReaderPlugins* mAvailableMediaReaderPlugins;
    ACAvailableMediaRendererPlugins* mAvailableMediaRendererPlugins;
    MediaCycle* media_cycle;
};

#endif	/* _ACPLUGINMANAGER_H */

