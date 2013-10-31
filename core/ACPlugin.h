/* 
 * File:   ACPlugin.h
 * Author: Julien Dubois
 *
 * @date 27 juillet 2009
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

#ifndef _ACPLUGIN_H
#define _ACPLUGIN_H

#include <string>
#include <vector>
#include <boost/unordered_map.hpp>

#include "ACMediaFeatures.h"
#include "ACMediaTimedFeature.h"
#include "ACMediaData.h"

class ACMedia;
class ACMediaThumbnail;
typedef std::map<long,ACMedia*> ACMedias;
class ACMediaBrowser;
class ACMediaTimedFeature;
class MediaCycle;

#include<iostream>

#include <sys/time.h>
extern double getTime();

#ifdef __APPLE__
extern std::string getExecutablePath();
#endif

#include <boost/function.hpp>
#include <boost/bind.hpp>
typedef boost::function<void()> ACParameterCallback;

#include <boost/any.hpp>
using boost::any_cast;

struct ACThirdPartyMetadata{
    std::string name,license,url;
    ACThirdPartyMetadata():name(""),license(""),url(""){}
    ACThirdPartyMetadata(std::string _name,std::string _license,std::string _url):name(_name),license(_license),url(_url){}
};

class ACParameter{
public:
    std::string name,desc;
    ACParameter()
        :name(""),desc(""){}
    ACParameter(std::string _name, std::string _desc)
        :name(_name),desc(_desc){ }
    virtual ~ACParameter(){}
};

class ACNumberParameter : public ACParameter{
public:
    float value,init,min,max,step;
    ACParameterCallback callback;
    ACNumberParameter()
        :ACParameter(),value(0.0f),init(0.0f),min(0.0f),max(0.0f),step(0.0f),callback(0){}
    ACNumberParameter(std::string _name, float _init, float _min, float _max, float _step, std::string _desc, ACParameterCallback _callback)
        :ACParameter(_name,_desc),value(_init),init(_init),min(_min),max(_max),step(_step),callback(_callback){ }
    virtual ~ACNumberParameter(){callback = 0;}
};

class ACStringParameter : public ACParameter{
public:
    std::string value,init;
    std::vector<std::string> values;
    ACParameterCallback callback;
    ACStringParameter()
        :ACParameter(),value(""),init(""),callback(0){}
    ACStringParameter(std::string _name, std::string _init, std::vector<std::string> _values, std::string _desc, ACParameterCallback _callback)
        :ACParameter(_name,_desc),value(_init),init(_init),values(_values),callback(_callback){ }
    virtual ~ACStringParameter(){callback = 0;}
};

class ACCallback : public ACParameter{
public:
    ACParameterCallback callback;
    ACCallback()
        :ACParameter(),callback(0){}
    ACCallback(std::string _name, std::string _desc, ACParameterCallback _callback)
        :ACParameter(_name,_desc),callback(_callback){ }
    virtual ~ACCallback(){callback = 0;}
};

typedef		unsigned int ACPluginType;
const ACPluginType	PLUGIN_TYPE_NONE				=	0x0000;/// null type for failsafe checks
const ACPluginType	PLUGIN_TYPE_FEATURES			=	0x0001;/// plugin type for extracting features
const ACPluginType	PLUGIN_TYPE_SEGMENTATION		=	0x0002;/// plugin type for segmenting media files
const ACPluginType	PLUGIN_TYPE_SERVER				=	0x0004;/// plugin type for server communication
const ACPluginType	PLUGIN_TYPE_CLIENT				=	0x0008;/// plugin type for client communication
const ACPluginType	PLUGIN_TYPE_CLUSTERS_METHOD		=	0x0010;/// plugin type for computing clusters
const ACPluginType	PLUGIN_TYPE_CLUSTERS_POSITIONS	=	0x0020;/// plugin type for updating positions in clusters mode
const ACPluginType	PLUGIN_TYPE_CLUSTERS_PIPELINE	=	0x0040;/// plugin type for computing clusters and updating positions in clusters mode
const ACPluginType	PLUGIN_TYPE_NEIGHBORS_METHOD	=	0x0080;/// plugin type for computing neighborhoods
const ACPluginType	PLUGIN_TYPE_NEIGHBORS_POSITIONS	=	0x0100;/// plugin type for updating positions in neighbors mode
const ACPluginType	PLUGIN_TYPE_NEIGHBORS_PIPELINE	=	0x0200;/// plugin type for computing neighborhoods and updating positions in neighbors mode
const ACPluginType	PLUGIN_TYPE_POSITIONS			=	0x0400;//TR todo
const ACPluginType	PLUGIN_TYPE_NOMETHOD_POSITIONS	=	0x0800;/// plugin type for disabling clusters or neighborhoods computation
const ACPluginType	PLUGIN_TYPE_ALLMODES_PIPELINE	=	0x1000;/// plugin type grouping clustering/neighborhoods-compliant methods and positioning
const ACPluginType	PLUGIN_TYPE_PREPROCESS			=	0x2000;/// plugin type for pre-processing (after importing, before clusters/neighborhods computation)
const ACPluginType	PLUGIN_TYPE_MEDIAREADER			=	0x4000;/// plugin type for media readers (file formats)
const ACPluginType	PLUGIN_TYPE_MEDIARENDERER		=	0x8000;/// plugin type for media renderers (visual browser, audio engine, etc...)
const ACPluginType	PLUGIN_TYPE_THUMBNAILER			=	0x10000;/// plugin for media thumbnailers (computing summaries: audio waveforms, etc...)
const ACPluginType	PLUGIN_TYPE_MEDIA_ANALYSIS		=	0x20000;/// plugin type for functionnalities shared by feature extractors, segmenters, thumbnailers
const ACPluginType	PLUGIN_TYPE_LIBRARY_FILE		=	0x40000;/// plugin type for library file management, shared by readers and writers
const ACPluginType	PLUGIN_TYPE_LIBRARY_READER		=	0x80000;/// plugin type for library file reading
const ACPluginType	PLUGIN_TYPE_LIBRARY_WRITER		=	0x100000;/// plugin type for library file writing

// From http://cottonvibes.blogspot.be/2010/07/maxmin-values-for-signed-and-unsigned.html
// u8, u16, u32... mean unsigned int of 8, 16, and 32 bits respectively
// s8, s16, s32... mean   signed int of 8, 16, and 32 bits respectively
//----------------------------------
//| type | max        | min        |
//----------------------------------
//| u8   | 0xff       | 0x0        |
//| u16  | 0xffff     | 0x0        |
//| u32  | 0xffffffff | 0x0        |
//----------------------------------
//| s8   | 0x7f       | 0x80       |
//| s16  | 0x7fff     | 0x8000     |
//| s32  | 0x7fffffff | 0x80000000 |
//----------------------------------

class ACPlugin {
public:
    ACPlugin();
    virtual ~ACPlugin();
    bool implementsPluginType(ACPluginType pType);
    bool mediaTypeSuitable(ACMediaType);
    std::string getName() {return this->mName;}
    std::string getIdentifier() {return this->mId;}
    std::string getDescription() {return this->mDescription;}
    ACMediaType getMediaType() {return this->mMediaType;}
    ACPluginType getPluginType() {return this->mPluginType;}
    void setMediaCycle(MediaCycle* _media_cycle);
    MediaCycle* getMediaCycle(){return media_cycle;}
    /// Re-implement this function to feed the newly set mediacycle instance to objects that require it
    virtual void mediaCycleSet(){}
    /// Event listener function feed by the plugin manager
    virtual void pluginLoaded(std::string plugin_name){}

protected:
    void addStringParameter(std::string _name, std::string _init, std::vector<std::string> _values, std::string _desc, ACParameterCallback _callback = 0);
    void updateStringParameter(std::string _name, std::string _init, std::vector<std::string> _values, std::string _desc = "", ACParameterCallback _callback = 0);
    void updateStringParameterCallback(std::string _name, ACParameterCallback _callback);
    void addNumberParameter(std::string _name, float _init, float _min, float _max, float _step, std::string _desc, ACParameterCallback _callback = 0);
    void updateNumberParameter(std::string _name, float _init, float _min, float _max, float _step, std::string _desc = "", ACParameterCallback _callback = 0);
    void addCallback(std::string _name, std::string _desc, ACParameterCallback _callback);
    void updateCallback(std::string _name, std::string _desc, ACParameterCallback _callback);

public:
    int getParametersCount();
    bool hasNumberParameterNamed(std::string _name);
    bool hasStringParameterNamed(std::string _name);
    bool hasCallbackNamed(std::string _name);

    void resetParameterValue(std::string _name);

    std::vector<ACStringParameter> getStringParameters();
    int getStringParametersCount();
    bool setStringParameterValue(std::string _name, std::string _value);
    std::string getStringParameterValue(std::string _name);
    int getStringParameterValueIndex(std::string _name);
    std::string getStringParameterInit(std::string _name);
    int getStringParameterInitIndex(std::string _name);
    std::string getStringParameterDesc(std::string _name);
    std::vector<std::string> getStringParameterValues(std::string _name);
    std::vector<std::string> getStringParametersNames();

    std::vector<ACNumberParameter> getNumberParameters();
    int getNumberParametersCount();
    bool setNumberParameterValue(std::string _name, float _value);
    float getNumberParameterValue(std::string _name);
    float getNumberParameterMin(std::string _name);
    float getNumberParameterMax(std::string _name);
    float getNumberParameterStep(std::string _name);
    float getNumberParameterInit(std::string _name);
    std::string getNumberParameterDesc(std::string _name);
    std::vector<std::string> getNumberParametersNames();

    std::vector<ACCallback> getCallbacks();
    int getCallbacksCount();
    std::string getCallbackDesc(std::string _name);
    std::vector<std::string> getCallbacksNames();
    bool triggerCallback(std::string _name);

    ACParameter getParameter(std::string _name);

protected:
    std::string mName;
    std::string mId;
    std::string mDescription;
    ACPluginType mPluginType;
    ACMediaType mMediaType;
    std::vector<ACStringParameter> mStringParameters;
    std::vector<ACNumberParameter> mNumberParameters;
    std::vector<ACCallback> mCallbacks;
    MediaCycle* media_cycle;
};

// plugin to verify which formats the readers can open (future: and save)
class ACMediaReaderPlugin:virtual public ACPlugin{
public:
    ACMediaReaderPlugin();
    virtual ACMedia* mediaFactory(ACMediaType mediaType, const ACMedia* media=0)=0;
    //virtual std::vector<std::string> getExtensionsFromMediaType(ACMediaType media_type)=0;
    virtual ACMediaData* mediaReader(ACMediaType mediaType)=0;
    virtual std::map<std::string, ACMediaType> getSupportedExtensions(ACMediaType media_type = MEDIA_TYPE_ALL)=0;
};

typedef enum {
    AC_TYPE_UNKNOWN='o',
    AC_TYPE_INT32='i',
    AC_TYPE_FLOAT = 'f',
    AC_TYPE_STRING = 's',
    AC_TYPE_DOUBLE = 'd',
    AC_TYPE_CHAR = 'c',
    AC_TYPE_BOOL = 'b'
} ACParameterType;

class ACMediaActionParameter {
public:
    ACParameterType type;
    ACParameter parameter;
    ACMediaActionParameter():parameter(ACParameter()),type(AC_TYPE_UNKNOWN){}
    ACMediaActionParameter(ACParameterType _type,ACParameter _parameter):parameter(_parameter),type(_type){}
    virtual ~ACMediaActionParameter(){}
    std::string getName(){return parameter.name;}
    ACParameterType getType(){return type;}
};

typedef std::vector<ACMediaActionParameter> ACMediaActionParameters;

// plugin to verify which formats the viewer can render
class ACMediaRendererPlugin: virtual public ACPlugin{
public:
    ACMediaRendererPlugin();
    virtual std::map<std::string, ACMediaType> getSupportedExtensions(ACMediaType media_type = MEDIA_TYPE_ALL)=0;
    virtual bool performActionOnMedia(std::string action, long int mediaId, std::vector<boost::any> arguments=std::vector<boost::any>()){return false;}
    /// Convenience cases for faster access
    bool performActionOnMedia(std::string action, long int mediaId, std::string argument);
    bool performActionOnMedia(std::string action, long int mediaId, int argument);
    bool performActionOnMedia(std::string action, long int mediaId, float argument);
    virtual std::map<std::string,ACMediaType> availableMediaActions(){return std::map<std::string,ACMediaType>();}
    virtual std::map<std::string,ACMediaActionParameters> mediaActionsParameters(){return std::map<std::string,ACMediaActionParameters>();}
    virtual void mediaCycleSet(){}
    virtual void disable(){}
    virtual void enable(){}
};

// plugin for internal use to report the import progress
class ACMediaAnalysisPlugin: virtual public ACPlugin
{
public:
    ACMediaAnalysisPlugin();
    /// Returns the current analysis progress between 0 and 1
    virtual float getProgress(){return 0;}
};

typedef boost::unordered::unordered_map<std::string,int> ACFeatureDimensions; // an unordered_map without C++11

// XS TODO : separate time & space plugins ?
// getTimedFeatures has no sense for image
class ACFeaturesPlugin: virtual public ACMediaAnalysisPlugin
{
protected:
    ACFeaturesPlugin();
public:
    virtual std::string requiresMediaReaderPlugin(){return "";}
    virtual std::vector<ACMediaFeatures*> calculate(ACMedia* theMedia, bool _save_timed_feat=false)=0;
    virtual ACFeatureDimensions getFeaturesDimensions()=0;
    // XS TODO is this the best way to proceed when no timed features ?
    virtual ACMediaTimedFeature* getTimedFeatures(std::string mtf_file_name){return 0;}
};

// separate time-dependent plugins from other
// e.g.,  getTimedFeatures has no sense for image
class ACTimedFeaturesPlugin: virtual public ACFeaturesPlugin{
protected:
    ACTimedFeaturesPlugin();
    // XS TODO add a protected setmftfilename instead of making mtf_file_name protected
    //std::vector<std::string> mtf_file_names// file in which features have been saved
public:
    //std::vector<std::string> getSavedFileName(){return mtf_file_name;}
    virtual ACMediaTimedFeature* getTimedFeatures(std::string mtf_file_name);
    virtual ACFeatureDimensions getTimedFeaturesDimensions(){return ACFeatureDimensions();}
    std::string saveTimedFeatures(ACMediaTimedFeature* mtf=0, std::string aFileName="", bool _save_timed_feat = true, bool _save_binary = true);
};

class ACSegmentationPlugin: virtual public ACMediaAnalysisPlugin
{
protected:
public:
    ACSegmentationPlugin();
    virtual std::string requiresMediaReaderPlugin(){return "";}
    virtual std::vector<ACMedia*> segment(ACMediaTimedFeature* _mtf, ACMedia*)=0;
    virtual std::vector<ACMedia*> segment(ACMedia*)=0;
};

class ACThumbnailerPlugin : virtual public ACMediaAnalysisPlugin{
public:
    ACThumbnailerPlugin();
    virtual std::vector<std::string> getThumbnailNames()=0;
    virtual std::map<std::string,ACMediaType> getThumbnailTypes()=0;
    virtual std::map<std::string,std::string> getThumbnailDescriptions()=0;
    virtual std::map<std::string,std::string> getThumbnailExtensions()=0;
    virtual std::string requiresMediaReaderPlugin(){return "";}
    virtual std::vector<std::string> requiresFeaturesPlugins()=0; // list of plugin names (not paths)
    virtual std::vector<std::string> requiresSegmentationPlugins()=0; // list of plugin names (not paths)
    virtual std::vector<ACMediaThumbnail*> summarize(ACMedia* theMedia)=0;
};

class ACNeighborMethodPlugin : virtual public ACPlugin {
public:
    ACNeighborMethodPlugin();
    virtual void updateNeighborhoods(ACMediaBrowser* )=0;
};

class ACClusterMethodPlugin : virtual public ACPlugin {
public:
    ACClusterMethodPlugin();
    virtual void updateClusters(ACMediaBrowser* mediaBrowser ,bool needsCluster=true)=0;//updateClustersKMeans(animate, needsCluster)
};

class ACPositionsPlugin : virtual public ACPlugin {
public:
    ACPositionsPlugin();
    virtual void updateNextPositions(ACMediaBrowser* )=0;
};

//TODO TR These three plugin has the same interface but not the same constructor. This should be replaced by a versus without ACMediaBrowser access
class ACClusterPositionsPlugin : virtual public ACPlugin {
public:
    ACClusterPositionsPlugin();
    virtual void updateNextPositions(ACMediaBrowser* )=0;
};

class ACNeighborPositionsPlugin : virtual public ACPlugin {
public:
    ACNeighborPositionsPlugin();
    virtual void updateNextPositions(ACMediaBrowser* )=0;
};

class ACNoMethodPositionsPlugin : virtual public ACPlugin {
public:
    ACNoMethodPositionsPlugin();
    virtual void updateNextPositions(ACMediaBrowser* )=0;
};

typedef void* preProcessInfo;

class ACPreProcessPlugin : virtual public ACPlugin {
public:
    ACPreProcessPlugin();
    virtual preProcessInfo update(ACMedias media_library)=0;
    virtual std::vector<ACMediaFeatures*> apply(preProcessInfo info,ACMedia* theMedia)=0;
    virtual void freePreProcessInfo(preProcessInfo &info)=0;
};

class ACClientPlugin : virtual public ACPlugin {
public:
    ACClientPlugin();
    virtual bool performActionOnMedia(std::string action, long int mediaId, std::vector<boost::any> arguments=std::vector<boost::any>()){return false;}
};

class ACServerPlugin : virtual public ACPlugin {
public:
    ACServerPlugin();
};

class ACClientServerPlugin : virtual public ACPlugin {
public:
    ACClientServerPlugin();
};

class ACMediaLibraryFilePlugin : virtual public ACPlugin {
public:
    ACMediaLibraryFilePlugin();
    virtual bool openLibrary(std::string filepath)=0;
    virtual bool closeLibrary(std::string filepath="")=0;
    virtual bool isLibraryOpened(std::string filepath="")=0;
    virtual std::vector<std::string> fileFormats()=0;
};

class ACMediaLibraryReaderPlugin : virtual public ACMediaLibraryFilePlugin {
public:
    ACMediaLibraryReaderPlugin();
    virtual std::vector<std::string> filenamesToOpen()=0;
    virtual int numberOfFilesToOpen(){return this->filenamesToOpen().size();}
    virtual ACMedia* importMedia(std::string mediafilepath)=0;
    virtual std::map<std::string,ACMediaType> requiredMediaReaders();
};

class ACMediaLibraryWriterPlugin : virtual public ACMediaLibraryFilePlugin {
public:
    ACMediaLibraryWriterPlugin();
    virtual bool saveLibraryMetadata()=0;
    virtual bool saveMedia(ACMedia* media)=0;
    //virtual std::map<std::string,ACMediaType> requiredMediaWriters();
};

// the types of the class factories
typedef ACPlugin* createPluginFactory(std::string);
typedef void destroyPluginFactory(ACPlugin*);
typedef std::vector<std::string> listPluginFactory();
typedef std::vector<ACThirdPartyMetadata> listThirdPartyMetadata();

#endif	/* _ACPLUGIN_H */
