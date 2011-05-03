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

using namespace std;

struct OpaqReceiver
{
	//char				oscBuffer[IP_MTU_SIZE];
	ACPacketListener 		*oscListener;
	UdpListeningReceiveSocket	*oscSocket;
	
	bool 				started;
	//pthread_mutex_t 		mutex;
	pthread_t			pthread;
	pthread_attr_t  		pthread_custom_attr;
	pthread_t			pthreadudp;
	pthread_attr_t  		pthreadudp_custom_attr;
	
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
			std::cout << "ProcessMessage" << std::endl;
			try{
				assert(mReceiver);
				
				if(mReceiver->callback)
				{
					osc::ReceivedMessageArgumentStream args = message.ArgumentStream();
					mReceiver->argStream = &args;
					mReceiver->callback(message.AddressPattern(), mReceiver->userData);
					mReceiver->argStream = 0;
				}
			}
			catch( osc::Exception& e ){
				// any parsing errors such as unexpected argument types, or missing arguments get thrown as exceptions.
				std::cout << "error while parsing message: " << message.AddressPattern() << ": " << e.what() << "\n";
			}
		}
	public:
		ACPacketListener(OpaqReceiver *aReceiver) : osc::OscPacketListener(), mReceiver(aReceiver) {}
};

void* udp_thread(void *arg)
{
	OpaqReceiver *receiver = (OpaqReceiver *)arg;
	bool break_ = false;
	while(!break_)
		receiver->oscSocket->Run();
}

void* osc_thread(void *arg)
{
	printf("osc thread...\n");
	OpaqReceiver *receiver = (OpaqReceiver *)arg;
	printf("Running...\n");
	
	pthread_attr_init(&receiver->pthreadudp_custom_attr);
	pthread_create(&receiver->pthreadudp, &receiver->pthreadudp_custom_attr, &udp_thread, receiver);
	
	//receiver->oscSocket->Run();
	
	
	while(1){
		std::cout << "receiver" << &receiver << std::endl;
		usleep(1000000);
	}
	printf("Stopped\n");
	
	return 0;
}



void ACOscBrowser::create(const char *hostname, int port)
{
	receiver = new OpaqReceiver;
	receiver->oscListener = new ACPacketListener(receiver);
	
	if(hostname == 0){
		try{
			receiver->oscSocket = new UdpListeningReceiveSocket(IpEndpointName(IpEndpointName::ANY_ADDRESS, port ), receiver->oscListener);
		}
		catch (const exception& e) {
			std::cout << "can't connect" << std::endl;
		}
	}
	else {
		try{
			receiver->oscSocket = new UdpListeningReceiveSocket(IpEndpointName(hostname, port ), receiver->oscListener);
		}
		catch (const exception& e) {
			std::cout << "can't connect" << std::endl;
		}
	}
}

void ACOscBrowser::release()
{
	if(receiver) {
		if (receiver->oscListener)
			delete receiver->oscListener;
		if (receiver->oscSocket)	
		delete receiver->oscSocket;
		delete receiver;
		receiver = 0;
	}
}

void ACOscBrowser::start()
{
	if(receiver->started) return;
	
	//pthread_mutex_init( &receiver->mutex, NULL );
	
	pthread_attr_init(&receiver->pthread_custom_attr);
	pthread_create(&receiver->pthread, &receiver->pthread_custom_attr, &osc_thread, receiver);
	
	return;
}

void ACOscBrowser::stop()
{
	if(!receiver) return;

	//if(!receiver->started) return;
	
	pthread_cancel(receiver->pthreadudp);
	
	receiver->oscSocket->AsynchronousBreak();
	
	pthread_cancel(receiver->pthread);//////pthread_join(receiver->pthread, 0);
	
	
	
	//pthread_mutex_destroy( &receiver->mutex );
}


void ACOscBrowser::setUserData(void *userData)
{
	receiver->userData = userData;
}

void ACOscBrowser::setCallback(ACOscBrowserCallback callback)
{
	receiver->callback = callback;
}

// these can be called only from the callback
void ACOscBrowser::readFloat(float *aVal)
{
	assert(receiver->argStream);
	assert(aVal);
	
	*receiver->argStream >> *aVal;
}
void ACOscBrowser::readInt(int *aVal)
{
	assert(receiver->argStream);
	assert(aVal);
	
	osc::int32 val;
	
	*receiver->argStream >> val;
	
	*aVal = val;
}
void ACOscBrowser::readString(char *aVal, int maxlen)
{	
	assert(receiver->argStream);
	assert(aVal);
	
	const char *string = 0;
	
	*receiver->argStream >> string;
	
	strncpy(aVal, string, maxlen);
}