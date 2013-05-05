/*
 *  ACEventManager.cpp
 *  MediaCycle
 *
 *  @author Thierry Ravet
 *  @date 24/02/12
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

#include "ACEventManager.h"
ACEventManager::ACEventManager(){

}

ACEventManager::~ACEventManager(){
    this->clean();
}

void ACEventManager::addListener(ACEventListener* eventListener){
    listeners.push_back(eventListener);
    connect_mediaImported.push_back(sig_mediaImported.connect(boost::bind(&ACEventListener::s_mediaImported, eventListener,_1, _2,_3))) ;
    connect_mediasImported.push_back(sig_mediasImported.connect(boost::bind(&ACEventListener::s_mediasImported, eventListener,_1, _2,_3))) ;
    connect_pluginProgress.push_back(sig_pluginProgress.connect(boost::bind(&ACEventListener::s_pluginProgress, eventListener, _1, _2, _3, _4)));
    connect_libraryCleaned.push_back(sig_libraryCleaned.connect(boost::bind(&ACEventListener::s_libraryCleaned, eventListener))) ;
    connect_activeFeatChanged.push_back(sig_activeFeatChanged.connect(boost::bind(&ACEventListener::s_activeFeatChanged, eventListener))) ;
    connect_browserModeChanged.push_back(sig_browserModeChanged.connect(boost::bind(&ACEventListener::s_browserModeChanged, eventListener, _1))) ;
    connect_updateDisplayNeeded.push_back(sig_updateDisplayNeeded.connect(boost::bind(&ACEventListener::s_updateDisplayNeeded, eventListener)));
    connect_pluginLoaded.push_back(sig_pluginLoaded.connect(boost::bind(&ACEventListener::s_pluginLoaded, eventListener, _1)));
    connect_mediaActionPerformed.push_back(sig_mediaActionPerformed.connect(boost::bind(&ACEventListener::s_mediaActionPerformed, eventListener, _1, _2, _3)));
}

void ACEventManager::clean(void){
    for (unsigned int i=0;i<connect_mediaImported.size();i++)
        connect_mediaImported[i].disconnect();
    connect_mediaImported.clear();
    for (unsigned int i=0;i<connect_pluginProgress.size();i++)
        connect_pluginProgress[i].disconnect();
    connect_pluginProgress.clear();
    for (unsigned int i=0;i<connect_libraryCleaned.size();i++)
        connect_libraryCleaned[i].disconnect();
    connect_libraryCleaned.clear();
    for (unsigned int i=0;i<connect_activeFeatChanged.size();i++)
        connect_activeFeatChanged[i].disconnect();
    connect_activeFeatChanged.clear();
    for (unsigned int i=0;i<connect_browserModeChanged.size();i++)
        connect_browserModeChanged[i].disconnect();
    connect_browserModeChanged.clear();
    for (unsigned int i=0;i<connect_updateDisplayNeeded.size();i++)
        connect_updateDisplayNeeded[i].disconnect();
    connect_updateDisplayNeeded.clear();
    for (unsigned int i=0;i<connect_pluginLoaded.size();i++)
        connect_pluginLoaded[i].disconnect();
    connect_pluginLoaded.clear();
    for (unsigned int i=0;i<connect_mediaActionPerformed.size();i++)
        connect_mediaActionPerformed[i].disconnect();
    connect_mediaActionPerformed.clear();
    for (unsigned int i=0;i<listeners.size();i++)
        listeners[i]=0;
    listeners.clear();
}
