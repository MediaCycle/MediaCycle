/*
 *  ACPlugin.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 24/03/11
 *  @copyright (c) 2011 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 */

#include "ACPlugin.h"
#include "ACMedia.h"
#include "ACEventManager.h"

#include <iostream>
using std::cout;
using std::endl;
using std::string;
#include "boost/filesystem.hpp"

double getTime()
{
    struct timeval tv = {0, 0};
    struct timezone tz = {0, 0};

    gettimeofday(&tv, &tz);

    return (double)tv.tv_sec + tv.tv_usec / 1000000.0;
}

#ifdef __APPLE__
#include <sys/param.h>
#include <mach-o/dyld.h> /* _NSGetExecutablePath : must add -framework CoreFoundation to link line */
#define MAXPATHLENGTH 256
std::string getExecutablePath(){
    char *given_path;
    std::string path("");
    given_path = new char[MAXPATHLENGTH * 2];
    if (!given_path) return path;
    unsigned int pathsize = MAXPATHLENGTH * 2;
    unsigned int result = _NSGetExecutablePath(given_path, &pathsize);
    if (result == 0){
        path = std::string (given_path);
        size_t current=0;
          while (current!=string::npos){
              current=path.find("./",2);
              if(current!=string::npos)
                   path.replace(current,2,"");
        }
		size_t executable=0;
		executable=path.find_last_of("/");
		if(executable!=string::npos)
			path.replace(executable+1,path.length()-executable,"");
		
    }
    free (given_path);
    return path;
}
#endif

ACPlugin::ACPlugin() {
    this->mName = "";
    this->mId = "";
    this->mDescription = "";
    this->mPluginType = PLUGIN_TYPE_NONE;
    this->mMediaType = MEDIA_TYPE_NONE;
    this->media_cycle = 0;
    this->event_manager = new ACEventManager;
}

ACPlugin::~ACPlugin() {
    mName.clear();
    mDescription.clear();
    mId.clear();
    this->media_cycle=0;
}

void ACPlugin::setMediaCycle(MediaCycle* _media_cycle)
{
    this->media_cycle=_media_cycle;
    if(!_media_cycle){
        std::cerr << "ACPlugin::setMediaCycle: null mediacycle instance" << std::endl;
        return;
    }
    if(this->event_manager)
        this->event_manager->addListener( this->media_cycle->getPluginEventListener() );
}

bool ACPlugin::hasNumberParameterNamed(std::string _name){
    for(std::vector<ACNumberParameter>::iterator NumberParameter = mNumberParameters.begin(); NumberParameter != mNumberParameters.end(); NumberParameter++ ){
        if((*NumberParameter).name == _name)
            return true;
    }
    return false;
}

bool ACPlugin::hasStringParameterNamed(std::string _name){
    for(std::vector<ACStringParameter>::iterator StringParameter = mStringParameters.begin(); StringParameter != mStringParameters.end(); StringParameter++ ){
        if((*StringParameter).name == _name)
            return true;
    }
    return false;
}

bool ACPlugin::hasCallbackNamed(std::string _name){
    for(std::vector<ACCallback>::iterator Callback = mCallbacks.begin(); Callback != mCallbacks.end(); Callback++ ){
        if((*Callback).name == _name)
            return true;
    }
    return false;
}

void ACPlugin::addStringParameter(std::string _name, std::string _init, std::vector<std::string> _values, std::string _desc, ACParameterCallback _callback)
{
    mStringParameters.push_back(ACStringParameter(_name,_init,_values,_desc,_callback));
}

void ACPlugin::updateStringParameter(std::string _name, std::string _init, std::vector<std::string> _values, std::string _desc, ACParameterCallback _callback){
    for(std::vector<ACStringParameter>::iterator StringParameter = mStringParameters.begin(); StringParameter != mStringParameters.end(); StringParameter++ ){
        if((*StringParameter).name == _name){
            (*StringParameter).values = _values;
            std::vector<std::string>::iterator value = std::find(_values.begin(),_values.end(),(*StringParameter).value);
            if(value == _values.end())
                (*StringParameter).value = _values.front();
            else
                (*StringParameter).value = _init;
            (*StringParameter).init = _init;
            if(_desc != "")
                (*StringParameter).desc = _desc;
            if(_callback)
                (*StringParameter).callback = _callback;
#ifdef USE_DEBUG
            std::cout << " ACPlugin::updateStringParameterValues: plugin '" << mName << "', parameter '"<< _name << std::endl;
#endif
            return;
        }
    }
}

