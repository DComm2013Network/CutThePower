/**
 * Description
 * @file NetworkRouter.cpp
 */

/*------------------------------------------------------------------------------------------
 * SOURCE FILE: NetworkRouter.cpp
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
 * DESIGNER:    Abhishek Bhardwaj
 *
 * PROGRAMMER:  Abhishek Bhardwaj
 *
 * NOTES:
 * 
 * This file contains ..
 *
 *----------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <SDL2/SDL_net.h>

#include "NetworkRouter.h"
#include "GameplayCommunication.h"
#include "ServerCommunication.h"
#include "PipeUtils.h"
#include "Packets.h"

extern int game_net_signalfd, game_net_lockfd;
extern uint32_t packet_sizes[NUM_PACKETS];

/*------------------------------------------------------------------------------------------
 * FUNCTION:    void networkRouter()
 *
 * DATE:        February 15, 2014
 *
 * REVISIONS:   (Date and Description)
 *
 * DESIGNER:    Abhishek Bhardwaj
 *
 * PROGRAMMER:  Abhishek Bhardwaj
 *
 * INTERFACE:   void networkRouter(int pipe[2])
 *                  int pipe[2]     -   The pipe descriptors to be used for 2-way
 *                                      communication with the gameplay module.
 *
 * RETURNS:     void
 *
 * NOTES:
 *
 * This is the main client-side network function that the Gameplay module will call inside
 * a new thread. 
 * 
 * This function will basically read data from the Gameplay module using [some
 * sort of IPC mechanism].
 *
 *----------------------------------------------------------------------------------------*/
void *networkRouter(void *args)
{
    int 		sendfd[2];
    int 		recvfd[2];
    fd_set 		listen_fds;
    fd_set		active;
    int 		max_fd;
    uint32_t 	type;
    uint64_t	timestamp, cached_timestamps[NUM_PACKETS] = {0}, sem_buf;
    void 		*packet, *cached_packets[NUM_PACKETS] = {0};
    pthread_t 	thread_send;
    pthread_t 	thread_receive;
    PDATA 		gameplay = (PDATA)args;
    
    NDATA 		send = (NDATA) malloc(sizeof(WNETWORK_DATA));
    NDATA 		receive = (NDATA) malloc(sizeof(WNETWORK_DATA));

    if(init_router(&max_fd, send, receive, gameplay, sendfd, recvfd, &thread_receive, &thread_send) == -1)
    	return NULL;

    FD_ZERO(&listen_fds);
    FD_SET(recvfd[READ_END], &listen_fds);
    FD_SET(gameplay->read_pipe, &listen_fds);
    FD_SET(game_net_signalfd, &listen_fds);

    while(1)
    {
    	int ret;
        int temptype, temptype2;
        active = listen_fds;
        ret = select(max_fd + 1, &active, NULL, NULL, NULL);
        /*if(ret < 0) 	Log an error */

        if(ret && FD_ISSET(recvfd[READ_END], &active))
        {
        	packet = read_data(recvfd[READ_END], &type);
        	timestamp = read(recvfd[READ_END], &timestamp, sizeof(timestamp));
			write_packet(gameplay->write_pipe, type, packet);
            write_pipe(gameplay->write_pipe, &timestamp, sizeof(timestamp));
			--ret;
        }
        if(ret && FD_ISSET(gameplay->read_pipe, &active))
        {

            // temptype = read_type(gameplay->read_pipe);
            // temptype2 = read_type(gameplay->read_pipe);
        	packet = read_data(gameplay->read_pipe, &type);
        	//timestamp = read(recvfd[READ_END], &timestamp, sizeof(timestamp));
			write_packet(sendfd[WRITE_END], type, packet);
			--ret;
        }
        if(ret && FD_ISSET(game_net_signalfd, &active))
        {
        	uint32_t num_changed;
        	unsigned changed_mask = 0;

        	read(game_net_signalfd, &sem_buf, sizeof(uint64_t)); /* Decrease the semaphore to 0 */
        	num_changed = determine_changed(cached_packets, &changed_mask);
        	write(gameplay->write_pipe, &num_changed, sizeof(num_changed));

        	for(uint32_t i = 0; i < NUM_PACKETS; ++i)
        	{
        		if(changed_mask & (1 << i))
        		{
        			write(gameplay->write_pipe, &i, sizeof(i));
        			write(gameplay->write_pipe, cached_packets[i], packet_sizes[i]);
        		}
        	}
        	write(game_net_lockfd, &sem_buf, sizeof(sem_buf)); /* Gameplay can now read from the pipe */
		}
        
    }
    return NULL;
}

