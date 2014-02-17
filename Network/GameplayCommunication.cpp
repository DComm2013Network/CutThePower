/*------------------------------------------------------------------------------------------
 * SOURCE FILE: GameplayCommunication.cpp
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
 * This file contains all methods responsible for communication with the gameplay module.
 *
 *----------------------------------------------------------------------------------------*/

#include "GameplayCommunication.h"
#include "Functions.h"
#include "Packet.h"

/*------------------------------------------------------------------------------------------
 * FUNCTION:    read_size_of_data
 *
 * DATE:        February 15, 2014
 *
 * REVISIONS:   (Date and Description)
 *
 * DESIGNER:    Abhishek Bhardwaj
 *
 * PROGRAMMER:  Abhishek Bhardwaj
 *
 * INTERFACE:   int read_size_of_data(int fd)
 *                  int fd              - file descriptor to read from 
 *
 * RETURNS:     int
 *                  [size of data]      -   On successful completion, the size of the following
 *                                          data struct is returned.
 *
 *                  0                   -   end of file .. nothing read
 *
 *                  -1                  -   On error and errno will be set to 
 *                                          indicate the type of error. [SET BY read_pipe]
 *
 * NOTES:
 *
 * A function for reading the size of the following data struct in the pipe.
 *
 *----------------------------------------------------------------------------------------*/
int read_size_of_data(int fd)
{
    int size, read_bytes;

    if( (read_bytes = read_pipe(fd, &size, sizeof(int))) < 0)
    {
        return -1; //error .. check error
    }

    if(read_bytes == 0)
    {
        return 0; //end of file .. nothing in pipe
    }

    return size;
}

/*------------------------------------------------------------------------------------------
 * FUNCTION:    read_packet
 *
 * DATE:        February 15, 2014
 *
 * REVISIONS:   (Date and Description)
 *
 * DESIGNER:    Abhishek Bhardwaj
 *
 * PROGRAMMER:  Abhishek Bhardwaj
 *
 * INTERFACE:   int read_packet(int fd, int size)
 *                  int fd              - file descriptor to read from
 *                  int size            - size of data to be scanned from the fd
 *
 * RETURNS:     int
 *                  [packet]            -   On successful completion, a packet is returned.
 *
 *                  0                   -   end of file .. nothing read
 *
 *                  -1                  -   On error and errno will be set to 
 *                                          indicate the type of error. [SET BY read_pipe]
 *
 *                  -2                  -   packet id not found.
 *
 * NOTES:
 *
 * A function for reading the size of the following data struct in the pipe.
 *
 *----------------------------------------------------------------------------------------*/
void* read_packet(int fd, int size)
{
    void* temp = (void*) malloc(size);

    if( (read_bytes = read_pipe(fd, &temp, size)) < 0)
    {
        return NULL; //error .. check error
    }

    if(read_bytes == 0)
    {
        return NULL; //end of file .. nothing in pipe
    }

    return temp; //packet id not found .. can also return the scanned data instead
}