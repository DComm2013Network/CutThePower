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
#include <SDL_net.h>

#include "NetworkRouter.h"
#include "GameplayCommunication.h"
#include "ServerCommunication.h"

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
    int fd[2];
    pthread_t thread_send;
    pthread_t thread_receive;
    PDATA gameplay = (PDATA)args;

    NDATA send = (NDATA) malloc(sizeof(NDATA));
    NDATA receive = (NDATA) malloc(sizeof(NDATA));

    IPaddress ipaddr;
    TCPsocket tcp_sock;
    UDPsocket udp_sock;

    resolve_host(&ipaddr, 42337, ip_address_string);

    tcp_sock = SDLNet_TCP_Open(&ipaddr);
    udp_sock = SDLNet_UDP_Open(42338);

    send->tcp_sock = tcp_sock;
    send->udp_sock = udp_sock;

    receive->tcp_sock = tcp_sock;
    receive->udp_sock = udp_sock;

    create_pipe(fd);

    send->read_pipe = fd[0];
    send->write_pipe = fd[1];

    receive->read_pipe = fd[0];
    receive->write_pipe = fd[1];

    pthread_create(&thread_send, NULL, recv_thread_func, (void *)&receive);
    pthread_detach(thread_send);

    pthread_create(&thread_receive, NULL, send_thread_func, (void *)&send);
    pthread_detach(thread_receive);

    while(1)
    {
        
    }

}

/*

The gameplay module will basically create the pipe and call the networkRouter() function
with the newly created pipe.

Wrapper functions for creating the pipe, reading and writing to/from the pipe are present
in the Functions.cpp file.

All the functions that the gameplay side will need to use to read/write to the pipe are
present in GameplayCommunication.cpp and Functions.cpp

*/
