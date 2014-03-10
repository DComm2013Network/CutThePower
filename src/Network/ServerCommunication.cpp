/*------------------------------------------------------------------------------------------
 * SOURCE FILE: ServerCommunication.cpp
 *
 * PROGRAM:     [BIG_GAME]
 *
 * FUNCTIONS:
 *                            
 *
 * DATE:        February 15, 2014
 *
 * REVISIONS:   (Date and Description)
 *
 * DESIGNER:    
 *
 * PROGRAMMER:  
 *
 * NOTES:
 * 
 * This file contains all methods responsible for communication with the server.
 *
 *----------------------------------------------------------------------------------------*/

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "NetworkRouter.h"
#include "ServerCommunication.h"
#include "GameplayCommunication.h"
#include "PipeUtils.h"
#include "Packets.h"

extern uint32_t packet_sizes[NUM_PACKETS];
static int cnt_errno = 0;

/*------------------------------------------------------------------------------------------
 * FUNCTION:    recv_thread_func
 *
 * DATE:        February 16, 2014
 *
 * REVISIONS:   (Date and Description)
 *
 * DESIGNER:    Shane Spoor
 *
 * PROGRAMMER:  Shane Spoor
 *
 * INTERFACE:   void recv_thread_func(void *ndata)
 *
 *										void *ndata: 	Pointer to a NETWORK_DATA struct
 *														containing socket descriptors and
 *														the pipe end to the Network Router
 *														thread.
 * RETURNS:     void
 *
 * NOTES:
 *
 * This thread will continuously monitor a TCP and a UDP socket which will receive data from
 * the server. Upon receiving data, the thread writes to the pipe connected to the network
 * router thread. The thread will return in case of any error condition (wrapper functions
 * are responsible for error message logging).
 *
 *----------------------------------------------------------------------------------------*/
 void *recv_thread_func(void *ndata)
 {
 	NDATA				recv_data = (NDATA)ndata;
 	int 				numready;
	uint32_t 			packet_type;
	uint64_t			timestamp;
	void 				*game_packet;
 	SDLNet_SocketSet 	set = make_socket_set(2, recv_data->tcp_sock, recv_data->udp_sock);
	
 	if(!set)
 		return NULL;
 	
 	while(1)
 	{
 		if((numready = check_sockets(set)) == -1)
 		{
 			SDLNet_FreeSocketSet(set);
 			return NULL;
 		}
 
		else if(numready)
		{
			if(SDLNet_SocketReady(recv_data->tcp_sock))
			{
				if((game_packet = recv_tcp_packet(recv_data->tcp_sock, &packet_type, &timestamp)) == NULL)
				{
					if(cnt_errno == -3){
						fprintf(stderr, "Failure in tcp recv : %s \n", SDLNet_GetError());
						exit(1);
					}

					if(cnt_errno == -2){
						printf("Connection lost, exiting client.\n");
						close_connections(set, recv_data->tcp_sock, recv_data->udp_sock);
						exit(2);	
					}
				}
				printf("Recieved TCP packet: %u\n", packet_type);		
				if(write_packet(recv_data->write_pipe, packet_type, game_packet) == -1 ||
				   write_pipe(recv_data->write_pipe, &timestamp, sizeof(timestamp)) == -1)
				{
					printf("TCP>Router: Error in write packet, flushing pipe");
					fflush((FILE*)&recv_data->write_pipe);
				}

				free(game_packet);
			}

    		if(SDLNet_SocketReady(recv_data->udp_sock))
    		{
				if((game_packet = recv_udp_packet(recv_data->udp_sock, &packet_type, &timestamp)) == NULL)
				{
					if(cnt_errno == -3)
					{
						printf("Failure in udp recv : %s ", SDLNet_GetError());
						continue;
					}
				}
				printf("Recieved TCP packet: %u\n", packet_type);	
				if(write_packet(recv_data->write_pipe, packet_type, game_packet) == -1 ||
				   write_pipe(recv_data->write_pipe, &timestamp, sizeof(timestamp)) == -1)
				{
					printf("UDP>Router: Error in write packet, flushing pipe");
					fflush((FILE*)&recv_data->write_pipe);
				}

				free(game_packet);
			}
    	}
 	}
}
/*------------------------------------------------------------------------------------------
 * FUNCTION:    send_thread_func
 *
 * DATE:        Febuary 13 2014
 *
 * REVISIONS:   None
 *
 * DESIGNER:    Ramzi Chennafi
 *
 * PROGRAMMER:  Ramzi Chennafi
 *
 * INTERFACE:   void* send_thread_func(void* ndata){
 *					void * ndata : NETWORK_DATA pointer containing
 *								   a tcp socket, udp socket and a file
 *								   descriptor to the network router
 *								   send pipe.
 *
 * RETURNS:     Nothing
 *
 * NOTES:
 *
 *  Sends data received from the network router pipe to the server. Data sent on 
 *  pipe is recieved as a snd_pkt struct containing the protocol and raw data.
 *
 *----------------------------------------------------------------------------------------*/
