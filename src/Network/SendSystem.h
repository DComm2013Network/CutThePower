#ifndef SEND_SYS_H
#define SEND_SYS_H

#include "Packets.h"
#include "../components.h"
#include "../systems.h"
#include "../world.h"
#include "time.h"
#include <signal.h>

void send_location(World *world, int fd);
void send_intialization(World *world, int fd, char * username);
void move_request(World * world, int fd, floorNo_t floor, pos_t xpos, pos_t ypos);
void send_status(World * world, int fd, teamNo_t team, int ready_status);
void send_tag(World * world, int fd, unsigned int tagger);
void send_chat(World * world, int fd, char * str);
void send_objectives(World * world, int fd);
void send_tiles(World * world, unsigned int entity, int fd);

#endif
