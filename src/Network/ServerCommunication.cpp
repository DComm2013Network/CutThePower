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
#include "ServerCommunication.h"
#include "GameplayCommunication.h"
#include "NetworkUtils.h"
#include "Packets.h"
 extern size_t packet_sizes[];
/*
 int main(){
	
 	//packet * pkt = (packet*) malloc(sizeof(packet));
	//pkt->protocol = TCP;
 	//pkt->data = "testing";

 	if(SDL_Init(0)==-1) {
     	fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
     	exit(1);
 	}

 	//init_send_data(pkt);

 	SDLNet_Quit();
 }
//////////////////////FOR TESTING PURPOSES//////////////////////////////////////////////////
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
 * INTERFACE:   void recv_thread_func(const int router_pipe_writefd, const char *host_ip_string)
 *                  int router_pipe_writefd:  	Identifies the write end of a pipe to the
 *												network router thread.
 *					uint16_t port:				The port on which to listen (for UDP).	
 *					TCPsocket tcp_recvsock:		TCP socket on which to listen for data from the
 *												server. This must be opened and passed as an
 *												argument since both the send and receive threads
 *												will use the same socket.
 *					UDPsocket upd_recvsock:		UDP socket on which to listen for data from the
 *												server. This will also be used in both threads.
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
 void recv_thread_func(int router_pipe_writefd, TCPsocket tcp_recvsock, UDPsocket udp_recvsock)
 {
 	int 				numready;
	int 				packet_type;
	void 				*game_packet;
 	SDLNet_SocketSet 	set = make_socket_set(2, tcp_recvsock, udp_recvsock);
 	
 	if(!set)
 		return;
 	 		
 	while(1)
 	{
 		if((numready = check_sockets(set)) == -1)
 		{
 			SDLNet_FreeSocketSet(set);
 			return;
 		}
 
		else if(numready)
		{
			if(SDLNet_SocketReady(tcp_recvsock))
			{
				if((game_packet = recv_tcp_packet(tcp_recvsock, &packet_type)) == NULL)
					return;

				if(write_packet(router_pipe_writefd, packet_type, game_packet) == -1)
				{
					free(game_packet);
					return;
				}
				free(game_packet);
			}

    		if(SDLNet_SocketReady(udp_recvsock))
    		{
				if((game_packet = recv_udp_packet(udp_recvsock, &packet_type)) == NULL)
					return;

				if(write_packet(router_pipe_writefd, packet_type, game_packet) == -1)
				{
					free(game_packet);
					return;
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

	NETWORK_DATA * snd_data = (NETWORK_DATA*) ndata;

	int protocol = 0, type = 0;
	char * data;

	while(1){	
	
		if((data = grab_send_packet(&protocol, &type, snd_data->read_pipe)) < 0){
			continue;
		}

		if(protocol == TCP){
			send_tcp(data, snd_data->tcpsock);
		}
		else if(protocol == UDP){
			send_udp(data, snd_data->udpsock);
		}
		else{
			perror("Invalid protocol.");
		}
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
int send_tcp(char * data, TCPsocket sock){

	int len=strlen(data);
	int result=SDLNet_TCP_Send(sock, data, len);
	if(result < len) {
    	fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
    	return -1;
	}

	SDLNet_TCP_Close(sock);
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
int send_udp(char * data, UDPsocket sock){

	int numsent;
	UDPpacket *pktdata = alloc_packet(data);

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
--      INTERFACE: void *recv_tcp_packet(TCPsocket sock, int *packet_type)
--										TCPsocket sock: 		The TCP socket from which to read.
--										int *game_packet_type: 	Pointer to an int in which to store the packet type.
--										size_t *packet_size:	Pointer to a size_t in which to store the packet size.
--
--      RETURNS: A data buffer containing the packet on success, or NULL on failure.
--
--      NOTES:
--      Reads the packet type first, then allocates and reads the packet into a data buffer.
----------------------------------------------------------------------------------------------------------------------*/
void *recv_tcp_packet(TCPsocket sock, int *game_packet_type)
{
	void *game_packet;
	
	if(recv_tcp(sock, game_packet_type, sizeof(int)) == -1)
		return NULL;

	int game_packet_size = packet_sizes[*game_packet_type];

	if((game_packet = malloc(game_packet_size)) == NULL)
	{
		perror("recv_tcp_packet: malloc");
		return NULL;
	}

	if(recv_tcp(sock, game_packet, game_packet_size) == -1)
		return NULL;
	
	return game_packet;
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
--      INTERFACE: UDPpacket *recv_udp_packet(UDPsocket sock, int *game_packet_type)
--
--      RETURNS: A data buffer containing the game packet on success, or NULL on failure.
--
--      NOTES:
--      Reads the packet type into packet_type, then reads the 
----------------------------------------------------------------------------------------------------------------------*/
void *recv_udp_packet(UDPsocket sock, int *game_packet_type)
{
	UDPpacket *pktdata = SDLNet_AllocPacket(MAX_UDP_RECV + sizeof(int)); /* Allocate space for the max + the packet type */
	void *game_packet;
	int packet_size;

	if(recv_udp(sock, pktdata) == -1)
		return NULL;

	*game_packet_type 	= ((int *)pktdata->data)[0];
	packet_size 		= packet_sizes[*game_packet_type];
	game_packet			= malloc(packet_size);

	if(!game_packet)
	{
		perror("recv_udp: malloc");
		return NULL;
	}

	SDLNet_FreePacket(pktdata);
	return game_packet;
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
--      INTERFACE: int recv_tcp(TCPsocket sock, void *buf, size_t bufsize)
--								TCPsocket sock: The TCP socket from which to receive data.
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
--      INTERFACE: int recv_udp(UDPsocket sock, UDPpacket *upd_packet)
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
char* grab_send_packet(int *protocol, int *type, int fd){

	*protocol = read_type(fd); //  grabs protocol
	*type = read_type(fd); // grabs type
	int size = packet_sizes[*type];

	char * data = (char*) malloc(sizeof(size));

	data = (char*)read_packet(fd, size); // reads data

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
--      FUNCTION: frame_data
--
--      DATE: Febuary 15, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: packet * frame_data(int type, void* data)
--
--      RETURNS: packet * - pointer to a packet structure
--
--      NOTES:
--     	Frames packets recieved from gameplay for sending to the server.
----------------------------------------------------------------------------------------------------------------------*/
internal_packet * frame_data(int type, void* data){

	internal_packet * framing_pkt = (internal_packet*) malloc(sizeof(internal_packet));

	switch(type){
		case P_NAME: // TCP PACKETS
			framing_pkt->protocol = TCP;
			framing_pkt->type = type;
			framing_pkt->data = (char*)data;
		break;
		case P_POSUPDATE:
			framing_pkt->protocol = UDP;
			framing_pkt->type = type;
			framing_pkt->data = (char*)data;
		break;
	}

	return framing_pkt;
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
--									...: A list of num_sockets of TCPsocket and UDPsocket struct pointers to be added.
--
--      RETURNS: An SDLNet_SocketSet pointer on success, or NULL on failure.
--
--      NOTES:
--      Creates an IPaddress struct holding the IP address and port information for the SDL network functions. If the
--		funciton fails, it will log an error and return NULL.
----------------------------------------------------------------------------------------------------------------------*/ 	
SDLNet_SocketSet make_socket_set(size_t num_sockets, ...)
{
	va_list socket_list;
	SDLNet_SocketSet set = SDLNet_AllocSocketSet(num_sockets);
	size_t i;

	if(!set)
	{
		fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		return NULL;
	}
	
	va_start(socket_list, num_sockets);
	for(i = 0; i < num_sockets; ++i)
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
	int numready = SDLNet_CheckSockets(set, INFINITE_TIMEOUT);

	if(numready == -1)
	{
		fprintf(stderr, "SDLNet_CheckSockets: %s\n", SDLNet_GetError());
		perror("SDLNet_CheckSockets");
	}

	return numready;
}

int read_protocol(int fd){
	
	int protocol, read_bytes;

    if( (read_bytes = read_pipe(fd, &protocol, sizeof(int))) < 0)
    {
        if(read_bytes == 0)
        {
            return 0; //end of file .. nothing in pipe
        }

        return -1; //error .. check error
    }

    return read_bytes;
}