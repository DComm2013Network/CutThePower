/** @ingroup Network */
/** @{ */

/**
 * This file contains all methods responsible for communication with the server.
 *
 * @todo Get rid of magic numbers in grab_send_packet
 * @todo Make writing to cnt_errno thread safe
 * @todo Write to network router requesting that keep alive be sent
 * 
 * @file ServerCommunication.cpp
 */

/** @} */
#include "Packets.h"
#include "ServerCommunication.h"
#include "GameplayCommunication.h"
#include "NetworkRouter.h"
#include "PipeUtils.h"
#include "packet_min_utils.h"
#include "ServerCommunication.h"
 
extern uint32_t packet_sizes[NUM_PACKETS];
static int cnt_errno = -1;
extern sem_t err_sem;
static uint64_t tcp_seq_num = 1;
extern int send_failure_fd;

/**
 * Monitors sockets to receive data from the server.
 *
 * Upon receiving data, the thread writes to the pipe connected to the network
 * router thread. The thread will return in case of any error condition (wrapper functions
 * are responsible for minor error handling; if the thread returns, network should stop
 * running altogether).
 *
 * @param[in] ndata Pointer to a NETWORK_DATA struct containing socket descriptors and
 *                  the pipe end to the Network Router thread.
 *
 * @return NULL upon termination.
 *
 * @designer Shane Spoor
 * @author   Shane Spoor
 *
 * @date February 16, 2014
 */
 void *recv_thread_func(void *ndata)
 {
 	NDATA				recv_data = (NDATA)ndata;
 	int 				numready;
 	SDLNet_SocketSet 	set = make_socket_set(2, recv_data->tcp_sock, recv_data->udp_sock);
	int 				res = 0;
    uint32_t            type;
    recv_cleanup_args   *clean_args;

    if(!set)
    {
        uint32_t err = NUM_PACKETS + 1;
        write_pipe(recv_data->write_pipe, &err, sizeof(err));
 		return NULL;
    }

 	clean_args = (recv_cleanup_args *)malloc(sizeof(recv_cleanup_args));
    if(!clean_args)
    {
        uint32_t err = NUM_PACKETS + 1;
        set_error(ERR_NO_MEM);
        write_pipe(recv_data->write_pipe, &err, sizeof(err));
        free_sockset(set, recv_data->tcp_sock, recv_data->udp_sock);
 		return NULL;
    }
    clean_args->set = set;
    clean_args->tcp_sock = recv_data->tcp_sock;
    clean_args->udp_sock = recv_data->udp_sock;
    pthread_cleanup_push(&recv_thread_clean, clean_args);

 	while(1)
 	{
 		if((numready = check_sockets(set)) == -1) // write error message to router
        {
            uint32_t err = NUM_PACKETS + 1;
            write_pipe(recv_data->write_pipe, &err, sizeof(err));
     	    break;
        }

		else
		{
            int oldstate;
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);

			if(SDLNet_SocketReady(recv_data->tcp_sock))
			{
				if((res = handle_tcp_in(recv_data->write_pipe, recv_data->tcp_sock)) == -1)
                    break;
            }

    		if(SDLNet_SocketReady(recv_data->udp_sock))
    		{
				if((res = handle_udp_in(recv_data->write_pipe, recv_data->udp_sock)) == -1)
                    break;
            }
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
    	}
 	}
    uint32_t err = NUM_PACKETS + 1;
    write_pipe(recv_data->write_pipe, &err, sizeof(err));
    pthread_cleanup_pop(1); // Calls the cleanup handler
    return NULL;
}

/**
 * Cleans up the receive thread.
 *
 * @param[in, out] cleanup_args Contains the socket set to be freed.
 *
 * @designer Shane Spoor
 * @author   Shane Spoor
 *
 * @date March 14th
 */
void recv_thread_clean(void *cleanup_args)
{
    recv_cleanup_args *args = (recv_cleanup_args *)cleanup_args;
 
    free_sockset(args->set, args->tcp_sock, args->udp_sock);
    if(args->tcp_sock)
        SDLNet_TCP_Close(args->tcp_sock);
    if(args->udp_sock);
        SDLNet_UDP_Close(args->udp_sock);
    free(cleanup_args);
}

