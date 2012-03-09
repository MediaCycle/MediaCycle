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

#include "ACEventListener.h"

ACEventListener::ACEventListener(){
	t=new boost::thread(boost::bind(&ACEventListener::loop,this));
};

ACEventListener::ACEventListener(MediaCycle* core){
	
	t=new boost::thread(boost::bind(&ACEventListener::loop,this));
	if (core!= NULL)
		core->addListener(this);
	service.post(boost::bind(&ACEventListener::testService, this));
	service.post(boost::bind(&ACEventListener::mediaImported, this,3,5));
	
	
};
ACEventListener::~ACEventListener(){
	service.stop();
	delete t;
	
};
void ACEventListener::testService(){
	
	cout << "service ran perfectly";
};

void ACEventListener::loop(){
	boost::asio::io_service::work work (service);
	service.run(); // processes the tasks
	cout << "event thread loop finished"<<endl;
};

bool ACEventListener::stopped(){
	return service.stopped(); // processes the tasks
};


void ACEventListener::s_mediaImported(int n,int nTot){
	cout<<service.stopped()<<endl;
	service.post(boost::bind(&ACEventListener::mediaImported, this,n,nTot));
};
void ACEventListener::s_libraryCleaned(){
	service.post(boost::bind(&ACEventListener::libraryCleaned, this));
};
void ACEventListener::s_activeFeatChanged(){
	service.post(boost::bind(&ACEventListener::activeFeatChanged, this));
};
void ACEventListener::s_browserModeChanged(ACBrowserMode mode){
	service.post(boost::bind(&ACEventListener::browserModeChanged, this,mode));
};
void ACEventListener::s_updateDisplayNeeded(){
	service.post(boost::bind(&ACEventListener::updateDisplayNeeded, this));
};