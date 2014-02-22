#ifndef SERVER_COMMUNICATION_H
#define SERVER_COMMUNICATION_H

#include <thread>
#include <SDL_net.h>
#include <unistd.h>
#include <SDL2.h>
#include <stdio.h>

void recv_thread_func(int router_pipe_writefd);
void send_thread_func(int router_pipe_readfd);
void init_send_data(packet pkt);
int send_tcp(char * data);
int send_udp(char * data);
TCPsocket intiate_tcp();
UDPsocket intiate_udp();
UDPpacket alloc_packet(char * data);

#endif
