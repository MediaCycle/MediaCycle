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
#include "ACMediaData.h"
#include <string>

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
	std::string filename;
	std::string filename_thumbnail;
	char  **text_tags;
	char  **hyper_links;
	std::vector<ACMedia*> segments;//CF
	
	float start, end; // seconds
	
public:
	ACMedia();
	ACMedia(const ACMedia&);
	virtual ~ACMedia();

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

	std::vector<ACMediaFeatures*> &getAllFeaturesVectors() { return features_vectors; }
	ACMediaFeatures* getFeaturesVector(int i);
	ACMediaFeatures* getFeaturesVector(string feature_name);
	int getNumberOfFeaturesVectors() {return features_vectors.size();}
	void addFeaturesVector(ACMediaFeatures *aFeatures) { this->features_vectors.push_back(aFeatures); }
	
	std::string getFileName() { return filename; }
	void setFileName(std::string s) { filename = s; }

	// thumbnail
	std::string getThumbnailFileName() { return filename_thumbnail; }
	void setThumbnailFileName(string ifilename) { filename_thumbnail=ifilename; }
	// the following 2 were re-introduced for audio...
	virtual int getThumbnailWidth() {return 0;}
	virtual int getThumbnailHeight() {return 0;}

//	virtual int computeThumbnail(std::string fname="", int w=0, int h=0){}
//	virtual int computeThumbnail(ACMediaData* data_ptr, int w=0, int h=0){}

	virtual void* getThumbnailPtr()=0;
	//XS TODO: remove the following one  -- type-specific !
	virtual void setThumbnailPtr(void *ptr){};

	// accessors -- these should not be redefined for each media
	int getWidth() {return width;}
	int getHeight() {return height;}
	void setWidth(int w) {width=w;}
	void setHeight(int h) {height=h;}
	ACMediaType	getType() {return this->media_type;}	
	
	void setStart(float st){this->start = st;};
	void setEnd(float en){this->end = en;};
	float getStart(){return this->start;};
	float getEnd(){return this->end;};
	
	// I/O -- these are media-specific (at least for the moment...) 
	virtual void save(FILE *){}
	virtual int load(FILE*){}
	virtual void saveACL(std::ofstream &library_file){}
	virtual void saveMCSL(std::ofstream &library_file){}//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
	virtual int loadACL(std::ifstream &library_file){}
	virtual int loadMCSL(std::ifstream &library_file){}//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
	virtual ACMediaData* extractData(std::string filename){}
	
	// function that calls the plugins and fills in info such as width, height, ...
	virtual int import(std::string _path, int id=-1, ACPluginManager *acpl=NULL);
	// XS 23/09/09 : I implemented import in ACMedia.cpp, since it is the same for all media
	// XS 23/09/09 : import returns 1 if it worked, 0 if it failed

	int ACMedia::segment(ACPluginManager *acpl );

	//	int segment();
};

#endif // ACMEDIA_H
