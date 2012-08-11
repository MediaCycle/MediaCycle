/*
 *  ACAbstractWidgetQt.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 13/06/2012
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

#ifndef ACAbstractWidgetQt_H
#define ACAbstractWidgetQt_H

#include <MediaCycle.h>
#include <string>

#include <QtGui>

#if defined (SUPPORT_AUDIO)
#include <ACAudioEngine.h>
#endif //defined (SUPPORT_AUDIO)

#if defined (USE_OSC)
#include <ACOscBrowser.h>
#include <ACOscFeedback.h>
#endif //defined (USE_OSC)

#include "ACOsgCompositeViewQt.h"

class ACAbstractWidgetQt {
public:
    ACAbstractWidgetQt()
        :media_cycle(0),osg_view(0)
    {
#if defined (SUPPORT_AUDIO)
        audio_engine = 0;
#endif //defined (SUPPORT_AUDIO)
#if defined (USE_OSC)
        osc_browser = 0;
        osc_feedback = 0;
#endif //defined (USE_OSC)
    };
    virtual ~ACAbstractWidgetQt(){
#if defined (SUPPORT_AUDIO)
        audio_engine = 0;
#endif //defined (SUPPORT_AUDIO)
#if defined (USE_OSC)
        osc_browser = 0;
        osc_feedback = 0;
#endif //defined (USE_OSC)
    };

    virtual void setMediaCycle(MediaCycle* _media_cycle){ media_cycle = _media_cycle;}
    MediaCycle* getMediaCycle() {return media_cycle;}
#if defined (SUPPORT_AUDIO)
    virtual void setAudioEngine(ACAudioEngine* _audio_engine){ audio_engine = _audio_engine;}
    ACAudioEngine* getAudioEngine() {return audio_engine;}
#endif //defined (SUPPORT_AUDIO)
    virtual void setOsgView(ACOsgCompositeViewQt* _osg_view){ osg_view = _osg_view;}
    ACOsgCompositeViewQt* getOsgView() {return osg_view;}
#if defined (USE_OSC)
    virtual void setOscBrowser(ACOscBrowser* _browser){this->osc_browser = _browser;}
    virtual void setOscFeedback(ACOscFeedback* _feedback){this->osc_feedback = _feedback;}
    ACOscBrowser* getControlHandler(){return osc_browser;}
    ACOscFeedback* getFeedbackHandler(){return osc_feedback;}
#endif //defined (USE_OSC)

    virtual void changeMediaType(ACMediaType media_type){}
    virtual void updatePluginsSettings(){}
    virtual void resetPluginsSettings(){}
    virtual void resetMediaType(ACMediaType _media_type){}

protected:
    MediaCycle *media_cycle;
#if defined (SUPPORT_AUDIO)
    ACAudioEngine *audio_engine;
#endif //defined (SUPPORT_AUDIO)
    ACOsgCompositeViewQt* osg_view;
#if defined (USE_OSC)
    ACOscBrowser *osc_browser;
    ACOscFeedback *osc_feedback;
#endif //defined (USE_OSC)
};

#endif // ACAbstractWidgetQt_H
