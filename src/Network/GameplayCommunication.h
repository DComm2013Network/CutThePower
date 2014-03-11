#ifndef GAMEPLAYCOMMUNICATION_H
#define GAMEPLAYCOMMUNICATION_H

#include <unistd.h> //For *nix specific POSIX calls
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cerrno>

/* Thread structure */
typedef struct THREAD_DATA
{
    int read_pipe;  /* pipe descriptor for reading */
    int write_pipe; /* pipe descriptor for writing */
    char *ip_address_string;
} WTHREAD_DATA, *PDATA;

/* Packet reading wrappers */
void *read_data(int fd, uint32_t *type);
uint32_t read_type(int fd);
void *read_packet(int fd, uint32_t size);

/* Packet writing wrapper */
int write_packet(int write_fd, uint32_t packet_type, void *packet);

/* Gameplay wrapper */
int update_data(void* packet, int fd);

#endif
