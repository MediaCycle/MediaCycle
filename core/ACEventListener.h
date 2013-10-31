/*
 *  ACEventListener.h
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

#ifndef ACEventListener_H
#define ACEventListener_H

//class ACEventListener;
class MediaCycle;

enum ACBrowserMode {
    AC_MODE_NONE=0,
    AC_MODE_CLUSTERS=1,
    AC_MODE_NEIGHBORS=2
};

#ifndef Q_MOC_RUN
#include <boost/signals2.hpp>

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <iostream>

#endif//(Q_MOC_RUN)

class ACEventListener{

#ifndef Q_MOC_RUN
protected:
    boost::asio::io_service service;

    void loop();
    boost::thread *t;

#endif//(Q_MOC_RUN)

public:
    ACEventListener();
    ~ACEventListener();
    ACEventListener(MediaCycle* core);
    //Slot
    bool stopped();
    void testService();
    virtual void mediaImported(int n,int nTot,int mId){
        std::cout<<"ACEventListener::mediaImported"<<std::endl;
    }
    virtual void mediasImported(int n,int nTot,std::vector<int> mIds){
        std::cout<<"ACEventListener::mediasImported"<<std::endl;
    }
    virtual void pluginProgress(std::string plugin_name,int n,int nTot,int mId){
        std::cout << "ACEventListener::pluginProgress: plugin '" << plugin_name << "' "<< n << "/" << nTot << " for media of id: " << mId << std::endl;
    }
    virtual void libraryCleaned(){std::cout<<"ACEventListener::libraryCleaned"<< std::endl;}
    virtual void activeFeatChanged(){}
    virtual void browserModeChanged(ACBrowserMode mode){}
    virtual void updateDisplayNeeded(){}
    virtual void pluginLoaded(std::string plugin_name){}
    virtual void mediaActionPerformed(std::string action, long int mediaId, std::vector<boost::any> arguments=std::vector<boost::any>()){}
    
    void s_mediaImported(int n,int nTot,int mId);
    void s_mediasImported(int n,int nTot,std::vector<int> mIds);
    void s_pluginProgress(std::string plugin_name,int n,int nTot,int mId);
    void s_libraryCleaned();
    void s_activeFeatChanged();
    void s_browserModeChanged(ACBrowserMode mode);
    void s_updateDisplayNeeded();
    void s_pluginLoaded(std::string plugin_name);
    void s_mediaActionPerformed(std::string action, long int mediaId, std::vector<boost::any> arguments=std::vector<boost::any>());

};

#endif //ACEventListener_H
