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

#if defined (SUPPORT_MULTIMEDIA)
#include<ACMediaDocument.h>
#endif

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
//#include "boost/filesystem/operations.hpp"
//#include "boost/filesystem/path.hpp"
namespace fs = boost::filesystem;

#include <fstream>
using std::ofstream;
using std::ifstream;

MediaCycle::MediaCycle(ACMediaType aMediaType, string local_directory, string libname) : Subject() {

    this->forwarddown = 0;
    //	this->playkeydown = true;

    this->local_directory = local_directory;
    this->libname = libname;
    this->networkSocket	= 0;

    ACMediaFactory::getInstance(); // this populates the available file extensions if not called before

    this->mediaLibrary = new ACMediaLibrary(aMediaType);

    this->mediaBrowser = new ACMediaBrowser();
    this->mediaBrowser->setLibrary(this->mediaLibrary);

    this->pluginManager = new ACPluginManager();
    this->pluginManager->setMediaCycle(this);
    this->mediaBrowser->changeClustersMethodPlugin( this->pluginManager->getPlugin("ACKMeansPlugin") );
    this->mediaBrowser->changeClustersMethodPlugin( this->pluginManager->getPlugin("ACClusterPositionsPropellerPlugin") );

    this->config_file_xml = "";

    this->prevLibrarySize = 0;
    this->eventManager=new ACEventManager;

    // when importing files:
    // 1) the library creates a media
    // 2) the browser creates a node
    // 3) mediacycle propagates the event to external listeners
}

MediaCycle::MediaCycle(const MediaCycle& orig) {
    // XS TODO: complete this copy-constructor or remove it !
}

MediaCycle::~MediaCycle() {
    delete this->mediaLibrary;
    this->mediaLibrary = 0;
    delete this->mediaBrowser;
    this->mediaBrowser = 0;
    delete this->pluginManager;
    this->pluginManager = 0;
    stopTcpServer(); // will delete this->networkSocket;
}

