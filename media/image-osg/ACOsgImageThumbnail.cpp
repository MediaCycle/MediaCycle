/**
 * @brief A class that allows to share an image texture between the browser and the timeline.
 * @author Christian Frisson
 * @date 16/12/2012
 * @copyright (c) 2012 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
 */

#include "ACOsgImageThumbnail.h"

#include <osg/ImageUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>

#include <boost/filesystem.hpp>

#include <fstream>
using namespace std;

// ----------- class constants
const int ACOsgImageThumbnail:: default_thumbnail_area = 16384; // 128*128

//------------------------------------------------------------------

ACOsgImageThumbnail::ACOsgImageThumbnail(std::string _filename, int media_width, int media_height) : ACOsgMediaThumbnail(_filename, media_width, media_height) {
	media_type = MEDIA_TYPE_IMAGE;
    name = "Image texture (OSG)";
    description = "Image texture (OSG)";
	thumbnail = 0;
    image_texture = 0;
    this->computeThumbnail(media_width,media_height);
}	

ACOsgImageThumbnail::~ACOsgImageThumbnail() {
    thumbnail = 0;
    image_texture = 0;
}

bool ACOsgImageThumbnail::computeThumbnail(int w, int h){
	if (!computeThumbnailSize(w, h)) return false;
	bool ok = true;

    // using OSG -- with ref_ptr to ensure proper garbage collection
    string ext=boost::filesystem::extension(filename).substr(1);
    
    //std::string ext = osgDB::getFileExtension( filename );
    osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(ext);

    if (!readerWriter){
        cerr << "<ACOsgImageThumbnail::computeThumbnail> (image id = " << this->getId() << ") : problem loading file, no OSG plugin available for "<<ext << endl;
        return false;
    }
    else{
        cout <<"<ACOsgImageThumbnail::computeThumbnail> (image id = " << this->getId() << ") using OSG plugin: "<< readerWriter->className() <<std::endl;
    }

    thumbnail = osgDB::readImageFile(filename);
    readerWriter = 0;

	if (!thumbnail){
        cerr << "<ACOsgImageThumbnail::computeThumbnail> problem converting thumbnail to osg" << endl;
		ok= false;
	}
    image_texture = new osg::Texture2D;
    image_texture->setImage(thumbnail);
	return ok;
}

bool ACOsgImageThumbnail::computeThumbnailSize(int w_, int h_){
	// we really want a specific (positive) thumbnail size
	bool ok = true;
	if ((w_ > 0) && (h_ > 0)) {
        float scale = sqrt((float)default_thumbnail_area/((float)w_*(float)h_));
        width = (int)(w_*scale);
        height = (int)(h_*scale);
	}
	else {
		std::cerr << "Image dimensions not set." << std::endl;
		ok = false;
	}
	return ok;
}
