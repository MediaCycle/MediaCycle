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

#include "ACMediaFeatures.h"
#include "ACMediaTypes.h"
#include "ACPluginManager.h"
#include "ACMediaTimedFeature.h"

#include <string>
#include <tinyxml.h>

class ACMedia {
	// contains the minimal information about a media
	// uses vector of vector to store media features. 
	// features_vectors[i] = vector of numbers calculated by plugin number i (starting at 0)
	// note 230210: features_vectors[i] could later be grouped with other features, depending on the configuration file (or the preferences menu)
protected:
		
	int mid;
	int parentid; //CF so that segments can be defined as ACMedia having other ACMedia as parents
	ACMediaType media_type;
	int height, width;
	std::vector<ACMediaFeatures*> features_vectors;
	std::vector<ACMediaFeatures*> preproc_features_vectors;
	std::string filename;
	std::string filename_thumbnail;
	std::string label;
	char  **text_tags;
	char  **hyper_links;
	std::vector<ACMedia*> segments;
	float start, end; // seconds
	//bool persistent_data; // true if data and thumbnail are kept in virtual memory
	//bool features_saved_xml;
	
	// XS TODO : add a MediaTimedFeatures member ?
	// so that we can segment "on-the-fly"
private:
	void init();
	
public:
	ACMedia();
//	ACMedia(const ACMedia&, bool reduce = true);
	virtual ~ACMedia(); // make this virtual to ensure that destructors of derived classes will be called

	ACMediaType getMediaType() {return media_type;};
	void setId(int _id) {mid = _id;} // SD TODO - should check for duplicate id?
	int getId() {return mid;}
	void setParentId(int _parentid) {parentid = _parentid;} //CF so that segments can be defined as ACMedia having other ACMedia as parents
	int getParentId() {return parentid;}
	
	double getDuration(){return this->getEnd()-this->getStart();}
	
	void addSegment(ACMedia* _segment){segments.push_back(_segment);}
	//void removeSegment(ACMedia* _segment){segments.erase(_segment);}//CF wow, tricky
	std::vector<ACMedia*> &getAllSegments() { return segments; }
	ACMedia* getSegment(int i) { return segments[i]; }
	int getNumberOfSegments(){return segments.size();}
	
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
	void cleanPreProcFeaturesVector(void);
	virtual void defaultPreProcFeatureInit(void);
	
	std::string getFileName() { return filename; }
	void setFileName(std::string s) { filename = s; }
	void setFileName(const char* c) { std::string s(c); filename = s; }

	// thumbnail
	virtual void* getThumbnailPtr()=0; // XS TODO change this
	std::string getThumbnailFileName() { return filename_thumbnail; }
	void setThumbnailFileName(std::string ifilename) { filename_thumbnail=ifilename; }
	// the following 2 were re-introduced for audio...
	virtual int getThumbnailWidth() {return 0;}
	virtual int getThumbnailHeight() {return 0;}
	
	std::string getLabel(void){return label;}	
	void getLabel(std::string iLabel){label=iLabel;}
	
	// data
	virtual void extractData(std::string filename) {}//todo bool output to test the import and to continue if a file is not readable

//	void setDataPersistence(bool persistence){persistent_data=persistence;}
//	bool getDataPersistence(){return persistent_data;}
//	void setData(ACMediaData* _data){data = _data;}
	virtual ACMediaData* getMediaData()=0;
	virtual void deleteData();
		
	// accessors -- these should not be redefined for each media
	int getWidth() {return width;}
	int getHeight() {return height;}
	void setWidth(int w) {width=w;}
	void setHeight(int h) {height=h;}
	ACMediaType	getType() {return this->media_type;}	
	
	// beginning and end as floats
	void setStart(float st){this->start = st;};
	void setEnd(float en){this->end = en;};
	float getStart(){return this->start;};
	float getEnd(){return this->end;};
	
	// I/O -- common part
	
	void fixWhiteSpace(std::string &str);
	void saveACL(std::ofstream &library_file, int mcsl=0);
	void saveXML(TiXmlElement* _medias);

	//int loadACL(std::ifstream &library_file, int mcsl=0);
	int loadACL(std::string media_path, std::ifstream &library_file, int mcsl=0);
	void loadXML(TiXmlElement* _pMediaNode);

	void saveMCSL(std::ofstream &library_file); //CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
	int loadMCSL(std::ifstream &library_file); //CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval

	// I/O -- media-specific part

	virtual void saveACLSpecific(std::ofstream &library_file) {}
	virtual void saveXMLSpecific(TiXmlElement* _media) {}
	virtual int loadACLSpecific(std::ifstream &library_file) {return -1;}
	virtual int loadXMLSpecific(TiXmlElement* _pMediaNode) {return -1;}

	
	// FEATURES computation (import) and segmentation (segment)
	// these methods are virtual, because each media could have a specific segmentation method
	// ex: audioSegmentationPlugin : also calculates features...
	virtual int import(std::string _path, int _mid=0, ACPluginManager *acpl=0, bool _save_timed_feat=false);
	virtual int segment(ACPluginManager *acpl, bool _saved_timed_features = false );
};

#endif // ACMEDIA_H
