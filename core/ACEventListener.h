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
class ACEventListener;

#include "MediaCycle.h"

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
	virtual void mediaImported(int n,int nTot){
		std::cout<<"ACEventListener::mediaImported"<<std::cout;
	};
	virtual void libraryCleaned(){std::cout<<"ACEventListener::libraryCleaned"<<std::cout;};
	virtual void activeFeatChanged(){};
	virtual void browserModeChanged(ACBrowserMode mode){};
	virtual void updateDisplayNeeded(){};

	void s_mediaImported(int n,int nTot);
	void s_libraryCleaned();
	void s_activeFeatChanged();
	void s_browserModeChanged(ACBrowserMode mode);
	void s_updateDisplayNeeded();
	
};

#endif //ACEventListener_H