#ifndef GAMEPLAYCOMMUNICATION_H
#define GAMEPLAYCOMMUNICATION_H

#include <unistd.h> //For *nix specific POSIX calls
#include <cstdlib>
#include <cstdio>
#include <cerrno>

/* Packet reading wrappers */
int read_size_of_data(int fd);
void *read_packet(int fd, int size);

/* Packet writing wrapper */
int write_packet(int write_fd, int packet_type, size_t packet_size, void *packet);

#endif
