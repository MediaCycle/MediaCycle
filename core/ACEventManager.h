/*
 *  ACEventManager.h
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

#ifndef ACEventManager_H
#define ACEventManager_H
class ACEventManager;
#include "ACEventListener.h"

#ifndef Q_MOC_RUN
#include <boost/numeric/ublas/io.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#endif

#include <iostream>

class ACEventManager{

#ifndef Q_MOC_RUN
protected:

    std::vector<ACEventListener *> listeners;
    
    std::vector<boost::signals2::connection> connect_mediaImported;
    std::vector<boost::signals2::connection> connect_mediasImported;
    std::vector<boost::signals2::connection> connect_pluginProgress;
    std::vector<boost::signals2::connection> connect_libraryCleaned;
    std::vector<boost::signals2::connection> connect_activeFeatChanged;
    std::vector<boost::signals2::connection> connect_browserModeChanged;
    std::vector<boost::signals2::connection> connect_updateDisplayNeeded;
    std::vector<boost::signals2::connection> connect_pluginLoaded;

#endif

public:
    ACEventManager();
    ~ACEventManager();

#ifndef Q_MOC_RUN
    boost::signals2::signal<void (int n,int nTot,int mId)> sig_mediaImported;
    boost::signals2::signal<void (int n,int nTot,std::vector<int> mIds)> sig_mediasImported;
    boost::signals2::signal<void (std::string plugin_name,int n,int nTot,int mId)> sig_pluginProgress;
    boost::signals2::signal<void ()> sig_libraryCleaned;
    boost::signals2::signal<void ()> sig_activeFeatChanged;
    boost::signals2::signal<void (ACBrowserMode mode)> sig_browserModeChanged;
    boost::signals2::signal<void ()> sig_updateDisplayNeeded;
    boost::signals2::signal<void (std::string plugin_name)> sig_pluginLoaded;
#endif
    void clean(void);
    void addListener(ACEventListener*);//we add this new output in output and we connect all the signals to these slot.

};
#endif//ACEventManager_H



