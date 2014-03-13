#ifndef SERVER_COMMUNICATION_H
#define SERVER_COMMUNICATION_H

#include <pthread.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdarg.h>

#define INFINITE_TIMEOUT -1   /**< Tells SDL to wait for an "infinite" (49 day) timeout */
#define MAX_TCP_RECV     2944 /**< Max TCP "packet" size from server to client (PKT_GAME_STATUS)*/
#define MAX_UDP_RECV     644  /**< Max UDP packet size from server to client (PKT_ALL_POS_UPDATE)*/	

#define ERR_STD_LIB     -1 /**< Indicates that a standard library function (e.g. malloc) failed. */
#define ERR_CONN_CLOSED -2 /**< Indicates that the TCP connection was closed. */
#define ERR_RECV_FAILED -3 /**< Indicates that receiving data from a socket failed. */
#define ERR_CORRUPTED   -4 /**< Indicates that a UDP packet contains corrupted data. */
#define INVALID_TYPE    -5
/* Thread functions */
void *recv_thread_func(void *ndata);
void *send_thread_func(void *ndata);

/* Socket send functions */
int send_tcp(void * data, TCPsocket sock, uint32_t type);
int send_udp(void * data, uint32_t * type, UDPsocket sock, uint32_t size);

void* grab_send_packet(uint32_t *type, int fd, int * ret);

/* Socket receive functions */
int recv_udp (UDPsocket sock, UDPpacket *udp_packet);
int recv_tcp (TCPsocket sock, void *buf, size_t bufsize);
void *recv_udp_packet(UDPsocket sock, uint32_t *packet_type, uint64_t *timestamp);
void *recv_tcp_packet(TCPsocket sock, uint32_t *packet_type, uint64_t *timestamp);
int handle_tcp_in(int router_pipe_fd, TCPsocket tcp_sock);
int handle_udp_in(int router_pipe_fd, UDPsocket udp_sock);

int get_protocol(uint32_t type);
/* Socket creation and utilities */
void close_connections(SDLNet_SocketSet set, TCPsocket tcpsock, UDPsocket udpsock);
TCPsocket initiate_tcp();
UDPsocket initiate_udp(uint16_t port);
UDPpacket *alloc_packet(uint32_t size);
int resolve_host(IPaddress *ip_addr, const uint16_t port, const char *host_ip_string);

/* Socket select functions */
SDLNet_SocketSet make_socket_set(int num_sockets, ...);
int check_sockets(SDLNet_SocketSet set);

#endif
