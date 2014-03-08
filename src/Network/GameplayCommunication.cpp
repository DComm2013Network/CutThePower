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

#include <cstring>
#include <pthread.h>
#include "PipeUtils.h"
#include "GameplayCommunication.h"
#include "Packets.h" /* extern packet_sizes[] */
#include "NetworkRouter.h"
uint32_t packet_sizes[NUM_PACKETS] = {
	sizeof(PKT_PLAYER_NAME),
	sizeof(PKT_PLAYER_CONNECT),
	sizeof(PKT_GAME_STATUS),
	sizeof(PKT_SND_CHAT),
	sizeof(struct pkt05),
	sizeof(struct pkt06),
	0,
	sizeof(PKT_OBJECTIVE_STATUS),
	0,
    sizeof(PKT_POS_UPDATE),
    sizeof(PKT_ALL_POS_UPDATE),
	sizeof(PKT_FLOOR_MOVE_REQUEST),
	sizeof(PKT_FLOOR_MOVE)
};

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
uint32_t read_type(int fd)
{
    uint32_t type;
    int read_bytes;

    if((read_bytes = read_pipe(fd, &type, sizeof(uint32_t))) <= 0)
    {
        if(read_bytes == 0)
        {
            perror("No data on pipe: read_type(int fd)\n");
            return 99;
        }
        perror("Error while reading from pipe: read_type(int fd)\n");
        return 98; //error .. check error
    }

    return type;
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
 * INTERFACE:   void *read_packet(int fd, int size)
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
 *--------------------------------- -------------------------------------------------------*/
void* read_packet(int fd, uint32_t size)
{
    void *temp = malloc(size);
	int read_bytes;

    if((read_bytes = read_pipe(fd, temp, size)) == -1){
        perror("Error on reading pipe : read_packet(int, uint32_t)\n");
        return NULL;
    }

    if(read_bytes == 0){
        perror("Nothing on pipe to read: read_packet(int, uint32_t)\n");
        return NULL;
    }

    return temp; 
}
/*------------------------------------------------------------------------------------------
 * FUNCTION:    iinit_client_network
 *
 * DATE:        March 6, 2014
 *
 * REVISIONS:   (Date and Description)
 *
 * DESIGNER:    Ramzi Chennafi
 *
 * PROGRAMMER:  Ramzi Chennafi
 *
 * INTERFACE:  void init_client_network(int send_router_fd[2], int rcv_router_fd[2])
 *                  send_router_fd : set of pipes created for passing data to the network router
 *                  rcv_router_fd : set of pipes created for grabing data from the network router
 *
 * RETURNS:    nothing
 *
 * NOTES:
 * Called by the main function of the game. Intializes the client network component of the game
 * Requires 2 pipes as arguments, these will be passed for communication to network router. The 
 * read descriptor of rcv_router_fd will be passed to the update system by the game, while the 
 * write descriptor of the send_router_fd will be passed to the send_system.
 *----------------------------------------------------------------------------------------*/
void init_client_network(int send_router_fd[2], int rcv_router_fd[2])
{
    pthread_t thread;
    NETWORK_DATA * ndata = (NETWORK_DATA*) malloc(sizeof(NETWORK_DATA));
    
    ndata->read_pipe = send_router_fd[READ_END];
    ndata->write_pipe = rcv_router_fd[WRITE_END];

    pthread_create(&thread, NULL, networkRouter, (void *)ndata);
    pthread_detach(thread);
}
/*------------------------------------------------------------------------------------------
 * FUNCTION:    write_packet
 *
 * DATE:        February 21, 2014
 *
 * REVISIONS:   (Date and Description)
 *
 * DESIGNER:    Shane Spoor
 *
 * PROGRAMMER:  Shane Spoor
 *
 * INTERFACE:   int write_packet(int fd, int packet_type, size_t packet_size, void *packet)
 *                  int write_fd        - The write end of a pipe (to gameplay or network module).
 *                  int packet_type     - The type of packet written, as defined in Packets.h
 *					void *packet		- A pointer to the packet structure.
 *
 * RETURNS:     0 on successful write, or -1 if the write failed.
 *
 * NOTES:
 * Writes the packet type, packet size, and the packet itself to the pipe specified by fd.
 * This function should be used in conjunction with read_data_size and read_packet to transfer
 * gameplay information between the gameplay and network client modules.
 *
 *----------------------------------------------------------------------------------------*/
int write_packet(int write_fd, uint32_t packet_type, void *packet)
{   
    int temp;
    if ((temp = write_pipe(write_fd, &packet_type, sizeof(packet_type))) <= 0)
    {
        perror("Failed to write packet: write_pipe");
        return -1;
    }
	
    if((temp = write_pipe(write_fd, packet, packet_sizes[packet_type - 1])) <= 0)
	{
		perror("Failed to write packet: write_pipe");
		return -1;
	}

	return 0; 
}

/*------------------------------------------------------------------------------------------
 * FUNCTION:    update_data
 *
 * DATE:        Febuary 14 2014
 *
 * REVISIONS:   None
 *
 * DESIGNER:    Ramzi Chennafi
 *
 * PROGRAMMER:  Ramzi Chennafi
 *
 * INTERFACE:   int read_data(void* packet, int fd)
 *
 *                    -2 on packet read error
 *                    0 on empty read pipe
 *                    type of packet on success
 *                  
 *
 * NOTES:
 *
 *  Function called by gameplay to update the game to the latest received packet.
 *  Takes a pointer to a malloced packet structure of MAX_PACKET_SIZE and a file 
 *  descriptor to the pipe to read. Returns the type on success.
 *
 *----------------------------------------------------------------------------------------*/
void *read_data(int fd, uint32_t *type){

    int read_bytes;
    void *packet;
    *type = read_type(fd);
    if(*type >= 90){
        perror("Failed to read packet type from pipe: read_data\n");
        return NULL;
    }

    if((packet = read_packet(fd, packet_sizes[*type - 1])) == NULL){
        perror("Failed to read packet : read_data\n");
        return NULL;
    }

    return packet;
}

