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

#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;

class ACMediaLibrary {
	// these methods should be common to all media
	// if classes were well defined
protected:
	ACMediaType media_type; // useful ??
	std::vector<ACMedia*> media_library;
	std::vector< std::vector<double> > mean_features, stdev_features;
	int index_last_normalized; // last item whose features have been normalized
public:
	ACMediaLibrary();
	~ACMediaLibrary() {}
	
	bool isEmpty();
	ACMediaType getMediaType(){return media_type;}
	void setMediaType(ACMediaType m){media_type = m;}
	int getSize() { return media_library.size(); };
	int createItem(){} // TODO
	int deleteItem(int i) {} //TODO
	void editItem() {} // TODO
	ACMedia* getItem(int i);
	std::string getThumbnail(int i);
	
	void normalizeFeatures();
	void denormalizeFeatures();
	void calculateStats();

	std::vector<ACMedia*> getMedia() {return media_library;};
	
	int importDirectory(std::string path, int recursive, int id=-1);

	// trying not to redefine it (except for audio...)
	int openLibrary(std::string _path, bool aInitLib=false);

	// have to be redefined for each media -- really ?
	void saveAsLibrary(std::string _path);
	//XS special for Thomas Israel
	void saveSorted(std::string ouput_file);
	
	void cleanLibrary();
	void cleanStats();

};

#endif // ACMEDIALIBRARY_H