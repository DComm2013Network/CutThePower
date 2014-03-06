#ifndef SERVER_COMMUNICATION_H
#define SERVER_COMMUNICATION_H

#include <pthread.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdarg.h>

#define INFINITE_TIMEOUT	-1 															/**< Tells SDL to wait for an "infinite" (49 day) timeout */
#define MAX_TCP_RECV		(MAX_PLAYERS * sizeof(int) * 3) + (MAX_PLAYERS * MAX_NAME)	/**< The maximum size of a game packet sent over TCP from server to client */
#define MAX_UDP_RECV		(MAX_PLAYERS * sizeof(int) * 5) + sizeof(int)				/**< The maximum size of a game packet sent over UDP from server to client */	
#define MAX_SIZE 			

/* Thread functions */
void *recv_thread_func(void *ndata);
void *send_thread_func(void *ndata);

/* Socket send functions */
int send_tcp(char * data, TCPsocket sock);
int send_udp(char * data, UDPsocket sock);
char* grab_send_packet(uint32_t *type, int fd, int * ret);
void* send_thread_func(void* ndata);

/* Socket receive functions; "packets" in this context refer to gameplay packets */
int recv_udp (UDPsocket sock, UDPpacket *udp_packet);
int recv_tcp (TCPsocket sock, void *buf, size_t bufsize);
void *recv_udp_packet(UDPsocket sock, uint32_t *game_packet_type);
void *recv_tcp_packet(TCPsocket sock, uint32_t *game_packet_type);

/* Socket creation and utilities */
TCPsocket initiate_tcp();
UDPsocket initiate_udp(uint16_t port);
UDPpacket *alloc_packet(char * data);
int resolve_host(IPaddress *ip_addr, const uint16_t port, const char *host_ip_string);

/* Socket select functions */
SDLNet_SocketSet make_socket_set(size_t num_sockets, ...);
int check_sockets(SDLNet_SocketSet set);

#endif
