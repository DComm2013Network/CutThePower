#ifndef SERVER_COMMUNICATION_H
#define SERVER_COMMUNICATION_H

#include <pthread.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstdio>
#include <cstdarg>
#include <cerrno>
#include <ctime>

#define INFINITE_TIMEOUT -1   /**< Tells SDL to wait for an "infinite" (49 day) timeout */
#define MAX_TCP_RECV     2944 /**< Max TCP "packet" size from server to client (PKT_GAME_STATUS)*/
#define MAX_UDP_RECV     644  /**< Max UDP packet size from server to client (PKT_ALL_POS_UPDATE)*/	


#define ERR_NO_CONN       1  /**< The TCP connection could not be opened. */
#define ERR_CONN_CLOSED   2  /**< The TCP connection was closed. */
#define ERR_TCP_RECV_FAIL 3  /**< Receiving data from a TCP socket failed. */
#define ERR_UDP_RECV_FAIL 4  /**< Receiving data from a UDP socket failed. */
#define ERR_TCP_SEND_FAIL 5  /**< Sending over a TCP socket failed. */
#define ERR_UDP_SEND_FAIL 6  /**< Sending over a UDP socket failed. */
#define ERR_CORRUPTED     7  /**< A packet contains corrupted data. */
#define ERR_NO_HOST       8  /**< The host couldn't be resolved. */
#define ERR_NO_MEM        9  /**< malloc failed. */
#define ERR_IPC_FAIL      10 /**< Writing to a pipe or other form of IPC failed. */
#define ERR_NO_SEM        11 /**< Couldn't acquire a semaphore. */
#define ERR_SOCK_RM       12 /**< Couldn't remove the socket from an SDL socket set. */
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

/* Error reporting functions */
void write_error(int error);
const char *get_error_string();

#endif
