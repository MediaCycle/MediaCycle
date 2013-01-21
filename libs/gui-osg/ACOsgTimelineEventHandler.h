/**
 * @brief The timeline event handler filters user interaction for all media tracks, implemented with OSG
 * @author Christian Frisson
 * @date 28/04/2010
 * @copyright (c) 2010 – UMONS - Numediart
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

#ifndef AC_OSG_TIMELINE_EVENT_HANDLER_H
#define AC_OSG_TIMELINE_EVENT_HANDLER_H

#include <cstdio>
#include <osgGA/GUIEventHandler>
#include <osgViewer/View>
#include "ACRefId.h"

#include "MediaCycle.h"
#include "ACOsgTimelineRenderer.h"

class ACOsgTimelineEventHandler : public  osgGA::GUIEventHandler {
public:
    ACOsgTimelineEventHandler();
    virtual ~ACOsgTimelineEventHandler(){}
    void clean();

    void setMediaCycle(MediaCycle *_media_cycle){this->media_cycle = _media_cycle;}
    void setRenderer(ACOsgTimelineRenderer* _renderer){this->renderer = _renderer;}
    bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

protected:
    void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, bool hover);

private:
    ACOsgTimelineRenderer *renderer;
    MediaCycle *media_cycle;
    bool selecting_zone,selecting_zone_begin,selecting_zone_end;
    bool selecting_summary_waveform,selecting_summary_frames,selecting_summary_slider,selecting_selection_frames;
    bool selecting_segments;
    ACRefId* selection;
    float selection_begin,selection_end;
    float pushed_x,moved_x;
    float pushed_y,moved_y;
};

#endif // AC_OSG_TIMELINE_EVENT_HANDLER_H
