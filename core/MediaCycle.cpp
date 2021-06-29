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

#include "boost/filesystem.hpp"
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
    ACMediaFactory::getInstance().setPluginManager(pluginManager);
    this->changeActivePlugin(PLUGIN_TYPE_CLUSTERS_METHOD,"MediaCycle KMeans");
    this->changeActivePlugin(PLUGIN_TYPE_CLUSTERS_POSITIONS,"MediaCycle Propeller");
    
    this->config_file_xml = "";
    this->current_config = "";
    
    this->prevLibrarySize = 0;
    this->eventManager=new ACEventManager;
    
    // when importing files:
    // 1) the library creates a media
    // 2) the browser creates a node
    // 3) mediacycle propagates the event to external listeners
    this->importing = false;
}

MediaCycle::MediaCycle(const MediaCycle& orig) {
    // XS TODO: complete this copy-constructor or remove it !
}

MediaCycle::~MediaCycle() {
    delete this->mediaLibrary;
    this->mediaLibrary = 0;
    delete this->mediaBrowser;
    this->mediaBrowser = 0;
    this->eventManager->clean();
    this->pluginManager->clean();
    delete this->pluginManager;
    this->pluginManager = 0;
    stopTcpServer(); // will delete this->networkSocket;
    for(std::map<std::string,ACAbstractDefaultConfig*>::iterator default_config = default_configs.begin(); default_config != default_configs.end();++default_config){
        delete default_config->second;
        default_config->second = 0;
    }
    default_configs.clear();
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
    //this->pluginManager->clean();
    this->pluginManager->setMediaCycle(this);
    this->changeActivePlugin(PLUGIN_TYPE_CLUSTERS_METHOD,"MediaCycle KMeans");
    this->changeActivePlugin(PLUGIN_TYPE_CLUSTERS_POSITIONS,"MediaCycle Propeller");
}

// == Default configs
ACAbstractDefaultConfig* MediaCycle::getDefaultConfig(std::string name){
    ACAbstractDefaultConfig* config = default_configs[name];
    return config;
}

std::map<std::string,ACAbstractDefaultConfig*> MediaCycle::getDefaultConfigs(){
    return default_configs;
}

std::vector<std::string> MediaCycle::getDefaultConfigsNames(){
    std::vector<std::string> config_names;
    for(std::map<std::string,ACAbstractDefaultConfig*>::iterator default_config = default_configs.begin(); default_config != default_configs.end();++default_config)
        config_names.push_back(default_config->first);
    return config_names;
}

bool MediaCycle::addDefaultConfig(ACAbstractDefaultConfig* _config){
    if(!_config){
        std::cerr << "MediaCycle::addDefaultConfig: config not instanciated" << std::endl;
        return false;
    }
    if(default_configs.find(_config->name())!=default_configs.end())
    {
        //std::cout << "MediaCycle::addDefaultConfig: config with exact same name already present" << std::endl;
        return true;
    }
    default_configs[_config->name()] = _config;
    return true;
}

int MediaCycle::getDefaultConfigsNumber(){
    return default_configs.size();
}

void MediaCycle::loadDefaultConfig(ACAbstractDefaultConfig* config){
    if(this->addDefaultConfig(config))
        this->loadDefaultConfig(config->name());
}

void MediaCycle::loadDefaultConfig(std::string name){
    ACAbstractDefaultConfig* config = this->getDefaultConfig(name);
    if(!config){
        throw runtime_error("Config '" + name + "' not available");
    }
    if(!this->addDefaultConfig(config)){
        throw runtime_error("Couldn't add config named '" + name + "'");
    }
    if(this->current_config == name){
        throw runtime_error("Current config already of the same name '" + name + "', not loading");
    }
    
    if(current_config!="")
        this->clean();
    
    if(this->getMediaType() != config->mediaType())
        this->changeMediaType(config->mediaType());
    if(this->getBrowserMode() != config->browserMode())
        this->changeBrowserMode(config->browserMode());
    
    this->current_config = name;
    
    std::vector<std::string> plugins = config->pluginLibraries();
    for(std::vector<std::string>::iterator plugin = plugins.begin();plugin != plugins.end();++plugin){
        if(config->staticLibraries()){
            ACPluginLibrary* library = 0;
            library = config->createPluginLibrary(*plugin);
            if(!library){
                throw runtime_error("Couldn't load plugin library '" + *plugin + "'");
            }
            int n_elements = this->addPluginLibrary(library);
            if( n_elements <= 0){
                throw runtime_error("Couldn't load any plugin from library '" + *plugin + "'.");
            }
        }
        else{
            std::string path = this->getPluginPathFromBaseName(*plugin);
            if(path == ""){
                //std::cerr << "Couldn't load plugin library " << *plugin << std::endl;
                //break;
                throw runtime_error("Couldn't load plugin library '" + *plugin + "'");
            }
            int n_elements = this->addPluginLibrary(path);
            if( n_elements <= 0){
                throw runtime_error("Couldn't load any plugin from library '" + *plugin + "' from path '" + path + "'.");
            }
        }
    }
    
    if(config->clustersMethodPlugin() != "")
        this->changeActivePlugin(PLUGIN_TYPE_CLUSTERS_METHOD,config->clustersMethodPlugin());
    if(config->clustersPositionsPlugin() != "")
        this->changeActivePlugin(PLUGIN_TYPE_CLUSTERS_POSITIONS,config->clustersPositionsPlugin());
    if(config->neighborsMethodPlugin() != "")
        this->changeActivePlugin(PLUGIN_TYPE_NEIGHBORS_METHOD,config->neighborsMethodPlugin());
    if(config->neighborsPositionsPlugin() != "")
        this->changeActivePlugin(PLUGIN_TYPE_NEIGHBORS_POSITIONS,config->neighborsPositionsPlugin());
    if(config->preProcessPlugin() != "")
        this->changeActivePlugin(PLUGIN_TYPE_PREPROCESS,config->preProcessPlugin());
    if(config->libraryReaderPlugin() != "")
        this->changeActivePlugin(PLUGIN_TYPE_LIBRARY_READER,config->libraryReaderPlugin());
    if(config->libraryWriterPlugin() != "")
        this->changeActivePlugin(PLUGIN_TYPE_LIBRARY_WRITER,config->libraryWriterPlugin());
    if(config->filteringPlugin() != "")
        this->changeActivePlugin(PLUGIN_TYPE_FILTERING,config->filteringPlugin());
    
#if defined (SUPPORT_MULTIMEDIA)
    mediaLibrary->setActiveMediaType( config->activeMediaType(), this->pluginManager );
    mediaLibrary->setMediaReaderPlugin( this->getPlugin( config->mediaReaderPlugin() ) );
#endif
}

ACAbstractDefaultConfig* MediaCycle::getCurrentConfig(){
    ACAbstractDefaultConfig* config(0);
    if(this->current_config != "")
        config = this->getDefaultConfig(this->current_config);
    else
        std::cerr << "MediaCycle::getCurrentConfig: no current config set" << std::endl;
    return config;
}

std::string MediaCycle::getCurrentConfigName(){
    std::string name("");
    ACAbstractDefaultConfig* config = this->getCurrentConfig();
    if(config)
        name = config->name();
    else
        std::cerr << "MediaCycle::getCurrentConfigName: couldn't access curent config" << std::endl;
    return name;
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
    MediaCycle* mc = (MediaCycle*)import_thread_arg;
    if(mc) 
        mc->importDirectories();
    return 0;
}


// == Media Library

