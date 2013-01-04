/**
 * @brief A class that allows to share a video stream between the browser and the timeline.
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

#include "ACOsgVideoThumbnail.h"

#include <osg/ImageUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>

#include <boost/filesystem.hpp>

#include <iostream>
using namespace std;

// ----------- class constants
const int ACOsgVideoThumbnail:: default_thumbnail_area = 4096; // 64 * 64

ACOsgVideoThumbnail::ACOsgVideoThumbnail(std::string _filename, int media_width, int media_height) : ACOsgMediaThumbnail(_filename, media_width, media_height) {
    media_type = MEDIA_TYPE_VIDEO;
    name = "Video image stream (OSG)";
    description = "Video image stream (OSG)";
    thumbnail = 0;
    image_stream=0;
    thumbnail=0;
    image_texture=0;
    this->computeThumbnail(media_width,media_height);
}

ACOsgVideoThumbnail::~ACOsgVideoThumbnail() {
	if (image_stream) image_stream->quit();	
	/*if (image_stream){
		osg::ImageStream::StreamStatus streamStatus = image_stream->getStatus();
		switch (streamStatus) {
			case osg::ImageStream::INVALID:
				//std::cout << "Image stream invalid status" << std::endl;
				break;
			case osg::ImageStream::PLAYING:
				image_stream->pause();
				break;
			case osg::ImageStream::PAUSED:
				break;
			case osg::ImageStream::REWINDING:
				//std::cout << "Image stream rewinding" << std::endl;
				break;
			default:
				break;
		}
	}*/	
	image_stream=0;	
	thumbnail=0;
    image_texture=0;
}

bool ACOsgVideoThumbnail::computeThumbnail(int w, int h){
    if (!computeThumbnailSize(w, h))
        return false;
	bool ok = true;
	//std::cout << boost::filesystem::extension(filename);
	/// prerequisites for loading OSG media files, 2 alternatives
	/// 1) standard procedure: checking for a plugin that can open the format of the media file
//	osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension(boost::filesystem::extension(filename).substr(1));
	/// 2) hack: forcing the use of the ffmpeg plugin by checking the plugin that can open the ffmpeg format (most probably the ffmpeg plugin)
	osg::ref_ptr<osgDB::ReaderWriter> readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension("ffmpeg");
	if (!readerWriter){
        cerr << "<ACOsgVideoThumbnail::computeThumbnail> (video id = " << this->getId() << ") : problem loading file, no OSG plugin available" << endl;
		return false;
	}
	else{
        cout <<"<ACOsgVideoThumbnail::computeThumbnail> (video id = " << this->getId() << ") using OSG plugin: "<< readerWriter->className() <<std::endl;
	}	

    thumbnail = osgDB::readImageFile(filename);
	readerWriter = 0;
	// XS TODO : needs rescaling !!
    //thumbnail->scaleImage(width,height,1);
	//thumbnail->setAllocationMode(osg::Image::NO_DELETE);
	
	if (!thumbnail){
        cerr << "<ACOsgVideoThumbnail::computeThumbnail> problem creating thumbnail" << endl;
		return false;
	}
	
	// Saving the video as texture for transmission
	image_texture = new osg::Texture2D;
	image_texture->setImage(thumbnail);
	
	// Converting the video as preloaded stream to transmit the same instance to multiple recipient with unified playback controls
	image_stream = dynamic_cast<osg::ImageStream*>(thumbnail.get());
	image_stream->setLoopingMode(osg::ImageStream::LOOPING); // todo: this won't work for segments which may not loop back from the beginning of the file
	
	// Hack to display a first valid frame, quite long!
	//while (thumbnail->isImageTranslucent())
	//	image_stream->play();
	//image_stream->pause();
	//image_stream->rewind();
	
	image_stream->seek(this->start); // to start with the correct frame, especially for segments
	
	// CF: Adding these two blank the geodes under OSX 10.6.8
	//image_stream->play(); // to display a frame
	//image_stream->pause(); // to stop the playback once a frame is displayed
	
	return ok;
}

// cut and paste from ACImage
// XS TODO should put this in ACSpatialMedia
bool ACOsgVideoThumbnail::computeThumbnailSize(int w_, int h_){
    // we really want a specific (positive) thumbnail size
    bool ok = true;
    if ((w_ > 0) && (h_ > 0)) {
        // we just scale the original width and height to the default thumbnail area
        float scale = sqrt((float)default_thumbnail_area/((float)w_*(float)h_));
        width = (int)(w_*scale);
        height = (int)(h_*scale);
    }
    else {
        std::cerr << "Video dimensions not set." << std::endl;
        ok = false;
    }
    return ok;
}
