/*
 *  ACOscFeedback.cpp
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

#include <stdlib.h>
#include <assert.h>

#include "ACOscFeedback.h"



//ACOscFeedbackRef
void ACOscFeedback::create(const char *hostname, int port)
{
	sender = new OpaqSender();

	IpEndpointName host(hostname , port);
	char hostIpAddress[ IpEndpointName::ADDRESS_STRING_LENGTH ];
	host.AddressAsString( hostIpAddress );

	sender->oscStream = new osc::OutboundPacketStream( sender->oscBuffer, IP_MTU_SIZE ); assert(sender->oscStream);
	sender->oscSocket = new UdpTransmitSocket(host); assert(sender->oscSocket);
	
	//return sender;
}

void ACOscFeedback::release()
{
	//sender = (OpaqSender*)aSender; assert(sender);
	if (sender) {
		if (sender->oscStream)
			delete sender->oscStream;
		if (sender->oscSocket)
			delete sender->oscSocket;
		delete sender;
	}
}

void ACOscFeedback::messageBegin(const char *tag)//ACOscFeedbackRef aSender,const char *tag)
{
	assert(tag);
	//sender = (OpaqSender*)aSender; assert(sender);
	
	sender->oscStream->Clear();
	*sender->oscStream << osc::BeginMessage( tag );
}
void ACOscFeedback::messageEnd()//ACOscFeedbackRef aSender)
{	
	//sender = (OpaqSender*)aSender; assert(sender);
	
	*sender->oscStream << osc::EndMessage;
}
void ACOscFeedback::messageSend()//ACOscFeedbackRef aSender)
{	
	//sender = (OpaqSender*)aSender; assert(sender);
	
	sender->oscSocket->Send( sender->oscStream->Data(), sender->oscStream->Size() );
}

void ACOscFeedback::messageAppendFloat(float aVal)//ACOscFeedbackRef aSender, float aVal)
{
	//sender = (OpaqSender*)aSender; assert(sender);
	
	*sender->oscStream << aVal;
}
void ACOscFeedback::messageAppendInt(int aVal)//ACOscFeedbackRef aSender, int aVal)
{
	//sender = (OpaqSender*)aSender; assert(sender);
	
	*sender->oscStream << aVal;
}
void ACOscFeedback::messageAppendString(const char *aVal)//ACOscFeedbackRef aSender, const char *aVal)
{
	//sender = (OpaqSender*)aSender; assert(sender);
	
	*sender->oscStream << aVal;
}