void ACPlugin::addNumberParameter(std::string _name, float _init, float _min, float _max, float _step, std::string _desc, ACParameterCallback _callback)
{
    mNumberParameters.push_back(ACNumberParameter(_name,_init,_min,_max,_step,_desc,_callback));
}

void ACPlugin::updateNumberParameter(std::string _name, float _init, float _min, float _max, float _step, std::string _desc, ACParameterCallback _callback){
    for(std::vector<ACNumberParameter>::iterator NumberParameter = mNumberParameters.begin(); NumberParameter != mNumberParameters.end(); NumberParameter++ ){
        if((*NumberParameter).name == _name){
            (*NumberParameter).init = _init;
            (*NumberParameter).min = _min;
            (*NumberParameter).max = _max;
            (*NumberParameter).step = _step;

            if((*NumberParameter).value < _min || (*NumberParameter).value > _max)
                (*NumberParameter).value = _init;

            if(_desc != "")
                (*NumberParameter).desc = _desc;
            if(_callback)
                (*NumberParameter).callback = _callback;
#ifdef USE_DEBUG
            std::cout << " ACPlugin::updateNumberParameterValues: plugin '" << mName << "', parameter '"<< _name << std::endl;
#endif
            return;
        }
    }
}


void ACPlugin::addCallback(std::string _name, std::string _desc, ACParameterCallback _callback)
{
    mCallbacks.push_back(ACCallback(_name,_desc,_callback));
}

void ACPlugin::updateCallback(std::string _name, std::string _desc, ACParameterCallback _callback){
    for(std::vector<ACCallback>::iterator Callback = mCallbacks.begin(); Callback != mCallbacks.end(); Callback++ ){
        if((*Callback).name == _name){
            if(_desc != "")
                (*Callback).desc = _desc;
            if(_callback)
                (*Callback).callback = _callback;
#ifdef USE_DEBUG
            std::cout << " ACPlugin::updateCallbackValues: plugin '" << mName << "', parameter '"<< _name << std::endl;
#endif
            return;
        }
    }
}

std::vector<ACStringParameter> ACPlugin::getStringParameters()
{
    return mStringParameters;
}

std::vector<ACNumberParameter> ACPlugin::getNumberParameters()
{
    return mNumberParameters;
}

int ACPlugin::getStringParametersCount()
{
    return mStringParameters.size();
}

int ACPlugin::getNumberParametersCount()
{
    return mNumberParameters.size();
}

int ACPlugin::getParametersCount()
{
    return mStringParameters.size()+mNumberParameters.size()+mCallbacks.size();
}

bool ACPlugin::setStringParameterValue(std::string _name, std::string _value){
    for(std::vector<ACStringParameter>::iterator StringParameter = mStringParameters.begin(); StringParameter != mStringParameters.end(); StringParameter++ ){
        if((*StringParameter).name == _name){
            (*StringParameter).value = _value;
#ifdef USE_DEBUG
            std::cout << " ACPlugin::setStringParameterValue: plugin '" << mName << "', parameter '"<< _name << "', value '" << _value << "'" << std::endl;
#endif
            if ( (*StringParameter).callback != 0 )
                (*StringParameter).callback();
            return true;
        }
    }
    return false;
}

bool ACPlugin::setNumberParameterValue(std::string _name, float _value){
    for(std::vector<ACNumberParameter>::iterator NumberParameter = mNumberParameters.begin(); NumberParameter != mNumberParameters.end(); NumberParameter++ ){
        if((*NumberParameter).name == _name){
            (*NumberParameter).value = _value;
#ifdef USE_DEBUG
            std::cout << " ACPlugin::setNumberParameterValue: plugin '" << mName << "', parameter '"<< _name << "', value '" << _value << "'" << std::endl;
#endif
            if( (*NumberParameter).callback != 0 )
                (*NumberParameter).callback();
            return true;
        }
    }
    return false;
}

