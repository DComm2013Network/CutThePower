#ifndef FUNCTIONS_H
#define FUNCTIONS_H

//Function prototypes
int create_pipe(int pipe[2]);
int write_pipe(int fd, const void *buf, size_t count);
int read_pipe(int fd, void *buf, size_t count);

#endif