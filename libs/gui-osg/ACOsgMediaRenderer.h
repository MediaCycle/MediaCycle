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

#ifndef __ACOSG_MEDIA_RENDERER_H__
#define __ACOSG_MEDIA_RENDERER_H__

#include <MediaCycle.h>
#include <ACMedia.h>

#include <osg/ref_ptr>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Geode>
#include <osg/Group>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/AutoTransform>
#include <osg/BlendFunc>
#include <osg/BlendColor>
#include <osg/Texture2D>
#include <osg/Image>
#include <osgUtil/SceneView>
#include <osgViewer/Viewer>
#include <osgText/Font>
#include <osgText/Text>

#include "ACRefId.h"

#include "ACOsgAbstractDefaultConfig.h"

#if defined(APPLE_IOS)
#define AUTO_TRANSFORM
#endif 

class ACOsgMediaRenderer {
protected:
    MediaCycle* media_cycle;
    ACMediaType media_type;
    ACSettingType setting;
    osg::ref_ptr<osg::Group>  local_group;
#ifdef AUTO_TRANSFORM
    osg::ref_ptr<osg::AutoTransform> media_node;
#else
    osg::ref_ptr<osg::MatrixTransform> media_node;
#endif
    int node_index;
    int cluster_index;
    ACMedia* media;
    std::string label;
    bool media_changed;
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
    std::string media_cycle_filename;
    int media_index;
    int prev_media_index;

    bool initialized;
    float frac;

#if defined(APPLE_IOS)
    float afac;
#else
    float afac;
#endif
    osg::ref_ptr<osgText::Font> font;

public:
    ACOsgMediaRenderer();
    virtual ~ACOsgMediaRenderer();

    void setMediaCycle(MediaCycle *_media_cycle) { this->media_cycle = _media_cycle; };
    ACMediaType getMediaType(){return media_type;}
    void setNodeIndex(int _node_index) { this->node_index = _node_index; };
    void setDistanceMouse(float _distance_mouse) { this->distance_mouse = _distance_mouse; };
    float getDistanceMouse(void) { return this->distance_mouse ; };
    //void setActivity(int _media_activity) { this->media_activity = _media_activity; }
    osg::ref_ptr<osg::Group>  getNode() { return local_group; };
    virtual osg::ref_ptr<osg::Geode> getMainGeode() { return new osg::Geode;}
    int	getNodeIndex() { return node_index; };
    void setFont(osg::ref_ptr<osgText::Font> _font){this->font = _font;}

    virtual void prepareNodes()=0;
    virtual void updateNodes(double ratio=0.0)=0;

    void changeNodeColor(osg::Vec4 _color){node_color = _color; user_defined_color = true;}
    void resetNodeColor(){node_color = osg::Vec4(1,1,0.5,1); user_defined_color = false;}

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
    //void setMediaIndex(int media_index);
    void setMedia(ACMedia* _media);
    void setFilename(std::string media_cycle_filename);

    // Setting specific
    virtual void changeSetting(ACSettingType _setting){this->setting = _setting;}// = 0;
};

#endif
