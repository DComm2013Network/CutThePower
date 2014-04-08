/**
 * @file network_systems.h
 */

#ifndef NETWORK_SYSTEMS_H
#define NETWORK_SYSTEMS_H

#include <sys/eventfd.h>
#include <cstdlib>
#include <cstring>
#include <sys/poll.h>
#include <climits>

#define CLIENT_PLAYER    0 			/**< The client's player's entity number (may also be passed into the function) */
#define UNASSIGNED       4294967295	/**< The player's number has not yet been assigned. */

#define STATUS_UPDATE    1	/**< Send the player's ready status. */	
#define POS_UPDATE       2 	/**< Send the player's current position information. */
#define OBJECTIVE_UPDATE 4 	/**< Send current obejctive states. */
#define FLOOR_REQUEST    8 	/**< Request a floor change (desired floor should be specified in the World struct). */

#define DIRECTION_RIGHT	 1
#define DIRECTION_LEFT	 2
#define DIRECTION_UP	 3
#define DIRECTION_DOWN	 4

typedef struct _objective_cache
{
	char  obj_state;
	short entity_no;
} objective_cache;

int init_client_update(World *world);
int client_update_system(World *world, int net_pipe);
void client_update_pos(World *world, void *packet);
void client_update_status(World *world, void *packet);
int client_update_info(World *world, void *packet);
void client_update_chat(World *world, void *packet);
void client_update_floor(World *world, void *packet);
void client_update_objectives(World *world, void *packet);
void player_tag_packet(World *world, void *packet);
void change_player(World * world, int type, PKT_GAME_STATUS * pkt, int playerNo);
void setup_character_animation(World * world, int character, int entity);
void update_special_tile(World *world, void * packet);

#endif

