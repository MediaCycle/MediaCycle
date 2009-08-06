/*
 *  ACImage[loop].h
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

#ifndef ACIMAGE_H
#define ACIMAGE_H

#include "ACOpenCVInclude.h"
#include "ACMedia.h"
#include <string>

// -----------------------------------
// TODO : better than these #define

#define IMAGE_LOOP_SHAPE_NUM_EL  24
#define IMAGE_LOOP_COLOR_NUM_EL 24
#define IMAGE_LOOP_TEXTURE_NUM_EL  24
#define FEATURES_NUM_EL	3

//#define INDEX_SHAPE 0
//#define INDEX_COLOR 1
//#define INDEX_TEXTURE 2

// -----------------------------------

class ACImage: public ACMedia {
	// contains the *minimal* information about an image
	// is this too much already ?
public:
	ACImage();
	~ACImage();
	
	void save(FILE* library_file); // was: saveLoop
	int load(FILE* library_file); // was: loadLoop
	void import(std::string _path); // was: importFile in library

	// TODO: make the following explicitely inlines ?
	void setThumbnail(IplImage *_thumbnail) { thumbnail = _thumbnail; thumbnail_width = _thumbnail->width; thumbnail_height = _thumbnail->height; }
// SD?	void setThumbnailSave(IplImage *_thumbnail);
	void* getThumbnailPtr() { return (void*)thumbnail; }
	
	// do we need the original size ?
	// the thumbnail size can be accessed by thumbnail->width/height !! ;
	int getWidth() {return width;}
	int getHeight() {return height;}
	void setWidth(int w) {width = w;} // XS TODO : check if it matches thumbnail?
	void setHeight(int h) {height = h;}
	
private:	
	char  *thumbnail_filename;
	int width, height; // delete
	int thumbnail_width, thumbnail_height; // delete
	IplImage *thumbnail;
};

#endif // ACIMAGE_H