void MediaCycle::clean(){
    this->prevLibrarySize = 0;
    this->forwarddown = 0;
    this->local_directory = "";
    this->libname = "";
    this->config_file_xml = "";

    this->prevLibrarySize = 0;
    this->mNeedsDisplay = false;

    this->import_recursive = 0;
    this->import_forward_order = false;
    this->import_doSegment = false;

    this->port = 0;
    this->max_connections = 0;

    this->mediaLibrary->cleanLibrary();
    this->mediaBrowser->clean();
    this->pluginManager->clean();
    this->pluginManager->setMediaCycle(this);
    this->mediaBrowser->changeClustersMethodPlugin( this->pluginManager->getPlugin("ACKMeansPlugin") );
    this->mediaBrowser->changeClustersMethodPlugin( this->pluginManager->getPlugin("ACClusterPositionsPropellerPlugin") );
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

// XS TODO return value does not make much sense
int MediaCycle::stopTcpServer() {
    if (this->networkSocket) {
        this->networkSocket->stop();
        delete this->networkSocket;
    }
    return 1;
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
            //size_t st = fread((*buffer_send)+13, 1, thumbnail_size, thumbnail_file);
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

// == Callback - SD to be replaced by OSC/UDP communication
// (XS or Observer Pattern...)

void *threadImport(void *import_thread_arg) {
    ((MediaCycle*)import_thread_arg)->importDirectories();
}


// == Media Library

// XS TODO return value does not make much sense, should add some test
// XS TODO this does not seem compatible with Qt GUI
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
    return 1;
}

// to be called from the threaded version
int MediaCycle::importDirectories() {
    return importDirectories(import_directories, import_recursive, import_forward_order, import_doSegment);
}

// scans directories, fills the filenames vector and calls importFile
// then normalize the features and updates the library ("libraryContentChanged")
// each time the library grows by a factor prevLibrarySizeMultiplier, re-normalize and re-cluster everything
int MediaCycle::importDirectories(vector<string> directories, int recursive, bool forward_order, bool doSegment) {
    int ok = 0;
    float prevLibrarySizeMultiplier = 2;
    int needsNormalizeAndCluster = 0;
    vector<string> filenames;

    mediaLibrary->scanDirectories(directories, recursive, filenames);

    int n;
    int i = 0;
    //double t1, t2; // in case we want to check execution time

#ifdef USE_OPENMP
    omp_set_num_threads(2);
#endif
    //t1 = getTime();

    n = filenames.size();
    eventManager->sig_mediaImported(0,n,-1);

    /*
#pragma omp parallel for
 */
    for (i=0;i<n;i++) {
        int media_id = mediaLibrary->importFile(filenames[i], this->pluginManager, doSegment, doSegment);
        if (media_id >-1){
            ok++;
            needsNormalizeAndCluster = 0;
            if ( (mediaLibrary->getSize() >= int(prevLibrarySizeMultiplier * prevLibrarySize))
                 || (i==n-1)) {
                needsNormalizeAndCluster = 1;
                prevLibrarySize = mediaLibrary->getSize();
            }
            needsNormalizeAndCluster = 1;
            normalizeFeatures(needsNormalizeAndCluster); // exclusively medialibrary
            //mediaBrowser->setNeedsNavigationUpdateLock(1);
            mediaBrowser->initializeNode(media_id);
#if defined (SUPPORT_MULTIMEDIA)
            if (this->getMediaType()==MEDIA_TYPE_MIXED){
                ACMedia* media =  mediaLibrary->getMedia(media_id);
                ACMediaContainer medias = (static_cast<ACMediaDocument*> (media))->getContainer();
                ACMediaContainer::iterator iter;
                for ( iter=medias.begin() ; iter!=medias.end(); ++iter ){
                    mediaBrowser->initializeNode(iter->second->getId());
                    //files_processed++;
                }
            }
#endif
            libraryContentChanged(needsNormalizeAndCluster); // exclusively mediabrowser, thus updateAfterFileImport and importDirectories can't be move to ACMediaLibrary
            //mediaBrowser->setNeedsNavigationUpdateLock(0);
            // this initiates node rendering, must be done after creating a media in the library and a node in the browser
            eventManager->sig_mediaImported(i+1,n,media_id);
        }
    }
    n = mediaLibrary->getSize(); // segmentation might have increased the number of medias in the library
    eventManager->sig_mediaImported(n,n,-1);

    //t2 = getTime();
    //printf("TTT - %f\n",float(t2-t1));
    //}

    filenames.empty();
    return ok;
}

int MediaCycle::importDirectory(string path, int recursive, bool forward_order, bool doSegment,TiXmlElement* _medias) {
    cout << "MediaCycle: importing directory: " << path << endl;
    int ok = 0;
    if (this->pluginManager == 0){
        cout << "no analysis plugins were loaded. you will need to load a plugin to use the application." << endl;
    }
    ok = this->mediaLibrary->importDirectory(path, recursive, this->pluginManager, forward_order, doSegment, _medias);
    return ok;
}

int MediaCycle::setPath(string path) {
    int ok = 0;
    ok = this->mediaLibrary->setPath(path);
    return ok;
}

void MediaCycle::libraryContentChanged(int needsNormalizeAndCluster) { mediaBrowser->libraryContentChanged(needsNormalizeAndCluster); }
void MediaCycle::saveACLLibrary(string path) {mediaLibrary->saveACLLibrary(path); }
void MediaCycle::saveXMLLibrary(string path) {mediaLibrary->saveXMLLibrary(path); }
void MediaCycle::saveMCSLLibrary(string path) {mediaLibrary->saveMCSLLibrary(path); }
void MediaCycle::cleanLibrary() { prevLibrarySize=0; eventManager->sig_libraryCleaned();mediaLibrary->cleanLibrary(); }

int MediaCycle::importACLLibrary(string path) {
    // XS import = open + normalize
    cout << "MediaCycle: importing ACL library: " << path << endl;
    int ok = 0;
    ok = this->mediaLibrary->openACLLibrary(path);
    if (ok>=1) this->mediaLibrary->normalizeFeatures();
    return ok;

}

int MediaCycle::importXMLLibrary(string path) {
    // XS import = open + normalize
    cout << "MediaCycle: importing XML library: " << path << endl;
    int ok = 0;
    ok = this->mediaLibrary->openXMLLibrary(path);
    //if (ok>=1) this->mediaLibrary->normalizeFeatures();//CF done by signals
    return ok;

}

//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
int MediaCycle::importMCSLLibrary(string path) {
    // XS import = open + some processing
    cout << "MediaCycle: importing MCSL library: " << path << endl;
    int ok = 0;
    ok = this->mediaLibrary->openMCSLLibrary(path);
    // Finding segments for each media
    std::vector<long> ids = mediaLibrary->getAllMediaIds();
    for (int i=0; i< ids.size(); i++){
        if (this->mediaLibrary->getMedia(ids[i])->getParentId() != -1){
            this->mediaLibrary->getMedia(this->mediaLibrary->getMedia(ids[i])->getParentId())->addSegment(this->mediaLibrary->getMedia(ids[i]));
        }
    }
    if (ok>=1) this->mediaLibrary->normalizeFeatures();
    return ok;
}

/* SD 2010 sep discontinued
int MediaCycle::importLibrary(string path) {
// XS import = open + some processing
 cout << "MediaCycle: importing library: " << path << endl;
 int ok = 0;
 ok = this->mediaLibrary->openLibrary(path);
 if (ok>=1) this->mediaLibrary->normalizeFeatures();
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
bool MediaCycle::hasBrowser() {
    bool ok = false;
    if (this->getBrowser()!=0)
        ok=true;
    return ok;
}
ACBrowserMode MediaCycle::getBrowserMode() {return mediaBrowser->getMode();}
void MediaCycle::setBrowserMode(ACBrowserMode _mode) {mediaBrowser->setMode(_mode);}

bool MediaCycle::changeBrowserMode(ACBrowserMode _mode){
    this->mediaBrowser->switchMode(_mode);
    return true;
};


//Listener Manager

void MediaCycle::addListener(ACEventListener* eventListener){
    if (eventManager!=NULL)
        eventManager->addListener(eventListener);
};


// Plugins
int MediaCycle::addPluginLibrary(string aPluginLibraryPath) {
    return this->pluginManager->addLibrary(aPluginLibraryPath);
}

int MediaCycle::removePluginLibrary(string aPluginLibraryPath) {
    return this->pluginManager->removeLibrary(aPluginLibraryPath);
}

ACPluginLibrary* MediaCycle::getPluginLibrary(string aPluginLibraryPath) const{
    return this->pluginManager->getPluginLibrary(aPluginLibraryPath);
}

bool MediaCycle::removePluginFromLibrary(std::string _plugin_name, std::string _library_path){
    return this->pluginManager->removePluginFromLibrary(_plugin_name, _library_path);
}

std::vector<std::string> MediaCycle::getListOfPlugins(){
    return this->pluginManager->getListOfPlugins();
}

std::vector<std::string> MediaCycle::getListOfActivePlugins(){
    return this->mediaLibrary->getListOfActivePlugins();
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


void MediaCycle::setPreProcessPlugin(std::string pluginName){
    ACPlugin* preProcessPlugin = this->getPluginManager()->getPlugin(pluginName);
    if (preProcessPlugin!=NULL&&(preProcessPlugin->mediaTypeSuitable(this->getLibrary()->getMediaType()))){

        this->getLibrary()->setPreProcessPlugin(preProcessPlugin);
        cout << "MediaCycle: Preprocessing plugin: " << preProcessPlugin->getName() << endl;
    }
    else {
        this->getLibrary()->setPreProcessPlugin(0);
        cout << "MediaCycle: impossible to import Preprocessing plugin: " << pluginName << endl;
    }
}
void MediaCycle::setMediaReaderPlugin(std::string pluginName){
    ACPlugin* mediaReaderPlugin = this->getPluginManager()->getPlugin(pluginName);
    if (mediaReaderPlugin!=NULL){
        this->getLibrary()->setMediaReaderPlugin(mediaReaderPlugin);
        cout << "MediaCycle: MediaReader plugin: " << mediaReaderPlugin->getName() << endl;
    }
    else
    {
        this->getLibrary()->setMediaReaderPlugin(0);
        cout << "MediaCycle: impossible to import MediaReader plugin: " << pluginName << endl;


    }
}

#ifdef SUPPORT_MULTIMEDIA

std::string MediaCycle::getActiveSubMediaKey(){
    return (mediaLibrary->getActiveSubMediaKey());

}
int MediaCycle::setActiveMediaType(std::string mediaName){
    int ret =mediaLibrary->setActiveMediaType(mediaName,pluginManager);
    return ret ;
}
#endif//def USE_MULTIMEDIA

void MediaCycle::dumpPluginsList(){this->pluginManager->dump();}

// == Media
ACMediaNode* MediaCycle::getMediaNode(int i) { return (mediaBrowser->getMediaNode(i)); }
ACMediaNode* MediaCycle::getNodeFromMedia(ACMedia* _media) { return (mediaBrowser->getNodeFromMedia(_media)); }
string MediaCycle::getMediaFileName(int i) { return mediaLibrary->getMedia(i)->getFileName(); }
ACMediaType MediaCycle::getMediaType(int i) { return mediaLibrary->getMedia(i)->getType(); }
void MediaCycle::setMediaType(ACMediaType mt) {mediaLibrary->setMediaType(mt); }
bool MediaCycle::changeMediaType(ACMediaType aMediaType) {
    bool changeMe = true ;
    if (aMediaType == this->getMediaType())
        // nothing to change
        changeMe = false;
    else{
        this->clean();
        this->mediaLibrary->changeMediaType(aMediaType);
        this->setMediaType(aMediaType);
    }
    return changeMe;
}


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
void MediaCycle::setClosestNode(int i,int p_index) { mediaBrowser->setClosestNode(i,p_index); }
int MediaCycle::getClosestNode(int p_index) { return mediaBrowser->getClosestNode(p_index); }
int	MediaCycle::getLastSelectedNode() { return mediaBrowser->getLastSelectedNode(); }

// == Cluster Display
void MediaCycle::updateState() { mediaBrowser->updateState(); }
int MediaCycle::getNavigationLevel() { return mediaBrowser->getNavigationLevel(); }
float MediaCycle::getFrac() { return mediaBrowser->getFrac(); }
void MediaCycle::incrementNavigationLevels(int i) { mediaBrowser->incrementNavigationLevels(i); }
void MediaCycle::setReferenceNode(int index) { mediaBrowser->setReferenceNode(index); }
int MediaCycle::getReferenceNode() { return mediaBrowser->getReferenceNode(); }
void MediaCycle::goBack() { mediaBrowser->goBack(); }
void MediaCycle::goForward() { mediaBrowser->goForward(); }
void MediaCycle::storeNavigationState() { mediaBrowser->storeNavigationState(); }
void MediaCycle::setClusterNumber(int n) { mediaBrowser->setClusterNumber(n); }
void MediaCycle::setWeight(int i, float weight) { mediaBrowser->setWeight(i, weight); }
void MediaCycle::setWeightVector(std::vector<float> fw) { mediaBrowser->setWeightVector(fw); }
vector<float> MediaCycle::getWeightVector(){return mediaBrowser->getWeightVector();}
float MediaCycle::getWeight(int i){return mediaBrowser->getWeight(i);}

void MediaCycle::setForwardDown(int i) { forwarddown = i; }
void MediaCycle::forwardNextLevel(){
    // enters in the cluster of the last selected node
    if (this->hasBrowser()){
        this->setForwardDown(true); // mediaBrowser->getMode() == AC_MODE_NEIGHBORS
        this->getBrowser()->forwardNextLevel();
         this->setForwardDown(false);
    }
}

// == Features
void MediaCycle::normalizeFeatures(int needsNormalize) { mediaLibrary->normalizeFeatures(needsNormalize); }
// Get Features Vector (identified by feature_name) in media i
FeaturesVector MediaCycle::getFeaturesVectorInMedia(int i, string feature_name) {
    ACMedia* lmedia;
    ACMediaFeatures* lfeatures;
    FeaturesVector lfeaturesvector;
    lmedia = mediaLibrary->getMedia(i);
    lfeatures = lmedia->getPreProcFeaturesVector(feature_name);
    if (lfeatures) {
        lfeaturesvector = lfeatures->getFeaturesVector();
    }
    return lfeaturesvector;
}

// == Playing time stamp
//XS TODO : add tests before setting values ?
void MediaCycle::setSourceCursor(int lid, int frame_pos) {
    mediaBrowser->setSourceCursor(lid, frame_pos);
}
void MediaCycle::setCurrentFrame(int lid, int frame_pos) {
    mediaBrowser->setCurrentFrame(lid, frame_pos);
}
void MediaCycle::muteAllSources() { mediaBrowser->muteAllSources(); }

// == POINTERS on VIEW
int MediaCycle::getNumberOfPointers() {
    return mediaBrowser->getNumberOfPointers();
}

ACPointer* MediaCycle::getPointerFromIndex(int i) {
    return mediaBrowser->getPointerFromIndex(i);
}

ACPointer* MediaCycle::getPointerFromId(int i) {
    return mediaBrowser->getPointerFromId(i);
}

void MediaCycle::resetPointers() {
    mediaBrowser->resetPointers();
}

void MediaCycle::addPointer(int p_id) {
    mediaBrowser->addPointer(p_id);
}

void MediaCycle::removePointer(int p_id) {
    mediaBrowser->removePointer(p_id);
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

void MediaCycle::pickedObjectCallback(int _mediaId) {
    std::cout << "MediaCycle::pickedObjectCallback node id " << _mediaId << " file " << this->getMediaFileName( _mediaId ) << std::endl;
    if(_mediaId < 0) {
        // clicked close to a node
        if (getNumberOfPointers() > 0)
            _mediaId = getClosestNode();
    }
    mediaBrowser->setClickedNode(_mediaId);
    if (forwarddown == 0){// & playkeydown) {//if (!forwarddown) { //CF forwardown is not a boolean
        mediaBrowser->toggleSourceActivity(_mediaId);
    }

    // with observer pattern
    this->notify();

}

void MediaCycle::hoverWithPointerId(float xx, float yy, int p_id) {
    if (this->mediaBrowser)
        mediaBrowser->hoverWithPointerId(xx, yy, p_id);
}

void MediaCycle::hoverWithPointerIndex(float xx, float yy, int p_index) {
    if (this->mediaBrowser)
        mediaBrowser->hoverWithPointerIndex(xx, yy, p_index);
}

void MediaCycle::updateDisplay(bool _animate) {
    mediaBrowser->updateDisplay(_animate);

}
void MediaCycle::initializeFeatureWeights() { mediaBrowser->initializeFeatureWeights();}

// reads in the XML file :
// - header information
// - media, features, segments, ...
TiXmlHandle MediaCycle::readXMLConfigFileHeader(string _fname) {
    if (_fname=="") return 0;
    TiXmlDocument doc( _fname.c_str() );
    try {
        if (!doc.LoadFile( ))
            throw runtime_error("error reading XML file");
    } catch (const exception& e) {
        cout << e.what( ) << "\n";
        exit(1);
        //return EXIT_FAILURE;
    }

    TiXmlHandle docHandle(&doc);
    TiXmlHandle rootHandle = docHandle.FirstChildElement( "MediaCycle" );

    TiXmlText* browserModeText=rootHandle.FirstChild( "BrowserMode" ).FirstChild().Text();
    std::stringstream tmp;
    tmp << browserModeText->ValueStr();
    int bm; // ACBrowserMode
    tmp >> bm;
    this->setBrowserMode(ACBrowserMode (bm));

    TiXmlText* mediaTypeText=rootHandle.FirstChild( "MediaType" ).FirstChild().Text();
    std::stringstream tmp2;
    tmp2 << mediaTypeText->ValueStr();
    int mt; //ACMediaType
    tmp2 >> mt;
    this->setMediaType(ACMediaType(mt));

    // features vector weights
    int n_feat=-1;
    TiXmlText* FeaturesWeightsText=rootHandle.FirstChild( "FeaturesWeights" ).FirstChild().Text();
    TiXmlElement* FeaturesWeightsNode=rootHandle.FirstChild( "FeaturesWeights" ).Element();
    FeaturesWeightsNode->QueryIntAttribute("NumberOfFeatures", &n_feat);
    if (n_feat < 0)
        throw runtime_error("corrupted XML file, wrong number of features weights");

    std::stringstream tmp3;
    tmp3 << FeaturesWeightsText->ValueStr();
    vector<float> fw;
    try {
        for (int j=0; j<n_feat; j++) {
            // XS TODO add tests !! on number of features
            float w;
            tmp3 >> w;
            fw.push_back(w);
        }
        this->setWeightVector(fw);
    }
    catch (...) {
        // attempt to catch potential problems and group them
        throw runtime_error("corrupted XML file, error reading feature weight");
    }

    return rootHandle;
}

// XS TODO return value, tests
int MediaCycle::readXMLConfigFileCore(TiXmlHandle _rootHandle) {

    TiXmlElement* media_element = this->mediaLibrary->openCoreXMLLibrary(_rootHandle);

    eventManager->sig_mediaImported(0,this->mediaLibrary->getNumberOfFilesToImport(),-1);

    int needsNormalizeAndCluster = 0;
    float prevLibrarySizeMultiplier = 2;

    while(  media_element != 0 ){
        //mediaBrowser->setNeedsNavigationUpdateLock(1);
        media_element = this->mediaLibrary->openNextMediaFromXMLLibrary(media_element);
        long int media_id = this->mediaLibrary->getNewestMediaId();
        int i = this->mediaLibrary->getNumberOfFilesProcessed();
        int n = this->mediaLibrary->getNumberOfFilesToImport();

        needsNormalizeAndCluster = 0;
        if ( (mediaLibrary->getSize() >= int(prevLibrarySizeMultiplier * prevLibrarySize))
             || (i==n-1)) {
            needsNormalizeAndCluster = 1;
            prevLibrarySize = mediaLibrary->getSize();
        }
        normalizeFeatures(needsNormalizeAndCluster); // exclusively medialibrary
        //mediaBrowser->setNeedsNavigationUpdateLock(1);
        mediaBrowser->initializeNode(media_id);
/*#if defined (SUPPORT_MULTIMEDIA)
            if (this->getMediaType()==MEDIA_TYPE_MIXED){
                ACMedia* media =  mediaLibrary->getMedia(media_id);
                ACMediaContainer medias = (static_cast<ACMediaDocument*> (media))->getContainer();
                ACMediaContainer::iterator iter;
                for ( iter=medias.begin() ; iter!=medias.end(); ++iter ){
                    mediaBrowser->initializeNode(iter->second->getId());
                    //files_processed++;
                }
            }
#endif*/
        libraryContentChanged(needsNormalizeAndCluster); // exclusively mediabrowser, thus updateAfterFileImport and importDirectories can't be move to ACMediaLibrary
        //mediaBrowser->setNeedsNavigationUpdateLock(0);
        eventManager->sig_mediaImported(this->mediaLibrary->getNumberOfFilesProcessed(),this->mediaLibrary->getNumberOfFilesToImport(),media_id);
    }

    int n = this->mediaLibrary->getSize(); // segmentation might have increased the number of medias in the library
    eventManager->sig_mediaImported(n,n,-1);
}