std::string ACPlugin::getStringParameterValue(std::string _name){
    for(std::vector<ACStringParameter>::iterator StringParameter = mStringParameters.begin(); StringParameter != mStringParameters.end(); StringParameter++ ){
        if((*StringParameter).name == _name){
            return (*StringParameter).value;
        }
    }
    std::cerr << "ACPlugin::getStringParameterValue: parameter '" << _name << "' doesn't exist." << std::endl;
    return "ERROR";
}

void ACPlugin::resetParameterValue(std::string _name){
    for(std::vector<ACNumberParameter>::iterator NumberParameter = mNumberParameters.begin(); NumberParameter != mNumberParameters.end(); NumberParameter++ ){
        if((*NumberParameter).name == _name){
            (*NumberParameter).value = (*NumberParameter).init;
        }
    }
    for(std::vector<ACStringParameter>::iterator StringParameter = mStringParameters.begin(); StringParameter != mStringParameters.end(); StringParameter++ ){
        if((*StringParameter).name == _name){
            (*StringParameter).value = (*StringParameter).init;
        }
    }
}

int ACPlugin::getStringParameterValueIndex(std::string _name){
    int index = -1;
    for(std::vector<ACStringParameter>::iterator StringParameter = mStringParameters.begin(); StringParameter != mStringParameters.end(); StringParameter++ ){
        if((*StringParameter).name == _name){
            std::vector<std::string> values = getStringParameterValues(_name);
            std::vector<std::string>::iterator value = std::find(values.begin(),values.end(),(*StringParameter).value);
            if(value != values.end())
                return std::distance(values.begin(),value);
            else{
                std::cerr << "ACPlugin::getStringParameterValueIndex: parameter '" << _name << "' value doesn't exist." << std::endl;
                return index;
            }
        }
    }
    std::cerr << "ACPlugin::getStringParameterValueIndex: parameter '" << _name << "' doesn't exist." << std::endl;
    return index;
}

float ACPlugin::getNumberParameterValue(std::string _name){
    for(std::vector<ACNumberParameter>::iterator NumberParameter = mNumberParameters.begin(); NumberParameter != mNumberParameters.end(); NumberParameter++ ){
        if((*NumberParameter).name == _name){
            return (*NumberParameter).value;
        }
    }
    std::cerr << "ACPlugin::getNumberParameterValue: parameter '" << _name << "' doesn't exist." << std::endl;
    return 0.0f;
}

float ACPlugin::getNumberParameterMin(std::string _name){
    for(std::vector<ACNumberParameter>::iterator NumberParameter = mNumberParameters.begin(); NumberParameter != mNumberParameters.end(); NumberParameter++ ){
        if((*NumberParameter).name == _name){
            return (*NumberParameter).min;
        }
    }
    return 0.0f;
}

float ACPlugin::getNumberParameterMax(std::string _name){
    for(std::vector<ACNumberParameter>::iterator NumberParameter = mNumberParameters.begin(); NumberParameter != mNumberParameters.end(); NumberParameter++ ){
        if((*NumberParameter).name == _name){
            return (*NumberParameter).max;
        }
    }
    return 0.0f;
}

float ACPlugin::getNumberParameterStep(std::string _name){
    for(std::vector<ACNumberParameter>::iterator NumberParameter = mNumberParameters.begin(); NumberParameter != mNumberParameters.end(); NumberParameter++ ){
        if((*NumberParameter).name == _name){
            return (*NumberParameter).step;
        }
    }
    return 0.0f;
}

float ACPlugin::getNumberParameterInit(std::string _name){
    for(std::vector<ACNumberParameter>::iterator NumberParameter = mNumberParameters.begin(); NumberParameter != mNumberParameters.end(); NumberParameter++ ){
        if((*NumberParameter).name == _name){
            return (*NumberParameter).init;
        }
    }
    return 0.0f;
}

std::string ACPlugin::getNumberParameterDesc(std::string _name){
    for(std::vector<ACNumberParameter>::iterator NumberParameter = mNumberParameters.begin(); NumberParameter != mNumberParameters.end(); NumberParameter++ ){
        if((*NumberParameter).name == _name){
            return (*NumberParameter).desc;
        }
    }
    return "";
}

