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

extern int network_ready;
teamNo_t player_team = 0;

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
void send_location(World *world, int fd) { 
	
		PKT_POS_UPDATE * pkt4 = (PKT_POS_UPDATE*)malloc(sizeof(PKT_POS_UPDATE));
		PKT_TAGGING * pkt14 = (PKT_TAGGING*)malloc(sizeof(PKT_TAGGING));

        if(!network_ready)
        {
            free(pkt4);
            free(pkt14);
            return;
        }
        
		for (int j = 0; j < MAX_ENTITIES; j++) {
			if (IN_THIS_COMPONENT(world->mask[j], COMPONENT_PLAYER | COMPONENT_POSITION | COMPONENT_CONTROLLABLE))
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
        free(pkt4);
        free(pkt14);
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
void send_intialization(World *world, int fd, char * username, char * serverip)
{
	PKT_PLAYER_NAME * pkt1 = (PKT_PLAYER_NAME *)malloc(sizeof(PKT_PLAYER_NAME));
	for (int j = 0; j < MAX_ENTITIES; j++) {
		if (IN_THIS_COMPONENT(world->mask[j], COMPONENT_PLAYER | COMPONENT_CONTROLLABLE))
		{
			memcpy(pkt1->client_player_name, world->player[j].name, sizeof(world->player[j].name));
			pkt1->selectedCharacter = world->player[j].character;
			break;
		}
	}	
	write_packet(fd, P_NAME, pkt1);
	free(pkt1);
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
void move_request(World * world, int fd, floorNo_t floor, pos_t xpos, pos_t ypos)
{
	PKT_FLOOR_MOVE_REQUEST * pkt = (PKT_FLOOR_MOVE_REQUEST*)malloc(sizeof(PKT_FLOOR_MOVE_REQUEST));
	for (int j = 0; j < MAX_ENTITIES; j++) {
		if (IN_THIS_COMPONENT(world->mask[j], COMPONENT_PLAYER | COMPONENT_CONTROLLABLE))
		{
			pkt->player_number = world->player[j].playerNo;
			pkt->current_floor = world->position[j].level;
			pkt->desired_floor = floor;
			pkt->desired_xPos = xpos;
			pkt->desired_yPos = ypos;
			break;
		}
	}	
	write_packet(fd, P_FLOOR_MOVE_REQ, pkt);
	free(pkt);
}
/**
 * Sends an update out specifying the users chosen tea and that they're ready for the game
 * to start. Only called when the user goes to the corresponding team spot in the lobby.
 *			
 * @param[in, out]  world  	game world, searched for updates
 * @param[in]		fd 		write file descriptor to the gameplay thread > network router thread
 * @param[in] 		team    team chosen 
 *
 * @return  void
 *
 * @designer    Ramzi Chennafi
 * @author      Ramzi Chennafi
 */
void send_status_ready(World * world, int fd, teamNo_t team)
{
	if(player_team != team)
	{
		PKT_READY_STATUS * pkt = (PKT_READY_STATUS*)malloc(sizeof(PKT_READY_STATUS));
		for (int j = 0; j < MAX_ENTITIES; j++) {
			if (IN_THIS_COMPONENT(world->mask[j], COMPONENT_PLAYER | COMPONENT_CONTROLLABLE))
			{
				pkt->player_number = world->player[j].playerNo;
				pkt->ready_status = PLAYER_STATE_READY;
				pkt->team_number = team;
				memcpy(pkt->player_name, world->player[j].name, MAX_NAME);
				player_team = team;
				break;
			}
		}
		printf("Changed to team %d\n", team);
		write_packet(fd, P_READY_STAT, pkt);
		free(pkt);	
	}
}

/*UNDERSTANDING THE lobby
	Lobby starts, activate lobby flag
	Players send a ready status when they choose a team
	Everyones ready, recieve a packet 3
	Recieve a packet 8 objectives
	Deactivate lobby flag */