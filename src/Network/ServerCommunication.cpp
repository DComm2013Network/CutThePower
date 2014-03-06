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
#include "NetworkRouter.h"
#include "ServerCommunication.h"
#include "GameplayCommunication.h"
#include "PipeUtils.h"
#include "Packets.h"

extern uint32_t packet_sizes[NUM_PACKETS];

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
					return NULL;
				if(write_packet(recv_data->write_pipe, packet_type, game_packet) == -1 ||
				   write_pipe(recv_data->write_pipe, &timestamp, sizeof(timestamp)) == -1)
				{
					free(game_packet);
					return NULL;
				}
				free(game_packet);
			}

    		if(SDLNet_SocketReady(recv_data->udp_sock))
    		{
				if((game_packet = recv_udp_packet(recv_data->udp_sock, &packet_type, &timestamp)) == NULL)
					return NULL;
				if(write_packet(recv_data->write_pipe, packet_type, game_packet) == -1 ||
				   write_pipe(recv_data->write_pipe, &timestamp, sizeof(timestamp)) == -1)
				{
					free(game_packet);
					return NULL;
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
	char * data;
	int ret = -1;
	fd_set listen_fds;

	while(1){	
		int ret;
        FD_ZERO(&listen_fds);
        FD_SET(snd_data->read_pipe, &listen_fds);
        ret = select(snd_data->read_pipe, &listen_fds, NULL, NULL, NULL);

        if(FD_ISSET(snd_data->read_pipe, &listen_fds))
        {  
        	data = grab_send_packet(&type, snd_data->read_pipe, &ret);
        	if(type >= 90)
				continue;
		}		
		// else if(protocol == UDP){
		// 	send_udp(data, snd_data->udp_sock);
		// }
		// else{
		// 	perror("Invalid protocol.");
		// }
	}

	return NULL;
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
--      RETURNS: A data buffer containing the packet on success, or NULL on failure.
--
--      NOTES:
--      Reads the packet type first, then allocates and reads the packet into a data buffer.
----------------------------------------------------------------------------------------------------------------------*/
void *recv_tcp_packet(TCPsocket sock, uint32_t *packet_type, uint64_t *timestamp)
{
	void *packet;
	
	if(recv_tcp(sock, packet_type, sizeof(uint32_t)) == -1) /* Read the type of the packet */
		return NULL;

	uint32_t packet_size = packet_sizes[(*packet_type) - 1];

	if((packet = malloc(packet_size)) == NULL)
	{
		perror("recv_tcp_packet: malloc");
		return NULL;
	}

	if(recv_tcp(sock, packet, packet_size) == -1) /* Read the remainder of the packet */
		return NULL;
	
	if(recv_tcp(sock, timestamp, sizeof(uint64_t)) == -1)
		return NULL;
	
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

	if(recv_udp(sock, pktdata) == -1)
		return NULL;

	*packet_type 	= ((uint32_t *)pktdata->data)[0];
	packet_size 	= packet_sizes[(*packet_type) - 1];
	packet			= malloc(packet_size);

	if(!packet)
	{
		perror("recv_udp_packet: malloc");
		return NULL;
	}
	
	memcpy(packet, pktdata->data + sizeof(uint32_t), packet_size); 					/* Read the packet, starting address is after the packet_size*/
	*timestamp = *((uint64_t *)pktdata->data + packet_size + sizeof(uint32_t)); /* Read the timestamp, starting address is after packet_size and packet */

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
    	return -1;
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
char *grab_send_packet(uint32_t *type, int fd, int *ret){

	if((*type = read_type(fd)) <= -1){
		*ret = -1;
		return NULL;
	}
	uint32_t size = packet_sizes[*type];

	char * data = (char*) malloc(sizeof(size + 1));

	data = (char*)read_packet(fd, size); // reads data

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
UDPpacket *alloc_packet(char *data){

	UDPpacket *pktdata = SDLNet_AllocPacket(strlen(data));

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
