/** @ingroup Network */
/** @{ */
/**
 * Header file for functions related to communication with gameplay.
 *
 * @file GameplayCommunication.h
 */
/** @} */
#ifndef GAMEPLAYCOMMUNICATION_H
#define GAMEPLAYCOMMUNICATION_H

#include <unistd.h> //For *nix specific POSIX calls
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cerrno>

/**
 * A structure of data that will be passed on to child threads.
 *
 * @struct WTHREAD_DATA, *PDATA
 */
typedef struct THREAD_DATA
{
    int read_pipe;  /**< pipe descriptor for reading */
    int write_pipe; /**< pipe descriptor for writing */
    char *ip_address_string; /**< ip address of the server */
} WTHREAD_DATA, *PDATA;

/* Packet reading wrappers */
void *read_data(int fd, uint32_t *type);
uint32_t read_type(int fd);
void *read_packet(int fd, uint32_t size);

/* Packet writing wrapper */
int write_packet(int write_fd, uint32_t packet_type, void *packet);

/* Gameplay wrapper */
int update_data(void* packet, int fd);
void init_client_network(int send_router_fd[2], int rcv_router_fd[2]);

#endif
