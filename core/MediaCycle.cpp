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
	
	this->mediaLibrary = 0;
	this->mediaBrowser = 0;
	
	this->forwarddown = 0;

    this->local_directory = local_directory;
    this->libname = libname;
    this->networkSocket	= 0;


    this->mediaLibrary = new ACMediaLibrary(aMediaType);
    //this->mediaLibrary->setMediaType(aMediaType);
    
    this->mediaBrowser = new ACMediaBrowser();
    this->mediaBrowser->setLibrary(this->mediaLibrary);

    this->pluginManager = new ACPluginManager();
	
	// Test Labels
/*	ACPoint p;
	p.x = -0.1; p.y = 0.0; p.z = 0.01;
	this->mediaBrowser->setLabel(0, "Label-1", p);
	p.x = 0.2; p.y = 0.0; p.z = 0.01;
	this->mediaBrowser->setLabel(1, "Label-2", p);
	p.x = 0.0; p.y = 0.1; p.z = 0.01;
	this->mediaBrowser->setLabel(2, "Label-3", p);
 */
}

MediaCycle::MediaCycle(const MediaCycle& orig) {
	// XS TODO: complete this copy-constructor or remove it !
}

MediaCycle::~MediaCycle() {
	// XS added delete for variables whose new is in constructor
	delete this->mediaLibrary;
	delete this->mediaBrowser;
	delete this->pluginManager;
    stopTcpServer(); // will delete this->networkSocket;
}

int MediaCycle::startTcpServer(int aPort, int aMaxConnections) {
    this->port = aPort;
    this->max_connections = aMaxConnections;
    if ((this->port>=FIRST_PORT_ID)&&(this->port<=LAST_PORT_ID)) {
            this->networkSocket = new ACNetworkSocketServer(this->port, this->max_connections, tcp_callback, this);
            this->networkSocket->start();
            return 0;
    }

    return -1;
}

int MediaCycle::startTcpServer(int aPort, int aMaxConnections, ACNetworkSocketServerCallback aCallback) {
    this->port = aPort;
    this->max_connections = aMaxConnections;
    if ((this->port>=FIRST_PORT_ID)&&(this->port<=LAST_PORT_ID)) {
            this->networkSocket = new ACNetworkSocketServer(this->port, this->max_connections, aCallback, this);
            this->networkSocket->start();
            return 0;
    }

    return -1;
}

int MediaCycle::stopTcpServer() {
     if (this->networkSocket) {
        this->networkSocket->stop();
        delete this->networkSocket;
    }
}

int MediaCycle::importDirectory(string path, int recursive, int mid) {
	cout << "importing directory: " << path << endl;
	int ret = this->mediaLibrary->importDirectory(path, recursive, mid, this->pluginManager);
	this->mediaLibrary->normalizeFeatures();
	this->mediaBrowser->libraryContentChanged();
	return ret;
}

int MediaCycle::importACLLibrary(string path) {
	int ret = this->mediaLibrary->openACLLibrary(path);
	this->mediaLibrary->normalizeFeatures();
	this->mediaBrowser->libraryContentChanged();
	return ret;
}

int MediaCycle::importLibrary(string path) {
	cout << "importing library: " << path << endl;
	int ret = this->mediaLibrary->openLibrary(path);
	this->mediaLibrary->normalizeFeatures();
	this->mediaBrowser->libraryContentChanged();
	return ret;
}

static void tcp_callback(char *buffer, int l, char **buffer_send, int *l_send, void *userData)
{
	MediaCycle *that = (MediaCycle*)userData;
	that->processTcpMessage(buffer, l, buffer_send, l_send);
}

//AM TODO processTcpMessage must be moved outside of MediaCycle main class
int MediaCycle::processTcpMessage(char* buffer, int l, char **buffer_send, int *l_send)
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
		thumbnail_filename = getThumbnailFileName(id);
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

