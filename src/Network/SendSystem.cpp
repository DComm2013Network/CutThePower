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
static teamNo_t player_team = 0;
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
    
	pkt4->xPos = world->position[controllable_player].x;
	pkt4->yPos = world->position[controllable_player].y;
	pkt4->xVel = world->movement[controllable_player].movX;
	pkt4->yVel = world->movement[controllable_player].movY;
	pkt4->floor = world->position[controllable_player].level;
	pkt4->player_number = world->player[controllable_player].playerNo;

	write_packet(fd, P_POSUPDATE, pkt4);
    free(pkt4);
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
			controllable_player = j;
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
/**
 * Sets up a timer for slowing down the frequency of location updates sent by the client.
 *			
 * @param[in, out]  timer 	A pointer to an empty timer variable. The timer created will be
 *							placed in this variable.
 *
 * @return  void
 *
 * @designer    Ramzi Chennafi
 * @author      Ramzi Chennafi
 */
void setup_send_timer(timer_t * timer)
{
	struct sigevent sevnt;
	struct itimerspec its;
	
	sevnt.sigev_notify = SIGEV_NONE;
    sevnt.sigev_value.sival_ptr = timer;
    if (timer_create(CLOCK_REALTIME, &sevnt, timer) == -1)
    {
    	printf("Failed to create timer.\n");
        exit(2);
   	}

	its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = SEND_FREQUENCY * NANO_SECONDS + 1;
   	its.it_interval.tv_sec = its.it_value.tv_sec;
   	its.it_interval.tv_nsec = its.it_value.tv_nsec;
   	if(timer_settime(*timer, 0, &its, NULL) == -1)
   	{
   		printf("Failed to set timer with errno: %d\n", errno);
   		exit(2);
   	}
}