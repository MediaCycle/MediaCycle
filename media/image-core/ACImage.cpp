/*
 *  ACImage.cpp
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

#include "ACImage.h"
#include <fstream>
#include <boost/filesystem.hpp>

using namespace std;

ACImage::ACImage() : ACMedia() {
	this->init();
}

void ACImage::init() {
	media_type = MEDIA_TYPE_IMAGE;
}	

// copy-constructor, used for example to generate fragments
ACImage::ACImage(const ACImage& m, bool reduce) : ACMedia(m) {
	this->init();
	// ... XS TODO
}

ACImage::~ACImage() {
}

// 1) extracts data 
// - height, width
bool ACImage::extractData(string fname){
    width = this->getMediaData()->getWidth();
    height = this->getMediaData()->getHeight();
    return true;
}

// obsolete + confusing with thumbnail width/height
void ACImage::saveACLSpecific(ofstream &library_file) {
	library_file << width << endl;
	library_file << height << endl;
}

// obsolete + confusing with thumbnail width/height
int ACImage::loadACLSpecific(ifstream &library_file) {
	library_file >> width;
	library_file >> height;
    /*if (computeThumbnail(filename, thumbnail_width , thumbnail_height) != 1){
		cerr << "<ACImage::loadACLSpecific> : problem computing thumbnail" << endl;
		return 0;
    }*/
	return 1;
}

void ACImage::saveXMLSpecific(TiXmlElement* _media){
	_media->SetAttribute("Width", width);
	_media->SetAttribute("Height", height);
    /*_media->SetAttribute("ThumbnailWidth", thumbnail_width);
    _media->SetAttribute("ThumbnailHeight", thumbnail_height);*/
}


int ACImage::loadXMLSpecific(TiXmlElement* _pMediaNode){
	int w=-1;
	int h=-1;
	int t_w=-1;
	int t_h=-1;

	_pMediaNode->QueryIntAttribute("Width", &w);
	if (w < 0)
		throw runtime_error("corrupted XML file, wrong image width");
	else
		this->width = w;

	_pMediaNode->QueryIntAttribute("Height", &h);
	if (h < 0)
		throw runtime_error("corrupted XML file, wrong image height");
	else
		this->height = h;
	
	// try to read thumbnail size
	// if undefined or missing attribute, t_w or t_h will be < 0
	// and computeThumbnailSize will take care of it
	
    /*_pMediaNode->QueryIntAttribute("ThumbnailWidth", &t_w);
	_pMediaNode->QueryIntAttribute("ThumbnailHeight", &t_h);
	
	if (!computeThumbnail(filename, t_w , t_h))
        throw runtime_error("<ACImage::loadXMLSpecific> : problem computing thumbnail");*/

	return 1;	
}
