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
/////////////////////FOR TESTING PURPOSES////////////////////////////////////////////////////
// int main(){
	
// 	packet * pkt = (packet*) malloc(sizeof(packet));
// 	pkt->protocol = TCP;
// 	pkt->data = "testing";

// 	if(SDL_Init(0)==-1) {
//     	fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
//     	exit(1);
// 	}

// 	init_send_data(pkt);

// 	SDLNet_Quit();
// }
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
 * INTERFACE:   void recv_thread_func(const int router_pipe_writefd, const sockaddr_in
 *									*server_info)
 *                  int router_pipe_writefd - 		Identifies the write end of a pipe to the
 *													network router thread.
 *					const char *host_ip_string -	A host name or IP address string to be
 *													resolved.
 *
 * RETURNS:     void
 *
 * NOTES:
 *
 * This thread will continuously monitor a TCP and UDP socket which will receive data from
 * the server. Upon receiving data, the thread writes to the pipe connected to the network
 * router thread.
 *
 *----------------------------------------------------------------------------------------*/
 void recv_thread_func(int router_pipe_writefd, const char *host_ip_string)
 {
 	
 	while(1)
 	{
 		
 	}
 }
 /*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_send_data
--
--      DATE: Febuary 15, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void init_send_data(packet pkt)
--
--      RETURNS: void
--
--      NOTES:
--      Program entry point. Disables terminal processing, creates 3 pipes and 2 children processes. These processes
--		are directed into their respective function paths.
----------------------------------------------------------------------------------------------------------------------*/
void init_send_data(packet pkt){

	switch(pkt->protocol){
		case TCP:
			send_tcp(pkt->data);
			break;
		case UDP:
			send_udp(pkt->data);
			break;
		default:
			fprintf(stderr, "Invalid protocol in init_send_data().");
	}
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
int send_tcp(char * data){
	TCPsocket sock = intiate_tcp();

	len=strlen(data)+1;
	result=SDLNet_TCP_Send(sock, data, len);
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
int send_udp(char * data){

	int numsent;
	UDPpacket pktdata = alloc_packet(data);
	UDPsocket sock = initiate_udp();

	numsent=SDLNet_UDP_Send(udpsock, packet.channel, packet);
	if(!numsent) {
    	fprintf(stderr,"SDLNet_UDP_Send: %s\n", SDLNet_GetError());
    	return -1;
	}

	SDLNet_FreePacket(pktdata);
	return 0;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: intiate_tcp
--
--      DATE: Febuary 15, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: TCPsocket intiate_tcp()
--
--      RETURNS: TCPSocket on successful establishment of a connection. NULL on failure.
--
--      NOTES:
--      Intiates a tcp connection to the specified host.
----------------------------------------------------------------------------------------------------------------------*/
TCPsocket intiate_tcp(){
	IPaddress ipaddress;
	SDLNet_ResolveHost(ipaddress, "127.0.0.1", 5151);

	if(!(tcpsock=SDLNet_TCP_Open(&ip))){
    	fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
    	return NULL;
	}

	return tcpsock;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: intiate_udp 
--
--      DATE: Febuary 15, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: TCPsocket intiate_udp()
--
--      RETURNS: TCPSocket on successful establishment of a connection. NULL on failure.
--
--      NOTES:
--      Intiates a udp connection on a random socket.
----------------------------------------------------------------------------------------------------------------------*/
UDPsocket intiate_udp(){
	UDPsocket udpsock;

	udpsock=SDLNet_UDP_Open(0);
	if(!udpsock) {
	    fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
	    return NULL;
	}

	return udpsock;
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
UDPpacket alloc_packet(char * data){
	UDPpacket *packet;

	packet=SDLNet_AllocPacket(sizeof(data));
	if(!packet) {
	    printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
	    return NULL;
	}

	return packet;
}