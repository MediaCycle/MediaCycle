/*
 *  ACMedia.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 11/05/09
 *  @copyright (c) 2009 – UMONS - Numediart
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

#ifndef ACMEDIA_H
#define ACMEDIA_H

#include "ACMediaThumbnail.h"
#include "ACMediaFeatures.h"
#include "ACMediaTypes.h"
#include "ACPluginManager.h"
#include "ACMediaTimedFeature.h"
#include "ACMediaData.h"

#include <string>
#define TIXML_USE_STL
#include <tinyxml.h>

#include <pthread.h>

/// These import steps must be ordered and indexed incrementally
typedef	unsigned int ACImportStep;
const ACImportStep	IMPORT_STEP_UNKNOWN     	=	0;
const ACImportStep	IMPORT_STEP_THUMB_PRE_FEAT	=	1;
const ACImportStep	IMPORT_STEP_FEATURES    	=	2;
const ACImportStep	IMPORT_STEP_THUMB_POST_FEAT	=	3;
const ACImportStep	IMPORT_STEP_SEGMENT     	=	4;
const ACImportStep	IMPORT_STEP_THUMB_POST_SEG	=	5;

// conversion between import steps and their names
typedef std::map<ACImportStep,std::string> ACImportSteps;
typedef std::map<ACImportStep,std::string>::const_iterator ACImportStepsIterator;

// initialize static class variable
static const ACImportSteps::value_type _initis[] = {
    //IMPORT_STEP_UNKNOWN shouldn't be mapped so that the number of import steps is known from import_steps.size()
    ACImportSteps::value_type(IMPORT_STEP_THUMB_PRE_FEAT, "Thumbnailing without features and segmentation"), \
    ACImportSteps::value_type(IMPORT_STEP_FEATURES, "Feature extraction"), \
    ACImportSteps::value_type(IMPORT_STEP_THUMB_POST_FEAT, "Thumbnailing with features, without segmentation"), \
    ACImportSteps::value_type(IMPORT_STEP_SEGMENT, "Segmentation"), \
    ACImportSteps::value_type(IMPORT_STEP_THUMB_POST_SEG, "Thumbnailing with features and segmentation")
};
static const int import_steps_pre_seg = 3; /// to be adapted if new import steps are created
// useful to loop over import steps
static const ACImportSteps import_steps(_initis, _initis + sizeof _initis / sizeof *_initis);

typedef std::vector<ACMediaThumbnail*> ACMediaThumbnails;

class ACMedia {
    // contains the minimal information about a media
    // uses vector of vector to store media features.
    // features_vectors[i] = vector of numbers calculated by plugin number i (starting at 0)
    // note 230210: features_vectors[i] could later be grouped with other features, depending on the configuration file (or the preferences menu)
protected:

    int mid;
    int parentid; //CF so that segments can be defined as ACMedia having other ACMedia as parents
    ACMedia* parent;
    ACMediaType media_type;
    float height, width, depth;
    std::vector<ACMediaFeatures*> features_vectors;
    std::vector<ACMediaFeatures*> preproc_features_vectors;
    std::string filename;
    std::string filename_thumbnail;
    std::string label;
    std::string key; // media document identifier
    std::string text_tags;
    std::string hyper_links;
    std::vector<ACMedia*> segments;
    float start, end; // seconds
    int startInt, endInt; // frame numbers
    std::vector<std::string> mtf_file_names;
    bool discarded; // allows to discard medias to be saved in libraries
    int taggedClassId;

public:
    ACMedia();
    ACMedia(const ACMedia& m);
    virtual ~ACMedia(); // make this virtual to ensure that destructors of derived classes will be called

public:
    ACMediaType getMediaType() {return media_type;}
    virtual ACMediaType getActiveSubMediaType() {return media_type;}

    void setId(int _id);
    int getId();
    void setParentId(int _parentid); //CF so that segments can be defined as ACMedia having other ACMedia as parents
    int getParentId();
    void setParent(ACMedia* _parent);
    ACMedia* getParent();

    double getDuration();

    void addSegment(ACMedia* _segment);
    std::vector<ACMedia*> &getAllSegments();
    void setAllSegments(std::vector<ACMedia*> _segments);
    void deleteAllSegments();
    ACMedia* getSegment(int i);
    int getNumberOfSegments();

    bool isSegment();
    bool isDocument();
    void setAsSegment();
    void setAsDocument();

protected:
    bool is_segment, is_document;

public:

    virtual std::vector<ACMediaFeatures*> &getAllFeaturesVectors() { return features_vectors; }
    virtual ACMediaFeatures* getFeaturesVector(int i);
    virtual ACMediaFeatures* getFeaturesVector(std::string feature_name);
    virtual int getNumberOfFeaturesVectors() {return features_vectors.size();}
    void addFeaturesVector(ACMediaFeatures *aFeatures) { this->features_vectors.push_back(aFeatures); }
    virtual std::vector<std::string> getListOfFeaturesPlugins();

    virtual std::vector<ACMediaFeatures*> &getAllPreProcFeaturesVectors() { return preproc_features_vectors; }
    virtual ACMediaFeatures* getPreProcFeaturesVector(int i);
    virtual ACMediaFeatures* getPreProcFeaturesVector(std::string feature_name);
    virtual int getNumberOfPreProcFeaturesVectors() {return preproc_features_vectors.size();}
    void addPreProcFeaturesVector(ACMediaFeatures *aFeatures) { this->preproc_features_vectors.push_back(aFeatures); }
    virtual std::vector<std::string> getListOfPreProcFeaturesPlugins();
    int replacePreProcFeatures(std::vector<ACMediaFeatures*> newFeatures);
    virtual void cleanPreProcFeaturesVector(void);
    virtual void defaultPreProcFeatureInit(void);

    std::string getFileName() { return filename; }
    void setFileName(std::string s) { filename = s; }
    void setFileName(const char* c) { std::string s(c); filename = s; }
    std::string getKey() { return key; }
    void setKey(std::string k) { key = k; }

    // thumbnail
protected:
    ACMediaThumbnails thumbnails;
public:
    int getNumberOfThumbnails(){return thumbnails.size();}
    void addThumbnail(ACMediaThumbnail* _thumbnail);
    ACMediaThumbnail* getThumbnail(std::string name);
    std::string getThumbnailFileName(std::string name);
    std::string getThumbnailFileName() { return filename_thumbnail; }
    void setThumbnailFileName(std::string ifilename) { filename_thumbnail=ifilename; }
    // the following 2 were re-introduced for audio...
    int getThumbnailWidth(std::string name);
    int getThumbnailHeight(std::string name);

    std::string getLabel(void){return label;}
    void setLabel(std::string iLabel){label=iLabel;}
    bool isDiscarded(){return discarded;}
    void setDiscarded(bool _discarded){discarded=_discarded;}

    // data
    virtual bool extractData(std::string filename) {return false;}
    ACMediaData* getMediaData(){return data;}
    void deleteData();
    void setMediaData(ACMediaData* _data){this->data=_data;}
protected:
    ACMediaData* data;

public:
    // accessors -- these should not be redefined for each media
    float getWidth() {return width;}
    float getHeight() {return height;}
    float getDepth() {return depth;}
    void setWidth(float w) {width=w;}
    void setHeight(float h) {height=h;}
    void setDepth(float d) {depth=d;}
    ACMediaType getType() {return this->media_type;}

    // beginning and end as floats
    void setStart(float st){this->start = st;}
    void setEnd(float en){this->end = en;}
    float getStart(){return this->start;}
    float getEnd(){return this->end;}
    void setStartInt(int sti){this->startInt = sti;}
    void setEndInt(int eni){this->endInt = eni;}
    float getStartInt(){return this->startInt;}
    float getEndInt(){return this->endInt;}
    virtual float getFrameRate() {return 0;}//CF video
    virtual int getSampleRate() {return 0;}//CF audio - merge both?

    // I/O -- common part
    // note : ACL deprecated as of spring 2011 -- but keep it for backward-compatibility (e.g., Dancers!)
    void fixWhiteSpace(std::string &str);
    void saveACL(std::ofstream &library_file, int mcsl=0);
    void saveXML(TiXmlElement* _medias);
    //int loadACL(std::ifstream &library_file, int mcsl=0);
    int loadACL(std::string media_path, std::ifstream &library_file, int mcsl=0);
    void loadXML(TiXmlElement* _pMediaNode, bool with_thumbnails=true);

    void saveMCSL(std::ofstream &library_file); //CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
    int loadMCSL(std::ifstream &library_file); //CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval

    // I/O -- media-specific part

    virtual void saveACLSpecific(std::ofstream &library_file) {}
    virtual void saveXMLSpecific(TiXmlElement* _media) {}
    virtual int loadACLSpecific(std::ifstream &library_file) {return -1;}
    virtual int loadXMLSpecific(TiXmlElement* _pMediaNode) {return -1;}

    // import does the following:
    // 1) load media from file
    // 2) compute thumbnail
    // 3) extract features
    virtual int import(std::string _path, int _mid, ACPluginManager *acpl=0, bool _save_timed_feat=false);
    float getImportProgress(bool with_segmentation);
    float getImportProgressAtStep(ACImportStep step);

    virtual std::string getTextMetaData(){return std::string("");}
    // FEATURES computation (extractFeatures) and segmentation (segment)
    // these methods are virtual, because each media could have a specific segmentation method
    // ex: audioSegmentationPlugin : also calculates features...
private:
    bool media_imported;
    std::map<ACImportStep, float> import_progress;
    std::map<ACImportStep, ACMediaAnalysisPlugin*> plugin_at_step;
    std::map<ACImportStep, int> plugins_per_step;
    pthread_mutex_t import_mutex;
    pthread_mutexattr_t import_mutex_attr;
    private:
        virtual int computeThumbnails(ACPluginManager *acpl, bool feature_extracted, bool segmentation_done);
protected:
    virtual int extractFeatures(ACPluginManager *acpl=0, bool _save_timed_feat=false);
public:
    virtual int segment(ACPluginManager *acpl, bool _saved_timed_features = false );
    void addTimedFileNames(std::string mtf_file_name){mtf_file_names.push_back(mtf_file_name);}
    std::vector<std::string> getTimedFileNames(){return mtf_file_names;}
    ACMediaTimedFeature* getTimedFeatures();
    ACMediaTimedFeature* getTimedFeatures(std::string feature_name);
    void setTaggedClassId(int pId){taggedClassId=pId;}
    int getTaggedClassId(void){ return taggedClassId;}
    bool isTagged(){return (taggedClassId>-1);}
    
};

#endif // ACMEDIA_H
