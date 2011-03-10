/*
 *  ACOscBrowser.cpp
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

#include "ACOscBrowser.h"

#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

#include <OscReceivedElements.h>
#include <OscPacketListener.h>
#include "../../3rdparty/oscpack/UdpSocket.h" // #include <UdpSocket.h> breaks with stk!

using namespace std;

struct OpaqReceiver;
class ACPacketListener;

struct OpaqReceiver
{
	//char				oscBuffer[IP_MTU_SIZE];
	ACPacketListener 		*oscListener;
	UdpListeningReceiveSocket	*oscSocket;
	
	bool 				started;
	//pthread_mutex_t 		mutex;
	pthread_t			pthread;
	pthread_attr_t  		pthread_custom_attr;
	
	void				*userData;
	ACOscBrowserCallback		callback;
	
	osc::ReceivedMessageArgumentStream *argStream; // valid only within the callback
	
	OpaqReceiver() : oscListener(0), oscSocket(0), started(false), userData(0), callback(0)
	{
		//mutex = PTHREAD_MUTEX_INITIALIZER;
	}
};


class ACPacketListener : public osc::OscPacketListener {
private:
	OpaqReceiver *mReceiver;
	
protected:
	virtual void ProcessMessage( const osc::ReceivedMessage& message, const IpEndpointName& remoteEndpoint )
	{
		try{
			assert(mReceiver);
			
			if(mReceiver->callback)
			{
				osc::ReceivedMessageArgumentStream args = message.ArgumentStream();
				
				mReceiver->argStream = &args;
				mReceiver->callback(mReceiver, message.AddressPattern(), mReceiver->userData);
				mReceiver->argStream = 0;
			}
		}
		catch( osc::Exception& e ){
			// any parsing errors such as unexpected argument types, or missing arguments get thrown as exceptions.
			//std::cout << "error while parsing message: "
			//	<< message.AddressPattern() << ": " << e.what() << "\n";
		}
	}
public:
	ACPacketListener(OpaqReceiver *aReceiver) : osc::OscPacketListener(), mReceiver(aReceiver) {}
};


void* osc_thread(void *arg)
{
	OpaqReceiver *receiver = (OpaqReceiver *)arg;
	printf("Running...\n");
	
	receiver->oscSocket->Run();
	//	printf("Stopped\n");
	
	return 0;
}

ACOscBrowserRef ACOscBrowser::create(const char *hostname, int port)
{
	OpaqReceiver *receiver;
	
	receiver = new OpaqReceiver;
	
	receiver->oscListener = new ACPacketListener(receiver);
	
	if(hostname == 0)
		receiver->oscSocket = new UdpListeningReceiveSocket(IpEndpointName(IpEndpointName::ANY_ADDRESS, port ), receiver->oscListener);
	else
		receiver->oscSocket = new UdpListeningReceiveSocket(IpEndpointName(hostname, port ), receiver->oscListener);
	
	return receiver;
}
void ACOscBrowser::release(ACOscBrowserRef aReceiver)
{
	OpaqReceiver *receiver = (OpaqReceiver *)aReceiver;
	
	delete receiver->oscListener;
	delete receiver->oscSocket;
	delete receiver;
}

void ACOscBrowser::start(ACOscBrowserRef aReceiver)
{
	OpaqReceiver *receiver = (OpaqReceiver *)aReceiver;
	
	if(receiver->started) return;
	
	pthread_attr_init(&receiver->pthread_custom_attr);
	pthread_create(&receiver->pthread, &receiver->pthread_custom_attr, &osc_thread, aReceiver);
	
	return;
}

void ACOscBrowser::stop(ACOscBrowserRef aReceiver)
{
	OpaqReceiver *receiver = (OpaqReceiver *)aReceiver;
	
	if(!receiver->started) return;
	
	receiver->oscSocket->AsynchronousBreak();
	pthread_join(receiver->pthread, 0);
}


void ACOscBrowser::setUserData(ACOscBrowserRef aReceiver, void *userData)
{
	OpaqReceiver *receiver = (OpaqReceiver *)aReceiver;
	
	receiver->userData = userData;
}
void ACOscBrowser::setCallback(ACOscBrowserRef aReceiver, ACOscBrowserCallback callback)
{
	OpaqReceiver *receiver = (OpaqReceiver *)aReceiver;
	
	receiver->callback = callback;
}

// these can be called only from the callback
void ACOscBrowser::readFloat(ACOscBrowserRef aReceiver, float *aVal)
{
	OpaqReceiver *receiver = (OpaqReceiver *)aReceiver;
	
	assert(receiver->argStream);
	assert(aVal);
	
	*receiver->argStream >> *aVal;
}
void ACOscBrowser::readInt(ACOscBrowserRef aReceiver, int *aVal)
{
	OpaqReceiver *receiver = (OpaqReceiver *)aReceiver;
	
	assert(receiver->argStream);
	assert(aVal);
	
	osc::int32 val;
	
	*receiver->argStream >> val;
	
	*aVal = val;
}
void ACOscBrowser::readString(ACOscBrowserRef aReceiver, char *aVal, int maxlen)
{	
	OpaqReceiver *receiver = (OpaqReceiver *)aReceiver;
	
	assert(receiver->argStream);
	assert(aVal);
	
	const char *string = 0;
	
	*receiver->argStream >> string;
	
	strncpy(aVal, string, maxlen);
}