// XS TODO return value, tests
int MediaCycle::readXMLConfigFilePlugins(TiXmlHandle _rootHandle) {
    if (!this->pluginManager) this->pluginManager = new ACPluginManager();
    this->pluginManager->setMediaCycle(this);

    TiXmlElement* MC_e_features_plugin_manager = _rootHandle.FirstChild("PluginsManager").ToElement();
    int nb_plugins_lib=0;
    if (MC_e_features_plugin_manager!=0){
        MC_e_features_plugin_manager->QueryIntAttribute("NumberOfPluginsLibraries", &nb_plugins_lib);

        //this->pluginManager->clean();
        //this->pluginManager->setMediaCycle(this);
        this->mediaBrowser->changeClustersMethodPlugin( this->pluginManager->getPlugin("ACKMeansPlugin") );
        this->mediaBrowser->changeClustersMethodPlugin( this->pluginManager->getPlugin("ACClusterPositionsPropellerPlugin") );

        TiXmlElement* pluginLibraryNode=MC_e_features_plugin_manager->FirstChild()->ToElement();
        for( pluginLibraryNode; pluginLibraryNode; pluginLibraryNode=pluginLibraryNode->NextSiblingElement()) {
            string libraryName = pluginLibraryNode->Attribute("LibraryPath");
            int lib_size=0;
            pluginLibraryNode->QueryIntAttribute("NumberOfPlugins", &lib_size);
            if(this->pluginManager->addLibrary(libraryName) == -1){
                this->pluginManager->addLibrary( this->getPluginPathFromBaseName(fs::basename(libraryName)) );
            }
        }
    }
}

