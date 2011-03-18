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

#include "ACMediaLibrary.h"
#include "ACMediaBrowser.h"
#include "ACNetworkSocket.h"
#include "ACPluginManager.h"

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <time.h>

#include "tinyxml.h"

enum MCActionType {
	MC_ACTION_ADDFILE,
	MC_ACTION_GETKNN,
	MC_ACTION_GETTHUMBNAIL
};

//static void tcp_callback(char *buffer, int l, char **buffer_send, int *l_send, void *userData);
//XS: removed this, it's in the cpp file -- otherwize declared static but never defined

typedef void (*ACMediaCycleCallback)(char* message, void *userData);

class MediaCycle {
public:
    MediaCycle(ACMediaType aMediaType, std::string local_directory="", std::string libname="");
    MediaCycle(const MediaCycle& orig);
    virtual ~MediaCycle();	
	void clean();

	// == TCP
    int startTcpServer(int port=12345, int max_connections=5);
    int startTcpServer(int port, int max_connections,ACNetworkSocketServerCallback aCallback);
    int stopTcpServer();
    int processTcpMessage(char* buffer, int l, char **buffer_send, int *l_send);     // Process incoming requests (addfile, getknn, ...)

	// == Callback - SD to be replaced by OSC/UDP communication
	int setCallback(ACMediaCycleCallback mediacycle_callback, void* user_data);
	
    // == Media Library
	int importDirectories();
	int importDirectories(std::vector<std::string> paths, int recursive, bool forward_order=true, bool doSegment=false);
	int importDirectoriesThreaded(std::vector<std::string> paths, int recursive, bool forward_order=true, bool doSegment=false);
    int importDirectory(std::string path, int recursive, bool forward_order=true, bool doSegment=false, TiXmlElement* _medias = 0);
	int setPath(std::string path);
	int importACLLibrary(std::string path);
	int importXMLLibrary(std::string path);
	int importMCSLLibrary(std::string path);//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
	void libraryContentChanged(int needsNormalizeAndCluster=1);
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

    // == Search by Similarity
    int getKNN(int id, std::vector<int> &ids, int k);
    int getKNN(ACMedia *aMedia, std::vector<ACMedia *> &result, int k);

    // Thumbnail
    std::string getThumbnailFileName(int id);

	// == Media Browser
    ACMediaBrowser* getBrowser() { return mediaBrowser;}
	void* hasBrowser();
	ACBrowserMode getBrowserMode();
	void setBrowserMode(ACBrowserMode _mode);
	bool changeBrowserMode(ACBrowserMode _mode);
	void cleanBrowser() { mediaBrowser->clean(); }

	// Plugins
    int addPluginLibrary(std::string aPluginLibraryPath);
    int removePluginLibrary(std::string aPluginLibraryPath);
	ACPluginManager* getPluginManager() { return pluginManager;}
	ACPluginLibrary* getPluginLibrary(std::string aPluginLibraryPath) const;
	bool removePluginFromLibrary(std::string _plugin_name, std::string _library_path);
	std::vector<std::string> getListOfPlugins();
	std::vector<std::string> getListOfActivePlugins();

	// XS TODO do we want so many methods ?
	void setClustersMethodPlugin(std::string pluginName);
	void setNeighborsMethodPlugin(std::string pluginName);
	void setClustersPositionsPlugin(std::string pluginName);
	void setNeighborsPositionsPlugin(std::string pluginName);
	void setVisualisationPlugin(std::string pluginName);
	void changeClustersMethodPlugin(std::string pluginName);
	void changeNeighborsMethodPlugin(std::string pluginName);
	void changeClustersPositionsPlugin(std::string pluginName);
	void changeNeighborsPositionsPlugin(std::string pluginName);
	//void changeVisualisationPlugin(std::string pluginName);
	void dumpPluginsList();
	
	// == Media
	ACMediaNode &getMediaNode(int i);
	std::string getMediaFileName(int i);
	ACMediaType getMediaType(int i);
	ACMediaType getMediaType() {return mediaLibrary->getMediaType();}
	void setMediaType(ACMediaType _mt);
	bool changeMediaType(ACMediaType aMediaType);
	
