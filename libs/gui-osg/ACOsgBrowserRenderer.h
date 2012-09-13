/*
 * ACOsgBrowserRenderer.h
 * MediaCycle
 *
 * @author Stéphane Dupont
 * @date 24/08/09
 *
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
 *
 */

#ifndef __ACOSG_BROWSER_RENDERER_H__
#define __ACOSG_BROWSER_RENDERER_H__

#include "MediaCycle.h"

#include "ACOsgMediaRenderer.h"
//#include "ACOsgLayoutRenderer.h"
#include "ACOsgNodeLinkRenderer.h"
#include "ACOsgLabelRenderer.h"

#include <osgDB/ReadFile>
#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Texture2D>
#include <osg/Image>
#include <osg/ImageStream>
#include <osg/io_utils>
#include <osg/LineWidth>
#include <osg/ShapeDrawable>
#include <osgUtil/SceneView>
#include <osgViewer/Viewer>

#include "ACRefId.h"
#include <pthread.h>

#include "ACEventListener.h"

typedef std::map<long int,ACOsgMediaRenderer*> ACOsgMediaRenderers;
typedef std::map<long int,ACOsgNodeLinkRenderer*> ACOsgNodeLinkRenderers;

class ACOsgBrowserRenderer: public ACEventListener {
public:
    ACOsgBrowserRenderer();
    ~ACOsgBrowserRenderer();
    void clean();
    void setMediaCycle(MediaCycle *media_cycle){ this->media_cycle = media_cycle; media_cycle->addListener(this);}

    //Callbacks
    void mediaImported(int n,int nTot,int mId);
    void libraryCleaned();

protected:
    MediaCycle *media_cycle;
    osg::ref_ptr<osg::Group> group;
    osg::ref_ptr<osg::Group> media_group;
    osg::ref_ptr<osg::Group> link_group;
    osg::ref_ptr<osg::Group> label_group;
    osg::ref_ptr<osgText::Font> font;
    ACOsgMediaRenderers node_renderers;
    ACOsgNodeLinkRenderers link_renderers;
    std::vector<ACOsgLabelRenderer*> label_renderer;
    //ACOsgLayoutRenderer* layout_renderer;
    //vector<bool> media_selected;
    std::map<long int, float> distance_mouse;
    //ACPlugin* mLayoutPlugin;
    //ACOsgLayoutType layout_type;
    //int displayed_nodes;

    // SD - Results from centralized request to MediaCycle - GLOBAL
    double media_cycle_time;
    double media_cycle_prevtime;
    double media_cycle_deltatime;
    float media_cycle_zoom;
    float media_cycle_angle;
    int media_cycle_mode;
    int media_cycle_global_navigation_level;

    // SD - Results from centralized request to MediaCycle - NODE SPECIFIC
    ACMediaNode* media_cycle_node;
    bool media_cycle_isdisplayed;
    ACPoint media_cycle_current_pos;
    ACPoint media_cycle_view_pos;
    ACPoint media_cycle_next_pos;
    int media_cycle_navigation_level;
    int media_cycle_activity;
    int node_index;
    int media_index;
    int prev_media_index;
    std::string media_cycle_filename;
    int nodes_prepared;
    ACBrowserAudioWaveformType audio_waveform_type;
    ACSettingType setting;
    
    
    /*pthread_mutex_t activity_update_mutex;
    pthread_mutexattr_t activity_update_mutex_attr;*/
    OpenThreads::Mutex activity_update_mutex;

public:
    //void setLayoutPlugin(ACPlugin* acpl){mLayoutPlugin=acpl;}
    //void setLayout(ACOsgBrowserLayoutType _type){layout_type = _type;}
    osg::ref_ptr<osg::Group> getShapes() { return group; }

    void prepareNodes(int start=0);
    void updateNodes(double ratio=0.0);

    void prepareLabels(int start=0);
    void updateLabels(double ratio=0.0);

    int computeScreenCoordinates(osgViewer::View* view, double ratio=0.0); //CF: use osgViewer::Viewer* for simple Viewers
    std::map<long int, float> getDistanceMouse() { return distance_mouse; }

    void setFont(osg::ref_ptr<osgText::Font> _font){this->font = _font;}
    void changeNodeColor(int _node, osg::Vec4 _color){activity_update_mutex.lock();node_renderers[_node]->changeNodeColor(_color);activity_update_mutex.unlock();}
    void resetNodeColor(int _node){activity_update_mutex.lock();node_renderers[_node]->resetNodeColor();activity_update_mutex.unlock();}

    ACBrowserAudioWaveformType getAudioWaveformType(){return audio_waveform_type;}
    void setAudioWaveformType(ACBrowserAudioWaveformType _type);

    void changeSetting(ACSettingType _setting);

private:
    bool removeNodes();
    bool addNode(long int _id);
    bool addLink(long int _id);
    bool removeLinks();
    bool removeLinks(int _first, int _last);
    bool removeLabels(int _first=0, int _last=0);
    bool addLabels(int _first=0, int _last=0);
};
#endif
