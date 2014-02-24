/*
 * File:   MediaCycle.h
 * Author: Alexis Moinet
 *
 * @date 21 juillet 2009
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

#ifndef _MEDIACYCLE_H
#define	_MEDIACYCLE_H

class MediaCycle;

#include "ACEventListener.h"
#include "ACMediaBrowser.h"

#include "ACAbstractDefaultConfig.h"
#include "ACMediaLibrary.h"
#include "ACNetworkSocket.h"
#include "ACPluginManager.h"
#include "Observer.h"
#include "ACEventManager.h"


#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <time.h>
#define TIXML_USE_STL
#include "tinyxml.h"

enum MCActionType {
    MC_ACTION_ADDFILE,
    MC_ACTION_GETKNN,
    MC_ACTION_GETTHUMBNAIL
};

//static void tcp_callback(char *buffer, int l, char **buffer_send, int *l_send, void *userData);
//XS: removed this, it's in the cpp file -- otherwize declared static but never defined

typedef void (*ACMediaCycleCallback)(const char* message, void *userData);

class MediaCycle : public Subject {
public:
    MediaCycle(ACMediaType aMediaType, std::string local_directory="", std::string libname="");
    MediaCycle(const MediaCycle& orig);
    virtual ~MediaCycle();
    void clean();

    /// Default configs
    ACAbstractDefaultConfig* getDefaultConfig(std::string name);
    std::map<std::string,ACAbstractDefaultConfig*> getDefaultConfigs();
    std::vector<std::string> getDefaultConfigsNames();
    bool addDefaultConfig(ACAbstractDefaultConfig*);
    int getDefaultConfigsNumber();
    /// Tries to load a default config. Throws runtime errors, to wrap with try/catch.
    void loadDefaultConfig(std::string name);
    void loadDefaultConfig(ACAbstractDefaultConfig* config);
    ACAbstractDefaultConfig* getCurrentConfig();
    std::string getCurrentConfigName();

    /// TCP
    int startTcpServer(int port=12345, int max_connections=5);
    int startTcpServer(int port, int max_connections,ACNetworkSocketServerCallback aCallback);
    int stopTcpServer();
    int processTcpMessage(char* buffer, int l, char **buffer_send, int *l_send);     // Process incoming requests (addfile, getknn, ...)

    /// Media Library
    int importDirectories();
    int importDirectories(std::vector<std::string> paths, int recursive, bool forward_order=true, bool doSegment=false);
    int importDirectoriesThreaded(std::vector<std::string> paths, int recursive, bool forward_order=true, bool doSegment=false);
    //int importDirectory(std::string path, int recursive, bool forward_order=true, bool doSegment=false, TiXmlElement* _medias = 0);
    int importDirectory(std::string path, int recursive, bool forward_order=true, bool doSegment=false, bool _save_timed_feat=false);
    int setPath(std::string path);
    int importACLLibrary(std::string path);
    int importXMLLibrary(std::string path);
    int importMCSLLibrary(std::string path);//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
    void libraryContentChanged(int needsNormalizeAndCluster=1);

    void saveLibrary(std::string _path, ACMediaLibraryWriterPlugin* plugin);
    void saveLibrary(std::string _path, std::string plugin_name);
    void saveACLLibrary(std::string path);
    void saveXMLLibrary(std::string path);
    void saveMCSLLibrary(std::string path);//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
    void cleanLibrary();
    int getLibrarySize(); // = getnumberofmedia
    int getNumberOfMediaNodes();
    ACMediaLibrary* getLibrary() { return mediaLibrary;}
    std::string getLocalDirectoryPath() {return local_directory;}

    // XS TODO this is obsolete (acl)
    std::string getLibName() {return libname;}

    /// Search by Similarity
    int getKNN(int id, std::vector<int> &ids, int k);
    int getKNN(ACMedia *aMedia, std::vector<ACMedia *> &result, int k);

    /// Thumbnail
    std::string getThumbnailFileName(int id);

    /// Media Browser
    ACMediaBrowser* getBrowser() { return mediaBrowser;}
    bool hasBrowser();
    ACBrowserMode getBrowserMode();
    void setBrowserMode(ACBrowserMode _mode);
    bool changeBrowserMode(ACBrowserMode _mode);
    void cleanBrowser() { mediaBrowser->clean(); }
    

    /// Listener manager
    void addListener(ACEventListener* eventListener);
    ACEventManager *getEventManager(){return eventManager;}

    /// Plugins
    // XS TODO cleanPlugins
    int addPluginLibrary(std::string aPluginLibraryPath);
    int addPluginLibrary(ACPluginLibrary* library);
    int loadPluginLibraryFromBasename(std::string basename);
    int removePluginLibrary(std::string aPluginLibraryPath);
    int removePluginLibraryFromBasename(std::string basename);
    ACPluginManager* getPluginManager() { return pluginManager;}
    ACPluginLibrary* getPluginLibrary(std::string aPluginLibraryPath) const;
    ACPlugin* getPlugin(std::string name);
    bool removePluginFromLibrary(std::string _plugin_name, std::string _library_path);
    std::vector<std::string> getListOfPlugins();
    std::vector<std::string> getListOfActivePlugins();
    std::string getPluginPathFromBaseName(std::string basename);
    std::string getLibraryPathFromPlugin(std::string name);

    bool changeActivePlugin(ACPluginType pluginType, std::string pluginName);
    std::vector<std::string> getActivePluginNames(ACPluginType pluginType, ACMediaType mediaType = MEDIA_TYPE_NONE);
    std::vector<std::string> getAvailablePluginNames(ACPluginType pluginType, ACMediaType mediaType = MEDIA_TYPE_NONE);

    void setPreProcessPlugin(std::string pluginName);

    void setMediaReaderPlugin(std::string pluginName);

#ifdef SUPPORT_MULTIMEDIA
    std::string getActiveSubMediaKey();
    int setActiveMediaType(std::string mediaName);
#endif//def SUPPORT_MULTIMEDIA

    void dumpPluginsList();

    /// Media
    ACMediaNode* getMediaNode(int i);
    ACMediaNode* getNodeFromMedia(ACMedia* _media);
    std::string getMediaFileName(int i);
    ACMediaType getMediaType(int i);
    ACMediaType getMediaType() {return mediaLibrary->getMediaType();}
    bool mediaIsTagged(int i);
    void setMediaType(ACMediaType _mt);
    bool changeMediaType(ACMediaType aMediaType);

    std::vector<std::string> getExtensionsFromMediaType(ACMediaType media_type){return mediaLibrary->getExtensionsFromMediaType(media_type);}
    int getWidth(int i);
    int getHeight(int i);
    int getNeedsDisplay3D();
    void setNeedsDisplay3D(bool _dis);
    int getNeedsDisplay();
    void setNeedsDisplay(bool _dis);

    /// View
    float getCameraZoom();
    float getCameraRotation();
    void setCameraRotation(float angle);
    void setCameraPosition(float x, float y);
    void getCameraPosition(float &x, float &y);
    void setCameraZoom(float z);
    void setCameraRecenter();
    void setAutoPlay(int i);
    bool getAutoPlay();
    void setAutoDiscard(bool status);
    bool getAutoDiscard();
    int getClickedNode();
    void setClickedNode(int i);
    void setClosestNode(int i,int p_index = 0);
    int getClosestNode(int p_index = 0);
    void setClosestDistance(float d,int p_index = 0);
    float getClosestDistance(int p_index = 0);
    int	getLastSelectedNode();

    /// Cluster Display
    void updateState();
    void storeNavigationState();
    int getNavigationLevel();
    float getFrac();
    void incrementNavigationLevels(int i);
    void setReferenceNode(int index);
    int getReferenceNode();
    void goBack();
    void goForward();
    void setClusterNumber(int n);
    void setForwardDown(int i);
    void forwardNextLevel();
    void changeReferenceNode();

    /// Media tagging
    void changeSelectedMediaTagId(int ClusterId);
    void setMediaTaggedClassId(int mediaId,int pId){if (mediaLibrary) mediaLibrary->setMediaTaggedClassId(mediaId,pId);};
    int getMediaTaggedClassId(int mediaId){return (mediaLibrary?mediaLibrary->getMediaTaggedClassId(mediaId):-1);};
    void transferClassToTag();
    void cleanAllMediaTag();
    //	void setPlayKeyDown(bool i){playkeydown = i;};
    //	bool getPlayKeyDown(){return playkeydown;};

    /// Features
    void normalizeFeatures(int needsNormalize=1);
    FeaturesVector getFeaturesVectorInMedia(int i, std::string feature_name);
    void setWeight(int i, float weight);
    void setWeightVector(std::vector<float> fw);
    std::vector<float> getWeightVector();
    float getWeight(int i);

    /// Pointers
    int getNumberOfPointers();
    ACPointer* getPointerFromIndex(int i); // for use when parsing pointers incrementally
    ACPointer* getPointerFromId(int i); // for use when parsing pointers from the ID set by the input device
    void resetPointers();
    std::list<int> getPointerIds();
    void addPointer(int p_id);
    void removePointer(int p_id);

    /// Labels
    int getLabelSize();
    std::string getLabelText(int i);
    ACPoint getLabelPos(int i);

    /// Playing time stamp
    void setSourceCursor(int lid, int frame_pos);
    void setCurrentFrame(int lid, int frame_pos);
    void muteAllSources();

    /// Update audio engine sources
    void setNeedsActivityUpdateLock(int i);
    void setNeedsActivityUpdateRemoveMedia();
    std::vector<int>* getNeedsActivityUpdateMedia();

    /// Media/node interaction callbacks
    void pickedObjectCallback(int pid);
    void hoverWithPointerId(float xx, float yy, int p_id = -1);
    void hoverWithPointerIndex(float xx, float yy, int p_index = 0);
    bool performActionOnMedia(std::string action, long int mediaId, std::vector<boost::any> arguments=std::vector<boost::any>());
    /// Convenience cases for faster access
    bool performActionOnMedia(std::string action, long int mediaId, std::string argument);
    bool performActionOnMedia(std::string action, long int mediaId, int argument);
    bool performActionOnMedia(std::string action, long int mediaId, float argument);

    /// NEW, replaces updateClusters and updateNeighborhoods
    void updateDisplay(bool animate);
    void initializeFeatureWeights();

    /// XML config
    TiXmlHandle readXMLConfigFileHeader(std::string _fname="");
    int readXMLConfigFileCore(TiXmlHandle rootHandle);
    int readXMLConfigFilePlugins(TiXmlHandle rootHandle);
    int readXMLConfigFile(std::string _fname="");
    TiXmlElement* saveXMLConfigFile(std::string _fname="");
    void setConfigFile(std::string _fname){config_file_xml = _fname;}

    /// Dump / used for Debug
    void dumpNavigationLevel();
    void dumpNavigationLevels();

    /// Testing
    void testThreads();
    void testLabels();

private:
    std::map<std::string,ACAbstractDefaultConfig*> default_configs;
    std::string current_config;
    int forwarddown;
    //	bool playkeydown;
    int port;
    int max_connections;
    std::string local_directory;
    std::string libname;
    ACMediaLibrary *mediaLibrary;
    ACMediaBrowser *mediaBrowser;
    ACNetworkSocketServer *networkSocket;
    ACPluginManager *pluginManager;
    ACEventManager *eventManager;

    // settings and features XML
    std::string config_file_xml;

    int prevLibrarySize;

    bool mNeedsDisplay;

    // Media Import Thread
    pthread_t import_thread;
    pthread_attr_t import_thread_attr;
    void* import_thread_arg;
    std::vector<std::string> import_directories;
    int import_recursive;
    bool import_forward_order;
    bool import_doSegment;
    bool importing;

    void* mediacycle_callback_data;

public:
    // Callback
    void mediaImported(int n,int nTot,int mId);
    bool isImporting(){return this->importing;}
};

#endif	/* _MEDIACYCLE_H */

