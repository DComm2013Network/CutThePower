#ifndef GAMEPLAYCOMMUNICATION_H
#define GAMEPLAYCOMMUNICATION_H

/* Thread structure */
typedef struct THREAD_DATA
{
    int read_pipe;  /* pipe descriptor for reading */
    int write_pipe; /* pipe descriptor for writing */
} WTHREAD_DATA, *PDATA;

/* Function Prototypes */
int read_size_of_data(int fd);
int read_packet(int fd, int size);

#endif