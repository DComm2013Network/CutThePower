/*------------------------------------------------------------------------------------------
 * SOURCE FILE: Functions.cpp
 *
 * PROGRAM:     [BIG_GAME]
 *
 * FUNCTIONS:
 *                            
 *
 * DATE:        February 15, 2014
 *
 * REVISIONS:   (Date and Description)
 *
 * DESIGNER:    Abhishek Bhardwaj
 *
 * PROGRAMMER:  Abhishek Bhardwaj
 *
 * NOTES:
 * 
 * This file contains all the utility functions that can be re-used in other parts of the
 * program.
 *
 *----------------------------------------------------------------------------------------*/

#include "Functions.h"

/*------------------------------------------------------------------------------------------
 * FUNCTION:    create_pipe
 *
 * DATE:        February 15, 2014
 *
 * REVISIONS:   (Date and Description)
 *
 * DESIGNER:    Abhishek Bhardwaj
 *
 * PROGRAMMER:  Abhishek Bhardwaj
 *
 * INTERFACE:   int create_pipe(int pipe[2])
 *                  int pipe - an identifier for the pipe
 *                      pipe[0] - for reading
 *                      pipe[1] - for writing
 *
 * RETURNS:     int
 *                  0   -   On successful completion
 *                  -1  -   On error and errno will be set to indicate the type of error.
 *
 * NOTES:
 *
 * A wrapper function for creating a pipe.
 *
 *----------------------------------------------------------------------------------------*/
int create_pipe(int pipe[2])
{
    return (pipe(pipe));
}

/*------------------------------------------------------------------------------------------
 * FUNCTION:    write_pipe
 *
 * DATE:        February 15, 2014
 *
 * REVISIONS:   (Date and Description)
 *
 * DESIGNER:    Abhishek Bhardwaj
 *
 * PROGRAMMER:  Abhishek Bhardwaj
 *
 * INTERFACE:   int write_pipe(int fd, const void *buf, size_t count)
 *                  int fd              - file descriptor to write to
 *                  const void *buf     - the buffer with the data
 *                  size_t count        - the number of bytes to write to fd from the buffer  
 *
 * RETURNS:     int
 *                  [number of bytes]   -   On successful completion, the number of bytes
 *                                          written is returned 
 *                                          (0 indicates nothing was written)
 *
 *                  -1                  -   On error and errno will be set to 
 *                                          indicate the type of error.
 *
 * NOTES:
 *
 * A wrapper function for writing to a pipe. 
 * 
 * This basically "writes up to count bytes from the buffer pointed buf to the file 
 * referred to by the file descriptor fd".
 *
 *----------------------------------------------------------------------------------------*/
int write_pipe(int fd, const void *buf, size_t count)
{
    return (write(fd, buf, count));
}

/*------------------------------------------------------------------------------------------
 * FUNCTION:    read_pipe
 *
 * DATE:        February 15, 2014
 *
 * REVISIONS:   (Date and Description)
 *
 * DESIGNER:    Abhishek Bhardwaj
 *
 * PROGRAMMER:  Abhishek Bhardwaj
 *
 * INTERFACE:   int read_pipe(int fd, void *buf, size_t count)
 *                  int fd              - file descriptor to read from
 *                  void *buf           - the buffer to which the data will be written to
 *                  size_t count        - the number of bytes to read from fd into the buffer  
 *
 * RETURNS:     int
 *                  [number of bytes]   -   On successful completion, the number of bytes
 *                                          read is returned (0 indicates end of file) and
 *                                          the file position is read by this number.
 *
 *                                          It's not an error if the number of bytes returned
 *                                          is smaller than the number of bytes requested.
 *
 *                  -1                  -   On error and errno will be set to 
 *                                          indicate the type of error.
 *
 * NOTES:
 *
 * A wrapper function for reading to a pipe. 
 * 
 * This basically "reads up to count bytes from file descriptor fd into the buffer 
 * starting at buf".
 *
 *----------------------------------------------------------------------------------------*/
int read_pipe(int fd, void *buf, size_t count)
{
    return (read(fd, buf, count));
}