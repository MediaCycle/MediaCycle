/*
 *  ACEventListener.cpp
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

#include "MediaCycle.h"
#include "ACEventListener.h"
#include "boost/version.hpp"

ACEventListener::ACEventListener(){
    t=new boost::thread(boost::bind(&ACEventListener::loop,this));
}

ACEventListener::ACEventListener(MediaCycle* core){

    t=new boost::thread(boost::bind(&ACEventListener::loop,this));
    if (core!= NULL)
        core->addListener(this);
    service.post(boost::bind(&ACEventListener::testService, this));
    service.post(boost::bind(&ACEventListener::mediaImported, this,3,5,-1));


}
ACEventListener::~ACEventListener(){
    service.stop();

#if BOOST_VERSION <104700
    usleep(100000);
#else 
    while (!service.stopped()){};
#endif
    delete t;

}
void ACEventListener::testService(){

    cout << "service ran perfectly";
}

void ACEventListener::loop(){
    boost::asio::io_service::work work (service);
    service.run(); // processes the tasks
    cout << "event thread loop finished"<<endl;
}

bool ACEventListener::stopped(){
#if BOOST_VERSION <104700
    return false;
#else
    return service.stopped(); // processes the tasks
#endif
}

void ACEventListener::s_mediaImported(int n,int nTot,int mId){
    service.post(boost::bind(&ACEventListener::mediaImported, this,n,nTot,mId));
}
void ACEventListener::s_mediasImported(int n,int nTot,std::vector<int> mIds){
    service.post(boost::bind(&ACEventListener::mediasImported, this,n,nTot,mIds));
}
void ACEventListener::s_pluginProgress(std::string plugin_name,int n,int nTot,int mId){
    service.post(boost::bind(&ACEventListener::pluginProgress, this,plugin_name,n,nTot,mId));
}
void ACEventListener::s_libraryCleaned(){
    service.post(boost::bind(&ACEventListener::libraryCleaned, this));
}
void ACEventListener::s_activeFeatChanged(){
    service.post(boost::bind(&ACEventListener::activeFeatChanged, this));
}
void ACEventListener::s_browserModeChanged(ACBrowserMode mode){
    service.post(boost::bind(&ACEventListener::browserModeChanged, this,mode));
}
void ACEventListener::s_updateDisplayNeeded(){
    service.post(boost::bind(&ACEventListener::updateDisplayNeeded, this));
}
void ACEventListener::s_pluginLoaded(std::string plugin_name){
    service.post(boost::bind(&ACEventListener::pluginLoaded, this,plugin_name));
}
void ACEventListener::s_mediaActionPerformed(std::string action, long int mediaId, std::vector<boost::any> arguments){
    service.post(boost::bind(&ACEventListener::mediaActionPerformed, this, action, mediaId, arguments));
}
