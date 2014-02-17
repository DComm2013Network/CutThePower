#ifndef GAMEPLAYCOMMUNICATION_H
#define GAMEPLAYCOMMUNICATION_H

#include <unistd.h> //For *nix specific POSIX calls

//Function Prototypes
int read_size_of_data(int fd);
int read_packet(int fd, int size);

#endif