// XS TODO return value does not make much sense, should add some test
// XS TODO this does not seem compatible with Qt GUI
int MediaCycle::importDirectoriesThreaded(vector<string> directories, int recursive, bool forward_order, bool doSegment) {
    this->importing = true;
    
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
int MediaCycle::importDirectories(vector<string> directories, int recursive, bool forward_order, bool doSegment, bool periodicNormalizeAndCluster) {
    this->importing = true;
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
    this->mediaLibrary->addNumberOfFilesToImport(n);
    eventManager->sig_mediaImported(0,n,-1);
    
    /*
#pragma omp parallel for
 */
    for (i=0;i<n;i++) {
        std::vector<int> media_ids = mediaLibrary->importFile(filenames[i], this->pluginManager, doSegment, doSegment);
        if (media_ids.size() >0){
            ok++;
            needsNormalizeAndCluster = 0;
            if ( (periodicNormalizeAndCluster && mediaLibrary->getSize() >= int(prevLibrarySizeMultiplier * prevLibrarySize)) || (i==n-1)) {
                needsNormalizeAndCluster = 1;
                prevLibrarySize = mediaLibrary->getSize();
            }
            if(periodicNormalizeAndCluster || (!periodicNormalizeAndCluster && needsNormalizeAndCluster)){  
                //needsNormalizeAndCluster = 1;
                normalizeFeatures(needsNormalizeAndCluster); // exclusively medialibrary
            }
            //mediaBrowser->setNeedsNavigationUpdateLock(1);
            for (vector<int>::iterator media_id=media_ids.begin();media_id!=media_ids.end();media_id++)
            {
                mediaBrowser->initializeNode(*media_id);
                eventManager->sig_mediaImported(i+1,n,*media_id);
            }
            
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
            if(periodicNormalizeAndCluster || (!periodicNormalizeAndCluster && needsNormalizeAndCluster)){
                libraryContentChanged(needsNormalizeAndCluster, /*bool needsDisplay*/periodicNormalizeAndCluster); // exclusively mediabrowser, thus updateAfterFileImport and importDirectories can't be move to ACMediaLibrary
            }
            //mediaBrowser->setNeedsNavigationUpdateLock(0);
            // this initiates node rendering, must be done after creating a media in the library and a node in the browser
        }
    }
    n = mediaLibrary->getSize(); // segmentation might have increased the number of medias in the library
    this->mediaImported(n,n,-1);
    /*usleep(1000000);
    ACMediaNodes tempNodes=mediaBrowser->getMediaNodes();
    std::cout<<"NodeIndex:\n";
    for (ACMediaNodes::iterator it=tempNodes.begin();it!=tempNodes.end();it++)
        std::cout<<it->first<<std::endl;
    ACMedias tempMedias= mediaLibrary->getAllMedia();
    std::cout<<"MediaIndex:\n";
    for (ACMedias::iterator it=tempMedias.begin();it!=tempMedias.end();it++)
        std::cout<<it->first<<std::endl;
*/
    //t2 = getTime();
    //printf("TTT - %f\n",float(t2-t1));
    //}
    
    filenames.empty();
    this->importing = false;
    return ok;
}

/*int MediaCycle::importDirectory(string path, int recursive, bool forward_order, bool doSegment,TiXmlElement* _medias) {
    cout << "MediaCycle: importing directory: " << path << endl;
    int ok = 0;
    if (this->pluginManager == 0){
        cout << "no analysis plugins were loaded. you will need to load a plugin to use the application." << endl;
    }
    ok = this->mediaLibrary->importDirectory(path, recursive, this->pluginManager, forward_order, doSegment, _medias);
    return ok;
}*/

int MediaCycle::importDirectory(string path, int recursive, bool forward_order, bool doSegment, bool _save_timed_feat) {
    this->importing = true;
    cout << "MediaCycle: importing directory: " << path << endl;
    int ok = 0;
    if (this->pluginManager == 0){
        cout << "no analysis plugins were loaded. you will need to load a plugin to use the application." << endl;
    }
    ok = this->mediaLibrary->importDirectory(path, recursive, this->pluginManager, forward_order, doSegment, _save_timed_feat);
    //CF there should be libraryContentChanged(needsNormalizeAndCluster) to initialise features weights but if forces clustering and positioning, not ok for CLIs
    this->importing = false;
    return ok;
}

int MediaCycle::setPath(string path) {
    int ok = 0;
    ok = this->mediaLibrary->setPath(path);
    return ok;
}

void MediaCycle::libraryContentChanged(int needsNormalizeAndCluster, bool needsDisplay)
{
    if (mediaBrowser){
        mediaBrowser->libraryContentChanged(needsNormalizeAndCluster, needsDisplay);
    }
}

void MediaCycle::saveLibrary(std::string _path, ACMediaLibraryWriterPlugin* plugin){
    if(mediaLibrary){
        if(mediaLibrary->getSize()==0)
            return;
        try{
            mediaLibrary->saveLibrary(_path,plugin);
        }catch(const std::exception& e){
            throw runtime_error(e.what());
        }
    }
}

void MediaCycle::saveLibrary(std::string _path, std::string plugin_name){
    if(!mediaLibrary)
        return;
    //CF saving an empty library writes a file that can be helpful (check its headers, test if the file is actually written)
    //if(mediaLibrary->getSize()==0)
    //    return;
    if(_path=="" || plugin_name =="")
        return;
    ACPlugin* plugin = this->getPlugin(plugin_name);
    if(!plugin)
        return;
    ACMediaLibraryWriterPlugin* writer_plugin = dynamic_cast<ACMediaLibraryWriterPlugin*>(plugin);
    if(!writer_plugin)
        return;
    try{
        mediaLibrary->saveLibrary(_path,writer_plugin);
    }catch(const std::exception& e){
        throw runtime_error(e.what());
    }
}


void MediaCycle::saveACLLibrary(string path)
{
    if(mediaLibrary){
        mediaLibrary->saveACLLibrary(path);
    }
}

void MediaCycle::saveXMLLibrary(string path)
{
    // JU added to store Feature weights and distance types
    /*if(mediaLibrary&&mediaBrowser){
        mediaLibrary->saveXMLLibrary(path,mediaBrowser);
        return;
    }*/
    // original function
    if(mediaLibrary){
        mediaLibrary->saveXMLLibrary(path);
    }
}

void MediaCycle::saveMCSLLibrary(string path)
{
    if(mediaLibrary){
        mediaLibrary->saveMCSLLibrary(path);
    }
}

void MediaCycle::cleanLibrary()
{
    std::vector<std::string> renderer_plugins = pluginManager->getAvailablePluginsNames(PLUGIN_TYPE_MEDIARENDERER, this->getMediaType());
    for(std::vector<std::string>::iterator renderer_plugin = renderer_plugins.begin();renderer_plugin!=renderer_plugins.end();renderer_plugin++){
        ACMediaRendererPlugin* plugin = dynamic_cast<ACMediaRendererPlugin*>(pluginManager->getPlugin(*renderer_plugin));
        if(plugin)
            plugin->disable();
    }
    
    prevLibrarySize=0;
    eventManager->sig_libraryCleaned();
    if(mediaLibrary){
        mediaLibrary->cleanLibrary();
    }
}

int MediaCycle::importACLLibrary(string path) {
    // XS import = open + normalize
    if(!mediaLibrary){
        return -1;
    }
    cout << "MediaCycle: importing ACL library: " << path << endl;
    int ok = 0;
    ok = this->mediaLibrary->openACLLibrary(path);
    if (ok>=1) this->mediaLibrary->normalizeFeatures();
    return ok;
    
}

int MediaCycle::importXMLLibrary(string path) {
    this->importing = true;
    // XS import = open + normalize
    if(!mediaLibrary){
        return -1;
    }
    cout << "MediaCycle: importing XML library: " << path << endl;
    int ok = 0;
    
    int beginIndex=this->mediaLibrary->getNewestMediaId();
    
    ok = this->mediaLibrary->openXMLLibrary(path);
    //if (ok>=1) this->mediaLibrary->normalizeFeatures();//CF done by signals
    normalizeFeatures(1);
    int lastIndex=this->mediaLibrary->getNewestMediaId();
    
    for (int newId=beginIndex+1;newId<=lastIndex;newId++){
        mediaBrowser->initializeNode(newId);
    }
    
    
    this->importing = false;
    return ok;
    
}

//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
int MediaCycle::importMCSLLibrary(string path) {
    // XS import = open + some processing
    if(!mediaLibrary){
        return -1;
    }
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

int MediaCycle::getLibrarySize()
{
    int _size = 0;
    if(mediaLibrary){
        _size = mediaLibrary->getSize();
    }
    return _size;
}

int MediaCycle::getNumberOfMediaNodes(){
    int _nodes;
    if(mediaBrowser){
        _nodes = mediaBrowser->getNumberOfMediaNodes();
    }
    return _nodes;
}

// == Search by Similarity

int MediaCycle::getClustersCenterMedia(vector<int> &ids) {
    int ret = -1;
    if (mediaBrowser){
        ret = this->mediaBrowser->getClustersCenterMedia(ids);
    }
    return ret;
}

int MediaCycle::getKNN(int id, vector<int> &ids, int k) {
    int ret = -1;
    if (mediaBrowser){
        ret = this->mediaBrowser->getKNN(id, ids, k);
    }
    return ret;
}

int MediaCycle::getKNN(ACMedia *aMedia, vector<ACMedia *> &result, int k) {
    int ret = -1;
    if (mediaBrowser){
        ret = this->mediaBrowser->getKNN(aMedia, result, k);
    }
    return ret;
}

// Thumbnail

string MediaCycle::getThumbnailFileName(int id) {
    if(mediaLibrary){
        return this->mediaLibrary->getThumbnailFileName(id);
    }
    else{
        return "";
    }
}

// Media Browser
bool MediaCycle::hasBrowser() {
    bool ok = false;
    if (this->getBrowser()!=0)
        ok=true;
    return ok;
}

ACBrowserMode MediaCycle::getBrowserMode()
{
    if (mediaBrowser){
        return mediaBrowser->getMode();
    }
    else{
        return AC_MODE_NONE;
    }
}

void MediaCycle::setBrowserMode(ACBrowserMode _mode){
    if (mediaBrowser){
        mediaBrowser->setMode(_mode);
    }
}

bool MediaCycle::changeBrowserMode(ACBrowserMode _mode){
    if (mediaBrowser){
        this->mediaBrowser->switchMode(_mode);
        return true;
    }
    else{
        return false;
    }
}

//Listener Manager
void MediaCycle::addListener(ACEventListener* eventListener){
    if (eventManager!=NULL){
        eventManager->addListener(eventListener);
    }
}

// Plugins
int MediaCycle::loadPluginLibraryFromBasename(std::string basename){
    std::string plugin = this->getPluginPathFromBaseName(basename);
    int n_elements = this->addPluginLibrary(plugin);
    if( n_elements == -1){
        std::cerr << "MediaCycle::loadPluginLibraryFromBasename: couldn't load library of basename " << basename << std::endl;
    }
    return n_elements;
}

int MediaCycle::addPluginLibrary(string aPluginLibraryPath) {
    std::vector<std::string> plugins_names = this->pluginManager->addLibrary(aPluginLibraryPath);
    for(std::vector<std::string>::iterator plugin_name = plugins_names.begin();plugin_name!=plugins_names.end();plugin_name++){
        eventManager->sig_pluginLoaded(*plugin_name);
    }
    return plugins_names.size();
}

int MediaCycle::addPluginLibrary(ACPluginLibrary* library) {
    if(!library){
        std::cerr << "MediaCycle::addPluginLibrary: empty library" << std::endl;
        return 0;
    }
    library->setLibraryPath( this->getPluginPathFromBaseName( library->getName() ) );
    std::vector<std::string> plugins_names = this->pluginManager->addLibrary(library);
    for(std::vector<std::string>::iterator plugin_name = plugins_names.begin();plugin_name!=plugins_names.end();plugin_name++){
        eventManager->sig_pluginLoaded(*plugin_name);
    }
    return plugins_names.size();
}

int MediaCycle::removePluginLibrary(string aPluginLibraryPath) {
    if(this->pluginManager == 0)
        return 0;
    return this->pluginManager->removeLibrary(aPluginLibraryPath);
}

int MediaCycle::removePluginLibraryFromBasename(std::string basename) {
    if(this->pluginManager == 0)
        return 0;
    std::string aPluginLibraryPath = this->getPluginPathFromBaseName(basename);
    return this->pluginManager->removeLibrary(aPluginLibraryPath);
}

ACPluginLibrary* MediaCycle::getPluginLibrary(string aPluginLibraryPath) const{
    if(this->pluginManager == 0)
        return 0;
    return this->pluginManager->getPluginLibrary(aPluginLibraryPath);
}

ACPlugin*  MediaCycle::getPlugin(std::string name){
    if(this->pluginManager == 0)
        return 0;
    return this->pluginManager->getPlugin(name);
}

bool MediaCycle::removePluginFromLibrary(std::string _plugin_name, std::string _library_path){
    if(this->pluginManager == 0)
        return false;
    return this->pluginManager->removePluginFromLibrary(_plugin_name, _library_path);
}

std::vector<std::string> MediaCycle::getListOfPlugins(){
    if(this->pluginManager == 0)
        return std::vector<std::string>();
    return this->pluginManager->getListOfPlugins();
}

std::vector<std::string> MediaCycle::getListOfActivePlugins(){
    if(this->mediaLibrary == 0)
        return std::vector<std::string>();
    return this->mediaLibrary->getListOfActivePlugins();
}

bool MediaCycle::changeActivePlugin(ACPluginType pluginType, std::string pluginName)
{
    bool ok = false;
    ACPlugin* plugin = this->getPluginManager()->getPlugin(pluginName);
    if(!plugin){
        std::cerr <<"MediaCycle::changePlugin: couldn't find plugin " << pluginName << std::endl;
        return false;
    }
    if(plugin->implementsPluginType(PLUGIN_TYPE_CLUSTERS_METHOD)){
        this->getBrowser()->changeClustersMethodPlugin(plugin);
        ok = true;
    }
    if(plugin->implementsPluginType(PLUGIN_TYPE_CLUSTERS_POSITIONS)){
        this->getBrowser()->changeClustersPositionsPlugin(plugin);
        ok = true;
    }
    if(plugin->implementsPluginType(PLUGIN_TYPE_NEIGHBORS_METHOD)){
        this->getBrowser()->changeNeighborsMethodPlugin(plugin);
        ok = true;
    }
    if(plugin->implementsPluginType(PLUGIN_TYPE_NEIGHBORS_POSITIONS)){
        this->getBrowser()->changeNeighborsPositionsPlugin(plugin);
        ok = true;
    }
    if(plugin->implementsPluginType(PLUGIN_TYPE_FILTERING)){
        this->getBrowser()->changeFilteringPlugin(plugin);
        ok = true;
    }
    if(plugin->implementsPluginType(PLUGIN_TYPE_SEGMENTATION)){
        this->getPluginManager()->setActiveSegmentPlugin(pluginName);
        ok = true;
    }
    if(plugin->implementsPluginType(PLUGIN_TYPE_PREPROCESS)){
        this->getLibrary()->setPreProcessPlugin(plugin);
        ok = true;
    }
    if(ok && this->getLibrarySize()>0){
        this->eventManager->sig_updateDisplayNeeded();
    }
    return ok;
}

std::vector<std::string> MediaCycle::getActivePluginNames(ACPluginType pluginType,ACMediaType mediaType){
    std::vector<std::string> names;
    if(!this->getPluginManager()){
        std::cerr << "MediaCycle::getActivePluginNames: no plugin manager" << std::endl;
        return names;
    }
    if(!this->getBrowser()){
        std::cerr << "MediaCycle::getActivePluginNames: no browser" << std::endl;
        return names;
    }
    ACPlugin* plugin(0);
    if(mediaType == MEDIA_TYPE_NONE)
        mediaType = this->getMediaType();
    switch ( pluginType ){
    case PLUGIN_TYPE_CLUSTERS_METHOD:
        plugin = this->getBrowser()->getClustersMethodPlugin();
        break;
    case PLUGIN_TYPE_CLUSTERS_POSITIONS:
        plugin = this->getBrowser()->getClustersPositionsPlugin();
        break;
    case PLUGIN_TYPE_NEIGHBORS_METHOD:
        plugin = this->getBrowser()->getNeighborsMethodPlugin();
        break;
    case PLUGIN_TYPE_NEIGHBORS_POSITIONS:
        plugin = this->getBrowser()->getNeighborsPositionsPlugin();
        break;
    case PLUGIN_TYPE_SEGMENTATION:
        names = this->getPluginManager()->getActiveSegmentPluginsNames(mediaType);
        break;
    case PLUGIN_TYPE_FEATURES:
        names = this->getPluginManager()->getAvailablePluginsNames(pluginType,mediaType); // might change with future active features
        break;
    case PLUGIN_TYPE_MEDIAREADER:
        names = this->getPluginManager()->getAvailablePluginsNames(pluginType,mediaType);
        break;
    case PLUGIN_TYPE_MEDIARENDERER:
        names = this->getPluginManager()->getAvailablePluginsNames(pluginType,mediaType);
        break;
    case PLUGIN_TYPE_PREPROCESS:
        plugin = this->getLibrary()->getPreProcessPlugin();
        break;
    case PLUGIN_TYPE_FILTERING:
        plugin = this->getBrowser()->getFilteringPlugin();
        break;
    default:
        names = this->getPluginManager()->getAvailablePluginsNames(pluginType,mediaType);
        break;
    }
    if(plugin){
        names.push_back(plugin->getName());
    }
    return names;
}

std::vector<std::string> MediaCycle::getAvailablePluginNames(ACPluginType pluginType,ACMediaType mediaType){
    std::vector<std::string> names;
    if(!this->getPluginManager()){
        std::cerr << "MediaCycle::getAvailablePluginNames: no plugin manager" << std::endl;
        return names;
    }
    if(!this->getBrowser()){
        std::cerr << "MediaCycle::getAvailablePluginNames: no browser" << std::endl;
        return names;
    }
    return this->getPluginManager()->getAvailablePluginsNames(pluginType,mediaType);
}

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
    if(this->mediaLibrary == 0)
        return "";
    return (mediaLibrary->getActiveSubMediaKey());
    
}

int MediaCycle::setActiveMediaType(std::string mediaName){
    int ret = -1;
    if(mediaLibrary && pluginManager){
        ret = mediaLibrary->setActiveMediaType(mediaName,pluginManager);
    }
    return ret ;
}
#endif//def USE_MULTIMEDIA

void MediaCycle::dumpPluginsList(){
    if(pluginManager){
        this->pluginManager->dump();
    }
}

// == Media
ACMediaNode* MediaCycle::getMediaNode(int i){
    if (mediaBrowser){
        return (mediaBrowser->getMediaNode(i));
    }
    else{
        return 0;
    }
}

ACMediaNode* MediaCycle::getNodeFromMedia(ACMedia* _media){
    if (mediaBrowser){
        return (mediaBrowser->getNodeFromMedia(_media));
    }
    else{
        return 0;
    }
}

string MediaCycle::getMediaFileName(int i){
    if (mediaLibrary){
        return (mediaLibrary->getMedia(i) ? mediaLibrary->getMedia(i)->getFileName():string(""));
    }
    else{
        return "";
    }
}

ACMediaType MediaCycle::getMediaType(int i){
    if (mediaLibrary){
        return mediaLibrary->getMedia(i)->getType();
    }
    else{
        return MEDIA_TYPE_NONE;
    }
}

bool MediaCycle::mediaIsTagged(int i) {
    if(mediaLibrary){
        return (mediaLibrary->getMedia(i)?mediaLibrary->getMedia(i)->isTagged():false);
    }
    else{
        return false;
    }
}

void MediaCycle::setMediaType(ACMediaType mt){
    if(mediaLibrary){
        mediaLibrary->setMediaType(mt);
    }
}

bool MediaCycle::changeMediaType(ACMediaType aMediaType) {
    bool changeMe = true ;
    if (aMediaType == this->getMediaType())
        // nothing to change
        changeMe = false;
    else{
        if(this->getMediaType()!=MEDIA_TYPE_NONE)
            this->clean();
        this->mediaLibrary->changeMediaType(aMediaType);
        this->setMediaType(aMediaType);
    }
    return changeMe;
}

int MediaCycle::getWidth(int i){
    if (mediaLibrary){
        return mediaLibrary->getMedia(i)->getWidth();
    }
    else{
        return -1;
    }
}

int MediaCycle::getHeight(int i){
    if (mediaLibrary){
        return mediaLibrary->getMedia(i)->getHeight();
    }
    else{
        return -1;
    }
}

int MediaCycle::getNeedsDisplay(){
    if (mediaLibrary){
        return mediaBrowser->getNeedsDisplay();
    }
    else{
        return -1;
    }
}

void MediaCycle::setNeedsDisplay(bool _dis) {
    if (mediaBrowser){
        mediaBrowser->setNeedsDisplay(_dis);
    }
    eventManager->sig_updateDisplayNeeded();
}

int MediaCycle::getNeedsDisplay3D(){return mNeedsDisplay;}

void MediaCycle::setNeedsDisplay3D(bool mNeedsDisplay){this->mNeedsDisplay = mNeedsDisplay;}

// == view
float MediaCycle::getCameraZoom(){
    if(mediaBrowser){
        return mediaBrowser->getCameraZoom();
    }
    else{
        return 0.0f;
    }
}

float MediaCycle::getCameraRotation(){
    if(mediaBrowser){
        return mediaBrowser->getCameraRotation();
    }
    else{
        return 0.0f;
    }
}

void MediaCycle::setCameraRotation(float angle){
    if(mediaBrowser){
        mediaBrowser->setCameraRotation(angle);
    }
}

void MediaCycle::setCameraPosition(float x, float y){
    if(mediaBrowser){
        mediaBrowser->setCameraPosition(x,y);
    }
}

void MediaCycle::getCameraPosition(float &x, float &y){
    if(mediaBrowser){
        mediaBrowser->getCameraPosition(x,y);
    }
}

void MediaCycle::setCameraZoom(float z)	{
    if(mediaBrowser){
        mediaBrowser->setCameraZoom(z);
    }
}

void MediaCycle::setCameraRecenter(){
    if(mediaBrowser){
        mediaBrowser->setCameraRecenter();
    }
}

void MediaCycle::setAutoPlay(int i) {
    // Block media actions while importing
    if(this->importing)
        return;
    if(this->getLibrarySize()==0)
        return;
    if(mediaBrowser){
        mediaBrowser->setAutoPlay(i);
    }
}

bool MediaCycle::getAutoPlay(){
    if(mediaBrowser){
        return mediaBrowser->getAutoPlay();
    }
    else{
        return false;
    }
}

void MediaCycle::setAutoDiscard(bool status){
    if(mediaBrowser){
        mediaBrowser->setAutoDiscard(status);
    }
}

bool MediaCycle::getAutoDiscard(){
    if(mediaBrowser){
        return mediaBrowser->getAutoDiscard();
    }
    else{
        return false;
    }
}

int MediaCycle::getClickedNode(){
    // Block media actions while importing
    if(this->importing)
        return -1;
    if(mediaBrowser){
        return mediaBrowser->getClickedNode();
    }
    else{
        return -1;
    }
}

void MediaCycle::setClickedNode(int i){
    if(mediaBrowser){
        mediaBrowser->setClickedNode(i);
    }
}

void MediaCycle::setClosestNode(int i,int p_index) {
    int current = mediaBrowser->getClosestNode(p_index);
    std::map<long int,int> nodeActivities = mediaBrowser->setClosestNode(i,p_index);
    if( nodeActivities.size()>0){// && current != i){
        
        for(std::map<long int,int>::iterator nodeActivity = nodeActivities.begin(); nodeActivity != nodeActivities.end(); nodeActivity++){
            if(nodeActivity->second == 2)
                this->performActionOnMedia("hover closest node", nodeActivity->first);
            else if(nodeActivity->second == 0)
                this->performActionOnMedia("hover off node", nodeActivity->first);
        }
    }
    if(current != i){
        std::vector<boost::any> arguments;
        arguments.push_back(p_index);
        this->performActionOnMedia("hover closest node",i,arguments);
    }
}

int MediaCycle::getClosestNode(int p_index){
    // Block media actions while importing
    if(this->importing)
        return -1;
    if(mediaBrowser){
        return mediaBrowser->getClosestNode(p_index);
    }
    else{
        return -1;
    }
}

void MediaCycle::setClosestDistance(float d,int p_index){
    mediaBrowser->setClosestDistance(d,p_index);
}

float MediaCycle::getClosestDistance(int p_index){
    return mediaBrowser->getClosestDistance(p_index);
}

int	MediaCycle::getLastSelectedNode(){
    // Block media actions while importing
    if(this->importing)
        return -1;
    if(mediaBrowser){
        return mediaBrowser->getLastSelectedNode();
    }
    else{
        return -1;
    }
}

// == Cluster Display
void MediaCycle::updateState(){
    if(mediaBrowser){
        mediaBrowser->updateState();
    }
}

int MediaCycle::getNavigationLevel(){
    if(mediaBrowser){
        return mediaBrowser->getNavigationLevel();
    }
    else{
        return -1;
    }
}

float MediaCycle::getFrac(){
    if(mediaBrowser){
        return mediaBrowser->getFrac();
    }
    else{
        return -1;
    }
}

void MediaCycle::incrementNavigationLevels(int i){
    if(mediaBrowser){
        mediaBrowser->incrementNavigationLevels(i);
    }
}

void MediaCycle::setReferenceNode(int index){
    if(mediaBrowser){
        mediaBrowser->setReferenceNode(index);
    }
}

int MediaCycle::getReferenceNode(){
    if(mediaBrowser){
        return mediaBrowser->getReferenceNode();
    }
    else{
        return -1;
    }
}

void MediaCycle::goBack(){
    if(mediaBrowser){
        mediaBrowser->goBack();
    }
}

void MediaCycle::goForward(){
    if(mediaBrowser){
        mediaBrowser->goForward();
    }
}

void MediaCycle::storeNavigationState(){
    if(mediaBrowser){
        mediaBrowser->storeNavigationState();
    }
}

void MediaCycle::setClusterNumber(int n) {
    if(mediaBrowser){
        mediaBrowser->setClusterNumber(n);
    }
}

int MediaCycle::getClusterNumber() {
    if(mediaBrowser){
        return mediaBrowser->getClusterNumber();
    }
    return 0;
}

void MediaCycle::setWeight(int i, float weight) {
    if(mediaBrowser){
        mediaBrowser->setWeight(i, weight);
    }
}

int MediaCycle::setWeight(std::string featureName, float weight) {
    int i=mediaLibrary->getFeatureIndex(featureName);
    cout << "Index of feature to modify: " << i << " ; new weight: " << weight << endl;
    if(i>=0&&mediaBrowser){
        mediaBrowser->setWeight(i, weight);
    }
    return i;
}

void MediaCycle::setWeight(std::string argumentsString)
{
    std::string featureName, tmp;
    float weight;
    size_t pos;
    while(argumentsString.length())
    {
        pos=argumentsString.find_first_of(" ");
        if(pos==string::npos)
        {
            return;
        }
        featureName=argumentsString.substr(0,pos);
        argumentsString.erase(0,pos+1);
        pos=argumentsString.find_first_of(" ");
        if(pos==string::npos)
        {
            tmp=argumentsString;
            argumentsString.clear();
        }
        else
        {
            tmp=argumentsString.substr(0,pos);
            argumentsString.erase(0,pos+1);
        }
        stringstream value;
        value << tmp;
        value >> weight;
        cout << "Featurename: " << featureName << "Weight: " << weight << endl;
        if(weight>=0&&weight<=1)
        {
            this->setWeight(featureName,weight);
        }
    }
}

void MediaCycle::setWeightVector(std::vector<float> fw){
    if(mediaBrowser){
        mediaBrowser->setWeightVector(fw);
    }
}

vector<float> MediaCycle::getWeightVector(){
    if(mediaBrowser){
        return mediaBrowser->getWeightVector();
    }
    else{
        return vector<float>();
    }
}

float MediaCycle::getWeight(int i){
    if(mediaBrowser){
        return mediaBrowser->getWeight(i);
    }
    else{
        return 0.0f;
    }
}

void MediaCycle::setForwardDown(int i) { forwarddown = i;}

void MediaCycle::forwardNextLevel(){
    // enters in the cluster of the last selected node
    if (this->hasBrowser()){
        this->setForwardDown(true); // mediaBrowser->getMode() == AC_MODE_NEIGHBORS
        this->getBrowser()->forwardNextLevel();
        this->setForwardDown(false);
    }
}
void MediaCycle::changeReferenceNode(){
    // enters in the cluster of the last selected node
    if (this->hasBrowser()){
        this->setForwardDown(true); // mediaBrowser->getMode() == AC_MODE_NEIGHBORS
        this->getBrowser()->changeReferenceNode();
        this->setForwardDown(false);
    }
}

void MediaCycle::changeSelectedMediaTagId(int ClusterId){
    int media_id = this->getClosestNode();
    if (mediaBrowser->getMediaNode(media_id)){
        if (mediaLibrary->getMediaTaggedClassId(media_id)!=ClusterId){
            std::cout << "MediaCycle::changeSelectedMediaTagId "<<ClusterId<<"for mediaId"<<media_id << std::endl;
            this->setMediaTaggedClassId(media_id, ClusterId);
            this->updateDisplay(false);
        }
    }
    
}

void MediaCycle::transferClassToTag(){
    bool changed=false;
    ACMediaNodes nodes=mediaBrowser->getMediaNodes();
    for (ACMediaNodes::iterator it=nodes.begin();it!=nodes.end();it++){
        if ((it->second->getClusterId()!=-1)&&it->second->getClusterId()!=mediaLibrary->getMediaTaggedClassId(it->first)){
            if (it->second){
                this->setMediaTaggedClassId(it->first, it->second->getClusterId());
                changed=true;
            }
        }
    }
    if (changed)
        this->updateDisplay(false);
}

void MediaCycle::cleanAllMediaTag(){
    bool changed=false;
    ACMedias medias=mediaLibrary->getAllMedia();
    for (ACMedias::iterator it=medias.begin();it!=medias.end();it++){
        if ((it->second)&&(it->second->getTaggedClassId()!=-1)){
            it->second->setTaggedClassId(-1);
            changed=true;
            
        }
    }
    if (changed)
        this->updateDisplay(false);
}


// == Features
void MediaCycle::normalizeFeatures(int needsNormalize) { mediaLibrary->normalizeFeatures(needsNormalize);}
// Get Features Vector (identified by feature_name) in media i
FeaturesVector MediaCycle::getFeaturesVectorInMedia(int i, string feature_name) {
    ACMedia* lmedia;
    ACMediaFeatures* lfeatures;
    FeaturesVector lfeaturesvector;
    lmedia = mediaLibrary->getMedia(i);
    if (lmedia==0)
        lfeatures=0;
    else
        lfeatures = lmedia->getPreProcFeaturesVector(feature_name);
    if (lfeatures) {
        lfeaturesvector = lfeatures->getFeaturesVector();
    }
    return lfeaturesvector;
}

// == Playing time stamp
void MediaCycle::setSourceCursor(int lid, int frame_pos) {
    if(mediaBrowser){
        mediaBrowser->setSourceCursor(lid, frame_pos);
    }
}

void MediaCycle::setCurrentFrame(int lid, int frame_pos) {
    if(mediaBrowser){
        mediaBrowser->setCurrentFrame(lid, frame_pos);
    }
}

void MediaCycle::muteAllSources(){
    if(mediaBrowser){
        mediaBrowser->muteAllSources();
        if(this->getLibrarySize()>0)
            this->performActionOnMedia("mute all",-1);
    }
}

// == POINTERS on VIEW
int MediaCycle::getNumberOfPointers() {
    // Block media actions while importing
    if(this->importing)
        return 0;
    if(mediaBrowser){
        return mediaBrowser->getNumberOfPointers();
    }
    else{
        return 0;
    }
}

ACPointer* MediaCycle::getPointerFromIndex(int i) {
    // Block media actions while importing
    if(this->importing)
        return 0;
    if(mediaBrowser){
        return mediaBrowser->getPointerFromIndex(i);
    }
    else{
        return 0;
    }
}

ACPointer* MediaCycle::getPointerFromId(int i) {
    // Block media actions while importing
    if(this->importing)
        return 0;
    if(mediaBrowser){
        return mediaBrowser->getPointerFromId(i);
    }
    else{
        return 0;
    }
}

void MediaCycle::resetPointers() {
    // Block media actions while importing
    if(this->importing)
        return;
    if(mediaBrowser){
        mediaBrowser->resetPointers();
    }
}

std::list<int> MediaCycle::getPointerIds() {
    // Block media actions while importing
    if(this->importing)
        return std::list<int>();;
    if(mediaBrowser){
        return mediaBrowser->getPointerIds();
    }
    else{
        return std::list<int>();
    }
}

void MediaCycle::addPointer(int p_id) {
    // Block media actions while importing
    if(this->importing)
        return;
    if(mediaBrowser){
        mediaBrowser->addPointer(p_id);
    }
}

void MediaCycle::removePointer(int p_id) {
    // Block media actions while importing
    if(this->importing)
        return;
    ACPointer* pointer = this->getPointerFromId(p_id);
    if(pointer){
        int closest = pointer->getClosestNode();
        if(closest != -1)
            this->performActionOnMedia("hover off node",closest);
        if(mediaBrowser)
            mediaBrowser->removePointer(p_id);
    }
}

// == LABELS on VIEW
int MediaCycle::getLabelSize(){
    if(mediaBrowser){
        return mediaBrowser->getLabelSize();
    }
    else{
        return -1;
    }
}

string MediaCycle::getLabelText(int i){
    if(mediaBrowser){
        return mediaBrowser->getLabelText(i);
    }
    else{
        return "";
    }
}

ACPoint MediaCycle::getLabelPos(int i){
    if(mediaBrowser){
        return mediaBrowser->getLabelPos(i);
    }
    else{
        return ACPoint();
    }
}


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
    return 0;
}
// == callbacks

