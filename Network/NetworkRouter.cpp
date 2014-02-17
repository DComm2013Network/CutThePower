/*------------------------------------------------------------------------------------------
 * SOURCE FILE: NetworkRouter.cpp
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
 * This file contains ..
 *
 *----------------------------------------------------------------------------------------*/

#include "NetworkRouter.h"
#include "Packets.h"

/*------------------------------------------------------------------------------------------
 * FUNCTION:    void networkRouter()
 *
 * DATE:        February 15, 2014
 *
 * REVISIONS:   (Date and Description)
 *
 * DESIGNER:    Abhishek Bhardwaj
 *
 * PROGRAMMER:  Abhishek Bhardwaj
 *
 * INTERFACE:   void networkRouter(int pipe[2])
 *                  int pipe[2]     -   The pipe descriptors to be used for 2-way
 *                                      communication with the gameplay module.
 *
 * RETURNS:     void
 *
 * NOTES:
 *
 * This is the main client-side network function that the Gameplay module will call inside
 * a new thread. 
 * 
 * This function will basically read data from the Gameplay module using [some
 * sort of IPC mechanism].
 *
 *----------------------------------------------------------------------------------------*/
void networkRouter(int pipe[2])
{
    /* while loop that will run forever .. until the thread is alive */
    while(1)
    {

    }
}

/*

The gameplay module will basically create the pipe and call the networkRouter() function
with the newly created pipe.

Wrapper functions for creating the pipe, reading and writing to/from the pipe are present
in the Functions.cpp file.

All the functions that the gameplay side will need to use to read/write to the pipe are
present in GameplayCommunication.cpp and Functions.cpp

*/