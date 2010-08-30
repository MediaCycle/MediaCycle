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

using namespace std;

enum MCActionType {
	MC_ACTION_ADDFILE,
	MC_ACTION_GETKNN,
	MC_ACTION_GETTHUMBNAIL
};

static void tcp_callback(char *buffer, int l, char **buffer_send, int *l_send, void *userData);
//XS TODO: declared static but never defined

class MediaCycle {
public:
    MediaCycle(ACMediaType aMediaType, string local_directory="",string libname="");
    MediaCycle(const MediaCycle& orig);
    virtual ~MediaCycle();

	// == TCP
    int startTcpServer(int port=12345, int max_connections=5);
    int startTcpServer(int port, int max_connections,ACNetworkSocketServerCallback aCallback);
    int stopTcpServer();
    int processTcpMessage(char* buffer, int l, char **buffer_send, int *l_send);     // Process incoming requests (addfile, getknn, ...)

    // == Media Library
    int importDirectory(std::string path, int recursive, int mid=0, bool forward_order=true, bool doSegment=false);
	int importACLLibrary(std::string path);
	int importMCSLLibrary(std::string path);//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
    int importLibrary(std::string path);
	int getLibrarySize(); // = getnumberofmedia
	int getNumberOfMediaNodes();
    ACMediaLibrary* getLibrary() { return mediaLibrary;}
    string getLocalDirectoryPath() {return local_directory;}
    string getLibName() {return libname;}

    // == Search by Similarity
    int getKNN(int id, vector<int> &ids, int k);
    int getKNN(ACMedia *aMedia, vector<ACMedia *> &result, int k);

    // Thumbnail
    string getThumbnailFileName(int id);

	// == Media Browser
    ACMediaBrowser* getBrowser() { return mediaBrowser;}
	void* hasBrowser();
	ACBrowserMode getMode();
	void setMode(ACBrowserMode _mode);

	// Plugins
    int addPlugin(std::string aPluginPath);
	ACPluginManager* getPluginManager() { return pluginManager;}
	void setClustersMethodPlugin(string pluginName);
	void setNeighborsMethodPlugin(string pluginName);
	void setClustersPositionsPlugin(string pluginName);
	void setNeighborsPositionsPlugin(string pluginName);
	void setVisualisationPlugin(string pluginName);
	void changeClustersMethodPlugin(string pluginName);
	void changeNeighborsMethodPlugin(string pluginName);
	void changeClustersPositionsPlugin(string pluginName);
	void changeNeighborsPositionsPlugin(string pluginName);
	//void changeVisualisationPlugin(string pluginName);
	void dumpPluginsList();
	
	// == Media
	const ACMediaNode &getMediaNode(int i);
	string getMediaFileName(int i);
	int getMediaType(int i);
	int getThumbnailWidth(int i);
	int getThumbnailHeight(int i);
	int getWidth(int i);
	int getHeight(int i);
	void* getThumbnailPtr(int i);
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
	void setPlayKeyDown(bool i){playkeydown = i;};
	bool getPlayKeyDown(){return playkeydown;};
	
	// == Features
	void normalizeFeatures();
	void openLibrary(string path);
	void libraryContentChanged();
	//	void saveAsLibrary(string path);
	void saveACLLibrary(string path);
	void saveMCSLLibrary(string path);//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
	void cleanLibrary();
	// Get Features Vector (identified by feature_name) in media i 
	vector<float> getFeaturesVectorInMedia(int i, string feature_name);
	void setWeight(int i, float weight);
	vector<float> getWeightVector();
	float getWeight(int i);
	
	// == LABELS on VIEW
 	int getLabelSize();
	string getLabelText(int i);
	ACPoint getLabelPos(int i);
	
	// == Playing time stamp
	int setSourceCursor(int lid, int frame_pos);
	void muteAllSources();

	// == Update audio engine sources
	void setNeedsActivityUpdateLock(int i);
	void setNeedsActivityUpdateRemoveMedia();	
	vector<int>* getNeedsActivityUpdateMedia();

	// == callbacks
	void pickedObjectCallback(int pid);
	void hoverObjectCallback(int pid);
	void hoverCallback(float x, float y);

	// == NEW, replaces updateClusters and updateNeighborhoods
	void updateDisplay(bool animate);
	void readConfigFile(string fname);
//	void dumpConfigFile();

	// == User log
	void cleanUserLog();

	// == Dump / used for Debug 
	void dumpNavigationLevel();
	void dumpLoopNavigationLevels();

private:
	int forwarddown;
	bool playkeydown;
	int port;
	int max_connections;
	string local_directory;
	string libname;
	ACMediaLibrary *mediaLibrary;
	ACMediaBrowser *mediaBrowser;
	ACNetworkSocketServer *networkSocket;
	ACPluginManager *pluginManager;
	string config_file;
};

#endif	/* _MEDIACYCLE_H */