void MediaCycle::pickedObjectCallback(int _mediaId) {
    std::cout << "MediaCycle::pickedObjectCallback node id " << _mediaId <<" cluster id "<<mediaBrowser->getMediaNode(_mediaId)->getClusterId() << std::endl;
    std::cout << "MediaCycle::pickedObjectCallback: tagged Class:"<< mediaLibrary->getMediaTaggedClassId(_mediaId)<<" file " << this->getMediaFileName( _mediaId ) << std::endl;
    if(_mediaId < 0) {
        // clicked close to a node
        if (getNumberOfPointers() > 0)
            _mediaId = getClosestNode();
    }
    mediaBrowser->setClickedNode(_mediaId);
    if (forwarddown == 0){// & playkeydown) {//if (!forwarddown) { //CF forwardown is not a boolean
        //this->performActionOnMedia("play", _mediaId);
        mediaBrowser->toggleSourceActivity(_mediaId);
    }
    
    // with observer pattern
    this->notify();
    
}

void MediaCycle::hoverWithPointerId(float xx, float yy, int p_id) {
    // Block media actions while importing
    if(this->importing)
        return;
    if (!this->mediaBrowser)
        return;
    mediaBrowser->hoverWithPointerId(xx, yy, p_id);
    std::vector<boost::any> arguments;
    arguments.push_back(p_id);
    arguments.push_back(xx);
    arguments.push_back(yy);
    if(this->getLibrarySize()>0)
        this->performActionOnMedia("hover pointer id",-1,arguments);
}