int MediaCycle::getKNN(int id, vector<int> &ids, int k) {
	int ret = this->mediaBrowser->getKNN(id, ids, k);
	return ret;
}
int MediaCycle::getKNN(ACMedia *aMedia, vector<ACMedia *> &result, int k) {
	int ret = this->mediaBrowser->getKNN(aMedia, result, k);
	return ret;
}

string MediaCycle::getThumbnailFileName(int id) {
	return this->mediaLibrary->getThumbnailFileName(id);
}

int MediaCycle::addPlugin(string aPluginPath) {
    return this->pluginManager->add(aPluginPath);
}

void MediaCycle::setVisualisationPlugin(string pluginName){
  ACPlugin* visPlugin = this->getPluginManager()->getPlugin(pluginName);
  this->getBrowser()->setVisualisationPlugin(visPlugin);
}

void MediaCycle::setPositionsPlugin(string pluginName){
	ACPlugin* posPlugin = this->getPluginManager()->getPlugin(pluginName);
	this->getBrowser()->setPositionsPlugin(posPlugin);
}

void MediaCycle::setNeighborhoodsPlugin(string pluginName){
	ACPlugin* neighborsPlugin = this->getPluginManager()->getPlugin(pluginName);
	this->getBrowser()->setNeighborhoodsPlugin(neighborsPlugin);
}

// ADDED FOR VISUAL and GUI
// API REQUIRED BY VISUAL and GUI
// 
int MediaCycle::getLibrarySize() { return mediaLibrary->getSize(); }
string MediaCycle::getMediaFileName(int i) { return mediaLibrary->getMedia(i)->getFileName(); }
int MediaCycle::getMediaType(int i) { return mediaLibrary->getMedia(i)->getType(); }
int MediaCycle::getThumbnailWidth(int i) { return mediaLibrary->getMedia(i)->getThumbnailWidth(); }
int MediaCycle::getThumbnailHeight(int i) { return mediaLibrary->getMedia(i)->getThumbnailHeight(); }
int MediaCycle::getWidth(int i) { return mediaLibrary->getMedia(i)->getWidth(); }
int MediaCycle::getHeight(int i) { return mediaLibrary->getMedia(i)->getHeight(); }
void* MediaCycle::getThumbnailPtr(int i) { return mediaLibrary->getMedia(i)->getThumbnailPtr(); }
int MediaCycle::getNeedsDisplay() {	return mediaBrowser->getNeedsDisplay(); }
void MediaCycle::setNeedsDisplay(int i) {
	if (mediaBrowser) {
		mediaBrowser->setNeedsDisplay(i);
	}
}
float MediaCycle::getCameraZoom() { return mediaBrowser->getCameraZoom(); }
float MediaCycle::getCameraRotation() { return mediaBrowser->getCameraRotation(); }
const ACLoopAttribute& MediaCycle::getLoopAttributes(int i) { return (mediaBrowser->getLoopAttributes()[i]); } 
int MediaCycle::getNavigationLevel() { return mediaBrowser->getNavigationLevel(); }
void MediaCycle::getMouse(float *mx, float *my) { mediaBrowser->getMouse(mx, my); }
// 
void MediaCycle::updateState() { mediaBrowser->updateState(); }
float MediaCycle::getFrac() { return mediaBrowser->getFrac(); }
void MediaCycle::setCameraRotation(float angle) { mediaBrowser->setCameraRotation(angle); }
int MediaCycle::getClickedLoop() { return mediaBrowser->getClickedLoop(); }
void MediaCycle::incrementLoopNavigationLevels(int i) { mediaBrowser->incrementLoopNavigationLevels(i); }
void MediaCycle::setSelectedObject(int index) { mediaBrowser->setSelectedObject(index); }
void MediaCycle::updateClusters(bool animate) { mediaBrowser->updateClusters(animate); }
void MediaCycle::updateNeighborhoods() { mediaBrowser->updateNeighborhoods(); }
void MediaCycle::setCameraPosition(float x, float y)		{ mediaBrowser->setCameraPosition(x,y); }
void MediaCycle::getCameraPosition(float &x, float &y)		{ mediaBrowser->getCameraPosition(x,y); }
void MediaCycle::setCameraZoom(float z)				{ mediaBrowser->setCameraZoom(z); }
void MediaCycle::setCameraRecenter()				{ mediaBrowser->setCameraRecenter(); }
// 
void MediaCycle::normalizeFeatures() { mediaLibrary->normalizeFeatures(); }
void MediaCycle::openLibrary(string path) { mediaLibrary->openLibrary(path); }
void MediaCycle::libraryContentChanged() { mediaBrowser->libraryContentChanged(); }
void MediaCycle::saveAsLibrary(string path) {mediaLibrary->saveAsLibrary(path); }
void MediaCycle::saveACLLibrary(string path) {mediaLibrary->saveACLLibrary(path); }
void MediaCycle::cleanLibrary() { mediaLibrary->cleanLibrary(); }
void MediaCycle::setBack() { mediaBrowser->setBack(); }
void MediaCycle::setForward() { mediaBrowser->setForward(); }
void MediaCycle::setClusterNumber(int n) { mediaBrowser->setClusterNumber(n); }
void MediaCycle::setWeight(int i, float weight) { mediaBrowser->setWeight(i, weight); }
void MediaCycle::setForwardDown(int i) { forwarddown = i; }
// 
void MediaCycle::setAutoPlay(int i) { mediaBrowser->setAutoPlay(i); }
void MediaCycle::setClickedLoop(int i) { mediaBrowser->setClickedLoop(i); }
void MediaCycle::setClosestLoop(int i) { mediaBrowser->setClosestLoop(i); }
int MediaCycle::getClosestLoop() { return mediaBrowser->getClosestLoop(); }
// 
void MediaCycle::muteAllSources() { mediaBrowser->muteAllSources(); }
void* MediaCycle::hasBrowser() { return mediaBrowser; }

