/*
 *  ACOsgRendererPlugin.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 1/11/2012
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

#ifndef _ACOSGRENDERERPLUGIN_H
#define _ACOSGRENDERERPLUGIN_H

#include <string>
#include <vector>

#include "ACPlugin.h"
#include "ACOsgMediaRenderer.h"
#include "ACOsgTrackRenderer.h"
#include "ACOsgBrowserRenderer.h"
#include "ACOsgTimelineRenderer.h"

#include "ACMedia.h"
#include "ACMediaThumbnail.h"
#include "ACMediaData.h"

#include "ACMediaBrowser.h"
#include "MediaCycle.h"

#include "ACPluginManager.h"

// plugin to verify which formats the viewer can render
class ACOsgRendererPlugin : virtual public ACMediaRendererPlugin{
public:
    ACOsgRendererPlugin();
    //virtual ~ACOsgRendererPlugin();
    virtual ACOsgMediaRenderer* createMediaRenderer(ACMediaType media_type){return 0;}
    virtual ACOsgTrackRenderer* createTrackRenderer(ACMediaType media_type){return 0;}
    virtual ACMediaThumbnail* createSharedThumbnail(ACMedia* media){return 0;}
    virtual std::string sharedThumbnailName(ACMediaType media_type){return "";}
    virtual std::vector<ACMediaType> getSupportedMediaTypes(){return std::vector<ACMediaType>();}//=0;
    void setBrowserRenderer(ACOsgBrowserRenderer* _browser);
    void setTimelineRenderer(ACOsgTimelineRenderer* _timeline);
    void changeBrowserThumbnail();
    void changeTimelinePlaybackThumbnail();
    void changeTimelineSelectionThumbnail();
    void changeTimelineSummaryThumbnail();
    void changeBrowserNodeColor();
    void changeBrowserNodeSize();
protected:
    ACOsgBrowserRenderer* browser;
    ACOsgTimelineRenderer* timeline;
};

#endif	/* _ACOSGRENDERERPLUGIN_H */
