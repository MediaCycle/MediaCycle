/**
 * @brief Abstract view to wrap as Qt widget with any rendering library
 * @author Christian Frisson
 * @date 21/04/2014
 * @copyright (c) 2014 – UMONS - Numediart
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

#ifndef ACAbstractViewQt_H
#define ACAbstractViewQt_H

#include <MediaCycle.h>
#include <string>

#include <QtGui>

#include "ACInputActionQt.h"

#include "ACAbstractBrowserRenderer.h"
#include "ACAbstractTimelineRenderer.h"

class ACAbstractViewQt /*: public QWidget*/ {
    //Q_OBJECT
public:
    ACAbstractViewQt()
        :/*QWidget(),*/media_cycle(0)
    {}
    virtual ~ACAbstractViewQt(){}

    //virtual void setParent(QWidget*){}
    virtual void addInputAction(ACInputActionQt* _action){}
    QList<ACInputActionQt*> getInputActions(){return inputActions;}

    virtual bool isLibraryLoaded()=0;
    virtual void setLibraryLoaded(bool load_status)=0;

    virtual void prepareBrowser()=0;
    virtual void prepareTimeline()=0;

    virtual void setMediaCycle(MediaCycle* _media_cycle){ media_cycle = _media_cycle;}
    MediaCycle* getMediaCycle() {return media_cycle;}

    virtual ACAbstractBrowserRenderer* getBrowser()=0;
    virtual ACAbstractTimelineRenderer* getTimeline()=0;

public:
    virtual void clean(){}
    virtual void changeSetting(ACSettingType _setting){}

protected:
    MediaCycle *media_cycle;
    QList<ACInputActionQt*> inputActions;

};

#endif // ACAbstractViewQt_H
