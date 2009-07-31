/* 
 * File:   MediaCycle.cpp
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

#include "MediaCycle.h"

MediaCycle::MediaCycle(ACMediaType aMediaType, string local_directory, string libname) {
    this->local_directory = local_directory;
    this->libname = libname;
    this->networkSocket = NULL;


    this->mediaLibrary = new ACMediaLibrary();
    this->mediaLibrary->setMediaType(aMediaType);
    
    this->mediaBrowser = new ACMediaBrowser();
    this->mediaBrowser->setLibrary(this->mediaLibrary);
    
    
    //this doesn't work, I don't know why
    //this->mediaLibrary->openLibrary(local_directory + libname);

    //this works
    /*string libpath(local_directory + libname);
    this->mediaLibrary->openLibrary(libpath);  
    this->mediaBrowser->libraryContentChanged();
     */
    /*
    this->mediaBrowser->setClusterNumber(2);
    this->mediaBrowser->setLibrary(this->mediaLibrary);
    this->mediaBrowser->libraryContentChanged();/* */
    /*
    this->mediaLibrary = new ACMediaLibrary();
    this->mediaBrowser = new ACMediaBrowser();
    
    this->mediaBrowser->setLibrary(this->mediaLibrary);
    this->mediaBrowser->setClusterNumber(10);/* */
    
    /*if ((this->port>=FIRST_PORT_ID)&&(this->port<=LAST_PORT_ID)) {
            this->networkSocket = new ACNetworkSocketServer(this->port, this->max_connections, tcp_callback, this);
            this->networkSocket->start();
    }/* */

    
}

MediaCycle::MediaCycle(const MediaCycle& orig) {

}

MediaCycle::~MediaCycle() {
    stopTcpServer();
}

int MediaCycle::startTcpServer(int aPort, int aMaxConnections) {
    this->port = aPort;
    this->max_connections = aMaxConnections;
    if ((this->port>=FIRST_PORT_ID)&&(this->port<=LAST_PORT_ID)) {
            this->networkSocket = new ACNetworkSocketServer(this->port, this->max_connections, tcp_callback, this);
            this->networkSocket->start();
            return 0;
    }

    return 1;
}

int MediaCycle::stopTcpServer() {
     if (this->networkSocket) {
        this->networkSocket->stop();
        delete this->networkSocket;
    }
}

int MediaCycle::importDirectory(string path, int recursive, int mid) {
	int ret = this->mediaLibrary->importDirectory(path, recursive, mid);
	this->mediaLibrary->normalizeFeatures();
	this->mediaBrowser->libraryContentChanged();
	return ret;
}

int MediaCycle::importLibrary(string path) {
	int ret = this->mediaLibrary->openLibrary(path);
        this->mediaLibrary->normalizeFeatures();
        this->mediaBrowser->libraryContentChanged();
        return ret;
}

static void tcp_callback(const char *buffer, int l, char **buffer_send, int *l_send, void *userData)
{
	MediaCycle *that = (MediaCycle*)userData;
	that->processTcpMessage(buffer, l, buffer_send, l_send);
}