void MediaCycle::hoverWithPointerIndex(float xx, float yy, int p_index) {
    // Block media actions while importing
    if(this->importing)
        return;
    if (!this->mediaBrowser)
        return;
    mediaBrowser->hoverWithPointerIndex(xx, yy, p_index);
    std::vector<boost::any> arguments;
    arguments.push_back(p_index);
    arguments.push_back(xx);
    arguments.push_back(yy);
    if(this->getLibrarySize()>0)
        this->performActionOnMedia("hover pointer index",-1,arguments);
}

bool MediaCycle::performActionOnMedia(std::string action, long int mediaId, std::vector<boost::any> arguments){
    if(!pluginManager){
        std::cerr << "MediaCycle::performActionOnMedia: plugin manager not set" << std::endl;
        return false;
    }
    
    // Block media actions while importing
    if(this->importing)
        return false;
    
    if(this->eventManager)
        eventManager->sig_mediaActionPerformed(action, mediaId, arguments);
    
    bool renderers_passed = true;
    std::vector<std::string> renderer_plugins = pluginManager->getAvailablePluginsNames(PLUGIN_TYPE_MEDIARENDERER, this->getMediaType());
    for(std::vector<std::string>::iterator renderer_plugin = renderer_plugins.begin();renderer_plugin!=renderer_plugins.end();renderer_plugin++){
        ACMediaRendererPlugin* plugin = dynamic_cast<ACMediaRendererPlugin*>(pluginManager->getPlugin(*renderer_plugin));
        if(plugin)
            renderers_passed &= plugin->performActionOnMedia(action,mediaId,arguments);
    }
    std::vector<std::string> client_plugins = pluginManager->getAvailablePluginsNames(PLUGIN_TYPE_CLIENT, this->getMediaType());
    bool clients_passed = true;
    for(std::vector<std::string>::iterator client_plugin = client_plugins.begin();client_plugin!=client_plugins.end();client_plugin++){
        ACClientPlugin* plugin = dynamic_cast<ACClientPlugin*>(pluginManager->getPlugin(*client_plugin));
        if(plugin)
            clients_passed &= plugin->performActionOnMedia(action,mediaId,arguments);
    }
    return renderers_passed && clients_passed;
}