void* send_thread_func(void* ndata){

	NDATA snd_data = (NDATA) ndata;

	int protocol = 0;
	uint32_t type = 0;
	void * data;
	int ret = -1;
	fd_set listen_fds;

	while(1){	
    	data = grab_send_packet(&type, snd_data->read_pipe, &ret);
    	if(ret != 1){
			continue;
		}
		
		protocol = get_protocol(type);
		if(protocol == TCP)
		{
			send_tcp(&type, snd_data->tcp_sock, sizeof(uint32_t));
			send_tcp(data, snd_data->tcp_sock, packet_sizes[type - 1]);
		}
		else if(protocol == UDP)
		{
			send_udp(data, &type, snd_data->udp_sock, packet_sizes[type - 1] + sizeof(uint32_t));
		}
		printf("Done sending\n");
	}
	return NULL;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: send_tcp
--
--      DATE: January 20, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int send_tcp(char * data)
--
--      RETURNS: int, 0 on success
--
--      NOTES:
--      Sends the packet data over the established tcp connection.
----------------------------------------------------------------------------------------------------------------------*/
int send_tcp(void * data, TCPsocket sock, uint32_t size){

	int result=SDLNet_TCP_Send(sock, data, size);
	if(result <= 0) {
    	fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
    	return -1;
	}

	return 0;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: send_udp
--
--      DATE: Febuary 15, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int send_udp(char * data)
--
--      RETURNS: int, returns -1 on failure, 0 on success.
--
--      NOTES:
--      Sends the specified data across UDP. Allocates the UDP packet, establishes the random socket for tranfer and then
--		sends the data on the established socket. Frees the packet after completion.
----------------------------------------------------------------------------------------------------------------------*/
int send_udp(void * data, uint32_t * type, UDPsocket sock, uint32_t size){

	int numsent;
	UDPpacket *pktdata = alloc_packet(size);
	memcpy(pktdata->data, type, sizeof(uint32_t));
	memcpy(pktdata->data + sizeof(uint32_t), data, size - sizeof(uint32_t));
	pktdata->len = size;

	numsent=SDLNet_UDP_Send(sock, pktdata->channel, pktdata);
	if(!numsent) {
    	fprintf(stderr,"SDLNet_UDP_Send: %s\n", SDLNet_GetError());
    	return -1;
	}

	SDLNet_FreePacket(pktdata);
	return 0;
}

/*-------------------------------------------------------------------------------------------------------------------
--      FUNCTION: recv_tcp_packet
--
--      DATE: February 18th, 2014
--      REVISIONS: none
--
--      DESIGNER: Shane Spoor
--      PROGRAMMER: Shane Spoor
--
--      INTERFACE: void *recv_tcp_packet(tcp_socket sock, int *packet_type)
--										tcp_socket sock: 		The TCP socket from which to read.
--										int *game_packet_type: 	Pointer to an int in which to store the packet type.
--										size_t *packet_size:	Pointer to a size_t in which to store the packet size.
--
--      RETURNS: A data buffer containing the packet on success, NULL on failure, -2 cast to void * on close connection.
--
--      NOTES:
--      Reads the packet type first, then allocates and reads the packet into a data buffer.
----------------------------------------------------------------------------------------------------------------------*/
void *recv_tcp_packet(TCPsocket sock, uint32_t *packet_type, uint64_t *timestamp)
{
	void *packet;
	int numread;

	numread = recv_tcp(sock, packet_type, sizeof(uint32_t));
	if(numread < 0)
		return NULL;

	uint32_t packet_size = packet_sizes[(*packet_type) - 1];

	if((packet = malloc(packet_size)) == NULL)
	{
		perror("recv_ tcp_packet: malloc");
		cnt_errno = -1;
		return NULL;
	}

	numread = recv_tcp(sock, packet, packet_size);
	numread = recv_tcp(sock, timestamp, sizeof(uint64_t));
	return packet;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: recv_udp_packet
--
--      DATE: Febuary 15, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: UDPpacket *recv_udp_packet(udp_socket sock, int *game_packet_type)
--
--      RETURNS: A data buffer containing the game packet on success, or NULL on failure.
--
--      NOTES:
--      Reads the packet type into packet_type, then reads the 
----------------------------------------------------------------------------------------------------------------------*/
void *recv_udp_packet(UDPsocket sock, uint32_t *packet_type, uint64_t *timestamp)
{
	UDPpacket *pktdata = SDLNet_AllocPacket(MAX_UDP_RECV + sizeof(uint32_t) + sizeof(uint64_t)); /* Allocate space for the max packet, the packet type, and the timestamp */
	void *packet;
	uint32_t packet_size;

	if(recv_udp(sock, pktdata) == -1){
		cnt_errno = -3;
		return NULL;
	}

	*packet_type 	= ((uint32_t *)pktdata->data)[0];
	packet_size 	= packet_sizes[(*packet_type) - 1];
	packet			= malloc(packet_size);

	if(!packet)
	{
		perror("recv_udp_packet: malloc");
		exit(3);
	}
	
	memcpy(packet, pktdata->data + sizeof(uint32_t), packet_size); 				/* Read the packet, starting address is after the packet_size*/
	*timestamp = *((uint64_t *)(pktdata->data + packet_size + sizeof(uint32_t))); /* Read the timestamp, starting address is after packet_size and packet */

	SDLNet_FreePacket(pktdata);
	return packet;
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: recv_tcp
--
--      DATE: Febuary 21, 2014
--      REVISIONS: none
--
--      DESIGNER: Shane Spoor
--      PROGRAMMER: Shane Spoor
--
--      INTERFACE: int recv_tcp(tcp_socket sock, void *buf, size_t bufsize)
--								tcp_socket sock: The TCP socket from which to receive data.
--								void *buf:		The buffer into which the data will be read.
--								size_t bufsize:	The size of buf.
--
--      RETURNS: 0 if the packet was received successfully; -1 on failure.
--
--      NOTES:
--      Reads bufsize bytes into the buffer pointed to by buf.
----------------------------------------------------------------------------------------------------------------------*/
int recv_tcp(TCPsocket sock, void *buf, size_t bufsize)
{
	int numread = SDLNet_TCP_Recv(sock, buf, bufsize);
	
	if(numread == -1)
	{
    	fprintf(stderr, "SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
    	return cnt_errno = -3;
	}
	else if(numread == 0){
		return cnt_errno = -2;
	}
	
	return numread;
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: recv_udp
--
--      DATE: Febuary 21, 2014
--      REVISIONS: none
--
--      DESIGNER: Shane Spoor
--      PROGRAMMER: Shane Spoor
--
--      INTERFACE: int recv_udp(udp_socket sock, UDPpacket *upd_packet)
--
--      RETURNS: 0 if the packet was received successfully; -1 on failure.
--
--      NOTES:
--      Reads a packet into the buffer pointed udp_packet.
----------------------------------------------------------------------------------------------------------------------*/
int recv_udp (UDPsocket sock, UDPpacket *udp_packet)
{
	if(SDLNet_UDP_Recv(sock, udp_packet) == -1)
	{
		fprintf(stderr, "SDLNet_UDP_Recv: %s\n", SDLNet_GetError());
		return -1;
	}
	
	return 0;
}

/*------------------------------------------------------------------------------------------
 * FUNCTION:    grab_send_packet
 *
 * DATE:        Febuary 20 2014
 *
 * REVISIONS:   None.
 *
 * DESIGNER:    Ramzi Chennafi
 *
 * PROGRAMMER:  Ramzi Chennafi
 *
 * INTERFACE:   char* grab_send_packet(int &protocol, int &type, int fd)
 *                  int &protocol : pointer to  a empty protocol int that is used to determine packet sending
 *					int &type     : pointer to an empty type int that is used to determine packet size (type)
 *					int fd        : file descriptor to the network router read pipe
 *					
 *
 * RETURNS:     char* : pointer to an array of chars containing packet data
 *
 * NOTES:
 *
 *  Grabs the first packet on the pipe to be sent by send thread.
 *
 *----------------------------------------------------------------------------------------*/
void*grab_send_packet(uint32_t *type, int fd, int *ret){

	*type = read_type(fd);
	if(*type >= 90){
		*ret = -1;
		return NULL;
	}

	uint32_t size = packet_sizes[*type - 1];

	void * data = (void*) malloc(sizeof(size));

	data = read_packet(fd, size); // reads data

	*ret = 1;

	return data;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: alloc_packet
--
--      DATE: Febuary 15, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: UDPpacket alloc_packet(char * data)
--
--      RETURNS: UDPpacket on success, null on failure
--
--      NOTES:
--      Creates a UDPpacket with the data passed to it.
----------------------------------------------------------------------------------------------------------------------*/
UDPpacket *alloc_packet(uint32_t size){

	UDPpacket *pktdata = SDLNet_AllocPacket(size);

	if(!pktdata) {
	    fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
	    return NULL;
	}

	return pktdata;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: resolve_host_ip
--
--      DATE: Febuary 15, 2014
--      REVISIONS: none
--
--      DESIGNER: Shane Spoor
--      PROGRAMMER: Shane Spoor
--
--      INTERFACE: resolve_host(IPaddress *ip_addr, const uint16_t port, const char *host_ip_string)
--								IPaddress *ip_addr:		Pointer to the IPaddress struct which will receive the address
--														information.
--								uint16_t port:			The port to use for sending/receiving on the socket created
--														with this IPaddr struct.
--								char *host_ip_string:	The hostname or IP address (in dotted string form) to resolve.
--
--      RETURNS: -1 on failure, or 0 on success.
--
--      NOTES:
--      Creates an IPaddress struct holding the IP address and port information for the SDL network functions. If the
--		funciton fails, it will log an error and return NULL.
----------------------------------------------------------------------------------------------------------------------*/
int resolve_host(IPaddress *ip_addr, const uint16_t port, const char *host_ip_string)
{
	if(SDLNet_ResolveHost(ip_addr, host_ip_string, port) == -1)
	{
    	fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    	return -1;
    }
    return 0;
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: make_socket_set
--
--      DATE: Febuary 15, 2014
--      REVISIONS: none
--
--      DESIGNER: Shane Spoor
--      PROGRAMMER: Shane Spoor
--
--      INTERFACE: make_socket_set(size_t num_sockets, ...)
--									size_t num_sockets: The number of sockets to be added to the set.
--									...: A list of num_sockets of tcp_socket and udp_socket struct pointers to be added.
--
--      RETURNS: An SDLNet_SocketSet pointer on success, or NULL on failure.
--
--      NOTES:
--      Creates an IPaddress struct holding the IP address and port information for the SDL network functions. If the
--		funciton fails, it will log an error and return NULL.
----------------------------------------------------------------------------------------------------------------------*/ 	
SDLNet_SocketSet make_socket_set(int num_sockets, ...)
{
	va_list socket_list; 	
	SDLNet_SocketSet set = SDLNet_AllocSocketSet(num_sockets);

	if(!set)
	{
		fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		return NULL;
	}
	
	va_start(socket_list, num_sockets);
	for(size_t i = 0; i < num_sockets; i++)
	{
		if(SDLNet_AddSocket(set, va_arg(socket_list, SDLNet_GenericSocket)) == -1)
		{
			fprintf(stderr, "SDLNet_AddSocket: %s\n", SDLNet_GetError());
			return NULL;
		}
	}

	va_end(socket_list);
	return set;
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: check_sockets
--
--      DATE: Febuary 21, 2014
--      REVISIONS: none
--
--      DESIGNER: Shane Spoor
--      PROGRAMMER: Shane Spoor
--
--      INTERFACE: check_sockets(SDLNet_SocketSet set)
--									SDENet_SocketSet set: The set to monitor for activity.
--
--      RETURNS: The number of sockets ready on success, or -1 on failure.
--
--      NOTES:
--      Runs select to determine whether the sockets have data to receive.
----------------------------------------------------------------------------------------------------------------------*/ 
int check_sockets(SDLNet_SocketSet set)
{
	int numready = SDLNet_CheckSockets(set, 100000);

	if(numready == -1)
	{
		fprintf(stderr, "SDLNet_CheckSockets: %s\n", SDLNet_GetError());
		perror("SDLNet_CheckSockets");
	}

	return numready;
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: get_protocol
--
--      DATE: Febuary 21, 2014
--      REVISIONS: none
--
--      DESIGNER: Shane Spoor
--      PROGRAMMER: Shane Spoor
--
--      INTERFACE: int get_protocol(uint32_t type)
--									int type : type of packet to get the protocol for
--
--      RETURNS: int : The correct protocol for the specified packet type.
--
--      NOTES:
--      Grabs the correct protocol for the specified packet.
----------------------------------------------------------------------------------------------------------------------*/ 
int get_protocol(uint32_t type)
{
	int protocol;

	switch(type)
	{
		case P_NAME:
		case P_CONNECT:
		case G_STATUS:
		case P_CHAT:
		case P_CLNT_LOBBY:
		case P_OBJCTV_LOC:
		case P_UNDEF:
		case P_UNDEF2:
		case P_OBJSTATUS:
			protocol = TCP;
			break;
		case P_POSUPDATE:
		case P_FLOOR_MOVE_REQ:
		case P_FLOOR_MOVE:
		case P_TAGGING:
			protocol = UDP;
			break;
	}

	return protocol;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: close_connections
--
--      DATE: Febuary 21, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void close_connections(SDLNet_SocketSet set, TCPsocket tcpsock, UDPsocket udpsock)
--									SDENet_SocketSet set: The set to exit.
--									TCPsocket tcpsock : tcp socket to close
--									UDPsocket udpsock : udp socket to cose
--
--      RETURNS: The number of sockets ready on success, or -1 on failure.
--
--      NOTES:
--      Removes sockets from set and closes open sockets.
----------------------------------------------------------------------------------------------------------------------*/ 
void close_connections(SDLNet_SocketSet set, TCPsocket tcpsock, UDPsocket udpsock)
{
	int numused;

	numused=SDLNet_UDP_DelSocket(set,udpsock);
	if(numused==-1) {
	    printf("SDLNet_DelSocket: %s\n", SDLNet_GetError());
	}
	numused=SDLNet_TCP_DelSocket(set,tcpsock);
	if(numused==-1) {
	    printf("SDLNet_DelSocket: %s\n", SDLNet_GetError());
	}

	SDLNet_TCP_Close(tcpsock);
	SDLNet_UDP_Close(udpsock);
}
