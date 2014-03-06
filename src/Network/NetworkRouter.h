#ifndef NETWORK_ROUTER_H
#define NETWORK_ROUTER_H

#include <SDL2/SDL_net.h>
#include <semaphore.h>
#include "GameplayCommunication.h"

#define READ_RECV_THREAD	0
#define WRITE_SEND_THREAD 	1

#define TCP_PORT			42337
#define UDP_PORT			42338

#define READ_END			0
#define WRITE_END			1

#define NUM_PACKETS			14

/* Thread structure */
typedef struct NETWORK_DATA
{
    int         read_pipe;      /* pipe descriptor for reading */
    int         write_pipe;     /* pipe descriptor for writing */
    TCPsocket   tcp_sock;       /* TCP socket for server communication */
    UDPsocket   udp_sock;       /* UDP socket for server communication */
    sem_t 		pipesem;
} WNETWORK_DATA, *NDATA;

void *networkRouter(void *args);
int dispatch_thread(void *(*function)(void *), void *params, pthread_t *handle);
uint32_t determine_changed(void **packets, unsigned *changed);
int init_router(int *max_fd, NDATA send, NDATA receive, PDATA gameplay, int sendfd[2],
				int recvfd[2], pthread_t *thread_receive, pthread_t *thread_send);

#endif