bool MediaCycle::performActionOnMedia(std::string action, long int mediaId, std::string argument){
    std::vector<boost::any> arguments;
    arguments.push_back(argument);
    return this->performActionOnMedia(action,mediaId,arguments);
}

bool MediaCycle::performActionOnMedia(std::string action, long int mediaId, int argument){
    std::vector<boost::any> arguments;
    arguments.push_back(argument);
    return this->performActionOnMedia(action,mediaId,arguments);
}

bool MediaCycle::performActionOnMedia(std::string action, long int mediaId, float argument){
    std::vector<boost::any> arguments;
    arguments.push_back(argument);
    return this->performActionOnMedia(action,mediaId,arguments);
}

void MediaCycle::updateDisplay(bool _animate) {
    if(mediaBrowser){
        mediaBrowser->updateDisplay(_animate);
    }
    eventManager->sig_updateDisplayNeeded();
}

void MediaCycle::initializeFeatureWeights(){
    if(mediaBrowser){
        mediaBrowser->initializeFeatureWeights();
    }
}

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
    
    // Camera
    TiXmlElement* nCameraNode=rootHandle.FirstChild( "Camera" ).Element();
    if(nCameraNode){
        if(nCameraNode->Attribute("Position")){
            std::stringstream strm;
            strm << nCameraNode->Attribute("Position");
            float x(0.0f),y(0.0f);
            strm >> x;
            strm >> y;
            this->mediaBrowser->setCameraPosition(x,y);
        }
        if(nCameraNode->Attribute("Zoom")){
            std::stringstream strm;
            strm << nCameraNode->Attribute("Zoom");
            float zoom(1.0f);
            strm >> zoom;
            this->mediaBrowser->setCameraZoom(zoom);
        }
        if(nCameraNode->Attribute("Rotation")){
            std::stringstream strm;
            strm << nCameraNode->Attribute("Rotation");
            float rotation(0.0f);
            strm >> rotation;
            this->mediaBrowser->setCameraRotation(rotation);
        }
    }
    
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
    
    // Camera
    TiXmlElement* nCameraNode=_rootHandle.FirstChild( "Camera" ).Element();
    if(nCameraNode){
        if(nCameraNode->Attribute("Position")){
            std::stringstream strm;
            strm << nCameraNode->Attribute("Position");
            float x(0.0f),y(0.0f);
            strm >> x;
            strm >> y;
            this->mediaBrowser->setCameraPosition(x,y);
        }
        if(nCameraNode->Attribute("Zoom")){
            std::stringstream strm;
            strm << nCameraNode->Attribute("Zoom");
            float zoom(1.0f);
            strm >> zoom;
            this->mediaBrowser->setCameraZoom(zoom);
        }
        if(nCameraNode->Attribute("Rotation")){
            std::stringstream strm;
            strm << nCameraNode->Attribute("Rotation");
            float rotation(0.0f);
            strm >> rotation;
            this->mediaBrowser->setCameraRotation(rotation);
        }
    }
    
    TiXmlElement* media_element = this->mediaLibrary->openCoreXMLLibrary(_rootHandle);
    
    eventManager->sig_mediaImported(0,this->mediaLibrary->getNumberOfFilesToImport(),-1);
    
    int needsNormalizeAndCluster = 0;
    float prevLibrarySizeMultiplier = 2;
    
    bool with_thumbnails = true;
    if(this->getCurrentConfig() && this->getCurrentConfig()->loadThumbnails() == false)
        with_thumbnails = false;
    
    int beginIndex=this->mediaLibrary->getNewestMediaId();
    while(  media_element != 0 ){
        //mediaBrowser->setNeedsNavigationUpdateLock(1);
        //media_element = this->mediaLibrary->openNextMediaFromXMLLibrary(media_element);
        try{
            media_element = this->mediaLibrary->openNextMediaFromXMLLibrary(media_element,with_thumbnails);
        }
        catch (const exception& e) {
            cout << "MediaCycle::readXMLConfigFileCore: error: " << e.what( ) << endl;
            int n = this->mediaLibrary->getSize(); // segmentation might have increased the number of medias in the library
            // TODO organize runtime error values to filter them and decide if:
            // a) the library/browser/renderer should be cleaned  (the whole library is corrupted)
            // b) or left as is with the last successfully imported file (some files are corrupted)
            // (comparing the number of files to import vs the library size might be misleading, especially with documents and/or segments)
            // For now we abort, since the renderer gives a clue on the number of imported elements.
            eventManager->sig_mediaImported(n,n,-1);
            std::string error = std::string(e.what()) + ", aborting import.";
            throw runtime_error(error);
        }
    }
    int lastIndex=this->mediaLibrary->getNewestMediaId();
    
    long int media_id = this->mediaLibrary->getNewestMediaId();
    int i = this->mediaLibrary->getNumberOfFilesProcessed();
    int n = this->mediaLibrary->getNumberOfFilesToImport();
    
    //   needsNormalizeAndCluster = 0;
    //if ( (mediaLibrary->getSize() >= int(prevLibrarySizeMultiplier * prevLibrarySize))
    //   || (i==n-1)) {
    needsNormalizeAndCluster = 1;
    //}
    normalizeFeatures(needsNormalizeAndCluster); // exclusively medialibrary
    //mediaBrowser->setNeedsNavigationUpdateLock(1);
    for (int newId=beginIndex+1;newId<=lastIndex;newId++){
        mediaBrowser->initializeNode(newId);
    }
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
    //std::cout<<"MediaCycle::readXMLConfigFileCore:"<<this->mediaLibrary->getNumberOfFilesProcessed()<<"/"<<this->mediaLibrary->getNumberOfFilesToImport()<<"("<<media_id<<")"<<std::endl;
    vector<int> locIds;
    for (int newId=beginIndex+1;newId<=lastIndex;newId++){
        locIds.push_back( newId);
        //        eventManager->sig_mediaImported(this->mediaLibrary->getNumberOfFilesProcessed(),this->mediaLibrary->getNumberOfFilesToImport(),newId);
    }
    eventManager->sig_mediasImported(this->mediaLibrary->getNumberOfFilesProcessed(),this->mediaLibrary->getNumberOfFilesToImport(),locIds);
    
    n = this->mediaLibrary->getSize(); // segmentation might have increased the number of medias in the library
    
    this->mediaImported(n,n,-1);
    return 1;
}

