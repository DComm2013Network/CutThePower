/** @ingroup Network */
/** @{ */

/**
 * This file contains all the utility functions related to IO with IPC pipes.
 *
 * @file PipeUtils.cpp
 */

/** @} */
#include "PipeUtils.h"

/**
 * A wrapper function for creating a set of non-blocking pipes.
 * 
 * @param[in]   pipe_ends  An identifier for the pipe. (pipe_ends[0] - for reading, pipe_ends[1] - for writing)
 *
 * Revisions: <ul>
 *            <li>Shane Spoor - March 12, 2014 - Removed non-blocking flag.</li>
 *            </ul> 
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
	if(ret < 0)
    {
        fprintf(stderr, "create_pipe (pipe_ends[0] = %d, pipe_ends[1] = %d): %s\n", pipe_ends[0], pipe_ends[1], strerror(errno));
		return ret;
    }

    return 0;
}

/**
 * A wrapper function for writing to a pipe. 
 * 
 * This basically "writes up to count bytes from the buffer pointed buf to the file 
 * referred to by the file descriptor fd".
 *
 * Revisions: <ul>
 *            <li>Shane Spoor - March 12, 2014 - Added error message printing on failure.</li>
 *            </ul> 
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
    int ret = write(fd, buf, count);
    
    if(ret == -1)
        fprintf(stderr, "write_pipe (fd = %d, buf = %p, count = %u): %s\n", fd, buf, (unsigned)count, strerror(errno));        

    return ret;
}

/**
 * A wrapper function for reading to a pipe. 
 * 
 * This basically "reads up to count bytes from file descriptor fd into the buffer 
 * starting at buf".
 *
 * NOTE: It's not an error if the number of bytes returned is smaller than the number of bytes requested.
 *
 * Revisions: <ul>
 *            <li>Shane Spoor - March 12, 2014 - Added error message printing on failure.</li>
 *            </ul> 
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
    int ret = read(fd, buf, count);
    
    if(ret == -1)
        fprintf(stderr, "read_pipe (fd = %d, buf = %p, count = %u): %s\n", fd, buf, (unsigned)count, strerror(errno));        

    return ret;
}