//AM TODO processTcpMessage & processTcpMessageFromSSI must be moved outside of MediaCycle main class
int MediaCycle::processTcpMessage(const char* buffer, int l, char **buffer_send, int *l_send)
{
	FILE *local_file;
	int ret, i;
	string sbuffer, subbuffer, path, fullpath, sbuffer_send;
	int bufpos1, bufpos2;
	int id, k;
        vector<int> ids;

	printf ("Processing TCP message of length %d: %s", l, buffer);

	// SD TODO - define OSC namespace, or else XML schema for these control messages

	sbuffer = string(buffer, l);
	bufpos1 = 0;
	bufpos2 = sbuffer.find(" ");
	subbuffer = sbuffer.substr(bufpos1, bufpos2-bufpos1);
	ostringstream osstream;

	string thumbnail_filename;
	FILE *thumbnail_file;
	struct stat file_status;
	int thumbnail_size;

	if (subbuffer == "addfile") {
		/*bufpos1 = bufpos2+1;
		subbuffer = sbuffer.substr(bufpos1);
		stringstream isstream(subbuffer);
		isstream >> id;
		isstream >> path;*/
		bufpos1 = bufpos2+1;
		bufpos2 = sbuffer.find(" ",bufpos1);
		subbuffer = sbuffer.substr(bufpos1, bufpos2-bufpos1);
		sscanf(subbuffer.c_str(), "%d", &id);
		bufpos1 = bufpos2+1;
		bufpos2 = sbuffer.find(" ",bufpos1);
		path = sbuffer.substr(bufpos1, bufpos2-bufpos1);
		bufpos1 = bufpos2+1;
		fullpath = local_directory + path;
		local_file = fopen(fullpath.c_str(),"wb");
		fwrite((void*)(buffer+bufpos1), 1, l-bufpos1, local_file);
		fclose(local_file);
		ret = importDirectory(fullpath, 0, id);
		// SD TODO - allow import of remote file (not transfered as data as done here for the LaughterCycle demo)
		//	but rather using TCP client
		osstream << "addfile " << ret;
		sbuffer_send = osstream.str();
		*buffer_send = (char*)(sbuffer_send).c_str();
		*l_send = sbuffer_send.length();
	}
	// store features in file
	if (subbuffer == "savelibrary") {
		//return 0;
		bufpos1 = bufpos2+1;
		path = sbuffer.substr(bufpos1);
		fullpath = local_directory + path + ".acl";
		getLibrary()->saveAsLibrary(fullpath);
		ret = 1;
		osstream << "savelibrary " << ret;
		sbuffer_send = osstream.str();
		*buffer_send = (char*)(sbuffer_send).c_str();
		*l_send = sbuffer_send.length();
	}
	else if (subbuffer == "loadlibrary") {
		*buffer_send = 0;
		*l_send = 0;
	}
	else if (subbuffer == "getknn") {
		bufpos1 = bufpos2+1;
		path = sbuffer.substr(bufpos1);
		sscanf(path.c_str(), "%d %d", &id, &k);
		ret = getKNN(id, ids, k);
		osstream << "getknn " << ret;
		for (i=0;i<ret;i++) {
			osstream << " " << ids[i];
		}
		sbuffer_send = osstream.str();
		*buffer_send = (char*)(sbuffer_send).c_str();
		*l_send = sbuffer_send.length();
	}
	else if (subbuffer == "getthumbnail") {
		bufpos1 = bufpos2+1;
		path = sbuffer.substr(bufpos1);
		sscanf(path.c_str(), "%d", &id);
		thumbnail_filename = getThumbnail(id);
		if (thumbnail_filename!="") {
			stat(thumbnail_filename.c_str(), &file_status);
			thumbnail_size = file_status.st_size;
			*buffer_send = new char[13+thumbnail_size];
			strcpy(*buffer_send, "getthumbnail ");
			thumbnail_file = fopen(thumbnail_filename.c_str(),"r");
			size_t st = fread((*buffer_send)+13, 1, thumbnail_size, thumbnail_file);
			fclose(thumbnail_file);
			*l_send = 13+thumbnail_size;
		}
		else {
			*buffer_send = 0;
			*l_send = 0;
		}
	}
	else if (subbuffer == "setweight") {
		*buffer_send = 0;
		*l_send = 0;
	}

        return 0;
}

//AM TODO processTcpMessage & processTcpMessageFromSSI must be moved outside of MediaCycle main class
/*
 * data structure sent by SSI (AVLaughterCycle) :
 *
 *   <uint32> tot_size
 *   <uint32> type_size
 *   <char> x type_size --> type_name
 *
 *   if type_name == 'addwavf'
 *     <uint32> wav_size
 *     <byte> x wav_size --> content of wave file
 *   elif type_name == 'request'
 *     no wave file is sent
 *   fi
 *
 *   <uint32> burst_size
 *   <char> x burst_size --> sequence of burst labels
 *   <uint32> stat_size
 *   <float>  stat_size --> sequence of statistics
 */
int MediaCycle::processTcpMessageFromSSI(char* buffer, int l, char **buffer_send, int *l_send) {
    //AM : TODO rewrite Tcp in C++ to get something better than a char* buffer ? (and then rewrite code below)

    unsigned long pos = 0;
    cout << "Processing TCP message of length" <<  l  << endl;

    unsigned int tot_size = *reinterpret_cast<int*>(buffer+pos);
    pos += sizeof(int);
    cout << "Actual length : " << tot_size << endl;

    unsigned int type_size = *reinterpret_cast<int*>(buffer+pos);
    pos += sizeof(int);

    std::string type_name(buffer+pos,type_size);
    pos += type_size;

    if ( type_name == "addwavf" ) {
        cout << "SSI : add wave file" << endl;
        //extract wavefile
        unsigned int wav_size = *reinterpret_cast<int*>(buffer+pos);
        pos += sizeof(int);
        //AM : TODO extract wave file and write it on disk + generate a name for it (date + rand) ?
        pos += wav_size;
    } else if ( type_name == "request" ) {
        cout << "SSI : request" << endl;
        //nothing specific to do
    } else {
        //not a valid request
        return -1;
    }

    unsigned int burst_size = *reinterpret_cast<int*>(buffer+pos);
    pos += sizeof(int);

    std::string burst_labels(buffer+pos,burst_size);
    pos += burst_size;

    unsigned int stat_size = *reinterpret_cast<int*>(buffer+pos);
    pos += sizeof(int);

    float *ssi_features = reinterpret_cast<float*>(buffer+pos);

    return 0;
}

int MediaCycle::getKNN(int id, vector<int> &ids, int k) {
	int ret = this->mediaBrowser->getKNN(id, ids, k);
	return ret;
}

string MediaCycle::getThumbnail(int id) {
	return this->mediaLibrary->getThumbnail(id);
}
