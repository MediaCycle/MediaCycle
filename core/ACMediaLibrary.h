/*
 *  ACMediaLibrary.h
 *  MediaCycle
 *
 *  @author Stéphane Dupont
 *  @date 21/10/08
 *  @author Xavier Siebert
 *
 *  @copyright (c) 2008 – UMONS - Numediart
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

#ifndef _ACMEDIALIBRARY_H
#define _ACMEDIALIBRARY_H

#include <string>
#include <vector>

#include "ACMediaTypes.h"
#include "ACMedia.h"
#include "ACMediaFactory.h"
#include "ACPluginManager.h"

#define TIXML_USE_STL
#include <tinyxml.h>

#include <boost/filesystem.hpp>

struct ACUserProfile {
        std::string name,email,website,location,picture;
        ACUserProfile() : name(""),email(""),website(""),location(""),picture("") {}
};

struct ACMediaLibraryMetadata {
        std::string title,author,year,publisher,license,website,cover;
        ACUserProfile curator;
        ACMediaLibraryMetadata() : title(""),author(""),curator(ACUserProfile()),year(""),publisher(""),license(""),website(""),cover("") {}
};

class ACMediaLibrary {
	// these methods are common to all media.
	// there is no need for an AudioLibrary, ImageLibrary, ...
	// media-specific methods (e.g., save, load) are in ACMedia and overloaded in ACAudio, ACImage, ACVideo, ...
protected:
	ACMediaType media_type;
        //std::string title,author,curator,year,publisher,license,website,cover;
        ACMediaLibraryMetadata metadata;
	std::vector<ACMedia*> media_library;
	std::vector< std::vector<double> > mean_features, stdev_features;
	int index_last_normalized; // last item whose features have been normalized
	long synthesisID;
	int mediaID; // mid of the media currently being imported. by default, starts at 0 and is incremented after each import.
	std::string media_path;
	preProcessInfo mPreProcessInfo;
	ACPreProcessPlugin* mPreProcessPlugin;
	ACMediaReaderPlugin* mReaderPlugin;
	
public:
	ACMediaLibrary();
	ACMediaLibrary(ACMediaType aMediaType);
	~ACMediaLibrary();
	bool changeMediaType(ACMediaType aMediaType);
	void cleanLibrary();
	std::vector<std::string> getExtensionsFromMediaType(ACMediaType media_type);
	bool isEmpty();
	ACMediaType getMediaType(){return media_type;};
	void setMediaType(ACMediaType m){media_type = m;};
	int getSize() { return media_library.size(); };

        void setTitle(std::string _title){this->metadata.title=_title;}
        void setAuthor(std::string _author){this->metadata.author=_author;}
        void setYear(std::string _year){this->metadata.year=_year;}
        void setPublisher(std::string _publisher){this->metadata.publisher=_publisher;}
        void setLicense(std::string _license){this->metadata.license=_license;}
        void setWebsite(std::string _website){this->metadata.website=_website;}
        void setCover(std::string _cover){this->metadata.cover=_cover;}
        void setCurator(ACUserProfile _curator){this->metadata.curator=_curator;}
        void setCuratorName(std::string _curator){this->metadata.curator.name=_curator;}
        std::string getTitle(){return this->metadata.title;}
        std::string getAuthor(){return this->metadata.author;}
        std::string getYear(){return this->metadata.year;}
        std::string getPublisher(){return this->metadata.publisher;}
        std::string getLicense(){return this->metadata.license;}
        std::string getWebsite(){return this->metadata.website;}
        std::string getCover(){return this->metadata.cover;}
        ACUserProfile getCurator(){return this->metadata.curator;}
        std::string getCuratorName(){return this->metadata.curator.name;}

	std::vector<ACMedia*> getAllMedia() {return media_library;};
	int addMedia(ACMedia *aMedia);
	ACMedia* getMedia(int i);
	int deleteMedia(int i);
	int getMediaIndex(std::string media_file_name);
	std::string getThumbnailFileName(int i);

	long getSynthesisID(){return synthesisID;};
	void setSynthesisID(long _id){synthesisID = _id;};
	
	std::vector<int> getParentIds(void);
	
	void normalizeFeatures(int needsNormalize=1);
	void denormalizeFeatures();
	void calculateStats();
	
	std::vector<std::string> getListOfActivePlugins();

	std::vector< std::vector<double> > getMeanFeatures() {return mean_features;};
	std::vector< std::vector<double> > getStdevFeatures() {return stdev_features;};
	
	int importDirectory(std::string _path, int recursive,  ACPluginManager *acpl=0, bool forward_order=true, bool doSegment=false, bool _save_timed_feat=false); //, TiXmlElement* _medias = 0);
	int importFile(std::string _filename, ACPluginManager *acpl=0, bool doSegment=false, bool _save_timed_feat = false); //, TiXmlElement* _medias = 0);

	// I/O (C++ version; plain C version discontinued sep 2010)
	int setPath(std::string path);
	std::string getPath() { return media_path; };
	//int parseACLLibrary(std::string _path, bool aInitLib=false);
	//int parseXMLLibrary(std::string _path, bool aInitLib=false);
	int openACLLibrary(std::string _path, bool aInitLib=false);
	int openMCSLLibrary(std::string _path, bool aInitLib=false);//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
	int saveACLLibrary(std::string _path);
	int saveMCSLLibrary(std::string _path);//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
	
	int openXMLLibrary(std::string _path, bool aInitLib=false);
	int openCoreXMLLibrary(TiXmlHandle _rootHandle);
	int saveXMLLibrary(std::string _path);
	int saveCoreXMLLibrary( TiXmlElement* _MC_e_root, TiXmlElement* _MC_e_medias);

	//XS special for Thomas Israel
	void saveSorted(std::string ouput_file);	
	
	int scanDirectories(std::vector<std::string> _path, int _recursive, std::vector<std::string>& filenames);
	
	void setPreProcessPlugin(ACPlugin* acpl);
	void setMediaReaderPlugin(ACPlugin* acpl);
	
	ACMediaType getActiveSubMediaType();
	#ifdef SUPPORT_MULTIMEDIA
	int setActiveMediaType(std::string mediaName);
	#endif//def SUPPORT_MULTIMEDIA
	
private:
	void deleteAllMedia();
	void cleanStats();
	int scanDirectory(std::string _path, int _recursive, std::vector<std::string>& filenames);
	void incrementMediaID(){mediaID++ ;}
	int getMediaID(){return mediaID ;}
	void setMediaID(int _id){mediaID=_id;} // XS TODO add checks ?

//	#if defined(SUPPORT_VIDEO)
//		int testFFMPEG(std::string _filename);
//	#endif //defined (SUPPORT_IMAGE OR SUPPORT_VIDEO)

};

#endif // ACMEDIALIBRARY_H
