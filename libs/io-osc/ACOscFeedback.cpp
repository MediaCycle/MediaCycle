/*
 *  ACOscFeedback.cpp
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

#include "ACOscFeedback.h"

void ACOscFeedback::create(const char *hostname, int port)
{
    /* an address to send messages to. sometimes it is better to let the server
     * pick a port number for you by passing NULL as the last argument */
	char portchar[6];
  	sprintf(portchar,"%d",port);
   	sendto = lo_address_new(hostname, portchar);
}

void ACOscFeedback::release()
{
	if (sendto){
		lo_address_free(sendto);
		sendto = 0;
	}
}

void ACOscFeedback::messageBegin(const char *_tag)
{
	if(message){
		lo_message_free(message);
	}
	message = lo_message_new();
	tag = _tag;
}

void ACOscFeedback::messageEnd()
{	
}

void ACOscFeedback::messageSend()
{	
	lo_send_message (sendto, tag, message);
}

void ACOscFeedback::messageAppendFloat(float val)
{
	lo_message_add_float (message,val);
}

void ACOscFeedback::messageAppendInt(int val)
{
	lo_message_add_int32 (message,val);
}

void ACOscFeedback::messageAppendString(const char *val)
{
 	lo_message_add_string (message, val);
}