/**
 * Handles the receipt of TCP data.
 *
 * Receives the TCP packet, if any, and writes it to the network router. Keep alive packets are
 * ignored. On error, it writes an error message (preceded by an packet type indicating an error)
 * and returns -1.
 *
 * @param[in] router_pipe_fd Descriptor for the write end of the pipe to the network router thread.
 * @param[in] tcp_sock       The TCP socket from which to receive data.
 *
 * @return 0 on success, or -1 if an error occurred. 
 *
 * @designer Shane Spoor
 * @author   Shane Spoor
 *
 */
int handle_tcp_in(int router_pipe_fd, TCPsocket tcp_sock)
{
    void *game_packet;
    uint32_t packet_type;
    uint64_t timestamp;
    
    if((game_packet = recv_tcp_packet(tcp_sock, &packet_type, &timestamp)) == NULL)
        return -1; // If it's a keep alive, ignore and return 0. 

    if(write_packet(router_pipe_fd, packet_type, game_packet) == -1 ||
        write_pipe(router_pipe_fd, &timestamp, sizeof(timestamp)) == -1)
    {
        fprintf(stderr, "handle_tcp_in: Error in write packet\n");
        set_error(ERR_IPC_FAIL);
        free(game_packet);
        return -1;
	}
    free(game_packet);
    return 0;
}

/**
 * Handles the receipt of UDP packets.
 *
 * Receives the UDP packet and writes it to the network router. On error, it writes an error message 
 * (preceded by an packet type indicating an error) and returns -1.
 *
 * @param[in] router_pipe_fd Descriptor for the write end of the pipe to the network router thread.
 * @param[in] udp_sock       The UDP socket from which to receive data.
 * 
 * @return 0 on success, or -1 if an error occurred. 
 *
 * @designer Shane Spoor
 * @author   Shane Spoor
 *
 * @date March 12, 2014
 */
int handle_udp_in(int router_pipe_fd, UDPsocket udp_sock)
{
    void *game_packet;
    uint32_t packet_type;
    uint64_t timestamp;

    if((game_packet = recv_udp_packet(udp_sock, &packet_type, &timestamp)) == NULL)
        return (packet_type == INVALID_PACKET_TYPE) - 1; // If it's a corrupted packet, that's fine; return 0

	// if it's a min pos update, decapsulate it first
	if(packet_type == P_MIN_POS)
	{
		packet_type = P_POSUPDATE;
		game_packet = (void *)decapsulate_pos_update((PKT_POS_UPDATE_MIN *)game_packet);
	}

	else if (packet_type == P_MIN_POS_ALL)
	{
		packet_type = G_ALLPOSUPDATE;
		game_packet = decapsulate_all_pos_update((PKT_ALL_POS_UPDATE_MIN *)game_packet);
	}
	
    if(write_packet(router_pipe_fd, packet_type, game_packet) == -1 ||
        write_pipe(router_pipe_fd, &timestamp, sizeof(timestamp)) == -1)
    {
        fprintf(stderr, "handle_udp_in: Error in write packet\n");
        set_error(ERR_IPC_FAIL);
        free(game_packet);
        return -1;
    }

    free(game_packet);
    return 0;
}

/**
 * Reads a packet from the specified TCP socket.
 *
 * The function stores the packet in an allocated buffer if it's read successfully
 * and stores the packet type and timestamp in the variables passed by the caller.
 *
 * @param[in]  sock        The TCP socket from which to read.
 * @param[out] packet_type Holds the packet type on successful return. It may hold an invalid
 *                         packet type on failure.
 * @param[out] timestamp   Holds the timestamp on successful return.
 *
 * @return  A data buffer containing the packet on success and NULL on failure.
 *
 * @designer Shane Spoor
 * @author   Shane Spoor
 *
 * @date February 18th, 2014
 */
