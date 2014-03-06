#include "SendSystem.h"
/*------------------------------------------------------------------------------------------
 * FUNCTION:    send_system
 *
 * DATE:        March 5, 2014
 *
 * REVISIONS:   
 *
 * DESIGNER:    Ramzi Chennafi
 *
 * PROGRAMMER:  Ramzi Chennafi
 *
 * INTERFACE:   send_system(World& world, int fd, sem_t gplay_sem)
 *                  world - game world, searched for updates
 *					fd    - write file descriptor to the gameplay thread > network router thread
 *					gplay_sem - semaphore for the gameplay thread > network router thread pipe
 *
 * RETURNS:     nothing
 *
 * NOTES:
 *
 * Checks the world for data and sends out data updates to be passed to the server. Currently sends out\
 * only a position update.
 *
 * FUTURE FEATURES: Cached location of data after first go through for called data
 *					Ability to specify which update will be sent
 *----------------------------------------------------------------------------------------*/
void send_system(World *world, int fd) { 

		// PKT_PLAYER_NAME * pkt1 = (PKT_PLAYER_NAME *)malloc(sizeof(PKT_PLAYER_NAME));
		// for (int j = 0; j < MAX_ENTITIES; j++) {
		// 	if (IN_THIS_COMPONENT(world.mask[j], COMPONENT_PLAYER | COMPONENT_CONTROLLABLE))
		// 	{
		// 		memcpy(pkt1->client_player_name, world.player[j].name, sizeof(world.player[j].name));
		// 		break;
		// 	}
		// }	
		// write_packet(fd, P_NAME, pkt1);

	
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
			}

			if(IN_THIS_COMPONENT(world->mask[j], COMPONENT_TAG)) 
			{
				pkt14->tagger_id = world->tag[j].tagger_id;
				pkt14->taggee_id = world->tag[j].taggee_id;
				destroy_entity(*world, j);
			}
		}
		write_packet(fd, P_POSUPDATE - 1, pkt4);
		// PKT_OBJECTIVE_STATUS * pkt5 = (PKT_OBJECTIVE_STATUS)malloc(sizeof(PKT_OBJECTIVE_STATUS));

		// for (int j = 0; j < MAX_ENTITIES; j++) {
		// 	if (IN_THIS_COMPONENT(world.mask[j], COMPONENT_POSITION | COMPONENT_CONTROLLABLE))
		// 			// 		break;
		// 	}
		// }
}
