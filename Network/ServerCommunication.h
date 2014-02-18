#ifndef SERVER_COMMUNICATION_H
#define SERVER_COMMUNICATION_H

#include <thread>
#include <SDL_net.h>
#include <unistd.h>

void recv_thread_func(int router_pipe_writefd);
void send_thread_func(int router_pipe_readfd);

#endif