void *recv_tcp_packet(TCPsocket sock, uint32_t *packet_type, uint64_t *timestamp)
{
	void     *packet;
    uint32_t packet_size;

	if(recv_tcp(sock, packet_type, sizeof(uint32_t)) == -1)
        return NULL;
	
    if(*packet_type == P_KEEPALIVE)
        return NULL;

	if(*packet_type <= 1 || *packet_type > NUM_PACKETS)
	{
		fprintf(stderr, "recv_tcp_packet: Received Invalid Packet Type!\n");
		set_error(ERR_CORRUPTED);
        *packet_type = INVALID_PACKET_TYPE; // set this to a known invalid value
		return NULL; 
	}

	packet_size = packet_sizes[(*packet_type) - 1];
	if((packet = malloc(packet_size)) == NULL)
	{
		perror("recv_ tcp_packet: malloc");
		set_error(ERR_NO_MEM);
		return NULL;
	}

	recv_tcp(sock, packet, packet_size);
	*timestamp = tcp_seq_num++;
    //printf("Type: %d\n", *packet_type);
	return packet;
}

/**
 * Receives and processes a UDP packet containing a packet type, game data,
 * and a timestamp.
 *
 * The packet type and timestamp will be stored in @a packet_type and @a timestamp
 * if the function completes successfully. @a packet_type will contain an invalid
 * packet type if one is read; similarly, the contents of timestamp will be invalid
 * if the function receives an invalid timestamp. Note that the timestamp is not checked
 * for validity.
 *
 * @param[in]  sock        The UDP socket to receive data from.
 * @param[out] packet_type Receives the packet type.
 * @param[out] timestamp   Receives the timestamp.
 *
 * @return <ul>
 *              <li>Returns a data buffer containing the packet on success.</li>
 *				<li>Returns NULL on failure.</li>
 *         </ul>
 *
 * @designer Shane Spoor
 * @designer Ramzi Chennafi
 * @author   Shane Spoor
 *
 * @date Febuary 15, 2014
 */
void *recv_udp_packet(UDPsocket sock, uint32_t *packet_type, uint64_t *timestamp)
{
	void      *packet;
	uint32_t  packet_size;
    UDPpacket *pktdata = SDLNet_AllocPacket(MAX_UDP_RECV + sizeof(*packet_type) + sizeof(*timestamp));

	if(recv_udp(sock, pktdata) == -1)
		return NULL;

	*packet_type 	= *((uint32_t *)pktdata->data);
    if(*packet_type < 1 || *packet_type > NUM_PACKETS) // Impossible packet type; packet is corrupted
    {
		fprintf(stderr, "recv_udp_packet: Received Invalid Packet Type!\n");
        set_error(ERR_CORRUPTED);
        *packet_type = INVALID_PACKET_TYPE; // set this to a known invalid value
        return NULL;
    }

	packet_size 	= packet_sizes[(*packet_type) - 1];
	packet			= malloc(packet_size);

	if(!packet)
	{
        perror("recv_udp_packet: malloc");
		set_error(ERR_NO_MEM); 
		return NULL;
	}
	
	memcpy(packet, pktdata->data + sizeof(uint32_t), packet_size);
    memcpy(timestamp, pktdata->data + packet_size + sizeof(uint32_t), sizeof(*timestamp));
	*timestamp = *((uint64_t *)(pktdata->data + packet_size + sizeof(uint32_t)));

	SDLNet_FreePacket(pktdata);    
	return packet;
}

/**
 * Reads the specified amount of data from a TCP socket.
 *
 * @param[in]  tcp_socket The TCP socket from which to receive data.
 * @param[out] buf        The buffer into which the data will be read.
 * @param[in]  bufsize    The size (in bytes) of buf.cd
 *
 * @return ERR_RECV_FAILED if SDLNet_TCP_Recv returns an error, and ERR_CONN_CLOSED if no
 *         data was read (i.e., received a RST or a FIN). Returns 0 on success.
 *
 * @designer Shane Spoor
 * @author   Shane Spoor
 *
 * @date Febuary 21, 2014
 */