// LABELS on VIEW
int MediaCycle::getLabelSize() { return mediaBrowser->getLabelSize(); }
string MediaCycle::getLabelText(int i) { return mediaBrowser->getLabelText(i); }
ACPoint MediaCycle::getLabelPos(int i) { return mediaBrowser->getLabelPos(i); }

// Get Features
vector<float> MediaCycle::getFeaturesVectorInMedia(int i, string feature_name) {
	ACMedia* lmedia;
	ACMediaFeatures* lfeatures;
	FeaturesVector lfeaturesvector;
	lmedia = mediaLibrary->getMedia(i);
	lfeatures = lmedia->getFeaturesVector(feature_name);
	if (lfeatures) {
		lfeaturesvector = *(lfeatures->getFeaturesVector());
	}
	return (vector<float>)lfeaturesvector;
}

// Playing time stamp
int MediaCycle::setSourceCurser(int lid, int frame_pos) {
	return mediaBrowser->setSourceCurser(lid, frame_pos);
}

// Update audio engine sources
void MediaCycle::setNeedsActivityUpdateLock(int i) {
	if (mediaBrowser) {
		mediaBrowser->setNeedsActivityUpdateLock(i);
	} 
}

void MediaCycle::setNeedsActivityUpdateRemoveMedia() {
	if (mediaBrowser) {
		mediaBrowser->setNeedsActivityUpdateRemoveMedia();
	}
}

vector<int>* MediaCycle::getNeedsActivityUpdateMedia() {
	if (mediaBrowser) {
		return mediaBrowser->getNeedsActivityUpdateMedia();
	}
}

void MediaCycle::pickedObjectCallback(int _pid) {
	int pid;
	pid = _pid;
	if(pid < 0) {
		pid = getClosestLoop();
	}
	mediaBrowser->setClickedLoop(pid);
	if (forwarddown == 0) {//if (!forwarddown) { //CF forwardown is not a boolean
		mediaBrowser->toggleSourceActivity(pid);
	}
}

void MediaCycle::hoverObjectCallback(int pid) {
}

void MediaCycle::hoverCallback(float x, float y) {
	mediaBrowser->setHoverLoop(-1, x, y);		
}
