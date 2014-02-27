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

#include "PipeUtils.h"
#include "GameplayCommunication.h"
#include "Packets.h" /* extern packet_sizes[] */

uint32_t packet_sizes[13] = {
	sizeof(struct pkt01),
	sizeof(struct pkt02),
	sizeof(struct pkt03),
	sizeof(struct pkt04),
	sizeof(struct pkt05),
	sizeof(struct pkt06),
	0,
	sizeof(struct pkt08),
	0,
	sizeof(struct pkt10),
	sizeof(struct pkt11),
	sizeof(struct pkt12),
	sizeof(struct pkt13)
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

    uint32_t type, read_bytes;

    if( (read_bytes = read_pipe(fd, &type, sizeof(int))) < 0)
    {
        if(read_bytes == 0)
        {
            return 0; //end of file .. nothing in pipe
        }

        return -1; //error .. check error
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
 *----------------------------------------------------------------------------------------*/
void* read_packet(int fd, uint32_t size)
{
    void* temp = (void*) malloc(size);
	int read_bytes;

    if( (read_bytes = read_pipe(fd, &temp, size)) < 0)
    {
        return NULL; /* error .. check error */
    }

    if(read_bytes == 0)
    {
        return NULL; /* end of file .. nothing in pipe */
    }

    return temp; //packet id not found .. can also return the scanned data instead
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
    if (write_pipe(write_fd, &packet_type, sizeof(packet_type)) == -1)
    {
        perror("write_packet: write");
        return -1;
    }
	if (write_pipe(write_fd, packet, packet_sizes[packet_type]) == -1)
	{
		perror("write_packet: write");
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
 * RETURNS:     int : -1 on pipe read error
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

	void *packet;
    
    if((*type = read_type(fd)) <= 0){
        return NULL;
    }
    
    if((packet = read_packet(fd, packet_sizes[*type])) == NULL){
        return (void *)-2;
    }

    return packet;
}
