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
namespace fs = boost::filesystem;

class ACMediaLibrary {
	// these methods are common to all media.
	// there is no need for an AudioLibrary, ImageLibrary, ...
	// media-specific methods (e.g., save, load) are in ACMedia and overloaded in ACAudio, ACImage, ACVideo, ...
protected:
	ACMediaType media_type;
	std::vector<ACMedia*> media_library;
	std::vector< std::vector<double> > mean_features, stdev_features;
	int index_last_normalized; // last item whose features have been normalized
public:
	ACMediaLibrary();
	ACMediaLibrary(ACMediaType aMediaType);
	~ACMediaLibrary();
	
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
	
	void normalizeFeatures();
	void denormalizeFeatures();
	void calculateStats();

	std::vector< std::vector<double> > getMeanFeatures() {return mean_features;};
	std::vector< std::vector<double> > getStdevFeatures() {return stdev_features;};
	
	int importDirectory(std::string path, int recursive, int id=-1, ACPluginManager *acpl=NULL);

	int openLibrary(std::string _path, bool aInitLib=false);
	void saveAsLibrary(std::string _path);
	// C++ versions
	int openACLLibrary(std::string _path, bool aInitLib=false);
	int saveACLLibrary(std::string _path);
	
	//XS special for Thomas Israel
	void saveSorted(std::string ouput_file);
	
	void cleanLibrary();
	void cleanStats();

	// pthreads
//	void* p_importSingleFile(void *arg);

};

#endif // ACMEDIALIBRARY_H
