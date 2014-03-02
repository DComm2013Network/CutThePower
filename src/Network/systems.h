/**
 * Declares the network update systems and their helper functions, and defines the client's player's entity number.
 *
 * @file system.h
 */

#ifndef NETWORK_SYSTEMS_H
#define NETWORK_SYSTEMS_H

#include "../world.h"

#define CLIENT_PLAYER 0 /**< The client's player's entity number (may also be passed into the function */

void client_update_system(World *world);
void client_update_pos(World *world);
void client_update_status(World *world);
void client_update_info(World *world);

void server_update_system(World *world, unsigned mask);
void server_update_pos(World *world);
void server_update_status(World *world);
void server_update_info(World *world);


#endif

