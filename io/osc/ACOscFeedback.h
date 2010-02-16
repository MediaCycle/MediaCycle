/*
 *  ACOscFeedback.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 16/02/10
 *  Based on Raphael Sebbe's TiCore Osc implementation from 2007.
 *
 *  @copyright (c) 2010 – UMONS - Numediart
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

#ifndef _ACOSCFEEDBACK_H_
#define _ACOSCFEEDBACK_H_

#include <OscOutboundPacketStream.h>
#include "oscpack/UdpSocket.h" // #include <UdpSocket.h> breaks with stk!
#include <IpEndpointName.h>

typedef void *ACOscFeedbackRef;

#define IP_MTU_SIZE 1536

struct OpaqSender
{
	char				oscBuffer[IP_MTU_SIZE];
	osc::OutboundPacketStream 	*oscStream;
	UdpTransmitSocket		*oscSocket;
	
	OpaqSender() : oscStream(NULL), oscSocket(NULL) {}
};

class ACOscFeedback {
	public:
		ACOscFeedback(){};
		~ACOscFeedback(){};
	
		void create(const char *hostname, int port);
		void release();
		void messageBegin(const char *tag);
		void messageEnd();
		void messageSend();
		void messageAppendFloat(float aVal);
		void messageAppendInt(int aVal);
		void messageAppendString(const char *aval);
	private:
		OpaqSender *sender;
};

#endif /* _ACOSCFEEDBACK_H_ */
