/**
 * @file network_systems.h
 */

#ifndef NETWORK_SYSTEMS_H
#define NETWORK_SYSTEMS_H

#include "../world.h"

#define CLIENT_PLAYER 		0 /**< The client's player's entity number (may also be passed into the function) */

#define POS_UPDATE			1 /**< Indicates that current position information will be sent. */
#define OBJECTIVE_UPDATE	2 /**< Indicates that current obejctive states will be sent. */
#define FLOOR_UPDATE		4 /**< Indicates that the player wants to move to a new floor. */

void client_update_system(World *world, void *packet);
void client_update_pos(World *world, void *packet);
void client_update_status(World *world, void *packet);
int client_update_info(World *world, void *packet);

void update_server(World *world, unsigned update_mask);

#endif

