/*
 *  ACNetworkSocket.cpp
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

#include "ACNetworkSocket.h"

PORT open_server(int iport, int count)
{
#ifdef WIN32
	WSADATA wsda;
#endif
	SOCKET s_listen;
	//SOCKET s_client;
	SOCKADDR_IN addr;
	//SOCKADDR_IN remote_addr;
	//int i_addr_len;
	
	if (iport < FIRST_PORT_ID || iport > LAST_PORT_ID)
		//throw_error(SocketServerError, "open_server: invalid port number! (%s)\n", iport);
		return -1;
	
#ifdef WIN32
	WSAStartup(MAKEWORD(1,1), &wsda);
#endif
	
	if ((s_listen = (PORT)socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
		//throw_error(SocketServerError, "open_server: call to 'socket' failed.\n");
		return -1;
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons((unsigned short)iport);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(addr.sin_zero), '\0', 8);
	
	if (bind(s_listen, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
		//throw_error(SocketServerError, "open_server: call to 'bind' failed.\n");
		return -1;
	
	if (listen(s_listen, count) == SOCKET_ERROR)
		//throw_error(SocketServerError, "open_server: call to 'listen' failed.\n");
		return -1;
	
	return ((PORT)s_listen);
	
	/*fprintf(stdout, "Waiting for client to connect ...\n");
	 
	 i_addr_len = sizeof(remote_addr);
	 if ((s_client = accept(s_listen, (struct sockaddr*)&remote_addr, &i_addr_len)) == SOCKET_ERROR)
	 throw_error(SocketServerError, "open_server: call to 'accept' failed.\n");
	 
	 fprintf(stdout, "%s connected\n", inet_ntoa(remote_addr.sin_addr));
	 
	 return s_client;*/
}

void close_server(PORT p)
{
	shutdown((SOCKET)p, SHUT_RDWR);
#ifdef WIN32
	WSACleanup();
#endif
}

SOCKET wait_connection(PORT p)
{
	SOCKET s;
	SOCKADDR_IN remote_addr;
	int i_addr_len;
	
	fprintf(stdout, "Waiting for client to connect ...\n");
	
	i_addr_len = sizeof(remote_addr);
	if ((s = accept((SOCKET)p, (struct sockaddr*)&remote_addr, (socklen_t*)&i_addr_len)) == SOCKET_ERROR) {
		//throw_error(SocketServerError, "wait_connection: call to 'accept' failed.\n");
		fprintf(stdout, "connection problem\n");
		return -1;
	}
	
	fprintf(stdout, "%s connected\n", inet_ntoa(remote_addr.sin_addr));
	
	return s;
}

void close_connection(SOCKET s)
{
	shutdown(s, SHUT_RDWR);
}

int server_receive(char *buf, int size, SOCKET s, unsigned int time_out)
{
	int ret, read;
	fd_set rfds;
	struct timeval to = {0,0};
	struct timeval *pto;
	
	to.tv_sec = time_out;
	pto = (time_out == 0) ? NULL : &to;
	
	FD_ZERO(&rfds);
	FD_SET(s, &rfds);
	
	ret = 0;
	read = 0;
	do {
		if (select(s+1, &rfds, NULL, NULL, pto) == SOCKET_ERROR)
			//throw_error(SocketServerError, "server_send: call to 'select' failed.\n");
			return -1;
		
		if (FD_ISSET(s, &rfds)) {
			ret = recv(s, buf + read, size - read, 0);
			if (ret == SOCKET_ERROR)
				//throw_error(SocketServerError, "server_receive: call to 'recv' failed.\n");
				return -1;
			read += ret;
		} else {
			//throw_error(SocketServerError, "server_receive: time out (%d sec). No data available !\n", time_out);
			if (read==0)
				return -1;
			else
				return read;
		}
	} while (read < size);
	
	return read;
}

