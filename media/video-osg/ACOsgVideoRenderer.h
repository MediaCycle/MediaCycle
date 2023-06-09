/*
 *  ACOsgBrowserRenderer.h
 *  MediaCycle
 *
 *  @author Stéphane Dupont
 *  @date 24/08/09
 *
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

#ifndef __ACOSG_VIDEO_RENDERER_H__
#define __ACOSG_VIDEO_RENDERER_H__

#include "ACOsgMediaRenderer.h"

#include <osg/ImageStream>

class ACOsgVideoRenderer : public ACOsgMediaRenderer  {
protected:
    osg::ref_ptr<osg::Geode> video_geode;
    osg::ref_ptr<osg::Geode> aura_geode;
    osg::ref_ptr<osg::Geode> border_geode;
    osg::ref_ptr<osg::Geode> ground_geode;
    osg::ref_ptr<osg::MatrixTransform> video_transform;
    osg::ref_ptr<osg::ImageStream> video_stream;
#ifdef AUTO_TRANSFORM
    osg::ref_ptr<osg::AutoTransform> thumbnail_transform;
#else
    osg::ref_ptr<osg::MatrixTransform> thumbnail_transform;
#endif
    osg::ref_ptr<osg::Geometry> thumbnail_geometry;
    osg::ref_ptr<osg::Geode> thumbnail_geode;

    void videoGeode(bool flip=false, float sizemul=1.0, float zoomin=1.0);
    void auraVideoGeode(bool flip=false, float sizemul=1.0, float zoomin=1.0);
    void auraGeode();

    double node_size;

public:
    ACOsgVideoRenderer();
    ~ACOsgVideoRenderer();
    void prepareNodes();
    void updateNodes(double ratio=0.0);
    virtual osg::ref_ptr<osg::Geode> getMainGeode() { return video_geode;}

    virtual void changeNodeThumbnail(std::string thumbnail);
    virtual void changeNodeSize(double _size);
};

#endif
