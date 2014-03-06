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
    int sendfd[2];
    int recvfd[2];
    sem_t send_sem, recv_sem;
    fd_set listen_fds;
    int max_fd;
    uint32_t type, protocol;
    void *packet;
    pthread_t thread_send;
    pthread_t thread_receive;
    NETWORK_DATA * ndata = (NETWORK_DATA*) args;
    
    int optval, optlen;

    create_pipe(sendfd);
    create_pipe(recvfd);

    NDATA send = (NDATA) malloc(sizeof(WNETWORK_DATA));
    NDATA receive = (NDATA) malloc(sizeof(WNETWORK_DATA));

    IPaddress ipaddr;
    TCPsocket tcp_sock, tcp_sock2; // sock2 for testing purposes
    UDPsocket udp_sock;

    max_fd = recvfd[0] > ndata->read_pipe ? recvfd[0] : ndata->read_pipe;
    resolve_host(&ipaddr, 5323, "localhost");

    // tcp_sock = SDLNet_TCP_Open(&ipaddr);
    // if(!tcp_sock) {
    //     printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
    //     exit(2);
    // }

    tcp_sock2 = SDLNet_TCP_Open(&ipaddr); //  for loopback, return to tcp_sock when done
    if(!tcp_sock2) {
        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(2);
    }//the fuck it fails, fucking error checking making me waste fuck

    udp_sock = SDLNet_UDP_Open(42338);
    if(!udp_sock) {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        exit(2);
    }
    optval = 1;

    // send->tcp_sock = tcp_sock;
    // send->udp_sock = udp_sock;

    receive->tcp_sock = tcp_sock2; // for loopback, return to tcp_sock when done
    receive->udp_sock = udp_sock;

    sem_init(&send_sem, 0, 1);
    send->read_pipe = sendfd[0];
    send->write_pipe = sendfd[1];
    send->pipesem = send_sem;

    sem_init(&recv_sem, 0, 1);
    receive->read_pipe = recvfd[0];
    receive->write_pipe = recvfd[1];
    receive->pipesem = recv_sem;

    pthread_create(&thread_receive, NULL, recv_thread_func, (void *)&receive);
    pthread_detach(thread_receive);

   // pthread_create(&thread_send, NULL, send_thread_func, (void *)&send);
    //pthread_detach(thread_send);

    while(1)
    {
    	int ret;
        FD_ZERO(&listen_fds);
        FD_SET(recvfd[READ_RECV_THREAD], &listen_fds);
        FD_SET(ndata->read_pipe, &listen_fds);
        
        ret = select(max_fd, &listen_fds, NULL, NULL, NULL);
        /*if(ret < 0) 	Log an error */

        if(FD_ISSET(recvfd[0], &listen_fds))
        {   
        	packet = read_data(recvfd[0], &type);
            if(type >= 90){
                continue;
            }
            write_packet(ndata->write_pipe, type, packet);
        }
        if(FD_ISSET(ndata->read_pipe, &listen_fds))
        {   
        	packet = read_data(ndata->read_pipe, &type);
            if(type >= 90){
                continue;
            }
			write_packet(sendfd[1], type, packet);
            sem_post(&send->pipesem);
        }
    }
    return NULL;
}

/*

The gameplay module will basically create the pipe and call the networkRouter() function
with the newly created pipe.

Wrapper functions for creating the pipe, reading and writing to/from the pipe are present
in the Functions.cpp file.

All the functions that the gameplay side will need to use to read/write to the pipe are
present in GameplayCommunication.cpp and Functions.cpp

*/