int recv_tcp(TCPsocket sock, void *buf, size_t bufsize)
{
    int numread = 0;
    int lastread = 0;
    while((unsigned int)numread < bufsize)
    {
	    lastread = SDLNet_TCP_Recv(sock, buf, bufsize);

        if(lastread == -1)
        {
            fprintf(stderr, "SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
            set_error(ERR_TCP_RECV_FAIL);
            return -1;
        }
        else if(lastread == 0){
            fprintf(stderr, "recv_tcp: Connection closed or reset.\n");
            set_error(ERR_CONN_CLOSED);
            return -1;
        }

        numread += lastread;
    }
	
	return 0;
}

/**
 * Reads a packet into the buffer pointed to by @a udp_packet.
 *
 * @param[in]  sock       The socket from which to read.
 * @param[out] udp_packet The udp packet buffer to hold the data. 
 *
 * @return -1 on failure (cnt_errno is set to ERR_RECV_FAILED) and 0 on success.
 *
 * @designer Shane Spoor
 * @author   Shane Spoor
 *
 * @date Febuary 21, 2014
 */
int recv_udp (UDPsocket sock, UDPpacket *udp_packet)
{
	if(SDLNet_UDP_Recv(sock, udp_packet) == -1)
	{
		fprintf(stderr, "SDLNet_UDP_Recv: %s\n", SDLNet_GetError());
        set_error(ERR_UDP_RECV_FAIL);
		return -1;
	}
	
	return 0;
}


/**
 * Frees the socket set upon cancellation of the receive thread.
 *
 * Sends data received from the network router pipe to the server.
 * 
 * The thread gets the data from the pipe and determines the protocol (UDP or TCP)
 * to use, then sends the packet on corresponding socket.
 *
 * @param[in] ndata NETWORK_DATA containing a tcp socket, udp socket and a file
 *                  descriptor to the network router send pipe.
 *
 * @return  NULL upon termination
 *
 * @designer Ramzi Chennafi
 * @author   Ramzi Chennafi
 * 
 * @date Febuary 13 2014
 */
void* send_thread_func(void* ndata){

	NDATA snd_data = (NDATA) ndata;

	int protocol = 0;
	uint32_t type = 0;
	void * data;
	uint64_t error = 1;

	while(1){	
    	data = grab_send_packet(&type, snd_data->read_pipe);
    	if(data == NULL){
            write(send_failure_fd, &error, sizeof(uint64_t));
            break;
		}
		
		protocol = get_protocol(type);
		if(protocol == TCP)
		{
			if(send_tcp(&type, snd_data->tcp_sock, sizeof(uint32_t)) == -1)
            {
                fprintf(stderr, "Failed to send packet type over TCP.\n");
                write(send_failure_fd, &error, sizeof(uint64_t));
                break;
            }
			if(send_tcp(data, snd_data->tcp_sock, packet_sizes[type - 1]) == -1)
            {
                fprintf(stderr, "Failed to send packet over TCP.\n");
                write(send_failure_fd, &error, sizeof(uint64_t));
                break;
            }
		}
		else if(protocol == UDP)
		{
			if(send_udp(data, &type, snd_data->udp_sock, packet_sizes[type - 1] + sizeof(uint32_t)) == -1)
            {
                fprintf(stderr, "Failed to send packet over UDP.\n");
                write(send_failure_fd, &error, sizeof(uint64_t));
                break;
            }
		}
	}

    free(data);
	return NULL;
}

/**
 * Sends the packet data over the established TCP connection.
 *
 * @param[in] data Pointer to the data packet to send over TCP.
 * @param[in] sock The socket on which to send the data.
 * @param[in] size The size of the packet being sent.
 *
 * @return <ul>
 *              <li>Returns 0 on success.</li>
 *				<li>Returns -1 if there's an error on send.</li>
 *          </ul> 
 *
 * @designer Ramzi Chennafi
 * @author   Ramzi Chennafi
 *
 * @date January 20, 2014
 */
int send_tcp(void * data, TCPsocket sock, uint32_t size){

	int result=SDLNet_TCP_Send(sock, data, size);
	if(result <= 0) {
    	fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
        set_error(ERR_TCP_SEND_FAIL);
    	return -1;
	}

	return 0;
}

/**
 * Sends the specified data over a UDP socket.
 *
 * Allocates the UDP packet, sends it, and frees the packet upon completion.
 * If sending the packet was unsuccessful, the function prints an error message.
 *
 * @param[in] data Pointer to the data packet to send over UDP.
 * @param[in] type The type of packet to send.
 * @param[in] sock The socket on which to send the data.
 * @param[in] size The size of the packet.
 *
 * @return <ul>
 *              <li>Returns 0 on success.</li>
 *				<li>Returns -1 if there's an error on send.</li>
 *         </ul>
 *
 * @designer Ramzi Chennafi
 * @author   Ramzi Chennafi
 *
 * @date Febuary 15, 2014
 */
int send_udp(void * data, uint32_t * type, UDPsocket sock, uint32_t size){

    int numsent;
    if(*type == P_POSUPDATE)
    {
    	*type = P_MIN_POS;
    	data = encapsulate_pos_update((PKT_POS_UPDATE *)data);
		size = packet_sizes[*type - 1] + sizeof(uint32_t);
	}
	else if(*type == G_ALLPOSUPDATE)
	{
		*type = P_MIN_POS_ALL;
		data = encapsulate_all_pos_update((PKT_ALL_POS_UPDATE *)data);
		size = packet_sizes[*type - 1] + sizeof(uint32_t);
	}
    
    UDPpacket *pktdata = alloc_packet(size);
    memcpy(pktdata->data, type, sizeof(uint32_t));
    memcpy(pktdata->data + sizeof(uint32_t), data, size - sizeof(uint32_t));
    pktdata->len = size;
    pktdata->channel = 0;

    numsent=SDLNet_UDP_Send(sock, pktdata->channel, pktdata);
    if(numsent < 0) {
        fprintf(stderr,"SDLNet_UDP_Send: %s\n", SDLNet_GetError());
        return -1;
    }

    SDLNet_FreePacket(pktdata);
    return 0;
}


/**
 * Grabs the first packet on the pipe to be sent by the send thread.
 *
 * @param[out] type Receives the type of packet to send.
 * @param[in]  fd   The read descriptor for the pipe between network router/send thread.
 * @param[out] ret  If the function is successful, this holds 1; otherwise, it will hold -1.
 *
 * @return The packet on success or NULL on failure.
 *
 * @designer Ramzi Chennafi
 * @author   Ramzi Chennafi
 *
 * @date Febuary 20 2014
 */
void *grab_send_packet(uint32_t *type, int fd){

	*type = read_type(fd);
	if(*type >= 90)
    {
        set_error(ERR_IPC_FAIL);
		return NULL;
	}

	uint32_t size = packet_sizes[*type - 1];

	void * data = read_packet(fd, size); // reads data
    if(data == NULL)
    {
        set_error(ERR_IPC_FAIL);
        return NULL;
    }

	return data;
}

/**
 * Creates a UDP packet of size @a size.
 *
 * If the function fails, it prints an error message before returning. 
 *
 * @param[in] size The size of the packet to create.
 *
 * @return A UDPpacket pointer on success or NULL on failure.
 *
 * @designer Ramzi Chennafi
 * @author Ramzi Chennafi
 *
 * @date Febuary 15, 2014
 */
UDPpacket *alloc_packet(uint32_t size){

	UDPpacket *pktdata = SDLNet_AllocPacket(size);

	if(!pktdata) {
	    fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
	    return NULL;
	}

	return pktdata;
}
/**
 * Creates an IPaddress struct holding the IP address and port information for the SDL network functions
 *
 * @param[out] ip_addr        Pointer to the IPaddress struct which will receive the address information.
 * @param[in]  port           The port to use for sending/receiving on the socket.
 * @param[in]  host_ip_string The hostname or IP address (in dotted string form) to resolve.
 *
 * @return -1 on failure, or 0 on success.
 *
 * @designer Shane Spoor
 * @author Shane Spoor
 *
 * @date Febuary 15, 2014
 */
int resolve_host(IPaddress *ip_addr, const uint16_t port, const char *host_ip_string)
{
    if(SDLNet_ResolveHost(ip_addr, host_ip_string, port) == -1)
    {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return -1;
    }
    return 0;
}

/**
 * Creates an IPaddress struct holding the IP address and port information for the SDL network functions.
 *
 * @param[in] num_sockets The number of sockets to be added to the set.
 * @param[in] ...         A list of size @a num_sockets of TCPsocket and UDPsocket structs 
 *                        to add to the set.
 *
 * @return An SDLNet_SocketSet pointer on success, or NULL on failure.
 *
 * @designer Shane Spoor
 * @author Shane Spoor
 *
 * @date Febuary 15, 2014
 */
SDLNet_SocketSet make_socket_set(int num_sockets, ...)
{
    int i;
	va_list socket_list; 	
	SDLNet_SocketSet set = SDLNet_AllocSocketSet(num_sockets);

	if(!set)
	{
		fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		return NULL;
	}
	
	va_start(socket_list, num_sockets);
	for(i = 0; i < num_sockets; i++)
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

/**
 * Runs select to determine whether the sockets have data to receive.
 *
 * @param[in] set The set to monitor for activity.
 *
 * @return The number of sockets ready on success, or -1 on failure.
 *
 * @designer Shane Spoor
 * @author Shane Spoor
 *
 * @date Febuary 21, 2014
 */
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

/**
 * Grabs the correct protocol for the specified packet.
 *
 * @param[in] type The type of packet to get the protocol for.
 *
 * @return The correct protocol for the specified packet type.
 *
 * @designer Ramzi Chennafi
 * @author   Ramzi Chennafi
 *
 * @date Febuary 21, 2014
 */
int get_protocol(uint32_t type)
{
	int protocol;

	switch(type)
	{
		case P_NAME:
		case P_CONNECT:
		case G_STATUS:
		case P_CHAT:
		case P_READY_STAT:
		case P_SPECIAL_TILE:
		case P_UNDEF:
		case P_KEEPALIVE:
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

/**
 * Removes sockets from set and closes open sockets.
 *
 * If removing either or both of the sockets fails, the function prints
 * an error message.
 *
 * Revisions:
 *   -# Shane Spoor - March 14th - Function does not close sockets, just deletes set
 *
 * @param[in] set     The set to free.
 * @param[in] tcpsock The TCP socket to close.
 * @param[in] udpsock The UDP socket to close.
 *
 * @designer Ramzi Chennafi
 * @author   Ramzi Chennafi
 *
 * @date Febuary 21, 2014
 */
void free_sockset(SDLNet_SocketSet set, TCPsocket tcpsock, UDPsocket udpsock)
{
	int numused;

	numused=SDLNet_UDP_DelSocket(set,udpsock);
	if(numused==-1) {
	    fprintf(stderr, "SDLNet_DelSocket: %s\n", SDLNet_GetError());
	}
	numused=SDLNet_TCP_DelSocket(set,tcpsock);
	if(numused==-1) {
	    fprintf(stderr, "SDLNet_DelSocket: %s\n", SDLNet_GetError());
	}
}

/**
 * Sets cnt_errno to the specified value.
 *
 * The function uses a semaphore to ensure thread safety. If a thread can't acquire
 * the semaphore to write to cnt_errno, it does not block; any error reported this
 * way will require the network threads to shut down, and all errors will be logged
 * regardless.
 *
 * This behaviour may change in the future (by creating a linked list of errors, for
 * example).
 *
 * @param[in] error The error number to store in cnt_errno.
 *
 * @designer Shane Spoor
 * @author   Shane Spoor
 *
 * @date March 12, 2014
 */
void set_error(int error)
{
    int ret = sem_trywait(&err_sem);
    if(ret != -1) // If we got the semaphore
    {
        cnt_errno = error;
        sem_post(&err_sem);
    }
}

/**
 * Retrieves the error string for the current value of cnt_errno.
 *
 * The function uses a semaphore to ensure thread safety and will block if another
 * thread owns the semaphore.
 *
 * The function may be extended to return a list of errors.
 *
 * @return The error string corresponding to the value of cnt_errno.
 *
 * @designer Shane Spoor
 * @author   Shane Spoor
 *
 * @date March 12, 2014
 */
const char *get_error_string()
{
    int err;
    static const char *error_strings[] = {
        "Could not open the connection.\n",
        "The server closed the connection.\n",
        "Failed to receive TCP data.\n",
        "Failed to receive a UDP packet.\n",
        "Failed to send TCP data.\n",
        "Failed to send a UDP packet.\n",
        "Received corrupted data.\n",
        "The remote host could not be resolved. Ensure the host name or IP address is valid.\n",
        "The program could not allocate enough memory.\n",
        "IPC failure: could not create or write to IPC, or read invalid data.\n",
        "Could not acquire a semaphore.\n",
        "Could not allocate a socket set.\n",
        "Network router thread failed to initialise.\n",
        "Error reading the socket set.\n"
    };

    if(sem_wait(&err_sem) == -1)
        return error_strings[ERR_NO_SEM - 1];
    else
    {
        err = cnt_errno;
        sem_post(&err_sem);
        if(!err)
            return NULL;

        err -= 1;
        return error_strings[err];
    }
}

