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

#ifndef __ACOSG_HUD_RENDERER_H__
#define __ACOSG_HUD_RENDERER_H__

#include "MediaCycle.h"

#include "ACOsgMediaRenderer.h"
//#include "ACOsgLayoutRenderer.h"
#include "ACOsgNodeLinkRenderer.h"
#include "ACOsgPointerRenderer.h"
#include "ACOsgLibraryRenderer.h"

#include <osgViewer/Viewer>

#include "ACRefId.h"

#include <sys/time.h>

class ACOsgHUDRenderer {
protected:
    MediaCycle *media_cycle;
    osg::ref_ptr<osg::Camera> camera;
    osg::ref_ptr<osg::Group> group;
    osg::ref_ptr<osg::Group> pointer_group;
    std::vector<ACOsgPointerRenderer*> pointer_renderer;
    ACOsgLibraryRenderer* library_renderer;
    ACSettingType setting;
    osg::ref_ptr<osgText::Font> font;

    // SD - Results from centralized request to MediaCycle
    ACPoint media_cycle_pointer_current_pos;
public:
    ACOsgHUDRenderer();
    ~ACOsgHUDRenderer();

    void clean();
    void cleanPointers();
    void cleanLibrary();

    double getTime();

    void setMediaCycle(MediaCycle *media_cycle);
    void setFont(osg::ref_ptr<osgText::Font> _font){this->font = _font;}

    osg::ref_ptr<osg::Camera> getCamera();

    void preparePointers(osgViewer::View* view = 0);
    void prepareLibrary(osgViewer::View* view = 0);

    void updatePointers(osgViewer::Viewer* viewer);//Cocoa - simple OSG viewer
    void updatePointers(osgViewer::View* viewer);//Qt - composite OSG viewer
    void updateLibrary(osgViewer::View* viewer);

    void changeSetting(ACSettingType _setting);

private:
    void updatePointers(int w, int h);//common
};

#endif
