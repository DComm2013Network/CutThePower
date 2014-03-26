#ifndef SEND_SYS_H
#define SEND_SYS_H

#include "Packets.h"
#include "../components.h"
#include "../systems.h"
#include "../world.h"
#include "GameplayCommunication.h"
#include "NetworkRouter.h"	
#include "PipeUtils.h"

void send_location(World *world, int fd);
void send_intialization(World *world, int fd, char * username, char * serverip);
void move_request(World * world, int fd, floorNo_t floor, pos_t xpos, pos_t ypos);
void send_status_ready(World * world, int sendpipe, teamNo_t team);
#endif
