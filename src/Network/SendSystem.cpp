#include "Packets.h"
#include "../components.h"
#include "../systems.h"
#include "../world.h"
#include "GameplayCommunication.h"
#include "NetworkRouter.h"
#include "PipeUtils.h"
#include <pthread.h>

void send_system(World& world, int fd, sem_t gplay_sem) {
	
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
		sem_wait(&gplay_sem);
		write_packet(fd, P_POSUPDATE, pkt4);
		sem_post(&gplay_sem);

		// PKT_OBJECTIVE_STATUS * pkt5 = (PKT_OBJECTIVE_STATUS)malloc(sizeof(PKT_OBJECTIVE_STATUS));

		// for (int j = 0; j < MAX_ENTITIES; j++) {
		// 	if (IN_THIS_COMPONENT(world.mask[j], COMPONENT_POSITION | COMPONENT_CONTROLLABLE))
		// 			// 		break;
		// 	}
		// }
		// write_packet(fd, P_OBJSTATUS, pkt);	
}

int main()
{
	pthread_t thread;
    int fd[2];
    uint32_t type = 0;
    sem_t gplay_sem;
    void * pkt;

	World * world = (World*) malloc(sizeof(World));

	create_player(*world, 10, 10, true);
	
	pthread_t router_thread;

	NETWORK_DATA * ndata = (NETWORK_DATA*) malloc(sizeof(NETWORK_DATA));

	create_pipe(fd);

	sem_init(&gplay_sem, 0, 1);
    ndata->read_pipe = fd[0];
    ndata->write_pipe = fd[1];
    ndata->pipesem = gplay_sem;

    send_system(*world, fd[1], gplay_sem);

    pthread_create(&thread, NULL, networkRouter, (void *)ndata);
    pthread_detach(thread);

   	while(1){
   		sem_wait(&gplay_sem);
   		pkt = read_data(fd[0], &type);
   		sem_post(&gplay_sem);
   		if(type != 99999)
   		{
   			printf("%d\n", type);
   			printf("test5\n");
			PKT_POS_UPDATE * pkt4 = (PKT_POS_UPDATE*)pkt;  
			printf("test32\n");		
			printf("num: %u x: %u y: %u", pkt4->player_number, pkt4->xPos, pkt4->yPos);	
   		}
   	}
}