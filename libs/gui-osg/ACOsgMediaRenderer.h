/**
 * @brief The media browser node renderer base class, implemented with OSG
 * @author Stéphane Dupont
 * @date 24/08/09
 * @copyright (c) 2009 – UMONS - Numediart
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

#ifndef __ACOSG_MEDIA_RENDERER_H__
#define __ACOSG_MEDIA_RENDERER_H__

#include "ACOsgBaseRenderer.h"

#include <osg/Geode>
#include <osg/Group>
#include <osg/MatrixTransform>

//#include "ACOsgAbstractDefaultConfig.h"

#if defined(APPLE_IOS)
#define AUTO_TRANSFORM
#endif 

class ACOsgMediaRenderer : public ACOsgBaseRenderer {
public:
    ACOsgMediaRenderer();
    virtual ~ACOsgMediaRenderer();
protected:
    osg::ref_ptr<osg::Group>  local_group;
#ifdef AUTO_TRANSFORM
    osg::ref_ptr<osg::AutoTransform> media_node;
#else
    osg::ref_ptr<osg::MatrixTransform> media_node;
#endif
    int node_index;
    int cluster_index;
    std::string label;
    //bool media_changed;
    float distance_mouse;
    // int media_activity;
    osg::Vec4 node_color;
    std::vector<osg::Vec4> cluster_colors;
    osg::Vec4 neighbor_color;
    bool user_defined_color;

    osg::ref_ptr<osg::Geode> metadata_geode;
    osg::ref_ptr<osgText::Text> metadata;
    void metadataGeode();

    // GLOBAL
    double media_cycle_deltatime;
    float media_cycle_zoom;
    float media_cycle_angle;
    int media_cycle_modxe;
    int media_cycle_global_navigation_level;
    int media_cycle_mode;

    // NODE SPECIFIC
    bool media_cycle_isdisplayed;
    bool media_cycle_isTagged;
    ACPoint media_cycle_current_pos;
    ACPoint media_cycle_view_pos;
    ACPoint media_cycle_next_pos;
    int media_cycle_navigation_level;
    int media_cycle_activity;
    int media_index;
    int prev_media_index;

    bool initialized;
    float frac;
    float afac;

public:
    void setNodeIndex(int _node_index) { this->node_index = _node_index;}
    void setDistanceMouse(float _distance_mouse) { this->distance_mouse = _distance_mouse;}
    float getDistanceMouse(void) { return this->distance_mouse;}
    //void setActivity(int _media_activity) { this->media_activity = _media_activity; }
    osg::ref_ptr<osg::Group>  getNode() { return local_group;}
    virtual osg::ref_ptr<osg::Geode> getMainGeode() { return new osg::Geode;}
    int	getNodeIndex() { return node_index;}

    virtual void prepareNodes()=0;
    virtual void updateNodes(double ratio=0.0)=0;

    void changeNodeColor(osg::Vec4 _color){node_color = _color; user_defined_color = true;}
    void resetNodeColor(){node_color = osg::Vec4(1,1,0.5,1); user_defined_color = false;}

    virtual void changeNodeSize(double _size){}

    /// This virtual function is called by ACOsgBrowserRenderer::changeNodeThumbnail
    virtual void changeThumbnail(std::string thumbnail){}

    ACOsgMediaThumbnail* getNodeThumbnail();
    osg::ref_ptr<osg::Image> getNodeThumbnailImage();
    osg::ref_ptr<osg::Texture2D> getNodeThumbnailTexture();
    osg::ref_ptr<osg::ImageStream> getNodeThumbnailStream();
protected:
    std::string node_thumbnail;

public:
    void setDeltaTime(double media_cycle_deltatime);
    void setZoomAngle(float media_cycle_zoom, float media_cycle_angle);
    void setMode(int media_cycle_mode);
    void setGlobalNavigation(int media_cycle_global_navigation_level);
    void setIsDisplayed(int media_cycle_isdisplayed);
    int getIsDisplayed(){return media_cycle_isdisplayed;}
    void setIsTagged(int media_cycle_isTagged);
    int getIsTagged(){return media_cycle_isTagged;}

    //void setPos(ACPoint &media_cycle_current_pos, ACPoint &media_cycle_next_pos);
    void setViewPos(ACPoint media_cycle_view_pos);
    void setCurrentPos(ACPoint media_cycle_current_pos);
    void setNextPos(ACPoint media_cycle_next_pos);
    ACPoint getViewPos();
    ACPoint getCurrentPos();
    ACPoint getNextPos();
    bool getInitialized() { return initialized; }
    void setFrac(float frac) { this->frac = frac; }
    float getFrac() {return this->frac; }
    void setNavigation(int media_cycle_navigation_level);
    void setActivity(int media_cycle_activity);

};

class ACOsgMediaNodeCallback :public osg::NodeCallback {
    public:
    ACOsgMediaNodeCallback(ACOsgMediaRenderer* _renderer=0):osg::NodeCallback(),renderer(_renderer){}
    virtual ~ACOsgMediaNodeCallback(){renderer=0;}
    void operator()( osg::Node* node, osg::NodeVisitor* nv );
protected:
    ACOsgMediaRenderer* renderer;
};

#endif
