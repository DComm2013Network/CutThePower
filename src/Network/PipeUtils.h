/** @ingroup Network */
/** @{ */
/**
 * @file PipeUtils.h
 */
/** @} */
#ifndef PIPE_UTILS_H
#define PIPE_UTILS_H

#include <cstdio>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>

// Function prototypes
int create_pipe(int pipe_ends[2]);
int write_pipe(int fd, const void *buf, size_t count);
int read_pipe(int fd, void *buf, size_t count);

#endif
