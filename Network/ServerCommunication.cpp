/*------------------------------------------------------------------------------------------
 * SOURCE FILE: ServerCommunication.cpp
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
 * DESIGNER:    
 *
 * PROGRAMMER:  
 *
 * NOTES:
 * 
 * This file contains all methods responsible for communication with the server.
 *
 *----------------------------------------------------------------------------------------*/
#include "ServerCommunication.h"

/*------------------------------------------------------------------------------------------
 * FUNCTION:    recv_thread_func
 *
 * DATE:        February 16, 2014
 *
 * REVISIONS:   (Date and Description)
 *
 * DESIGNER:    Shane Spoor
 *
 * PROGRAMMER:  Shane Spoor
 *
 * INTERFACE:   void recv_thread_func(const int router_pipe_writefd, const sockaddr_in
 *									*server_info)
 *                  int router_pipe_writefd - 		Identifies the write end of a pipe to the
 *													network router thread.
 *					const char *host_ip_string -	A host name or IP address string to be
 *													resolved.
 *
 * RETURNS:     void
 *
 * NOTES:
 *
 * This thread will continuously monitor a TCP and UDP socket which will receive data from
 * the server. Upon receiving data, the thread writes to the pipe connected to the network
 * router thread.
 *
 *----------------------------------------------------------------------------------------*/
 void recv_thread_func(int router_pipe_writefd, const char *host_ip_string)
 {
 	
 	while(1)
 	{
 		
 	}
 }
 
