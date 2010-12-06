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

#ifdef USE_OPENMP
#include <omp.h>
#endif

#include <fstream>
using std::ofstream;
using std::ifstream;

static double getTime() {
	
    struct timeval tv = {0, 0};
    struct timezone tz = {0, 0};
    
    gettimeofday(&tv, &tz);
    
    return (double)tv.tv_sec + tv.tv_usec / 1000000.0;
}

MediaCycle::MediaCycle(ACMediaType aMediaType, string local_directory, string libname) {
	
	this->mediaLibrary = 0;
	this->mediaBrowser = 0;
	this->mediaFactory = 0;
	
	this->forwarddown = 0;
	this->playkeydown = true;

	this->local_directory = local_directory;
	this->libname = libname;
	
	this->networkSocket	= 0;
	
	this->mediaLibrary = new ACMediaLibrary(aMediaType);
  
	this->mediaBrowser = new ACMediaBrowser();
	this->mediaBrowser->setLibrary(this->mediaLibrary);
	
	this->mediaFactory = new ACMediaFactory();
	
	this->pluginManager = new ACPluginManager();
	
	this->config_file = "";
	
	this->prevLibrarySize = 0;
	// Test Labels
/*	ACPoint p;
	p.x = -0.1; p.y = 0.0; p.z = 0.01;
	this->mediaBrowser->setLabel(0, "Label-1", p);
	p.x = 0.2; p.y = 0.0; p.z = 0.01;
	this->mediaBrowser->setLabel(1, "Label-2", p);
	p.x = 0.0; p.y = 0.1; p.z = 0.01;
	this->mediaBrowser->setLabel(2, "Label-3", p);
 */

	/*
	int nthreads, tid, NumberOfProcs;

	NumberOfProcs = omp_get_num_procs();
	printf("\nWorking on %d Processors",NumberOfProcs);
	
	omp_set_num_threads(NumberOfProcs);
	
#pragma omp parallel private(tid)
	{
		tid = omp_get_thread_num();
		printf("\nWoohoo from thread : %d", tid);
		if (tid==0){
			nthreads = omp_get_num_threads();
			printf("\nMaster says : There is %d out there!", nthreads);
		}
	}
	printf("\n");
	// No more threads

	*/
	
	/*
	double t1 = getTime();
	int s = 500000;
	float *x = new float[s];
	
	for (unsigned int i=0; i<s; i++) {
		
		x[i] = rand();
		x[i] = cos(sin(cos(sin(cos(sin(cos(sin(x[i]))))))));
		
	}

	double t2 = getTime();
	
	printf("TTT - %f\n",float(t2-t1));
	
	t1 = getTime();
	
#pragma omp parallel for
	for (unsigned int i=0; i<s; i++) {		
		x[i] = rand();
		x[i] = cos(sin(cos(sin(cos(sin(cos(sin(x[i]))))))));
	}
	
	t2 = getTime();

	printf("TTT - %f\n",float(t2-t1));
	 */
}

MediaCycle::MediaCycle(const MediaCycle& orig) {
	// XS TODO: complete this copy-constructor or remove it !
}

MediaCycle::~MediaCycle() {
	// XS added delete for variables whose new is in constructor
	delete this->mediaLibrary;
	delete this->mediaBrowser;
	delete this->mediaFactory;
	delete this->pluginManager;
    stopTcpServer(); // will delete this->networkSocket;
}

bool MediaCycle::changeMediaType(ACMediaType aMediaType)
{
	return this->mediaLibrary->changeMediaType(aMediaType);
}	