/**
 * Creates and dispatches a new thread.
 *
 * The thread runs independently of the thread that distpaches it. The thread handle is
 * stored in @a handle.
 * 
 * @param[in]  function	Function which the new thread will run.
 * @param[in]  params	The parameter(s) to pass to the thread function.
 * @param[out] handle	Stores a handle to the thread (for sending signals etc).
 * @return 0 on success, or -1 on failure.
 *
 * @designer Shane Spoor
 * @author	 Shane Spoor
 */
int dispatch_thread(void *(*function)(void *), void *params, pthread_t *handle)
{
	if(pthread_create(handle, NULL, function, params) == -1)
	{
		perror("dispatch_thread");
		return -1;
	}

	pthread_detach(*handle);
	return 0;
}

/**
 * Determines which packets have new data.
 *
 * 
 * @param packets The array containing all currently cached packets.
 * @param changed A mask indicating which packets have new data.
 * @return The number of packets with new data.
 *
 * @author 		Shane Spoor
 * @designer 	Shane Spoor
 */
uint32_t determine_changed(void **packets, unsigned *changed)
{
	size_t i;
	uint32_t nchanged = 0;
	
	for(i = 0; i < NUM_PACKETS; ++i)
	{
		if(packets[i])
		{
			*changed |= 1 << i;
			++nchanged;
		}
	}
	
	return nchanged;
}


/**
 * Initialises the variables used in the router thread or the send/receive threads.
 *
 * @param[out] max_fd         The highest value of the gameplay write file descriptor, the receive
 *                            thread read file descriptor, and the semaphore file descriptor (for select()).
 * @param[out] send           Pointer to a struct to contain the data passed to the send thread.
 * @param[out] receive        Pointer to a struct to contain the data passed to the receive thread.
 * @param[out] gameplay       Pointer to a struct containing the objects necessary for communication with gameplay.
 * @param[out] fd             An integer array to hold two ends of a pipe.
 * @param[out] thread_receive Holds a handle to the receive thread.
 * @param[out] thread_send    Holds a handle to the send thread.
 * @return 0 on success, or -1 on failure.
 *
 * @author   Shane Spoor
 * @designer Shane Spoor, Abhishek Bhardwaj
 */
int init_router(int *max_fd, NDATA send, NDATA receive, PDATA gameplay, int sendfd[2], 
				int recvfd[2], pthread_t *thread_receive, pthread_t *thread_send)
{
    IPaddress ipaddr, udpaddr;
	TCPsocket tcp_sock;
	UDPsocket udp_sock;
    int channel;
	               
	create_pipe(sendfd);
	create_pipe(recvfd);
	
    *max_fd = recvfd[READ_END] > gameplay->read_pipe ? recvfd[READ_END] : gameplay->read_pipe;
    resolve_host(&ipaddr, TCP_PORT, "192.168.0.22");
    resolve_host(&udpaddr, UDP_PORT, "192.168.0.22");
    
    tcp_sock = SDLNet_TCP_Open(&ipaddr);
    if(!tcp_sock) {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        exit(2);
    }

    udp_sock = SDLNet_UDP_Open(UDP_PORT);
    if(!udp_sock) {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        exit(2);
    }

    channel = SDLNet_UDP_Bind(udp_sock, 0, &udpaddr);
    if(channel ==   -1) {
        printf("SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
    }

    send->tcp_sock = tcp_sock;
    send->udp_sock = udp_sock;

    receive->tcp_sock = tcp_sock;
    receive->udp_sock = udp_sock;

    send->read_pipe = sendfd[READ_END];
    send->write_pipe = sendfd[WRITE_END];

    receive->read_pipe = recvfd[READ_END];
    receive->write_pipe = recvfd[WRITE_END];

	if(dispatch_thread(send_thread_func, (void *)send, thread_send) == -1 ||
        dispatch_thread(recv_thread_func, (void *)receive, thread_receive) == -1)
	   return -1;
	
	return 0;
}

/*

The gameplay module will basically create the pipe and call the networkRouter() function
with the newly created pipe.

Wrapper functions for creating the pipe, reading and writing to/from the pipe are present
in the Functions.cpp file.

All the functions that the gameplay side will need to use to read/write to the pipe are
present in GameplayCommunication.cpp and Functions.cpp

*/