// XS TODO return value, tests
int MediaCycle::readXMLConfigFile(string _fname) {
    TiXmlHandle rootHandle = readXMLConfigFileHeader (_fname);
    this->readXMLConfigFileCore (rootHandle);
}

std::string MediaCycle::getPluginPathFromBaseName(std::string basename)
{
    std::string prefix("mc_");
    size_t found = basename.find(prefix);
    if(found != std::string::npos){
        basename = basename.substr(found+prefix.size());
        std::cout << "MediaCycle::getPluginPathFromBaseName: new basename: " << basename << std::endl;
    }

    char c_path[2048];
    // use the function to get the path
    getcwd(c_path, 2048);
    std::string s_path = c_path;
    std::string plugins_path(""),plugin_subfolder(""),plugin_ext("");

    std::string build_type ("Release");
#ifdef USE_DEBUG
    build_type = "Debug";
#endif //USE_DEBUG

#if defined(__APPLE__)
    plugin_ext = ".dylib";
#if not defined (USE_DEBUG) and not defined (XCODE) // needs "make install" to be ran to work
    plugins_path = "@executable_path/../MacOS/";
    plugin_subfolder = "";
#else
#if defined(XCODE)
    plugins_path = s_path + "/../../plugins/";
    plugin_subfolder = basename + "/" + build_type + "/";
#else
    plugins_path = s_path + "/../../plugins/";
    plugin_subfolder = basename + "/";
#endif
#endif
#elif defined (__WIN32__)
    plugin_ext = ".dll";
    plugins_path = s_path + "/";
#ifdef USE_DEBUG
    plugins_path = s_path + "/../../plugins/";
    plugin_subfolder = basename + "/";
#endif USE_DEBUG
#else // Linux
    plugin_ext = ".so";
#if not defined (USE_DEBUG) // needs "make package" to be ran to work
    plugins_path = "/usr/lib/"; // or a prefix path from CMake?
    plugin_subfolder = "";
#else
    plugins_path = s_path + "/../../plugins/";
    plugin_subfolder = basename + "/";
#endif
#endif
    return plugins_path + plugin_subfolder + prefix + basename + plugin_ext;
}

