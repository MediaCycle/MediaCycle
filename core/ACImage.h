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

#if defined (APPLE_IOS)//CF ugly
#else

#include "ACOpenCVInclude.h"
#include "ACMedia.h"
#include <string>

// -----------------------------------

class ACImage: public ACMedia {
	// contains the *minimal* information about an image
public:
	ACImage();
	~ACImage();
	
	void saveACLSpecific(ofstream &library_file);
	int loadACLSpecific(ifstream &library_file);

	//void setThumbnail(osg::Image* _thumbnail) { thumbnail = _thumbnail; thumbnail_width = _thumbnail->width; thumbnail_height = _thumbnail->height; }
	void setThumbnail(osg::Image* _thumbnail) { thumbnail = _thumbnail; thumbnail_width = _thumbnail->s(); thumbnail_height = _thumbnail->t(); }
	// IplImage* getThumbnail() { return thumbnail; }
	int getThumbnailWidth() {return thumbnail_width;}
	int getThumbnailHeight() {return thumbnail_height;}
	void* getThumbnailPtr() { return (void*)thumbnail; }
	
	IplImage* getData(){return data->getImageData();}
	void setData(IplImage* _data);
	
	ACMediaData* extractData(std::string fname);
	
	osg::Image* Convert_OpenCV_TO_OSG_IMAGE(IplImage* cvImg);
		
private:	
	static const int default_thumbnail_width, default_thumbnail_height, default_thumbnail_area;
	char  *thumbnail_filename;
	int thumbnail_width, thumbnail_height; 
	osg::Image* thumbnail;
	
	int computeThumbnail(string _fname, int w=0, int h=0);
	int computeThumbnail(ACMediaData* data_ptr, int w=0, int h=0);
	int checkWidth(int w);
	int checkHeight(int h);
	void computeThumbnailSize();
};

#endif//CF APPLE_IOS
#endif // ACIMAGE_H
