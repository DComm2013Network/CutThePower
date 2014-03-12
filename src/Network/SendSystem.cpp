/** @ingroup Network */
/** @{ */

/**
 * This file contains all methods responsible for reading data from gameplay and sending it 
 * to the server.
 *
 * @file SendSystem.cpp
 */

/** @} */
#include "SendSystem.h"

/**
 * Checks the world for data and sends out data updates to be passed to the server. Currently sends out\
 * only a position update.
 *
 * FUTURE FEATURES: - Cached location of data after first go through for called data
 *					- Ability to specify which update will be sent
 *					
 * @param[in, out]  world  	game world, searched for updates
 * @param[in]		fd 		write file descriptor to the gameplay thread > network router thread
 *
 * @return  void
 *
 * @designer    Ramzi Chennafi
 * @author      Ramzi Chennafi
 */
void send_system(World *world, int fd) { 
	
		PKT_POS_UPDATE * pkt4 = (PKT_POS_UPDATE*)malloc(sizeof(PKT_POS_UPDATE));
		PKT_TAGGING * pkt14 = (PKT_TAGGING*)malloc(sizeof(PKT_TAGGING));

		for (int j = 0; j < MAX_ENTITIES; j++) {
			if (IN_THIS_COMPONENT(world->mask[j], COMPONENT_POSITION | COMPONENT_CONTROLLABLE))
			{
				pkt4->xPos = world->position[j].x;
				pkt4->yPos = world->position[j].y;
				pkt4->xVel = world->movement[j].movX;
				pkt4->yVel = world->movement[j].movY;
				pkt4->floor = world->position[j].level;
				pkt4->player_number = world->player[j].playerNo;
				break;
			}
		}
		write_packet(fd, P_POSUPDATE, pkt4);
}

/**
 * Checks the world for data and sends out data updates to be passed to the server. Currently sends out\
 * only a position update.
 *
 * FUTURE FEATURES: - Cached location of data after first go through for called data
 *					- Ability to specify which update will be sent
 *					
 * @param[in, out]  world  	game world, searched for updates
 * @param[in]		fd 		write file descriptor to the gameplay thread > network router thread
 *
 * @return  void
 *
 * @designer    Ramzi Chennafi
 * @author      Ramzi Chennafi
 */
void send_intialization(World *world, int fd)
{
	PKT_PLAYER_NAME * pkt1 = (PKT_PLAYER_NAME *)malloc(sizeof(PKT_PLAYER_NAME));
		for (int j = 0; j < MAX_ENTITIES; j++) {
			if (IN_THIS_COMPONENT(world->mask[j], COMPONENT_PLAYER | COMPONENT_CONTROLLABLE))
			{
				memcpy(pkt1->client_player_name, world->player[j].name, sizeof(world->player[j].name));
				break;
			}
		}	
		write_packet(fd, P_NAME, pkt1);

}
