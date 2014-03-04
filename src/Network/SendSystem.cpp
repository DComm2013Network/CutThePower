#include "Packets.h"
#include "../components.h"
#include "../systems.h"
#include "../world.h"
#include "GameplayCommunication.h"

void send_system(World& world, int fd) {
	
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

		for (int j = 0; j < MAX_ENTITIES; j++) {
			if (IN_THIS_COMPONENT(world.mask[j], COMPONENT_POSITION | COMPONENT_CONTROLLABLE))
			{
				pkt4->xPos = world.position[j].x;
				pkt4->yPos = world.position[j].y;
				pkt4->xVel = world.movement[j].movX;
				pkt4->yVel = world.movement[j].movY;
				pkt4->floor = world.position[j].level;
				pkt4->player_number = world.player[j].playerNo;
				break;	
			}
		}
		write_packet(fd, P_POSUPDATE, pkt4);

		// PKT_OBJECTIVE_STATUS * pkt5 = (PKT_OBJECTIVE_STATUS)malloc(sizeof(PKT_OBJECTIVE_STATUS));

		// for (int j = 0; j < MAX_ENTITIES; j++) {
		// 	if (IN_THIS_COMPONENT(world.mask[j], COMPONENT_POSITION | COMPONENT_CONTROLLABLE))
		// 	{
		// 		break;
		// 	}
		// }
		// write_packet(fd, P_OBJSTATUS, pkt);	
}