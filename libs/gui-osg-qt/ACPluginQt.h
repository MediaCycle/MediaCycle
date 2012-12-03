/*
 *  ACPluginQt.h
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

#ifndef ACPluginQt_H
#define ACPluginQt_H

#include <MediaCycle.h>
#include <QtCore/QObject>
#include <QtGui>
#include "ACAbstractDockWidgetQt.h"
#include "ACInputActionQt.h"
#include <ACOsgBrowserRenderer.h>
#include <ACOsgTimelineRenderer.h>

/**
 * @brief The ACPluginQt class allows to load GUI functionalities from core ACPlugin derivatives
 *
 * To create such a plugin, inherit first from QObject, then ACPluginQt, then any ACPlugin derivative
 * The following need to be the first lines of the class declaration:
 * Q_OBJECT
 * Q_INTERFACES(ACPluginQt)
 */

class ACPluginQt {
public:
    ACPluginQt():browser_renderer(0),timeline_renderer(0){}
    virtual ~ACPluginQt(){}
    virtual ACAbstractDockWidgetQt* createDock(QWidget *parent){return 0;}
    virtual std::vector<ACInputActionQt*> providesInputActions(){return std::vector<ACInputActionQt*>();}
    virtual std::string dockName(){return "";}
    void setBrowserRenderer(ACOsgBrowserRenderer* _browser_renderer){browser_renderer = _browser_renderer;}
    void setTimelineRenderer(ACOsgTimelineRenderer* _timeline_renderer){timeline_renderer = _timeline_renderer;}
    ACOsgBrowserRenderer* getBrowserRenderer(){return browser_renderer;}
    ACOsgTimelineRenderer* getTimelineRenderer(){return timeline_renderer;}
protected:
    ACOsgBrowserRenderer *browser_renderer;
    ACOsgTimelineRenderer *timeline_renderer;
};

Q_DECLARE_INTERFACE(ACPluginQt, "ACPluginQt")

#endif // ACPluginQt_H
