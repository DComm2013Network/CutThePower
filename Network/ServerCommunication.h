#ifndef SERVER_COMMUNICATION_H
#define SERVER_COMMUNICATION_H

#include <thread>
#include <SDL_net.h>
#include <unistd.h>
#include <SDL.h>
#include <stdio.h>
#include "Packets.h"

void recv_thread_func(int router_pipe_writefd);
void send_thread_func(int router_pipe_readfd);
packet * frame_data(int type, void* data);
void init_send_data(packet * pkt);
int send_tcp(char * data);
int send_udp(char * data);
TCPsocket initiate_tcp();
UDPsocket initiate_udp();
UDPpacket * alloc_packet(char * data);

#endif