std::string ACPlugin::getStringParameterDesc(std::string _name){
    for(std::vector<ACStringParameter>::iterator StringParameter = mStringParameters.begin(); StringParameter != mStringParameters.end(); StringParameter++ ){
        if((*StringParameter).name == _name){
            return (*StringParameter).desc;
        }
    }
    return "";
}

std::string ACPlugin::getStringParameterInit(std::string _name){
    for(std::vector<ACStringParameter>::iterator StringParameter = mStringParameters.begin(); StringParameter != mStringParameters.end(); StringParameter++ ){
        if((*StringParameter).name == _name){
            return (*StringParameter).init;
        }
    }
    return "";
}

int ACPlugin::getStringParameterInitIndex(std::string _name){
    int index = -1;
    for(std::vector<ACStringParameter>::iterator StringParameter = mStringParameters.begin(); StringParameter != mStringParameters.end(); StringParameter++ ){
        if((*StringParameter).name == _name){
            std::vector<std::string> values = getStringParameterValues(_name);
            std::vector<std::string>::iterator value = std::find(values.begin(),values.end(),(*StringParameter).init);
            if(value != values.end())
                return std::distance(values.begin(),value);
            else{
                std::cerr << "ACPlugin::getStringParameterInitIndex: parameter '" << _name << "' init doesn't exist." << std::endl;
                return index;
            }
        }
    }
    std::cerr << "ACPlugin::getStringParameterInitIndex: parameter '" << _name << "' doesn't exist." << std::endl;
    return index;
}

std::vector<std::string> ACPlugin::getStringParameterValues(std::string _name){
    for(std::vector<ACStringParameter>::iterator StringParameter = mStringParameters.begin(); StringParameter != mStringParameters.end(); StringParameter++ ){
        if((*StringParameter).name == _name){
            return (*StringParameter).values;
        }
    }
    return std::vector<std::string>();
}


std::vector<std::string> ACPlugin::getStringParametersNames()
{
    std::vector<std::string> names;
    for(std::vector<ACStringParameter>::iterator StringParameter = mStringParameters.begin(); StringParameter != mStringParameters.end(); StringParameter++ ){
        names.push_back((*StringParameter).name);
    }
    return names;
}

std::vector<std::string> ACPlugin::getNumberParametersNames()
{
    std::vector<std::string> names;
    for(std::vector<ACNumberParameter>::iterator NumberParameter = mNumberParameters.begin(); NumberParameter != mNumberParameters.end(); NumberParameter++ ){
        names.push_back((*NumberParameter).name);
    }
    return names;
}

std::vector<ACCallback> ACPlugin::getCallbacks(){
    return mCallbacks;
}

int ACPlugin::getCallbacksCount(){
    return mCallbacks.size();
}

std::string ACPlugin::getCallbackDesc(std::string _name){
    for(std::vector<ACCallback>::iterator Callback = mCallbacks.begin(); Callback != mCallbacks.end(); Callback++ ){
        if((*Callback).name == _name){
            return (*Callback).desc;
        }
    }
    return "";
}

std::vector<std::string> ACPlugin::getCallbacksNames(){
    std::vector<std::string> names;
    for(std::vector<ACCallback>::iterator Callback = mCallbacks.begin(); Callback != mCallbacks.end(); Callback++ ){
        names.push_back((*Callback).name);
    }
    return names;
}

bool ACPlugin::triggerCallback(std::string _name){
    for(std::vector<ACCallback>::iterator Callback = mCallbacks.begin(); Callback != mCallbacks.end(); Callback++ ){
        if((*Callback).name == _name){
#ifdef USE_DEBUG
            std::cout << " ACPlugin::triggerCallback: plugin '" << mName << "', callback '"<< _name << "'" << std::endl;
#endif
            if ( (*Callback).callback != 0 )
                (*Callback).callback();
            return true;
        }
    }
    return false;
}

bool ACPlugin::implementsPluginType(ACPluginType pType) {
    int test = mPluginType&pType;
    if (test == 0)
        return false;
    else
        return true;
}

