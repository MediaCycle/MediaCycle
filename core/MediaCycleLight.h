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

#include <ACMediaTypes.h>
#include <ACMediaNode.h>

enum MCActionType {
	MC_ACTION_ADDFILE,
	MC_ACTION_GETKNN,
	MC_ACTION_GETTHUMBNAIL
};

//static void tcp_callback(const char *buffer, int l, char **buffer_send, int *l_send, void *userData);

class MediaCycle {
public:
    MediaCycle(ACMediaType aMediaType, std::string local_directory="",std::string libname="");
    MediaCycle(const MediaCycle& orig);
    ~MediaCycle();

    int startTcpServer(int port=12345, int max_connections=5);
    int stopTcpServer();
    // Process incoming requests (addfile, getknn, ...)
    int processTcpMessage(const char* buffer, int l, char **buffer_send, int *l_send);
    // Process incoming tcp request from SSI (AVLaughterCycle)
    int processTcpMessageFromSSI(char* buffer, int l, char **buffer_send, int *l_send);
    // Media Library
    int importDirectory(std::string path, int recursive, bool forward_order=true);
	int importACLLibrary(std::string path);
    int importLibrary(std::string path);
    // Plugins
    int addPluginLibrary(std::string aPluginPath);

    // Search by Similarity
    int getKNN(int id, std::vector<int> &ids, int k);

    // Thumbnail
    std::string getThumbnailFileName(int id);

    std::string getLocalDirectoryPath() {return local_directory;}
    std::string getLibName() {return libname;}

	// API REQUIRED BY VISUAL and GUI
	//
	int getLibrarySize();
	std::string getMediaFileName(int i);
	ACMediaType getMediaType(int i);
	int getThumbnailWidth(int i);
	int getThumbnailHeight(int i);
	int getWidth(int i);
	int getHeight(int i);
	void* getThumbnailPtr(int i);
	int getNeedsDisplay();
	void setNeedsDisplay(bool _dis);
	float getCameraZoom();
	float getCameraRotation();
	const ACMediaNode &getMediaNode(int i);
	int getNavigationLevel();
	//
	void updateState();
	void storeNavigationState();
	float getFrac();
	void setCameraRotation(float angle);
	int getClickedNode();
        void incrementNavigationLevels(int i);
	void setReferenceNode(int index);
	void updateClusters(bool animate);
	void updateNeighborhoods();
	void setCameraPosition(float x, float y);
	void getCameraPosition(float &x, float &y);
	void setCameraZoom(float z);
	void setCameraRecenter();
	//
	void normalizeFeatures();
	void openLibrary(std::string path);
	void libraryContentChanged();
	//	void saveAsLibrary(std::string path);
	void saveACLLibrary(std::string path);
	void cleanLibrary();
	void goBack();
	void goForward();
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
	void hoverWithPointerId(float xx, float yy, int p_id = -1);
	void hoverWithPointerIndex(float xx, float yy, int p_index = 0);
	void muteAllSources();
	//
	void* hasBrowser();
	void setVisualisationPlugin(std::string pluginName);
	void setNeighborhoodsMethodPlugin(std::string pluginName);
	void setPositionsPlugin(std::string pluginName);
	//	int addPluginLibrary(std::string aPluginPath);

	// LABELS on VIEW
	int getLabelSize();
	std::string getLabelText(int i);
	ACPoint getLabelPos(int i);

	// Get Features
	std::vector<float> getFeaturesVectorInMedia(int i, std::string feature_name);

	// Playing time stamp
	int setSourceCursor(int lid, int frame_pos);
	int setCurrentFrame(int lid, int frame_pos);

	// Update audio engine sources
	void setNeedsActivityUpdateLock(int i);
	void setNeedsActivityUpdateRemoveMedia();
	std::vector<int>* getNeedsActivityUpdateMedia();

private:
	int forwarddown;
	int port;
    int max_connections;
    std::string local_directory;
    std::string libname;
	int ah1n1[4];
};

#endif	/* _MEDIACYCLE_H */