// XS TODO return value, tests
int MediaCycle::readXMLConfigFilePlugins(TiXmlHandle _rootHandle) {
    //if (!this->pluginManager) this->pluginManager = new ACPluginManager();
    this->pluginManager->setMediaCycle(this);
    
    TiXmlElement* MC_e_plugin_manager = _rootHandle.FirstChild("PluginsManager").ToElement();
    int nb_plugins_lib=0;
    if (MC_e_plugin_manager!=0){
        MC_e_plugin_manager->QueryIntAttribute("NumberOfPluginsLibraries", &nb_plugins_lib);
        
        //this->pluginManager->clean();
        //this->pluginManager->setMediaCycle(this);
        //this->mediaBrowser->changeClustersMethodPlugin( this->pluginManager->getPlugin("MediaCycle KMeans") );
        //this->mediaBrowser->changeClustersPositionsPlugin( this->pluginManager->getPlugin("MediaCycle Propeller") );
        
        TiXmlElement* pluginLibraryNode=MC_e_plugin_manager->FirstChild()->ToElement();
        for( pluginLibraryNode; pluginLibraryNode; pluginLibraryNode=pluginLibraryNode->NextSiblingElement()) {
            string libraryName("");
            if(pluginLibraryNode->Attribute("LibraryPath"))
                libraryName = pluginLibraryNode->Attribute("LibraryPath");
            else if(pluginLibraryNode->Attribute("Name"))
                libraryName = pluginLibraryNode->Attribute("Name");
            if(libraryName!=""){
                int lib_size=0;
                pluginLibraryNode->QueryIntAttribute("NumberOfPlugins", &lib_size);
                std::vector<std::string> plugins_names;
                //#if defined(__APPLE__) && !defined(DEBUG)
                std::cout << "Trying to load bundled plugin " << fs::basename(libraryName) << std::endl;
                this->pluginManager->addLibrary( this->getPluginPathFromBaseName(fs::basename(libraryName)));
                //plugins_names = this->pluginManager->getLibraryPluginNames(fs::basename(libraryName));
                //#else
                //                plugins_names = this->pluginManager->addLibrary(libraryName);
                //if(plugins_names.size() == 0){
                //    plugins_names = this->pluginManager->addLibrary( this->getPluginPathFromBaseName(fs::basename(libraryName)));
                //#endif
                //if(plugins_names.size()>0){
                
                TiXmlElement* pluginNode=pluginLibraryNode->FirstChild()->ToElement();
                for( pluginNode; pluginNode; pluginNode=pluginNode->NextSiblingElement()) {
                    string pluginName("");
                    if(pluginNode->Attribute("Name")){
                        std::string _plugin_name = pluginNode->Attribute("Name");
                        ACPlugin* _plugin = this->pluginManager->getPlugin(_plugin_name);
                        if(_plugin){
                            TiXmlNode* numberParametersNode=pluginNode->FirstChild("NumberParameters");
                            if(numberParametersNode){
                                TiXmlElement* numberParameters=pluginNode->FirstChild("NumberParameters")->ToElement();
                                TiXmlElement* numberParameter=numberParameters->FirstChild()->ToElement();
                                for( numberParameter; numberParameter; numberParameter=numberParameter->NextSiblingElement()) {
                                    if(numberParameter->Attribute("Name") && numberParameter->Attribute("Value")){
                                        double value = -1;
                                        bool success = (numberParameter->QueryDoubleAttribute("Value",&value) == TIXML_SUCCESS);
                                        if(success && _plugin->getNumberParameterValue(numberParameter->Attribute("Name"))!= value)
                                            _plugin->setNumberParameterValue(numberParameter->Attribute("Name"),value);
                                    }
                                }
                            }
                            TiXmlNode* stringParametersNode=pluginNode->FirstChild("StringParameters");
                            if(stringParametersNode){
                                TiXmlElement* stringParameters=pluginNode->FirstChild("StringParameters")->ToElement();
                                TiXmlElement* stringParameter=stringParameters->FirstChild()->ToElement();
                                for( stringParameter; stringParameter; stringParameter=stringParameter->NextSiblingElement()) {
                                    if(stringParameter->Attribute("Name") && stringParameter->Attribute("Value")){
                                        string _name = stringParameter->Attribute("Name");
                                        string _value = stringParameter->Attribute("Value");
                                        if(_plugin->getStringParameterValue(_name) != _value)
                                            _plugin->setStringParameterValue(_name,_value);
                                        std::cout << "Plugin " << _plugin_name << " parameter " << stringParameter->Attribute("Name") << " value " << stringParameter->Attribute("Value") << std::endl;
                                    }
                                }
                            }
                            eventManager->sig_pluginLoaded(_plugin_name);
                        }
                    }
                    
                    //#if defined(__APPLE__) && !defined(DEBUG)
                    //#else
                    //}
                    //#endif
                    //
                    //for(std::vector<std::string>::iterator plugin_name = plugins_names.begin();plugin_name!=plugins_names.end();plugin_name++){
                    //    eventManager->sig_pluginLoaded(*plugin_name);
                    //}
                }
                //}
            }
        }
        if(this->getLibrarySize()==0){
            TiXmlElement* MC_e_active_plugins = _rootHandle.FirstChild("ActivePlugins").Element();
            if(MC_e_active_plugins){
                TiXmlElement* MC_e_active_plugin = MC_e_active_plugins->FirstChild()->ToElement();
                for( MC_e_active_plugin; MC_e_active_plugin; MC_e_active_plugin=MC_e_active_plugin->NextSiblingElement()) {
                    std::string type = MC_e_active_plugin->ValueStr();
                    if(MC_e_active_plugin){
                        try{
                            ACPlugin* plugin = this->getPluginManager()->getPlugin( MC_e_active_plugin->GetText() );
                            if(type == "ClustersMethod")
                                this->getBrowser()->setClustersMethodPlugin(plugin);
                            if(type == "ClustersPositions")
                                this->getBrowser()->setClustersPositionsPlugin(plugin);
                            if(type == "NeighborsMethod")
                                this->getBrowser()->setNeighborsMethodPlugin(plugin);
                            if(type == "NeighborsPositions")
                                this->getBrowser()->setNeighborsPositionsPlugin(plugin);
                            if(type == "Filtering")
                                this->getBrowser()->setFilteringPlugin(plugin);
                        }
                        catch(const std::exception e){
                            // Nothing yet, no plugin set
                        }
                    }
                }
            }
            
        }
    }
    return 1;
}