// XS TODO what else to put in the config ?
// XS TODO separate in header/core/plugins ?
void MediaCycle::saveXMLConfigFile(string _fname) {
    // or set it to config_file_xml ?
    TiXmlDocument MC_doc;
    TiXmlDeclaration* MC_decl = new TiXmlDeclaration( "1.0", "", "" );
    MC_doc.LinkEndChild( MC_decl );

    TiXmlElement* MC_e_root = new TiXmlElement( "MediaCycle" );
    MC_doc.LinkEndChild( MC_e_root );

    // "header"
    TiXmlElement* MC_e_browser_mode = new TiXmlElement( "BrowserMode" );
    MC_e_root->LinkEndChild( MC_e_browser_mode );
    // XS  TODO get it as text e.g. this->getBrowserModeAsString()
    std::stringstream tmp_bm;
    tmp_bm << this->getBrowserMode();
    TiXmlText* MC_t_bm = new TiXmlText( tmp_bm.str() );
    MC_e_browser_mode->LinkEndChild( MC_t_bm );

    TiXmlElement* MC_e_media_type = new TiXmlElement( "MediaType" );
    MC_e_root->LinkEndChild( MC_e_media_type );
    // XS  TODO get it as text e.g. this->getMediaTypeAsString()
    std::stringstream tmp_mt;
    tmp_mt << this->getMediaType();
    TiXmlText* MC_t_mt = new TiXmlText( tmp_mt.str() );
    MC_e_media_type->LinkEndChild( MC_t_mt );

    // "medias and features"
    TiXmlElement* MC_e_features_weights = new TiXmlElement("FeaturesWeights");
    MC_e_root->LinkEndChild( MC_e_features_weights );
    vector<float> features_weights = this->getWeightVector();
    MC_e_features_weights->SetAttribute("NumberOfFeatures", features_weights.size());

    // concatenate feature weights separated by a " "
    std::string sfw;
    std::stringstream tmp;
    for (unsigned int j=0; j<features_weights.size(); j++) {
        tmp << features_weights[j]<< " " ;
    }
    sfw = tmp.str();
    TiXmlText* MC_t_features_weights = new TiXmlText(sfw.c_str());
    MC_e_features_weights->LinkEndChild( MC_t_features_weights );

    std::string media_identifier = "Medias";
    if(mediaLibrary->getMediaType() == MEDIA_TYPE_MIXED){
        media_identifier = "MediaDocuments";
    }
    TiXmlElement* MC_e_medias = new TiXmlElement(media_identifier);

    this->mediaLibrary->saveCoreXMLLibrary(MC_e_root, MC_e_medias);
    MC_e_root->LinkEndChild( MC_e_medias );

    // "plugins"
    // XS TODO put this in a method getPluginsNames(blabla)
    if (pluginManager) {
        TiXmlElement* MC_e_features_plugin_manager = new TiXmlElement( "PluginsManager" );
        MC_e_root->LinkEndChild( MC_e_features_plugin_manager );
        int n_librarires = pluginManager->getSize();
        MC_e_features_plugin_manager->SetAttribute("NumberOfPluginsLibraries", n_librarires);
        for (int i=0; i<n_librarires; i++) {
            TiXmlElement* MC_e_features_plugin_library = new TiXmlElement( "PluginLibrary" );
            MC_e_features_plugin_manager->LinkEndChild( MC_e_features_plugin_library );
            int n_plugins = pluginManager->getPluginLibrary(i)->getSize();
            MC_e_features_plugin_library->SetAttribute("NumberOfPlugins", n_plugins);
            MC_e_features_plugin_library->SetAttribute("LibraryPath", pluginManager->getPluginLibrary(i)->getLibraryPath());
            for (int j=0; j<n_plugins; j++) {
                TiXmlElement* MC_e_features_plugin = new TiXmlElement( "FeaturesPlugin" );
                MC_e_features_plugin_library->LinkEndChild( MC_e_features_plugin );
                std::stringstream tmp_p;
                tmp_p << pluginManager->getPluginLibrary(i)->getPlugin(j)->getName() ;
                TiXmlText* MC_t_pm = new TiXmlText( tmp_p.str() );
                MC_e_features_plugin->LinkEndChild( MC_t_pm );
            }
        }
    }

    MC_doc.SaveFile(_fname.c_str());
    cout << "saved XML config : " << _fname << endl;
    // children of MC_Doc get deleted automatically
}

