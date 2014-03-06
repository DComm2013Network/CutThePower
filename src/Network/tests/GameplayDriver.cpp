#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/eventfd.h>

#include "../GameplayCommunication.h"
#include "../NetworkRouter.h"
#include "../Packets.h"
#include "../../world.h"
#include "../SendSystem.h"

int game_net_signalfd, game_net_lockfd;

int main()
{
    pthread_t thread;
    int send_router_fd[2], rcv_router_fd[2];
    uint32_t type = 0;
    sem_t gplay_sem;
    void * pkt;

    World * world = (World*) malloc(sizeof(World));

    create_player(*world, 10, 10, true);
    
    pthread_t router_thread;

    NETWORK_DATA * ndata = (NETWORK_DATA*) malloc(sizeof(NETWORK_DATA));

    create_pipe(rcv_router_fd);
    create_pipe(send_router_fd);
	
	game_net_signalfd 	= eventfd(0, EFD_SEMAPHORE);
	game_net_lockfd		= eventfd(0, EFD_SEMAPHORE);
	
    sem_init(&gplay_sem, 0, 0);
    ndata->read_pipe = send_router_fd[READ];
    ndata->write_pipe = rcv_router_fd[WRITE];
    ndata->pipesem = gplay_sem;

    send_system(*world, send_router_fd[WRITE], gplay_sem);

    pthread_create(&thread, NULL, networkRouter, (void *)ndata);
    pthread_detach(thread);

    while(1)
    {   
        printf("Waiting\n");
        pkt = read_data(rcv_router_fd[READ], &type);
        if(type != 99999)
        {
            printf("Packet recieved\n");
            printf("test5\n");
            PKT_POS_UPDATE * pkt4 = (PKT_POS_UPDATE*)pkt;   
            printf("num: %u x: %u y: %u\n", pkt4->player_number, pkt4->xPos, pkt4->yPos);   
        }
    }
}