// XS TODO return value, tests
int MediaCycle::readXMLConfigFile(string _fname) {
    TiXmlHandle rootHandle = readXMLConfigFileHeader (_fname);
    return this->readXMLConfigFileCore (rootHandle);
}

std::string MediaCycle::getPluginPathFromBaseName(std::string basename)
{
    
    //std::string s_path = QApplication::applicationDirPath().toStdString();
    //std::cout << "Qt app path " << s_path << std::endl;
    
#ifdef __APPLE__
    //std::cout << "Executable path '" << getExecutablePath() << "'" << std::endl;
    boost::filesystem::path e_path( getExecutablePath() );
    std::string r_path = e_path.parent_path().parent_path().string() + "/Resources/";
    //std::cout << "Resources path " << r_path << std::endl;
#endif
    
    // Add the path to the makam toolbox and yin mex files
    //boost::filesystem::path s_path( __FILE__ );
    //std::cout << "Main source path: " << s_path.parent_path().parent_path().parent_path() << std::endl;
    boost::filesystem::path b_path( boost::filesystem::current_path() );
    //std::cout << "Main build path " << b_path << std::endl;
    
    std::string prefix("mc_");
    size_t found = basename.find(prefix);
    if(found != std::string::npos){
        basename = basename.substr(found+prefix.size());
        //std::cout << "MediaCycle::getPluginPathFromBaseName: new basename: " << basename << std::endl;
    }
    
    char c_path[2048];
    // use the function to get the path
    getcwd(c_path, 2048);
    boost::filesystem::path s_path(c_path);
    std::string plugins_path(""),plugin_subfolder(""),plugin_ext("");
    //std::cout << "Current path " << s_path << std::endl;
    
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
#if defined(XCODE_OLD)
    plugins_path = s_path.parent_path().parent_path().parent_path().string() + "/plugins/";
#else
    plugins_path = s_path.parent_path().parent_path().string() + "/plugins/";
#endif
    plugin_subfolder = basename + "/" + build_type + "/";
#else
    plugins_path = s_path.parent_path().parent_path().string() + "/plugins/";
    plugin_subfolder = basename + "/";
#endif
#endif
#elif defined (__WIN32__)
    plugin_ext = ".dll";
    plugins_path = s_path.string() + "/";
#ifdef USE_DEBUG
    plugins_path = s_path.parent_path().parent_path().string() + "/plugins/";
    plugin_subfolder = basename + "/";
#endif
#else // Linux
    plugin_ext = ".so";
#if not defined (USE_DEBUG) // needs "make package" to be ran to work
    plugins_path = "/usr/lib/"; // or a prefix path from CMake?
    plugin_subfolder = "";
#else
    // JU modified for debugging in Netbeans: 
    plugins_path = s_path.parent_path().parent_path().string() + "/plugins/";
    //plugins_path = s_path.string() + "/Builds/linux-x86/plugins/";
    plugin_subfolder = basename + "/";
#endif
#endif
    return plugins_path + plugin_subfolder + prefix + basename + plugin_ext;
}

std::string MediaCycle::getLibraryPathFromPlugin(std::string name){
    std::string path("");
    if(this->pluginManager)
        path = this->pluginManager->getLibraryPathFromPlugin(name);
    return path;
}

