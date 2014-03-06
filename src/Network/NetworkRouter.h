#ifndef NETWORK_ROUTER_H
#define NETWORK_ROUTER_H

#include <SDL2/SDL_net.h>
#include <semaphore.h>
#define READ_RECV_THREAD	0
#define WRITE_SEND_THREAD 	1

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

#endif