// == Dump / used for Debug
void MediaCycle::dumpNavigationLevel(){
    cout << "Global Navigation Level = " <<  getNavigationLevel() << endl;
}

void MediaCycle::dumpNavigationLevels(){
    int nl = this->getNumberOfMediaNodes();
    cout << "Navigation levels for :" << endl;
    for (int i=0; i < nl; i++){
        cout << this->getBrowser()->getMediaNode(i)->getNavigationLevel() << " ";
    }
    cout << endl;
}

// == testing
void MediaCycle::testThreads(){
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

void MediaCycle::testLabels(){
    // Test Labels
    ACPoint p;
    p.x = -0.1; p.y = 0.0; p.z = 0.01;
    this->mediaBrowser->setLabel(0, "Label-1", p);
    p.x = 0.2; p.y = 0.0; p.z = 0.01;
    this->mediaBrowser->setLabel(1, "Label-2", p);
    p.x = 0.0; p.y = 0.1; p.z = 0.01;
    this->mediaBrowser->setLabel(2, "Label-3", p);
}

void MediaCycle::mediaImported(int n,int nTot,int mId){
    std::cout << "MediaCycle::mediaImported media id " << mId << " ("<< n << "/" << nTot << ")" << std::endl;
    eventManager->sig_mediaImported(n,nTot,mId);
}