// == TCP
static void tcp_callback(char *buffer, int l, char **buffer_send, int *l_send, void *userData)
{
	MediaCycle *that = (MediaCycle*)userData;
	that->processTcpMessage(buffer, l, buffer_send, l_send);
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
		ret = importDirectory(fullpath, 0, id, true);
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
		getLibrary()->saveACLLibrary(fullpath);
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

// == Media Library

void *threadImport(void *import_thread_arg) {
	
	((MediaCycle*)import_thread_arg)->importDirectories();
}

int MediaCycle::importDirectoriesThreaded(vector<string> directories, int recursive, bool forward_order, bool doSegment) {
	
	import_directories = directories;
	import_recursive = recursive;
	import_forward_order = forward_order;
	import_doSegment = doSegment;
	
	pthread_attr_init(&import_thread_attr);
	import_thread_arg = (void*)this;
	pthread_create(&import_thread, &import_thread_attr, &threadImport, import_thread_arg);
	pthread_attr_destroy(&import_thread_attr);
	//pthread_cancel(import_thread_arg); // SD will destroy itseld when function returns
}

int MediaCycle::importDirectories() {
	
	return importDirectories(import_directories, import_recursive, import_forward_order, import_doSegment);
}

int MediaCycle::importDirectories(vector<string> directories, int recursive, bool forward_order, bool doSegment) {

	float prevLibrarySizeMultiplier = 2;
	int needsNormalizeAndCluster;
	vector<string> filenames;
	
	// SD TODO - need to check if files where not alrady scanned before
		
	mediaLibrary->scanDirectories(directories, recursive, filenames);
	
	int tid;
	int n;
	unsigned int i = 0;
	double t1, t2;
	
	#ifdef USE_OPENMP
	omp_set_num_threads(2);
	#endif
	t1 = getTime();
		
	n = filenames.size();
	
/*
#pragma omp parallel for
	for (i=0; i<10; i++) {
		int x = 1;
	}
*/	
	
	for (i=0;i<n;i++) {
		
		mediaLibrary->importFile(filenames[i], this->pluginManager, doSegment);
		
		needsNormalizeAndCluster = 0;
		if ( (mediaLibrary->getSize() >= int(prevLibrarySizeMultiplier * prevLibrarySize))
			|| (i==filenames.size()-1) ) {
			needsNormalizeAndCluster = 1;
			prevLibrarySize = mediaLibrary->getSize();
		}		
		
		//needsNormalizeAndCluster = 1;
		
		normalizeFeatures(needsNormalizeAndCluster); // actually just calls mediaLibrary->normalizeFeatures();
		
		libraryContentChanged(needsNormalizeAndCluster); // actually just calls mediaBrowser->libraryContentChanged();
		

	}
		
	t2 = getTime();
	
	printf("TTT - %f\n",float(t2-t1));
	
		
	//}
	
	filenames.empty();
	
	//[self updatedLibrary];
}

int MediaCycle::importDirectory(string path, int recursive, bool forward_order, bool doSegment) {
// XS import = import + some processing 
	cout << "MediaCycle: importing directory: " << path << endl;	
	int ok = 0;
	if (this->pluginManager == NULL){
		cout << "no analysis plugins were loaded. you will need to load a plugin to use the application." << endl;
	}
	ok = this->mediaLibrary->importDirectory(path, recursive, this->pluginManager, forward_order, doSegment);
	// XS normalizing automatically is a problem, for example if we load a bunch of files instead of a directory,
	//    it should not normalize after each file.
	// XS TODO ? if (ok>=1) this->mediaLibrary->normalizeFeatures();
	// XS TODO ? this->mediaBrowser->libraryContentChanged();	
	return ok;
}

int MediaCycle::importACLLibrary(string path) {
// XS import = open + some processing 
	cout << "MediaCycle: importing library: " << path << endl;
	int ok = 0;
	// Xs debug ACL
	ok = this->mediaLibrary->openACLLibrary(path);
	if (ok>=1) this->mediaLibrary->normalizeFeatures();
	//	XS TODO this->mediaBrowser->libraryContentChanged();	
	return ok;
	
}

//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
int MediaCycle::importMCSLLibrary(string path) {
	// XS import = open + some processing 
	cout << "MediaCycle: importing MCSL library: " << path << endl;
	int ok = 0;
	ok = this->mediaLibrary->openMCSLLibrary(path);
	// Finding segments for each media
	for (int i=0; i<this->mediaLibrary->getSize(); i++){
		if (this->mediaLibrary->getMedia(i)->getParentId() != -1){
			this->mediaLibrary->getMedia(this->mediaLibrary->getMedia(i)->getParentId())->addSegment(this->mediaLibrary->getMedia(i));
		}
	}
	if (ok>=1) this->mediaLibrary->normalizeFeatures();
	//	XS TODO this->mediaBrowser->libraryContentChanged();	
	return ok;
}

/* SD 2010 sep discontinued
int MediaCycle::importLibrary(string path) {
// XS import = open + some processing 
	cout << "MediaCycle: importing library: " << path << endl;
	int ok = 0;
	ok = this->mediaLibrary->openLibrary(path);
	if (ok>=1) this->mediaLibrary->normalizeFeatures();
	//	XS TODO this->mediaBrowser->libraryContentChanged();	
	return ok;
}
*/

int MediaCycle::getLibrarySize() { return mediaLibrary->getSize(); }
int MediaCycle::getNumberOfMediaNodes(){return mediaBrowser->getNumberOfMediaNodes();}



// == Search by Similarity

int MediaCycle::getKNN(int id, vector<int> &ids, int k) {
	int ret = this->mediaBrowser->getKNN(id, ids, k);
	return ret;
}
int MediaCycle::getKNN(ACMedia *aMedia, vector<ACMedia *> &result, int k) {
	int ret = this->mediaBrowser->getKNN(aMedia, result, k);
	return ret;
}

// Thumbnail

string MediaCycle::getThumbnailFileName(int id) {
	return this->mediaLibrary->getThumbnailFileName(id);
}

// Media Browser
void* MediaCycle::hasBrowser() { return mediaBrowser; }
ACBrowserMode MediaCycle::getMode() {return mediaBrowser->getMode();}
void MediaCycle::setMode(ACBrowserMode _mode) {mediaBrowser->setMode(_mode);}

// Plugins
int MediaCycle::addPlugin(string aPluginPath) {
    return this->pluginManager->add(aPluginPath);
}

void MediaCycle::setClustersMethodPlugin(string pluginName){
	ACPlugin* clustersPlugin = this->getPluginManager()->getPlugin(pluginName);
	this->getBrowser()->setClustersMethodPlugin(clustersPlugin);
}

void MediaCycle::setNeighborsMethodPlugin(string pluginName){
	ACPlugin* neighborsPlugin = this->getPluginManager()->getPlugin(pluginName);
	this->getBrowser()->setNeighborsMethodPlugin(neighborsPlugin);
}

void MediaCycle::setClustersPositionsPlugin(string pluginName){
	ACPlugin* clustersPlugin = this->getPluginManager()->getPlugin(pluginName);
	this->getBrowser()->setClustersPositionsPlugin(clustersPlugin);
}

void MediaCycle::setNeighborsPositionsPlugin(string pluginName){
	ACPlugin* neighborsPlugin = this->getPluginManager()->getPlugin(pluginName);
	this->getBrowser()->setNeighborsPositionsPlugin(neighborsPlugin);
}

void MediaCycle::setVisualisationPlugin(string pluginName){
	ACPlugin* visPlugin = this->getPluginManager()->getPlugin(pluginName);
	this->getBrowser()->setVisualisationPlugin(visPlugin);
}

void MediaCycle::changeClustersMethodPlugin(string pluginName){
	ACPlugin* clustersPlugin = this->getPluginManager()->getPlugin(pluginName);
	this->getBrowser()->changeClustersMethodPlugin(clustersPlugin);
}

void MediaCycle::changeNeighborsMethodPlugin(string pluginName){
	ACPlugin* neighborsPlugin = this->getPluginManager()->getPlugin(pluginName);
	this->getBrowser()->changeNeighborsMethodPlugin(neighborsPlugin);
}

void MediaCycle::changeClustersPositionsPlugin(string pluginName){
	ACPlugin* clustersPlugin = this->getPluginManager()->getPlugin(pluginName);
	this->getBrowser()->changeClustersPositionsPlugin(clustersPlugin);
}

void MediaCycle::changeNeighborsPositionsPlugin(string pluginName){
	ACPlugin* neighborsPlugin = this->getPluginManager()->getPlugin(pluginName);
	this->getBrowser()->changeNeighborsPositionsPlugin(neighborsPlugin);
}
/*
void MediaCycle::changeVisualisationPlugin(string pluginName){
	ACPlugin* visPlugin = this->getPluginManager()->getPlugin(pluginName);
	this->getBrowser()->changeVisualisationPlugin(visPlugin);
}
*/

void MediaCycle::dumpPluginsList(){this->pluginManager->dump();}

// == Media
ACMediaNode& MediaCycle::getMediaNode(int i) { return (mediaBrowser->getMediaNode(i)); } 
string MediaCycle::getMediaFileName(int i) { return mediaLibrary->getMedia(i)->getFileName(); }
int MediaCycle::getMediaType(int i) { return mediaLibrary->getMedia(i)->getType(); }
int MediaCycle::getThumbnailWidth(int i) { return mediaLibrary->getMedia(i)->getThumbnailWidth(); }
int MediaCycle::getThumbnailHeight(int i) { return mediaLibrary->getMedia(i)->getThumbnailHeight(); }
int MediaCycle::getWidth(int i) { return mediaLibrary->getMedia(i)->getWidth(); }
int MediaCycle::getHeight(int i) { return mediaLibrary->getMedia(i)->getHeight(); }
void* MediaCycle::getThumbnailPtr(int i) { return mediaLibrary->getMedia(i)->getThumbnailPtr(); }
int MediaCycle::getNeedsDisplay() {	return mediaBrowser->getNeedsDisplay(); }
void MediaCycle::setNeedsDisplay(bool _dis) {
	if (mediaBrowser) {
		mediaBrowser->setNeedsDisplay(_dis);
	}
}
int MediaCycle::getNeedsDisplay3D() { return mNeedsDisplay; }
void MediaCycle::setNeedsDisplay3D(bool mNeedsDisplay) { this->mNeedsDisplay = mNeedsDisplay; }

// == view
void MediaCycle::getMouse(float *mx, float *my) { mediaBrowser->getMouse(mx, my); }
float MediaCycle::getCameraZoom() { return mediaBrowser->getCameraZoom(); }
float MediaCycle::getCameraRotation() { return mediaBrowser->getCameraRotation(); }
void MediaCycle::setCameraRotation(float angle) { mediaBrowser->setCameraRotation(angle); }
void MediaCycle::setCameraPosition(float x, float y)		{ mediaBrowser->setCameraPosition(x,y); }
void MediaCycle::getCameraPosition(float &x, float &y)		{ mediaBrowser->getCameraPosition(x,y); }
void MediaCycle::setCameraZoom(float z)				{ mediaBrowser->setCameraZoom(z); }
void MediaCycle::setCameraRecenter()				{ mediaBrowser->setCameraRecenter(); }
void MediaCycle::setAutoPlay(int i) { mediaBrowser->setAutoPlay(i); }
int MediaCycle::getClickedNode() { return mediaBrowser->getClickedNode(); }
void MediaCycle::setClickedNode(int i) { mediaBrowser->setClickedNode(i); }
void MediaCycle::setClosestNode(int i) { mediaBrowser->setClosestNode(i); }
int MediaCycle::getClosestNode() { return mediaBrowser->getClosestNode(); }
int	MediaCycle::getLastSelectedNode() { return mediaBrowser->getLastSelectedNode(); }

// == Cluster Display
void MediaCycle::updateState() { mediaBrowser->updateState(); }
int MediaCycle::getNavigationLevel() { return mediaBrowser->getNavigationLevel(); }
float MediaCycle::getFrac() { return mediaBrowser->getFrac(); }
void MediaCycle::incrementLoopNavigationLevels(int i) { mediaBrowser->incrementLoopNavigationLevels(i); }
void MediaCycle::setReferenceNode(int index) { mediaBrowser->setReferenceNode(index); }
int MediaCycle::getReferenceNode() { return mediaBrowser->getReferenceNode(); }
void MediaCycle::goBack() { mediaBrowser->goBack(); }
void MediaCycle::goForward() { mediaBrowser->goForward(); }
void MediaCycle::storeNavigationState() { mediaBrowser->storeNavigationState(); }
void MediaCycle::setClusterNumber(int n) { mediaBrowser->setClusterNumber(n); }
void MediaCycle::setWeight(int i, float weight) { mediaBrowser->setWeight(i, weight); }
vector<float> MediaCycle::getWeightVector(){return mediaBrowser->getWeightVector();}
float MediaCycle::getWeight(int i){return mediaBrowser->getWeight(i);}

void MediaCycle::setForwardDown(int i) { forwarddown = i; }

// == Features
void MediaCycle::normalizeFeatures(int needsNormalize) { mediaLibrary->normalizeFeatures(needsNormalize); }
// void MediaCycle::openLibrary(string path) { mediaLibrary->openLibrary(path); }		// discontinued SD 2010 sep
void MediaCycle::libraryContentChanged(int needsNormalizeAndCluster) { mediaBrowser->libraryContentChanged(needsNormalizeAndCluster); }
//void MediaCycle::saveAsLibrary(string path) {mediaLibrary->saveAsLibrary(path); }
void MediaCycle::saveACLLibrary(string path) {mediaLibrary->saveACLLibrary(path); }
void MediaCycle::saveMCSLLibrary(string path) {mediaLibrary->saveMCSLLibrary(path); }
void MediaCycle::cleanLibrary() { prevLibrarySize=0; mediaLibrary->cleanLibrary(); }
// Get Features Vector (identified by feature_name) in media i 
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

// == Playing time stamp
int MediaCycle::setSourceCursor(int lid, int frame_pos) {
	return mediaBrowser->setSourceCursor(lid, frame_pos);
}
int MediaCycle::setCurrentFrame(int lid, int frame_pos) {
	return mediaBrowser->setCurrentFrame(lid, frame_pos);	
}	
void MediaCycle::muteAllSources() { mediaBrowser->muteAllSources(); }

// == POINTERS on VIEW
int MediaCycle::getPointerSize() {
	return mediaBrowser->getPointerSize();
}

ACPointer& MediaCycle::getPointer(int i) {
	return mediaBrowser->getPointer(i);
} 

// == LABELS on VIEW
int MediaCycle::getLabelSize() { return mediaBrowser->getLabelSize(); }
string MediaCycle::getLabelText(int i) { return mediaBrowser->getLabelText(i); }
ACPoint MediaCycle::getLabelPos(int i) { return mediaBrowser->getLabelPos(i); }

// == Update audio engine sources
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
// == callbacks

void MediaCycle::pickedObjectCallback(int _nodeId) {
	printf("PICKED\n");
	int nodeId = _nodeId;
	if(nodeId < 0) {
		// clicked close to a node
		nodeId = getClosestNode();
	}
	mediaBrowser->setClickedNode(nodeId);
	if (forwarddown == 0){// & playkeydown) {//if (!forwarddown) { //CF forwardown is not a boolean
		mediaBrowser->toggleSourceActivity(nodeId);
	}
}

void MediaCycle::hoverObjectCallback(int pid) {
	// XS duh ?
}

void MediaCycle::hoverCallback(float xx, float yy) {
	mediaBrowser->setHoverLoop(-1, xx, yy);		
}


// == NEW
void MediaCycle::updateDisplay(bool _animate) { mediaBrowser->updateDisplay(_animate);}

void MediaCycle::readConfigFile(string _fname) {
	ifstream library_file;
	library_file.open(_fname.c_str());
	if (!library_file.good()){
		cerr << "<MediaCycle::readConfigFile> : bad config file" << endl;
	}
	
	int aMediaType;
	library_file >> aMediaType; // ex: MEDIA_TYPE_IMAGE  // type of media
	
	cout << aMediaType << endl;
	
	int aBrowserMode;
	library_file >> aBrowserMode; // ex: AC_MODE_CLUSTERS // navigation mode
	cout << aBrowserMode << endl;

	string features_plugin_directory; 
	library_file >> features_plugin_directory;  //ex:  /Users/xavier/development/Fall09/ticore-app/Applications/Numediart/MediaCycle/src/Builds/darwin-xcode/plugins/image/Debug/mc_image.dylib   // directory with plugins
	cout << features_plugin_directory << endl;

	int n_active_features; // ex:	3 // number of sliders
	library_file >> n_active_features;
	cout << n_active_features << endl;

	vector<string> v_feat;
	for (int i=0; i<n_active_features; i++) {
		string tmp;
		library_file >> tmp; //ex: ACImageColorMomentsPlugin // plugins for 1st sliders
		v_feat.push_back(tmp);
		cout << tmp << endl;
	}
}

// == user log

void MediaCycle::cleanUserLog() { 
  mediaBrowser->getUserLog()->clean(); 
}


// == Dump / used for Debug 
void MediaCycle::dumpNavigationLevel(){
	cout << "Global Navigation Level = " <<  getNavigationLevel() << endl;
}
									
void MediaCycle::dumpLoopNavigationLevels(){
	int nl = this->getNumberOfMediaNodes();
	cout << "loops nav lev for :" << endl;
	for (int i=0; i < nl; i++){
		cout << this->getBrowser()->getMediaNode(i).getNavigationLevel() << " ";
	}
	cout << endl;
}

