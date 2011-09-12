/*
 *  Testenvoi.cpp
 *  HallPSpace
 *
 *  @author Julien Leroy
 *  @date 29/11/10
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

#include "Testenvoi.h"

#include <iostream>
#include <string>

#include "OscOutboundPacketStream.h"

#include "UdpSocket.h"
#include "IpEndpointName.h"

//using namespace std;

#define IP_MTU_SIZE 1536
/*
namespace osc{
    
	void RunSendTests( const IpEndpointName& host )
	{
		char buffer[IP_MTU_SIZE];
		osc::OutboundPacketStream p( buffer, IP_MTU_SIZE );
		UdpTransmitSocket socket( host );
		
		
		p.Clear();
		p << osc::BeginMessage( "/test2" )
		<< true << 23 << (float)3.1415 << "hello" << osc::EndMessage;
		socket.Send( p.Data(), p.Size() );
		
	}
	
} // namespace osc*/

#ifndef NO_OSC_TEST_MAIN

int oscenvoie(char* typemess,int userid,int dx,int dy, int dz)
{
	
    char *hostName = "localhost";
    int port = 12000;
	
	
	IpEndpointName host( hostName, port );
	
	char hostIpAddress[ IpEndpointName::ADDRESS_STRING_LENGTH ];
	host.AddressAsString( hostIpAddress );
	
    //std::cout << "sending test messages to " << hostName 
	//<< " (" << hostIpAddress << ") on port " << port << "...\n";
	
    char buffer[IP_MTU_SIZE];
	osc::OutboundPacketStream p( buffer, IP_MTU_SIZE );
	UdpTransmitSocket socket( host );
	
	
	p.Clear();
	p << osc::BeginMessage( "/Send" ) <<typemess<<userid << dx << dy << dz << osc::EndMessage;
	socket.Send( p.Data(), p.Size() );
	
}
int oscenvoie(char* typemess,int dist)
{
	
    char *hostName = "localhost";
    int port = 12000;
	
	
	IpEndpointName host( hostName, port );
	
	char hostIpAddress[ IpEndpointName::ADDRESS_STRING_LENGTH ];
	host.AddressAsString( hostIpAddress );
	
    //std::cout << "sending test messages to " << hostName 
	//<< " (" << hostIpAddress << ") on port " << port << "...\n";
	
    char buffer[IP_MTU_SIZE];
	osc::OutboundPacketStream p( buffer, IP_MTU_SIZE );
	UdpTransmitSocket socket( host );
	
	
	p.Clear();
	p << osc::BeginMessage( "/Send" ) <<typemess <<dist<< osc::EndMessage;
	socket.Send( p.Data(), p.Size() );
	
}

#endif /* NO_OSC_TEST_MAIN */
