/**
 * @file network_systems.h
 */

#ifndef NETWORK_SYSTEMS_H
#define NETWORK_SYSTEMS_H

#include <sys/eventfd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>

#define CLIENT_PLAYER 		0 			/**< The client's player's entity number (may also be passed into the function) */
#define UNASSIGNED			4294967295	/**< The player's number has not yet been assigned. */

#define STATUS_UPDATE		1	/**< Send the player's ready status. */	
#define POS_UPDATE			2 	/**< Send the player's current position information. */
#define OBJECTIVE_UPDATE	4 	/**< Send current obejctive states. */
#define FLOOR_REQUEST		8 	/**< Request a floor change (desired floor should be specified in the World struct). */

int client_update_system(World *world, int net_pipe);
void client_update_pos(World *world, void *packet);
void client_update_status(World *world, void *packet);
int client_update_info(World *world, void *packet);
void player_tag_packet(World *world, void *packet);
void client_update_chat(World *world, void *packet);

void update_server(World *world, unsigned update_mask);
void client_update_obj_loc(World *world, void *packet);
void client_update_obj_status(World *world, void *packet);
void client_update_floor(World *world, void *packet);

#endif

