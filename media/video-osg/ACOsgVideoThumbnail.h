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

#ifndef ACOsgVideoThumbnail_H
#define ACOsgVideoThumbnail_H

#include "ACOsgMediaThumbnail.h"
#include <string>

class ACOsgVideoThumbnail: virtual public ACOsgMediaThumbnail {
public:
    ACOsgVideoThumbnail(std::string _filename, int media_width, int media_height);
    virtual ~ACOsgVideoThumbnail();

    virtual osg::ref_ptr<osg::Image> getImage() { return thumbnail; }
    virtual osg::ref_ptr<osg::Texture2D> getTexture() { return image_texture; }
    virtual osg::ref_ptr<osg::ImageStream> getStream() {return image_stream;}

    bool computeThumbnail(int w=0, int h=0);
	bool computeThumbnailSize(int w_, int h_);

private:
	static const int default_thumbnail_area;
	osg::ref_ptr<osg::ImageStream>image_stream;	
	osg::ref_ptr<osg::Image> thumbnail;
	osg::ref_ptr<osg::Texture2D> image_texture;
	
};
#endif // ACOsgVideoThumbnail_H