	std::vector<std::string> getExtensionsFromMediaType(ACMediaType media_type){return ACMediaFactory::getInstance()->getExtensionsFromMediaType(media_type);}
	int getThumbnailWidth(int i);
	int getThumbnailHeight(int i);
	int getWidth(int i);
	int getHeight(int i);
	void* getThumbnailPtr(int i);
	int getNeedsDisplay3D();
	void setNeedsDisplay3D(bool _dis);
	int getNeedsDisplay();
	void setNeedsDisplay(bool _dis);
	
	// == view
	void getMouse(float *mx, float *my);
	float getCameraZoom();
	float getCameraRotation();
	void setCameraRotation(float angle);
	void setCameraPosition(float x, float y);
	void getCameraPosition(float &x, float &y);
	void setCameraZoom(float z);
	void setCameraRecenter();
	void setAutoPlay(int i);
	int getClickedNode();
	void setClickedNode(int i);
	void setClosestNode(int i);
	int getClosestNode();
	int	getLastSelectedNode();
	
	// == Cluster Display
	void updateState();
	void storeNavigationState();
	int getNavigationLevel();
	float getFrac();
	void incrementLoopNavigationLevels(int i);
	void setReferenceNode(int index);
	int getReferenceNode();
	void goBack();
	void goForward();
	void setClusterNumber(int n);
	void setForwardDown(int i);
//	void setPlayKeyDown(bool i){playkeydown = i;};
//	bool getPlayKeyDown(){return playkeydown;};
	
	// == Features
	void normalizeFeatures(int needsNormalize=1);
	std::vector<float> getFeaturesVectorInMedia(int i, std::string feature_name);
	void setWeight(int i, float weight);
	std::vector<float> getWeightVector();
	float getWeight(int i);
	
	// == Pointers
	int getPointerSize();
	ACPointer& getPointer(int i);
	
	// == LABELS on VIEW
 	int getLabelSize();
	std::string getLabelText(int i);
	ACPoint getLabelPos(int i);	
	
	// == Playing time stamp
	int setSourceCursor(int lid, int frame_pos);
	int setCurrentFrame(int lid, int frame_pos);
	void muteAllSources();

	// == Update audio engine sources
	void setNeedsActivityUpdateLock(int i);
	void setNeedsActivityUpdateRemoveMedia();	
	std::vector<int>* getNeedsActivityUpdateMedia();

	// == callbacks
	void pickedObjectCallback(int pid);
	void hoverObjectCallback(int pid);
	void hoverCallback(float xx, float yy);

	// == NEW, replaces updateClusters and updateNeighborhoods
	void updateDisplay(bool animate);
	
	// == config (in XMl !!)
	TiXmlHandle readXMLConfigFileHeader(std::string _fname="");
	int readXMLConfigFileCore(TiXmlHandle rootHandle);
	int readXMLConfigFilePlugins(TiXmlHandle rootHandle);

	int readXMLConfigFile(std::string _fname="");
	void saveXMLConfigFile(std::string _fname="");
	void setConfigFile(std::string _fname){config_file_xml = _fname;}

	// == User log
	void cleanUserLog();

	// == Dump / used for Debug 
	void dumpNavigationLevel();
	void dumpLoopNavigationLevels();

	// == testing 
	void testThreads();
	void testLabels();
	
private:
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
	
	// settings and features XML
	std::string config_file_xml;
	TiXmlElement* MC_e_medias; // xml node to hook up medias  

	int prevLibrarySize;
	
	bool mNeedsDisplay;
	
	// Media Import Thread
	pthread_t	   import_thread;
	pthread_attr_t import_thread_attr;
	void* import_thread_arg;
	std::vector<std::string> import_directories;
	int import_recursive;
	bool import_forward_order;
	bool import_doSegment;
	
	ACMediaCycleCallback mediacycle_callback;
	void* mediacycle_callback_data;
	
};

#endif	/* _MEDIACYCLE_H */

