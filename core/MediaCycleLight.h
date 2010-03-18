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

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <vector>
//#include <ACPlugin.h>

using namespace std;

#include <ACMediaTypes.h>
#include <ACMediaNode.h>

// XS the following 2 structs are now in ACMediaNode
//struct ACPoint
//{
//	float x, y, z;
//};

//struct ACLoopAttribute
//{
//	ACPoint 	currentPos, nextPos, nextPosGrid;
//	ACPoint		viewPos;
//	float		distanceMouse;
//	int 		cluster; //cluster index
//	int			active;  // plying or not - and in which mode
//	int			cursor;
//	int 		navigationLevel; // initially all set to zero, while traversing, only the one incremented are kept
//	int			hover;
//	bool		isDisplayed;//CF
//	ACLoopAttribute() : cluster(0), active(0), navigationLevel(0), hover(0), isDisplayed(false) {}
//};//CF: how come isDisplayed is missing here while it is part of ACMediaBrowser!

enum MCActionType {
	MC_ACTION_ADDFILE,
	MC_ACTION_GETKNN,
	MC_ACTION_GETTHUMBNAIL
};

static void tcp_callback(const char *buffer, int l, char **buffer_send, int *l_send, void *userData);

class MediaCycle {
public:
    MediaCycle(ACMediaType aMediaType, string local_directory="",string libname="");
    MediaCycle(const MediaCycle& orig);
    ~MediaCycle();

    int startTcpServer(int port=12345, int max_connections=5);
    int stopTcpServer();
    // Process incoming requests (addfile, getknn, ...)
    int processTcpMessage(const char* buffer, int l, char **buffer_send, int *l_send);
    // Process incoming tcp request from SSI (AVLaughterCycle)
    int processTcpMessageFromSSI(char* buffer, int l, char **buffer_send, int *l_send);
    // Media Library
    int importDirectory(std::string path, int recursive, int mid=-1);
	int importACLLibrary(std::string path);
    int importLibrary(std::string path);
    // Plugins
    int addPlugin(std::string aPluginPath);

    // Search by Similarity
    int getKNN(int id, std::vector<int> &ids, int k);

    // Thumbnail
    string getThumbnailFileName(int id);

    string getLocalDirectoryPath() {return local_directory;}
    string getLibName() {return libname;}
 	
	// API REQUIRED BY VISUAL and GUI
	// 
	int getLibrarySize();
	string getMediaFileName(int i);
	int getMediaType(int i);
	int getThumbnailWidth(int i);
	int getThumbnailHeight(int i);
	int getWidth(int i);
	int getHeight(int i);
	void* getThumbnailPtr(int i);
	int getNeedsDisplay();
	void setNeedsDisplay(int i);
	float getCameraZoom();
	float getCameraRotation();
	const ACMediaNode &getMediaNode(int i);
	int getNavigationLevel();
	void getMouse(float *mx, float *my);
	// 
	void updateState();
	float getFrac();
	void setCameraRotation(float angle);
	int getClickedLoop();
	void incrementLoopNavigationLevels(int i);
	void setSelectedObject(int index);
	void updateClusters(bool animate);
	void updateNeighborhoods();
	void setCameraPosition(float x, float y);
	void getCameraPosition(float &x, float &y);
	void setCameraZoom(float z);
	void setCameraRecenter();
	// 
	void normalizeFeatures();
	void openLibrary(string path);
	void libraryContentChanged();
	void saveAsLibrary(string path);
	void saveACLLibrary(string path);
	void cleanLibrary();
	void setBack();
	void setForward();
	void setClusterNumber(int n);
	void setWeight(int i, float weight);
	void setForwardDown(int i);
	// 
	void setAutoPlay(int i);
	void setClickedNode(int i);
	void setClosestNode(int i);
	int getClosestNode();
	// 
	void pickedObjectCallback(int pid);
	void hoverObjectCallback(int pid);
	void hoverCallback(float x, float y);
	void muteAllSources();
	//
	void* hasBrowser();
	void setVisualisationPlugin(string pluginName);
	void setNeighborhoodsPlugin(string pluginName);
	void setPositionsPlugin(string pluginName);
	//	int addPlugin(std::string aPluginPath);
	
	// LABELS on VIEW
	int getLabelSize();
	string getLabelText(int i);
	ACPoint getLabelPos(int i);

	// Get Features
	vector<float> getFeaturesVectorInMedia(int i, string feature_name);
	
	// Playing time stamp
	int setSourceCursor(int lid, int frame_pos);
	
	// Update audio engine sources
	void setNeedsActivityUpdateLock(int i);
	void setNeedsActivityUpdateRemoveMedia();	
	vector<int>* getNeedsActivityUpdateMedia();
	
private:
	int forwarddown;
	int port;
    int max_connections;
    string local_directory;
    string libname;
	int ah1n1[4];
};

#endif	/* _MEDIACYCLE_H */