int server_send(char *buf, int size, SOCKET s)
{
	int ret;
	
	ret = send(s, buf, size, 0);
	if (ret == SOCKET_ERROR)
		//throw_error(SocketServerError, "server_send: call to 'send' failed.\n");
		return -1;
	
	return ret;
}

ACNetworkSocketServer::ACNetworkSocketServer(int port, int max_connections, ACNetworkSocketServerCallback callback, void *user_data) {
	server_port = port;
	server_max_connections = max_connections;
	server_callback = callback;
	server_callback_user_data = user_data;
	server_buffer = new char[BUFSIZE];
	server_buffer_send = new char[BUFSIZE];
	server_factory = open_server(server_port, server_max_connections);
}

ACNetworkSocketServer::~ACNetworkSocketServer() {
	close_server(server_port);
	delete server_buffer;
}

void *threadNetworkSocketServerFunction(void *_network_engine_arg) {
	((ACNetworkSocketServer*)_network_engine_arg)->thread();
}

void ACNetworkSocketServer::start() {
	pthread_attr_init(&server_engine_attr);
	server_engine_arg = (void*)this;
	pthread_create(&server_engine, &server_engine_attr, &threadNetworkSocketServerFunction, server_engine_arg);
	pthread_attr_destroy(&server_engine_attr);
}

void ACNetworkSocketServer::stop() {
	pthread_exit(&server_engine);
}

// SD TODO - need a loop around server_receive to collect larger amounts of packets
void ACNetworkSocketServer::thread() {
	int read;
	char *bigbuffer;
	int bigbufferstep = 65536;
	int bigbufferl = bigbufferstep;
	//vector<char> bigbuffer;
	//bigbuffer.resize(bigbufferl);

	while(1) {
		server_socket = wait_connection(server_factory);
		memset(server_buffer, 0, BUFSIZE);
		bigbuffer = new char[bigbufferl];
		read = 0;
		while ( (ret = server_receive(server_buffer, BUFSIZE, server_socket, 1)) >= 0 ) {
			if (read+ret>bigbufferl) {
				bigbufferl += bigbufferstep;
				bigbuffer = (char*)realloc(bigbuffer, bigbufferl);
			}
			memcpy(bigbuffer+read, server_buffer, ret);
			read += ret;
		}
		printf ("Ret %d, Received %s\n", ret, server_buffer);
		if (read>0) {
			server_callback(bigbuffer, read, &server_buffer_send, &ret_send, server_callback_user_data);
			if (ret_send) {
				ret = server_send(server_buffer_send, ret_send, server_socket);
				//delete[] server_buffer_send;
			}
		}
		delete[] bigbuffer;
		close_connection(server_socket);
	}
}

