/*
 *  ACNetworkSocket.h
 *  MediaCycle
 *
 *  @author Stéphane Dupont
 *  @date 18/06/09
 *  @copyright (c) 2009 – UMONS - Numediart
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

#ifndef _ACNETWORKSOCKET_H_
#define _ACNETWORKSOCKET_H_

#include <pthread.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFSIZE 512

typedef int SOCKET;
#define SOCKET_ERROR (-1)
typedef struct sockaddr_in SOCKADDR_IN;
typedef int PORT;

#define FIRST_PORT_ID 1024
#define LAST_PORT_ID 49151

#ifdef  __cplusplus
extern "C" {
#endif

PORT open_server(int iport, int count);

void close_server(PORT p);

SOCKET wait_connection(PORT p);

void close_connection(SOCKET s);

int server_send(char *buf, int size, SOCKET s);

int server_receive(char *buf, int size, SOCKET s, unsigned int time_out);

typedef void (*ACNetworkSocketServerCallback)(const char *buffer, int l, char **buffer_send, int *l_send, void *user_data);

class ACNetworkSocketServer {
	
public:
	ACNetworkSocketServer(int port, int max_connections, ACNetworkSocketServerCallback callback, void *user_data);
	~ACNetworkSocketServer();
	
	void start();
	void stop();
	void thread();

private:
	int ret;
	int ret_send;
	
	ACNetworkSocketServerCallback server_callback;
	void *server_callback_user_data;
	
	int server_port;
	int server_max_connections;
	int server_factory;
	int server_socket;
	sockaddr_in server_addr;
	sockaddr_in client_addr;
	char *server_buffer;
	char *server_buffer_send;
	
	pthread_t	   server_engine;
	pthread_attr_t server_engine_attr;
	void* server_engine_arg;
};

SOCKET open_client(int iport, char *ip_address);

void close_client(SOCKET s);

int client_send(char *buf, int size, SOCKET s);

int client_receive(char *buf, int size, SOCKET s, unsigned int time_out);
	
class ACNetworkSocketClient {
		
public:
	ACNetworkSocketClient(char *ip_adress, int port);
	~ACNetworkSocketClient();
		
	void start();
	void stop();
	void send(char *buf, int size);
	void receive(char *buf, int size);
		
private:
	int ret;
	int ret_send;

	int server_port;
	int server_max_connections;
	int server_factory;
	int server_socket;
	sockaddr_in server_addr;
	sockaddr_in client_addr;
	char *server_buffer;
	char *server_buffer_send;
};
	
#ifdef  __cplusplus
}
#endif

#endif  // _ACNETWORKSOCKET_H_