// XS TODO what else to put in the config ?
// XS TODO separate in header/core/plugins ?
TiXmlElement* MediaCycle::saveXMLConfigFile(string _fname) {
    // or set it to config_file_xml ?
    TiXmlDocument MC_doc(_fname);
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
    
    // Camera
    TiXmlElement* MC_e_camera = new TiXmlElement("Camera");
    MC_e_root->LinkEndChild(  MC_e_camera );
    float cx,cy;
    mediaBrowser->getCameraPosition(cx,cy);
    std::stringstream cpstrm,czstrm,crstrm;
    cpstrm << cx << " " << cy;
    MC_e_camera->SetAttribute("Position", cpstrm.str());
    czstrm << mediaBrowser->getCameraZoom();
    MC_e_camera->SetAttribute("Zoom", czstrm.str());
    crstrm << mediaBrowser->getCameraRotation();
    MC_e_camera->SetAttribute("Rotation", crstrm.str());
    
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
        TiXmlElement* MC_e_plugin_manager = new TiXmlElement( "PluginsManager" );
        MC_e_root->LinkEndChild( MC_e_plugin_manager );
        int n_librarires = pluginManager->getSize();
        MC_e_plugin_manager->SetAttribute("NumberOfPluginsLibraries", n_librarires);
        for (int i=0; i<n_librarires; i++) {
            TiXmlElement* MC_e_plugin_library = new TiXmlElement( "PluginLibrary" );
            MC_e_plugin_manager->LinkEndChild( MC_e_plugin_library );
            int n_plugins = pluginManager->getPluginLibrary(i)->getSize();
            MC_e_plugin_library->SetAttribute("NumberOfPlugins", n_plugins);
            MC_e_plugin_library->SetAttribute("LibraryPath", pluginManager->getPluginLibrary(i)->getLibraryPath());
            for (int j=0; j<n_plugins; j++) {
                ACPlugin* _plugin = pluginManager->getPluginLibrary(i)->getPlugin(j);
                if(_plugin){
                    TiXmlElement* MC_e_plugin = new TiXmlElement( "Plugin" );
                    MC_e_plugin_library->LinkEndChild( MC_e_plugin );
                    MC_e_plugin->SetAttribute("Name",_plugin->getName());
                    /*std::stringstream tmp_p;
                    tmp_p << _plugin->getName() ;
                    TiXmlText* MC_t_pm = new TiXmlText( tmp_p.str() );
                    MC_e_plugin->LinkEndChild( MC_t_pm );*/
                    
                    std::vector<std::string> number_param_names = _plugin->getNumberParametersNames();
                    if(number_param_names.size()>0){
                        TiXmlElement* MC_e_number_params = new TiXmlElement( "NumberParameters" );
                        MC_e_number_params->SetAttribute("Size",number_param_names.size());
                        MC_e_plugin->LinkEndChild( MC_e_number_params );
                        for (std::vector<std::string>::iterator number_param_name=number_param_names.begin();number_param_name!=number_param_names.end();number_param_name++){
                            TiXmlElement* MC_e_number_param = new TiXmlElement( "NumberParameter" );
                            MC_e_number_params->LinkEndChild( MC_e_number_param );
                            MC_e_number_param->SetAttribute("Name",*number_param_name);
                            MC_e_number_param->SetDoubleAttribute("Value",_plugin->getNumberParameterValue(*number_param_name));
                        }
                    }
                    
                    std::vector<std::string> string_param_names = _plugin->getStringParametersNames();
                    if(string_param_names.size()>0){
                        TiXmlElement* MC_e_string_params = new TiXmlElement( "StringParameters" );
                        MC_e_string_params->SetAttribute("Size",string_param_names.size());
                        MC_e_plugin->LinkEndChild( MC_e_string_params );
                        for (std::vector<std::string>::iterator string_param_name=string_param_names.begin();string_param_name!=string_param_names.end();string_param_name++){
                            TiXmlElement* MC_e_string_param = new TiXmlElement( "StringParameter" );
                            MC_e_string_params->LinkEndChild( MC_e_string_param );
                            MC_e_string_param->SetAttribute("Name",*string_param_name);
                            MC_e_string_param->SetAttribute("Value",_plugin->getStringParameterValue(*string_param_name));
                        }
                    }
                }
            }
        }
        TiXmlElement* MC_e_active_plugins = new TiXmlElement( "ActivePlugins" );
        MC_e_root->LinkEndChild( MC_e_active_plugins );
        TiXmlElement* MC_e_clusters_method_plugin = new TiXmlElement( "ClustersMethod" );
        MC_e_active_plugins->LinkEndChild( MC_e_clusters_method_plugin );
        TiXmlText* MC_e_clusters_method_plugin_name = new TiXmlText( this->getBrowser()->getActivePluginName(PLUGIN_TYPE_CLUSTERS_METHOD) );
        MC_e_clusters_method_plugin->LinkEndChild( MC_e_clusters_method_plugin_name );
        TiXmlElement* MC_e_clusters_pos_plugin = new TiXmlElement( "ClustersPositions" );
        MC_e_active_plugins->LinkEndChild( MC_e_clusters_pos_plugin );
        TiXmlText* MC_e_clusters_pos_plugin_name = new TiXmlText( this->getBrowser()->getActivePluginName(PLUGIN_TYPE_CLUSTERS_POSITIONS) );
        MC_e_clusters_pos_plugin->LinkEndChild( MC_e_clusters_pos_plugin_name );
        TiXmlElement* MC_e_neighbors_method_plugin = new TiXmlElement( "NeighborsMethod" );
        MC_e_active_plugins->LinkEndChild( MC_e_neighbors_method_plugin );
        TiXmlText* MC_e_neighbors_method_plugin_name = new TiXmlText( this->getBrowser()->getActivePluginName(PLUGIN_TYPE_NEIGHBORS_METHOD) );
        MC_e_neighbors_method_plugin->LinkEndChild( MC_e_neighbors_method_plugin_name );
        TiXmlElement* MC_e_neighbors_pos_plugin = new TiXmlElement( "NeighborsPositions" );
        MC_e_active_plugins->LinkEndChild( MC_e_neighbors_pos_plugin );
        TiXmlText* MC_e_neighbors_pos_plugin_name = new TiXmlText( this->getBrowser()->getActivePluginName(PLUGIN_TYPE_NEIGHBORS_POSITIONS) );
        MC_e_neighbors_pos_plugin->LinkEndChild( MC_e_neighbors_pos_plugin_name );
        TiXmlElement* MC_e_filtering_plugin = new TiXmlElement( "Filtering" );
        MC_e_active_plugins->LinkEndChild( MC_e_filtering_plugin );
        TiXmlText* MC_e_filtering_plugin_name = new TiXmlText( this->getBrowser()->getActivePluginName(PLUGIN_TYPE_FILTERING) );
        MC_e_filtering_plugin->LinkEndChild( MC_e_filtering_plugin_name );
    }
    
    bool success = MC_doc.SaveFile(_fname.c_str());
    if(success)
        cout << "saved XML config : " << _fname << endl;
    else{
        cerr << "saving file " << _fname << " failed" << endl;
        
        /// When batch-importing files one-by-one with videocycle-cli, some won't output an XML file when saving, even the following workaround won't work
        //        // Declare a printer
        //        TiXmlPrinter printer;
        
        //        // attach it to the document you want to convert in to a std::string
        //        MC_doc.Accept(&printer);
        
        //        // Create a std::string and copy your document data in to the string
        //        std::string str = printer.CStr();
        
        //        std::ofstream outfile(_fname.c_str(), ios::out);
        //        if(!outfile.is_open())
        //        {
        //            cout<<"MediaCycle::saveXMLConfigFile: could not open "<< _fname <<" for writing, maybe the parent directory isn't writable"<<endl;
        //            return MC_e_root;
        //        }
        //        outfile << str;
        //        outfile.close();
    }
    
    // children of MC_Doc get deleted automatically
    return MC_e_root;
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
    if(!mediaBrowser){
        return;
    }
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
    if(n < nTot){
        this->importing = true;
    }
    else if(n==nTot && mId==-1){
        this->importing = false;
        std::vector<std::string> renderer_plugins = pluginManager->getAvailablePluginsNames(PLUGIN_TYPE_MEDIARENDERER, this->getMediaType());
        for(std::vector<std::string>::iterator renderer_plugin = renderer_plugins.begin();renderer_plugin!=renderer_plugins.end();renderer_plugin++){
            ACMediaRendererPlugin* plugin = dynamic_cast<ACMediaRendererPlugin*>(pluginManager->getPlugin(*renderer_plugin));
            if(plugin)
                plugin->enable();
        }
    }
    std::cout << "MediaCycle::mediaImported media id " << mId << " ("<< n << "/" << nTot << ")" << std::endl;
    eventManager->sig_mediaImported(n,nTot,mId);
}
