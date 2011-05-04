/*
 *  ACOscBrowser.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 03/04/11
 *
 *  @copyright (c) 2011 – UMONS - Numediart
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

#include "ACOscBrowser.h"

using namespace std;

void error(int num, const char *msg, const char *path)
{
    std::cout << "liblo server error " << num << " in path " << path << ": " << msg << std::endl;
    fflush(stdout);
}

void ACOscBrowser::create(const char *hostname, int port)
{
	char portchar[6];
  	sprintf(portchar,"%d",port);
	server_thread = lo_server_thread_new(portchar, error);
}

void ACOscBrowser::release()
{
	if (server_thread){
		lo_server_thread_free(server_thread);
		server_thread = 0;
	}
}

void ACOscBrowser::start()
{
	lo_server_thread_start(server_thread);
}

void ACOscBrowser::stop()
{
	lo_server_thread_stop(server_thread);
}

void ACOscBrowser::setUserData(void *_user_data)
{
	user_data = _user_data;
}

void ACOscBrowser::setCallback(ACOscBrowserCallback* _callback)
{
	callback = _callback;
	lo_server_thread_add_method(server_thread, NULL, NULL, *_callback, user_data);
}

// these can be called only from the callback
void ACOscBrowser::readFloat(float *val)
{
}

void ACOscBrowser::readInt(int *val)
{
}

void ACOscBrowser::readString(char *val, int maxlen)
{	
}
