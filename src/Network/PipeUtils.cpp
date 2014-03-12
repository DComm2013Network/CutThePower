/** @ingroup Network */
/** @{ */

/**
 * This file contains all the utility functions related to IO with IPC pipes.
 *
 * @file PipeUtils.cpp
 */

/** @} */
#include <unistd.h>
#include <fcntl.h>
#include "PipeUtils.h"

/**
 * A wrapper function for creating a set of non-blocking pipes.
 * 
 * @param[in]   pipe_ends  An identifier for the pipe. (pipe_ends[0] - for reading, pipe_ends[1] - for writing)
 *
 * @return  <ul>
 *              <li>Returns 0 on successful completion.</li>
 *              <li>Returns -1 on error and errno is set to indicate the type of error.</li>
 *          </ul>
 *
 * @designer    Abhishek Bhardwaj
 * @author      Abhishek Bhardwaj
 */
int create_pipe(int pipe_ends[2])
{
	int ret = pipe(pipe_ends);
	int flags[2];
	if(ret < 0)
		return ret;
	
	if((flags[0] = fcntl(pipe_ends[0], F_GETFD, 0)) == -1 || (flags[1] = fcntl(pipe_ends[1], F_SETFD, O_NONBLOCK)) == -1)
		return -1;
	
	if(fcntl(pipe_ends[0], F_SETFD, flags[0] | O_NONBLOCK) == -1 || fcntl(pipe_ends[1], F_SETFD, flags[1] | O_NONBLOCK) == -1)
		return -1;

    return 0;
}

/**
 * A wrapper function for writing to a pipe. 
 * 
 * This basically "writes up to count bytes from the buffer pointed buf to the file 
 * referred to by the file descriptor fd".
 * 
 * @param[in]   fd      File descriptor to write to.
 * @param[in]   buf     A pointer to the buffer with the data.
 * @param[in]   count   The number of bytes to write to the pipe fd from the buffer.
 *
 * @return  <ul>
 *              <li>Returns [number of bytes] on successful completion.</li>
 *              <li>Returns 0 to indicate that nothing was written.</li>
 *              <li>Returns -1 on error and errno is set to indicate the type of error.</li>
 *          </ul>
 *
 * @designer    Abhishek Bhardwaj
 * @author      Abhishek Bhardwaj
 */
int write_pipe(int fd, const void *buf, size_t count)
{
    return (write(fd, buf, count));
}

/**
 * A wrapper function for reading to a pipe. 
 * 
 * This basically "reads up to count bytes from file descriptor fd into the buffer 
 * starting at buf".
 *
 * NOTE: It's not an error if the number of bytes returned is smaller than the number of bytes requested.
 * 
 * @param[in]   fd      File descriptor to read from.
 * @param[in]   buf     A pointer to the buffer with the data.
 * @param[in]   count   The number of bytes to read from the pipe fd into the buffer.
 *
 * @return  <ul>
 *              <li>Returns [number of bytes] on successful completion.</li>
 *              <li>Returns 0 to indicate end of file or that nothing was read.</li>
 *              <li>Returns -1 on error and errno is set to indicate the type of error.</li>
 *          </ul>
 *
 * @designer    Abhishek Bhardwaj
 * @author      Abhishek Bhardwaj
 */
int read_pipe(int fd, void *buf, size_t count)
{
    return (read(fd, buf, count));
}
