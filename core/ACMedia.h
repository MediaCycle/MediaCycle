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

#include <string>

class ACMedia {
	// contains the minimal information about a media
	// uses vector of vector to store media features. 
	// e.g., for image, features[0] = color descriptors, etc.
protected:
	int mid;
	std::vector<ACMediaFeatures*> features;
	std::string filename;
	std::string filename_thumbnail;
	ACMediaType _type;
	char  **text_tags;
	char  **hyper_links;	
	
public:
	ACMedia() { mid = -1; };
	virtual ~ACMedia() {};
	
	void setId(int _id) {mid = _id;}; // SD TODO - should check for duplicate id?
	int getId() {return mid;};
	
	std::vector<ACMediaFeatures*> &getFeatures() { return features; }
	ACMediaFeatures* &getFeatures(int i);
	int getNumberOfFeatures() {return features.size();}

	int addFeatures(ACMediaFeatures *aFeatures) { this->features.push_back(aFeatures); };
	
	std::string getFileName() { return filename; };
	std::string getThumbnail() { return filename_thumbnail; };
	void setFileName(std::string s) { filename = s; };
	virtual void* getThumbnailPtr()=0;
	virtual int getWidth()=0;
	virtual int getHeight()=0;
	
	ACMediaType	getType() {return this->_type;}	
	virtual void save(FILE *){}
	virtual int load(FILE*){}
	virtual int import(std::string _path, int id=-1, ACPluginManager *acpl=NULL);
	// XS 23/09/09 : I implemented import in ACMedia.cpp, since it is the same for all media
	// XS 23/09/09 : import returns 1 if it worked, 0 if it failed
};

#endif // ACMEDIA_H
