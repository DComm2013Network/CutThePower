/** @ingroup Network */
/** @{ */

/**
 * This file contains all methods responsible for communication with the gameplay module.
 *
 * @file GameplayCommunication.cpp
 */

/** @} */
#include <cstring>
#include <pthread.h>
#include "PipeUtils.h"
#include "GameplayCommunication.h"
#include "Packets.h" /* extern packet_sizes[] */
#include "NetworkRouter.h"

uint32_t packet_sizes[NUM_PACKETS] = {
	sizeof(PKT_PLAYER_NAME),         //0
	sizeof(PKT_PLAYER_CONNECT),      //1
	sizeof(PKT_GAME_STATUS),         //2
	sizeof(PKT_SND_CHAT),            //3
	sizeof(pkt05),                   //4
	sizeof(PKT_OBJ_LOC),             //5
	0,                               //6
	sizeof(PKT_OBJECTIVE_STATUS),    //7
	0,                               //8
    sizeof(PKT_POS_UPDATE),          //9
    sizeof(PKT_ALL_POS_UPDATE),      //10
	sizeof(PKT_FLOOR_MOVE_REQUEST),  //11
	sizeof(PKT_FLOOR_MOVE),          //12
    sizeof(PKT_TAGGING)              //13
};

/**
 * A function for reading the type of the following data struct in the pipe.
 * 
 * @param[in]   fd  The file descriptor to read from.
 *
 * @return  <ul>
 *              <li>Returns type of the following data struct on a successful read from the pipe.</li>
 *              <li>Returns 99 if no data is found on the pipe.</li>
 *              <li>Returns 98 if there's an error while reading from the pipe. </li>
 *          </ul>
 *
 * @designer    Abhishek Bhardwaj
 * @author      Abhishek Bhardwaj
 */
uint32_t read_type(int fd)
{
    uint32_t type;
    int read_bytes;

    if((read_bytes = read_pipe(fd, &type, sizeof(uint32_t))) <= 0)
    {
        if(read_bytes == 0)
        {
            perror("read_type: No data on pipe");
            return 99;
        }
        perror("read_type: Error while reading from pipe");
        return 98; //error .. check error
    }

    return type;
}

/**
 * A function for reading the packet from the pipe.
 * 
 * @param[in]   fd      The file descriptor to read from.
 * @param[in]   size    size of the data to be scanned from the file descriptor.
 *
 * @return  <ul>
 *              <li>Returns the scanned packet from the pipe if successful [Casted as void*].</li>
 *              <li>Returns NULL if there's an error while reading or if the there's nothing on the pipe.</li>
 *          </ul>
 *
 * @designer    Abhishek Bhardwaj
 * @author      Abhishek Bhardwaj
 */
void* read_packet(int fd, uint32_t size)
{
    void *temp = malloc(size);
	int read_bytes;

    if((read_bytes = read_pipe(fd, temp, size)) == -1){
        perror("Error on reading pipe : read_packet(int, uint32_t)");
        return NULL;
    }

    if(read_bytes == 0){
        perror("Nothing on pipe to read: read_packet(int, uint32_t)");
        return NULL;
    }

    return temp; 
}

/**
 * Called by the main function of the game. Intializes the client network component of the game.
 * Requires 2 pipes as arguments, these will be passed for communication to network router. The 
 * read descriptor of rcv_router_fd will be passed to the update system by the game, while the 
 * write descriptor of the send_router_fd will be passed to the send_system.
 * 
 * @param[in]   send_router_fd      Set of pipes created for passing data to the network router.
 * @param[in]   rcv_router_fd       Set of pipes created for grabbing data from the network router.
 *
 * @return      void
 *
 * @designer    Ramzi Chennafi
 * @author      Ramzi Chennafi
 */
void init_client_network(int send_router_fd[2], int rcv_router_fd[2])
{
    pthread_t thread;
    NETWORK_DATA * ndata = (NETWORK_DATA*) malloc(sizeof(NETWORK_DATA));
    
    ndata->read_pipe = send_router_fd[READ_END];
    ndata->write_pipe = rcv_router_fd[WRITE_END];

    pthread_create(&thread, NULL, networkRouter, (void *)ndata);
    pthread_detach(thread);
}

/**
 * Writes the packet type, packet size, and the packet itself to the pipe specified by fd.
 * This function should be used in conjunction with read_data_size and read_packet to transfer
 * gameplay information between the gameplay and network client modules.
 * 
 * @param[in]   write_fd      The write end of a pipe (to gameplay or network module).
 * @param[in]   packet_type   The type of packet written, as defined in Packets.h
 * @param[in]   packet        A pointer to the packet structure.
 *
 * @return      <ul>
 *                  <li>Returns 0 on a successful write.</li>
 *                  <li>Returns -1 if the write failed.</li>
 *              </ul>
 *
 * @designer    Shane Spoor
 * @author      Shane Spoor
 */
int write_packet(int write_fd, uint32_t packet_type, void *packet)
{   
    int temp;
    if ((temp = write_pipe(write_fd, &packet_type, sizeof(packet_type))) <= 0)
    {
        perror("write_packet: Failed to write packet");
        return -1;
    }
	
    if((temp = write_pipe(write_fd, packet, packet_sizes[packet_type - 1])) <= 0)
	{
		perror("write_packet: Failed to write packet");
		return -1;
	}

	return 0; 
}

/**
 * A wrapper function around read_type() and read_packet() for reading data from the pipe 
 * and returning it.
 * 
 * @param[in]   fd      The file descriptor to read from.
 * @param[in]   type    A pointer to the packet type.
 *
 * @return      <ul>
 *                  <li>Returns the packet if successful.</li>
 *                  <li>Returns NULL if it fails to read either of the type or the packet from the pipe.</li>
 *              </ul>
 *
 * @designer    Ramzi Chennafi
 * @author      Ramzi Chennafi
 */
void *read_data(int fd, uint32_t *type){
    int read_bytes;
    void *packet;
    *type = read_type(fd);
    if(*type <= 0 || *type > 14){
        perror("read_data: Failed to read packet type from pipe");
        return NULL;
    }

    if((packet = read_packet(fd, packet_sizes[*type - 1])) == NULL){
        perror("read_data: Failed to read packet");
        return NULL;
    }

    return packet;
}

