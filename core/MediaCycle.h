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

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

using namespace std;

enum MCActionType {
	MC_ACTION_ADDFILE,
	MC_ACTION_GETKNN,
	MC_ACTION_GETTHUMBNAIL
};

static void tcp_callback(const char *buffer, int l, char **buffer_send, int *l_send, void *userData);

class MediaCycle {
public:
    MediaCycle(int port=12345, int max_connections=5, string local_directory="",string libname="");
    MediaCycle(const MediaCycle& orig);
    virtual ~MediaCycle();
    // Process incoming requests (addfile, getknn, ...)
    int processTcpMessage(const char* buffer, int l, char **buffer_send, int *l_send);
    // Media Library
    int importDirectory(std::string path, int recursive, int mid=-1);

    // Search by Similarity
    int getKNN(int id, vector<int> &ids, int k);

    // Thumbnail
    string getThumbnail(int id);

    ACMediaLibrary* getLibrary() { return mediaLibrary;}
    ACMediaBrowser* getBrowser() { return mediaBrowser;}
private:
    int port;
    int max_connections;
    string local_directory;
    string libname;
    ACMediaLibrary *mediaLibrary;
    ACMediaBrowser *mediaBrowser;
    ACNetworkSocketServer *networkSocket;    
};

#endif	/* _MEDIACYCLE_H */

