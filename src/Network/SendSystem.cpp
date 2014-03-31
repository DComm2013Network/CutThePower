/** @ingroup Network */
/** @{ */

/**
 * This file contains all methods responsible for reading data from gameplay and sending it 
 * to the server.
 *
 * @file SendSystem.cpp
 */

/** @} */
#include "Packets.h"
#include "GameplayCommunication.h"
#include "NetworkRouter.h"	
#include "PipeUtils.h"
#include "SendSystem.h"

extern int network_ready;
static int controllable_player;
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
void send_location(World *world, int fd) 
{ 
	PKT_POS_UPDATE * pkt4 = (PKT_POS_UPDATE*)malloc(sizeof(PKT_POS_UPDATE));
    for (int i = 0; i < MAX_ENTITIES; i++)
	{
		if (IN_THIS_COMPONENT(world->mask[i], COMPONENT_MOVEMENT | COMPONENT_POSITION | COMPONENT_PLAYER | COMPONENT_CONTROLLABLE))
		{
			pkt4->xPos = world->position[i].x;
			pkt4->yPos = world->position[i].y;
			pkt4->xVel = world->movement[i].movX;
			pkt4->yVel = world->movement[i].movY;
			pkt4->floor = world->position[i].level;
			pkt4->player_number = world->player[i].playerNo;
		}
	}
	write_packet(fd, P_POSUPDATE, pkt4);
    free(pkt4);
}

/**
 * Sends a tag packet when the client tags another player.
 *
 * @param[in] world  A pointer to the world struct.
 * @param[in] fd     The write end of a pipe to the network router.
 * @param[in] taggee The player that the client tagged.
 *
 * @designer Ramzi Chennafi
 * @author   Ramzi Chennafi
 */
void send_tag(World * world, int fd, unsigned int taggee)
{
	PKT_TAGGING * pkt = (PKT_TAGGING*) malloc(sizeof(PKT_TAGGING));

	pkt->tagger_id = world->player[controllable_player].playerNo;
	pkt->taggee_id = taggee;

	write_packet(fd, P_TAGGING, pkt);
    free(pkt);
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
void send_intialization(World *world, int fd, char * username)
{
	PKT_PLAYER_NAME * pkt1 = (PKT_PLAYER_NAME *)malloc(sizeof(PKT_PLAYER_NAME));
	for (int j = 0; j < MAX_ENTITIES; j++) {
		if (IN_THIS_COMPONENT(world->mask[j], COMPONENT_PLAYER | COMPONENT_CONTROLLABLE))
		{
			size_t name_len = strlen(username);
			controllable_player = j;
			memcpy(pkt1->client_player_name, username, name_len);
			pkt1->client_player_name[name_len] = 0;
			memcpy(world->player[j].name, username, name_len);
			world->player[j].name[name_len] = 0;
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

	pkt->player_number = world->player[controllable_player].playerNo;
	pkt->current_floor = world->position[controllable_player].level;
	pkt->desired_floor = floor;
	pkt->desired_xPos = xpos;
	pkt->desired_yPos = ypos;

	write_packet(fd, P_FLOOR_MOVE_REQ, pkt);
	free(pkt);
}
/**
 * Sends an update out specifying the users chosen team and if they're ready for the game to start.
 *			
 * @param[in, out]  world  	game world, searched for updates
 * @param[in]		fd 		write file descriptor to the gameplay thread > network router thread
 * @param[in] 		team    team chosen 
 * @param[in]		ready_status 	PLAYER_STATE_READY or PLAYER_STATE_WAITING, specifed if the player is
 *								is ready to play or has moved to the lobby. PLAYER_STATE_WAITING considered
 *								to mean that the player wishes to choose a team still.
 *
 * @return  void
 *
 * @designer    Ramzi Chennafi
 * @author      Ramzi Chennafi
 */
void send_status(World * world, int fd, teamNo_t team, int ready_status)
{
	if(player_team != team)
	{
		PKT_READY_STATUS * pkt = (PKT_READY_STATUS*)malloc(sizeof(PKT_READY_STATUS));

		pkt->player_number = world->player[controllable_player].playerNo;
		pkt->ready_status = ready_status;
		pkt->team_number = team;
		memcpy(pkt->player_name, world->player[controllable_player].name, MAX_NAME);
		player_team = team;

		printf("Changed to team %d\n", team);
		write_packet(fd, P_READY_STAT, pkt);
		free(pkt);
	}
}