/*
SOCKET open_client(int iport, char *ip_address)
{
	int i;
#ifdef WIN32
	WSADATA wsda;
#endif
	SOCKET s;
	SOCKADDR_IN addr;
	struct hostent *host;
	
	if (iport < 1024 || iport > 65563) {
		//  throw_error(SocketClientError, "open_client: invalid port number! (%s)\n", iport);
		error_message();
	}
	
#ifdef WIN32
	if ( WSAStartup(MAKEWORD(2,2), &wsda) != NO_ERROR ) {
		error_message();
	}
#endif
	
	if ( (s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET ) {
		// throw_error(SocketClientError, "open_client: error\n\tCall to 'socket' failed.\n");
		error_message();
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons((unsigned short)iport);
	addr.sin_addr.s_addr = inet_addr(ip_address);
	
	if (addr.sin_addr.s_addr == INADDR_NONE)
    {
		host = NULL;
		if ((host = gethostbyname(ip_address)) == NULL) {
			// throw_error(SocketClientError, "open_client: unknown host: %s\n", ip_address);
			error_message();
		}
		memcpy(&addr.sin_addr, host -> h_addr_list[0], host -> h_length);
    }
	
	fprintf(stdout, "Trying to connect to %s", inet_ntoa(addr.sin_addr));
	for (i = 0; i < 25; i++) { // time out 25 x 200ms = 5s 
		fprintf(stdout, ".");
		sleep(0.2);
		if (connect(s, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
			continue;
		else {
			fprintf(stdout, "\rConnected to %s                    \n", inet_ntoa(addr.sin_addr));
			return s;
		}
	}
	
	// throw_error(SocketClientError, "open_client: time out ! Unable to connect to %s.\n", inet_ntoa(addr.sin_addr));
	error_message();
	
	return SOCKET_ERROR;
}

void close_client(SOCKET s)
{
	shutdown(s, SHUT_RDWR);
#ifdef WIN32
	WSACleanup();
#endif
}

int client_send(char *buf, int size, SOCKET s)
{
	int ret;
	ret = send(s, buf, size, 0);
	if (ret == SOCKET_ERROR) {
		// throw_error(SocketClientError, "client_send: call to 'send' failed.\n");
		error_message();
	}
	return ret;
}

int client_receive(char *buf, int size, SOCKET s, unsigned int time_out)
{
	int ret, read;
	fd_set rfds;
	struct timeval to = {0,0};
	struct timeval *pto;
	
	to.tv_sec = time_out;
	pto = (time_out == 0) ? NULL : &to;
	
	FD_ZERO(&rfds);
	FD_SET(s, &rfds);
	
	ret = 0;
	read = 0;
	do {
		if (select(s+1, &rfds, NULL, NULL, pto) == SOCKET_ERROR) {
			//	throw_error(SocketClientError, "server_send: call to 'select' failed.\n");
			error_message();
		}
		
		if (FD_ISSET(s, &rfds)) {
			ret = recv(s, buf + read, size - read, 0);
			if (ret == SOCKET_ERROR) {
				//	throw_error(SocketClientError, "server_receive: call to 'recv' failed.\n");
				error_message();
			}
			read += ret;
		} else {
			// throw_error(SocketClientError, "server_receive: time out (%d sec). No data available !\n", time_out);
			error_message();
		}
	} while (read < size);
	
	return read;
}
*/

/*
ACNetworkSocketClient::ACNetworkSocketClient(char *ip_adress, int port) {
	server_ip = ip_adress;
	server_port = port;
	server_max_connections = max_connections;
	server_callback = callback;
	server_callback_user_data = user_data;
	server_buffer = new char[BUFSIZE];
	server_buffer_send = new char[BUFSIZE];
	server_factory = open_server(server_port, server_max_connections);
}

ACNetworkSocketServer::~ACNetworkSocketServer() {
	close_server(server_port);
	delete server_buffer;
}

void *threadNetworkSocketServerFunction(void *_network_engine_arg) {
	((ACNetworkSocketServer*)_network_engine_arg)->thread();
}

void ACNetworkSocketServer::start() {
	pthread_attr_init(&server_engine_attr);
	server_engine_arg = (void*)this;
	pthread_create(&server_engine, &server_engine_attr, &threadNetworkSocketServerFunction, server_engine_arg);
	pthread_attr_destroy(&server_engine_attr);
}

void ACNetworkSocketServer::stop() {
	pthread_exit(&server_engine);
}

// SD TODO - need a loop around server_receive to collect larger amounts of packets
void ACNetworkSocketServer::thread() {
	while(1) {
		server_socket = wait_connection(server_factory);
		memset(server_buffer, 0, BUFSIZE);
		ret = server_receive(server_buffer, BUFSIZE, server_socket, 10000);
		printf ("Ret %d, Received %s\n", ret, server_buffer);
		if (ret>0) {
			server_callback(server_buffer, ret, &server_buffer_send, &ret_send, server_callback_user_data);
			if (ret_send) {
				ret = server_send(server_buffer_send, ret_send, server_socket);
			}
		}
		close_connection(server_socket);
	}
}
*/
