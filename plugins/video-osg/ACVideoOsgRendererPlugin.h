/*
 *  ACVideoOsgRendererPlugin.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 04/11/12
 *  @copyright (c) 2012 – UMONS - Numediart
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

#include <ACPlugin.h>

#ifndef ACVideoOsgRendererPlugin_H
#define	ACVideoOsgRendererPlugin_H

#include "ACOsgRendererPlugin.h"

class ACVideoOsgRendererPlugin : public ACOsgRendererPlugin
{
public:
    ACVideoOsgRendererPlugin();
    ~ACVideoOsgRendererPlugin();
    virtual void setMediaCycle(MediaCycle* _media_cycle);
    virtual std::map<std::string, ACMediaType> getSupportedExtensions(ACMediaType media_type = MEDIA_TYPE_ALL);
    virtual ACOsgMediaRenderer* createMediaRenderer(ACMediaType media_type);
    virtual ACOsgTrackRenderer* createTrackRenderer(ACMediaType media_type);
    virtual std::vector<ACMediaType> getSupportedMediaTypes();
    void changeTimelineSelection();
    void changeTimelineSummmary();
    void toggleTimelinePlayback();
protected:
    std::vector<std::string> timeline_selection, timeline_summary;
};

#endif