/**
 * @brief Base media renderer, for browser and timeline
 * @author Christian Frisson
 * @date 20/12/2012
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

#ifndef __ACOsgBaseRenderer_H__
#define __ACOsgBaseRenderer_H__

#include <MediaCycle.h>
#include <ACMedia.h>

#include "ACRefId.h"

#include <string>

#include <osg/ref_ptr>
#include <osg/Geometry>
#include <osgText/Font>
#include <osgText/Text>

// Common OSG classes for derived classes
#include <osg/LineWidth>
#include <osg/ShapeDrawable>

class ACOsgBaseRenderer {
public:
    ACOsgBaseRenderer();
    virtual ~ACOsgBaseRenderer();
protected:
    MediaCycle* media_cycle;
    ACMediaType media_type;
    ACSettingType setting;
    ACMedia* media;
    bool media_changed;
    std::string media_cycle_filename;
    std::string shared_thumbnail;
    osg::ref_ptr<osgText::Font> font;
public:
    void setMediaCycle(MediaCycle *_media_cycle) { this->media_cycle = _media_cycle;}
    ACMediaType getMediaType(){return media_type;}
osg::ref_ptr<osg::Geometry> thumbnailGeometry(std::string _thumbnail_name);
    //void setMediaIndex(int media_index);
    void setMedia(ACMedia* _media);
    ACMedia* getMedia(){return media;}
    void setFilename(std::string media_cycle_filename);
    void setSharedThumbnailName(std::string name){this->shared_thumbnail=name;}
    std::string getSharedThumbnailName(){return this->shared_thumbnail;}

    void setFont(osg::ref_ptr<osgText::Font> _font){this->font = _font;}

    // Setting specific
    virtual void changeSetting(ACSettingType _setting){this->setting = _setting;}// = 0;
};

#endif
