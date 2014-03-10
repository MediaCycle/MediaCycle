/*
 *  ACOsgNodeLinkRenderer.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 16/03/2010
 *
 *  @copyright (c) 2010 – UMONS - Numediart
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

#ifndef __ACOSG_NODELINK_RENDERER_H__
#define __ACOSG_NODELINK_RENDERER_H__

#include "MediaCycle.h"

#include <osg/ref_ptr>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Geode>
#include <osg/Group>
#include <osg/MatrixTransform>

#include "ACOsgMediaRenderer.h"

class ACOsgNodeLinkRenderer {
public:	
    void setMediaCycle(MediaCycle *_media_cycle);
    osg::ref_ptr<osg::MatrixTransform> getLink();
    void setNodeIn(ACOsgMediaRenderer* _node);
    void setNodeOut(ACOsgMediaRenderer* _node);
public:
    ACOsgNodeLinkRenderer();
    ~ACOsgNodeLinkRenderer();
    void prepareLinks();
    void updateLinks();
    void setWidth(float _width);
    void setColor(osg::Vec4 _color);
protected:
    MediaCycle* media_cycle;
    osg::ref_ptr<osg::MatrixTransform> link_node;
    osg::ref_ptr<osg::Geode> link_geode;
    void linkGeode(double to_x, double to_y);
    ACOsgMediaRenderer* node_in, *node_out;
    osg::Vec4 link_color;
    float width;
    osg::ref_ptr<osg::Geometry> link_geometry;
};
#endif