bool ACPlugin::mediaTypeSuitable(ACMediaType pType) {
    int test = mMediaType&pType;
    if (test == 0)
        return false;
    else
        return true;
}

ACMediaReaderPlugin::ACMediaReaderPlugin() : ACPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_MEDIAREADER;
}

ACMediaRendererPlugin::ACMediaRendererPlugin() : ACPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_MEDIARENDERER;
}

ACFeaturesPlugin::ACFeaturesPlugin() : ACPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_FEATURES;
}

// XS TODO : add (?)
// this->mPluginType=mPluginType|PLUGIN_TYPE_TIMED_FEATURES;

ACTimedFeaturesPlugin::ACTimedFeaturesPlugin() : ACFeaturesPlugin() {
    //this->mtf_file_name = "";
}

// the plugin knows internally where it saved the mtf
// thanks to mtf_file_name

ACMediaTimedFeature* ACTimedFeaturesPlugin::getTimedFeatures(std::string mtf_file_name) {
    if (mtf_file_name == "") {
        cout << "<ACTimedFeaturesPlugin::getTimedFeatures> : missing file name " << endl;
        cout << "                                            in plugin : " << this->getName() << endl;
        return 0;
    }
    ACMediaTimedFeature* ps_mtf = new ACMediaTimedFeature();
    if (ps_mtf->loadFromFile(mtf_file_name) <= 0) {
        return 0;
    }
    return ps_mtf;
}

// saving timed features on disk (if _save_timed_feat flag is on)
// try to keep the convention : _b.mtf = binary ; _t.mtf = ascii text
// XS TODO add checks and setup/use default params
std::string ACTimedFeaturesPlugin::saveTimedFeatures(ACMediaTimedFeature* mtf, string aFileName, bool _save_timed_feat, bool _save_binary) {
    if (_save_timed_feat) {
		std::string mtf_file_name;
        
        string aFileName_noext = aFileName.substr(aFileName.find_last_of('/'),aFileName.find_last_of('.')-aFileName.find_last_of('/'));
        string aFileName_direct = aFileName.substr(0,aFileName.find_last_of('/'));
        boost::filesystem::path dir(aFileName_direct+"/mtf");
        boost::filesystem::create_directory(dir);
        
        string file_ext = "_b.mtf";
        if(!_save_binary)
            file_ext =  "_t.mtf";
        mtf_file_name = aFileName_direct+"/mtf"+aFileName_noext + "_" +this->mDescription + file_ext;
        
        if  (mtf->saveInFile(mtf_file_name, _save_binary)) // error message if failed
			return mtf_file_name;
    }
    return string(""); // nothing to do
}

ACSegmentationPlugin::ACSegmentationPlugin() : ACPlugin(){
    this->mPluginType = mPluginType | PLUGIN_TYPE_SEGMENTATION;
}

ACThumbnailerPlugin::ACThumbnailerPlugin() : ACPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_THUMBNAILER;
}

ACClusterMethodPlugin::ACClusterMethodPlugin() : ACPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_CLUSTERS_METHOD;
}

ACNeighborMethodPlugin::ACNeighborMethodPlugin() : ACPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_NEIGHBORS_METHOD;
}

ACPositionsPlugin::ACPositionsPlugin() : ACPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_POSITIONS;
}

ACClusterPositionsPlugin::ACClusterPositionsPlugin() : ACPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_CLUSTERS_POSITIONS;
}

ACNeighborPositionsPlugin::ACNeighborPositionsPlugin() : ACPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_NEIGHBORS_POSITIONS;
}

ACNoMethodPositionsPlugin::ACNoMethodPositionsPlugin() : ACPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_NOMETHOD_POSITIONS;
}

ACPreProcessPlugin::ACPreProcessPlugin() : ACPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_PREPROCESS;
}

ACClientPlugin::ACClientPlugin() : ACPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_CLIENT;
}

ACServerPlugin::ACServerPlugin() : ACPlugin() {
    this->mPluginType = mPluginType | PLUGIN_TYPE_SERVER;
}

ACClientServerPlugin::ACClientServerPlugin() : ACPlugin() {
    this->mPluginType = mPluginType | (PLUGIN_TYPE_CLIENT | PLUGIN_TYPE_SERVER);
}
