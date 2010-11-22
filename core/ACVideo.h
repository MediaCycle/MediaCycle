/*
 *  ACVideo.h
 *  MediaCycle
 *
 *  @author Xavier Siebert
 *  @date 25/05/09
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

#ifndef ACVIDEO_H
#define ACVIDEO_H

#if !defined (APPLE_IOS)

#include "ACOpenCVInclude.h"
#include "ACMedia.h"
#include <string>

class ACVideo: public ACMedia {
	// contains the *minimal* information about a video
public:
	ACVideo();
	ACVideo(const ACVideo& m);
	~ACVideo();
	
	void saveACLSpecific(ofstream &library_file);
	int loadACLSpecific(ifstream &library_file);

	void setThumbnail(IplImage *_thumbnail) { thumbnail = _thumbnail; thumbnail_width = _thumbnail->width; thumbnail_height = _thumbnail->height; }
	// IplImage* getThumbnail() { return thumbnail; }
	void* getThumbnailPtr() { return NULL; } // 
	int getThumbnailWidth() {return thumbnail_width;}
	int getThumbnailHeight() {return thumbnail_height;}

	ACMediaData* extractData(std::string fname);
	
private:	
	char  *thumbnail_filename;
	int thumbnail_width, thumbnail_height;
	IplImage *thumbnail;
	
	int computeThumbnail(ACMediaData* data_ptr, int w=0, int h=0);
};
#endif//CF APPLE_IOS
#endif // ACVIDEO_H
