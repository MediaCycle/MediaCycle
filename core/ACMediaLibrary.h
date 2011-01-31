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

#include "boost/filesystem.hpp"

class ACMediaLibrary {
	// these methods are common to all media.
	// there is no need for an AudioLibrary, ImageLibrary, ...
	// media-specific methods (e.g., save, load) are in ACMedia and overloaded in ACAudio, ACImage, ACVideo, ...
protected:
	ACMediaType media_type;
	std::vector<ACMedia*> media_library;
	std::vector< std::vector<double> > mean_features, stdev_features;
	int index_last_normalized; // last item whose features have been normalized
	long synthesisID;
	// XS new 300810
	int mediaID; // mid of the media currently being imported. by default, starts at 0 and is incremented after each import.
	std::string media_path;
	
public:
	ACMediaLibrary();
	ACMediaLibrary(ACMediaType aMediaType);
	~ACMediaLibrary();
	bool changeMediaType(ACMediaType aMediaType);
	void cleanLibrary();
	void cleanStats();
	
	bool isEmpty();
	ACMediaType getMediaType(){return media_type;};
	void setMediaType(ACMediaType m){media_type = m;};
	int getSize() { return media_library.size(); };

//	void editMedia() {}; // TODO ?

	std::vector<ACMedia*> getAllMedia() {return media_library;};
	int addMedia(ACMedia *aMedia);
	ACMedia* getMedia(int i);
	int deleteMedia(int i);
	
	std::string getThumbnailFileName(int i);

	long getSynthesisID(){return synthesisID;};
	void setSynthesisID(long _id){synthesisID = _id;};
	
	void normalizeFeatures(int needsNormalize=1);
	void denormalizeFeatures();
	void calculateStats();

	std::vector< std::vector<double> > getMeanFeatures() {return mean_features;};
	std::vector< std::vector<double> > getStdevFeatures() {return stdev_features;};
	
	int importDirectory(std::string _path, int recursive,  ACPluginManager *acpl=NULL, bool forward_order=true, bool doSegment=false, bool _save_timed_feat=false);
	int importFile(std::string _filename, ACPluginManager *acpl=NULL, bool doSegment=false, bool _save_timed_feat = false);

	//int openLibrary(std::string _path, bool aInitLib=false); // SD 2010 sep discontinued
//	void saveAsLibrary(std::string _path);
	// C++ versions
	int setPath(std::string path);
	std::string getPath() { return media_path; };
	int openACLLibrary(std::string _path, bool aInitLib=false);
	int openMCSLLibrary(std::string _path, bool aInitLib=false);//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
	int saveACLLibrary(std::string _path);
	int saveMCSLLibrary(std::string _path);//CF 31/05/2010 temporary MediaCycle Segmented Library (MCSL) for AudioGarden, adding a parentID for segments to the initial ACL, awaiting approval
	
	//XS special for Thomas Israel
	void saveSorted(std::string ouput_file);	

	// XS TODO: add pthreads
	//	void* p_importSingleFile(void *arg);
	
	int scanDirectories(std::vector<std::string> _path, int _recursive, std::vector<std::string>& filenames);
	int testFFMPEG(std::string _filename);
	
private:
	void deleteAllMedia();
	int scanDirectory(std::string _path, int _recursive, std::vector<std::string>& filenames);
	void incrementMediaID(){mediaID++ ;}
	int getMediaID(){return mediaID ;}

};

#endif // ACMEDIALIBRARY_H
