/**
 * @brief Process to extract video features through MediaCycle commandline interfaces (CLI)
 * @author Christian Frisson
 * @date 17/12/2012
 * @copyright (c) 2012 – UMONS - Numediart
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

#include "ACServerProcess.h"
#include<iostream>

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

ACServerProcess::ACServerProcess(MediaCycle* _media_cycle)
    : ACAbstractProcessCLI(_media_cycle)
{
}

ACServerProcess::~ACServerProcess(){
}


#include "MediaCycle.h"

#include <stdlib.h>
#include <string>
#include <cstring>
#include <iostream>
#include <signal.h>
#include "ACAudio.h"
#include "sndfile.h"

//sleep()
#include <iostream>
#ifdef __MINGW32__
#include <windows.h>
#define sleep(n) Sleep(1000 * n)
#endif

using namespace std;

void mediacycle_tcp_callback(char *buffer, int l, char **buffer_send, int *l_send, void *userData);

int processTcpMessageFromClient(MediaCycle *that, char *buffer, int l, char **buffer_send, int *l_send); 

void mediacycle_tcp_callback(char *buffer, int l, char **buffer_send, int *l_send, void *userData) {
	MediaCycle *that = (MediaCycle*)userData;
	processTcpMessageFromClient(that, buffer, l, buffer_send, l_send);
}

int processTcpMessageFromClient(MediaCycle *that, char* buffer, int l, char **buffer_send, int *l_send)
{
	FILE *local_file;
	int ret, i;
	string sbuffer, subbuffer, path, fullpath, sbuffer_send;
	int bufpos1, bufpos2;
	int id, lid, k;
	vector<int> ids;
		
	sbuffer = string(buffer, l);

	printf ("Processing TCP message of length %d: %s\n", l, sbuffer.c_str());
	
	bufpos1 = 0;
	bufpos2 = sbuffer.find(" ");
	subbuffer = sbuffer.substr(bufpos1, bufpos2-bufpos1);
	ostringstream osstream;
	
	string thumbnail_filename;
	FILE *thumbnail_file;
	struct stat file_status;
	int thumbnail_size;
	
	// analyse and add a nex file (audio / image) to the library
	if (subbuffer == "addfile") {
		bufpos1 = bufpos2+1;
		bufpos2 = sbuffer.find(" ",bufpos1);
		subbuffer = sbuffer.substr(bufpos1, bufpos2-bufpos1);
		sscanf(subbuffer.c_str(), "%d", &id);
		bufpos1 = bufpos2+1;
		bufpos2 = sbuffer.find(" ",bufpos1);
		path = sbuffer.substr(bufpos1, bufpos2-bufpos1);
		bufpos1 = bufpos2+1;
		fullpath = path;
		//uncomment to receive the files through tcp (unnecessary if the php website and mediacycle are running on the same host)
		/*local_file = fopen(fullpath.c_str(),"wb");
		fwrite((void*)(buffer+bufpos1), 1, l-bufpos1, local_file);
		fclose(local_file);*/
		ret = that->importDirectory(fullpath, 0);
		osstream << "addfile " << ret;
		sbuffer_send = osstream.str();
		*buffer_send = (char*)(sbuffer_send).c_str();
		*l_send = sbuffer_send.length();
	}
	// store library in file
	if (subbuffer == "savelibrary") {
		bufpos1 = bufpos2+1;
		path = sbuffer.substr(bufpos1);
		fullpath = path;
		that->saveXMLLibrary(fullpath);
		ret = 1;
		osstream << "savelibrary " << ret;
		sbuffer_send = osstream.str();
		*buffer_send = (char*)(sbuffer_send).c_str();
		*l_send = sbuffer_send.length();
	}
	// load library from file
	else if (subbuffer == "loadlibrary") {
		bufpos1 = bufpos2+1;
		path = sbuffer.substr(bufpos1);
		fullpath = path;
		that->cleanLibrary();
		that->importXMLLibrary(fullpath);
                that->initializeFeatureWeights();
		ret = 1;
		osstream << "loadlibrary " << ret;
		sbuffer_send = osstream.str();
		*buffer_send = (char*)(sbuffer_send).c_str();
		*l_send = sbuffer_send.length();		
	}
	// get k most similar items
	else if (subbuffer == "getknn") {
		bufpos1 = bufpos2+1;
		path = sbuffer.substr(bufpos1);
		sscanf(path.c_str(), "%d %d", &id, &k);
		id--;
		ret = that->getKNN(id, ids, k);
		osstream << "getknn " << ret;
		for (i=0;i<ret;i++) {
			lid = ids[i];
			lid++;
			osstream << " " << lid;
		}
		sbuffer_send = osstream.str();
		*buffer_send = (char*)(sbuffer_send).c_str();
		*l_send = sbuffer_send.length();
	}
	// get thumbnail for display - to be checked
	else if (subbuffer == "getthumbnail") {
		bufpos1 = bufpos2+1;
		path = sbuffer.substr(bufpos1);
		sscanf(path.c_str(), "%d", &id);
		id--;
		thumbnail_filename = that->getThumbnailFileName(id);
		if (thumbnail_filename!="") {
			stat(thumbnail_filename.c_str(), &file_status);
			thumbnail_size = file_status.st_size;
			*buffer_send = new char[13+thumbnail_size]; 
			strcpy(*buffer_send, "getthumbnail ");
			thumbnail_file = fopen(thumbnail_filename.c_str(),"r");
			//size_t st = fread((*buffer_send)+13, 1, thumbnail_size, thumbnail_file);
			fclose(thumbnail_file);
			*l_send = 13+thumbnail_size;
		}
		else {
			*buffer_send = 0;
			*l_send = 0;
		}
	}
	// set number of cluster of media
	else if (subbuffer == "setclusternumber") {
		bufpos1 = bufpos2+1;
		path = sbuffer.substr(bufpos1);
		sscanf(path.c_str(), "%d", &k);
		that->setClusterNumber(k);
		osstream << "setclusternumber " << ret;
		sbuffer_send = osstream.str();
		*buffer_send = (char*)(sbuffer_send).c_str();
		*l_send = sbuffer_send.length();
	}
        // modify feature weights - to be implemented
	else if (subbuffer == "setweight") {
		bufpos1 = bufpos2+1;
		path = sbuffer.substr(bufpos1);
                that->setWeight(path);
	}
	
	
	// TO BE DONE
	/*
	// get media cluster centers (f.i. 1 representative image from each cluster)
	else if (subbuffer == "getclustercentroids") {
		*buffer_send = 0;
		*l_send = 0;
	}
	// dig deeper into one cluster (f.i. when user select a cluster and want to see more details of it)
	else if (subbuffer == "navigateforward") {
		that->goForward();
		osstream << "navigateforward " << ret;
		sbuffer_send = osstream.str();
		*buffer_send = (char*)(sbuffer_send).c_str();
		*l_send = sbuffer_send.length();
	}
	// move back to previous level of hierarchy
	else if (subbuffer == "navigateback") {
		that->goBack();
		osstream << "navigateback " << ret;
		sbuffer_send = osstream.str();
		*buffer_send = (char*)(sbuffer_send).c_str();
		*l_send = sbuffer_send.length();
	}	
	*/
	return 0;
}
void ACServerProcess::run(int argc, char *argv[]){
    
	MediaCycle *media_cycle;
 
	ACMediaType _media_type = MEDIA_TYPE_AUDIO;
	
	media_cycle = new MediaCycle(_media_type,"/tmp/","mediacycle.acl");
	
    // -- media-specific features plugin + generic segmentation and visualisation plugins--
 	string smedia = "none";
	switch (_media_type) {
		case MEDIA_TYPE_3DMODEL:
			smedia="3Dmodel";
            //media_cycle->loadPluginLibraryFromBasename(smedia);
			break;	
		case MEDIA_TYPE_AUDIO:
			smedia="audio";
            //media_cycle->loadPluginLibraryFromBasename("audio-features-yaafe");
            /*media_cycle->loadPluginLibraryFromBasename("audio-features-armadillo");
            media_cycle->loadPluginLibraryFromBasename("audio-reader-sndfile");
            media_cycle->loadPluginLibraryFromBasename("laughter-intensity-armadillo");*/
            //media_cycle->loadPluginLibraryFromBasename("audio-segmentation");
			break;
		case MEDIA_TYPE_IMAGE:
			smedia="image";
            //media_cycle->loadPluginLibraryFromBasename(smedia);
			break;
		case MEDIA_TYPE_VIDEO:
			smedia="video";
            //media_cycle->loadPluginLibraryFromBasename(smedia);
            //media_cycle->loadPluginLibraryFromBasename("video-segmentation");
			break;
		default:
			break;
	}
        
        media_cycle->importXMLLibrary("/media/Data/Data/Ilhaire/databases/Belfast_Storytelling/mediacycle_features4.xml");
        media_cycle->initializeFeatureWeights();
        //media_cycle->loadPluginLibraryFromBasename("visualisation");
    //media_cycle->loadPluginLibraryFromBasename("segmentation");
	
	// SD - check if needed
	// mediacyclesetClusterNumber(10);

	media_cycle->startTcpServer(12345, 5, mediacycle_tcp_callback);
	
    cout.flush();
  
    while(1) {
        sleep(30);
    }

    delete media_cycle;
	
    return;// (EXIT_SUCCESS);
}
