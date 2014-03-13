/**
 * The client update system retrieves from the network module and
 * applies them to the world.
 *
 * @file client_update_system.cpp
 */

#include <stdlib.h>
#include <string.h>
#include "Packets.h"
#include "network_systems.h"
#include "GameplayCommunication.h"
#include "PipeUtils.h"
#include "../world.h"
#include <sys/poll.h>
 
extern int game_net_signalfd;
extern int network_ready;
static unsigned int *player_table = NULL; /**< A lookup table mapping server player numbers to client entities. */

/**
 * Receives all updates from the server and applies them to the world.
 *
 * The function updates all relevant networking information: movement data from other players,
 * objective updates, floor changes, and the initial player information (names, team numbers and
 * player numbers).
 *
 * @param[in] world 	The world struct to be updated.
 * @param[in] net_pipe	The read end of a pipe connected to the network module.
 *
 * @designer Shane Spoor
 * @designer Clark Allenby
 * @author Shane Spoor
 */
void client_update_system(World *world, int net_pipe) {
	void* 		packet;
	uint32_t 	type;
	uint32_t 	num_packets;
	uint64_t	signal = 1;
	unsigned	i;
	
	if(!player_table)
	{
		player_table = (unsigned int *)malloc(sizeof(unsigned int) * MAX_PLAYERS);
		memset(player_table, 255, MAX_PLAYERS * sizeof(unsigned int)); 
	}

    if(!network_ready) // Don't try to read the pipe until the network module has been initialised
        return;

	write(game_net_signalfd, &signal, sizeof(uint64_t));
	num_packets = read_type(net_pipe); // the function just reads a 32 bit value, so this works; semantically, not ideal

	for(i = 0; i < num_packets; ++i)
	{
		packet = read_data(net_pipe, &type);
		printf("Updating with packet type %u\n", type + 1);
		switch (type + 1) { // the cached packets minuses one from the value 
			case P_CONNECT:
				if(client_update_info(world, packet) == CONNECT_CODE_DENIED)
					return; // Pass error up to someone else to deal with
				break;
			case G_STATUS:
				client_update_status(world, packet);
				break;
			case P_CHAT:
				client_update_chat(world, packet);
				break;
			case P_OBJCTV_LOC:
				client_update_obj_loc(world, packet);
				break;
			case 7: // undefined
				// Floor stuff
				break;
			case P_OBJSTATUS:
				client_update_obj_status(world, packet);
				break;
			case G_ALLPOSUPDATE:
				client_update_pos(world, packet);
				break;
			case P_FLOOR_MOVE:
				client_update_floor(world, packet);
				break;
			case P_TAGGING:
				player_tag_packet(world, packet);
				break;
			default:
				break;
		}
		free(packet);
	}
}
void client_update_chat(World *world, void *packet)
{
	PKT_SND_CHAT *snd_chat = (PKT_SND_CHAT*)packet;
}
void client_update_obj_loc(World *world, void *packet)
{
	PKT_OBJ_LOC *obj_loc = (PKT_OBJ_LOC*) packet;
}
void client_update_obj_status(World *world, void *packet)
{
	PKT_OBJECTIVE_STATUS *obj_status =  (PKT_OBJECTIVE_STATUS*)packet;
}
void client_update_floor(World *world, void *packet)
{
	PKT_FLOOR_MOVE* floor_move = (PKT_FLOOR_MOVE*)packet;
}
/**
 * Updates the positions and movement properties of every other player.
 *
 * The function will ignore players that aren't on the current floor and the client's
 * own player, since they're said to be authoritative over their own position (except
 * for their floor).
 *
 * @param[in, out]	world 	The world struct holding the data to be updated.
 * @param[in] 		packet	The packet containing update information.
 *
 * @designer Shane Spoor
 * @author Shane Spoor
 */
void client_update_pos(World *world, void *packet)
{
	PKT_ALL_POS_UPDATE *pos_update = (PKT_ALL_POS_UPDATE *)packet;
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (!pos_update->players_on_floor[i] || player_table[i] == CLIENT_PLAYER) // If they're not on this floor or it's our player
		{
			if(!pos_update->players_on_floor[i])
			{
				destroy_entity(world, i);
				player_table[i] = UNASSIGNED;
			}

			continue;
		}

		world->movement[player_table[i]].movX	= pos_update->xVel[i];
		world->movement[player_table[i]].movY 	= pos_update->yVel[i];
		world->position[player_table[i]].x		= pos_update->xPos[i];
		world->position[player_table[i]].y		= pos_update->yPos[i];
		world->position[player_table[i]].level	= pos_update->floor;
	}
}

/**
 * If this function is called, it means the current player got tagged.
 * This packet is going to be passed to the gameplay and they'll manage it.
 *
 * @param[in, out]	world 	The world struct holding the data to be updated.
 * @param[in] 		packet	The packet containing update information.
 *
 * @designer Abhishek Bhardwaj
 * @author Abhishek Bhardwaj
 */
void player_tag_packet(World *world, void *packet)
{
	PKT_TAGGING * pkt14 = (PKT_TAGGING*)packet;

	uint32_t entity = create_entity(world, COMPONENT_TAG);
	world->tag[entity].tagger_id = pkt14->tagger_id;
	world->tag[entity].taggee_id = pkt14->taggee_id;
}

/**
 * Udpates the objective statuses and the game state.
 *
 * If all objectives have been captured, the game is over. The server will
 * indicate this using the game status property.
 *
 * @param[out]	world 	The world struct containing the ojective states to be updated.
 * @param[in]	packet	The packet containing objective update information.
 *
 * @designer
 * @author
 */
void client_update_objectives(World *world, void *packet)
{
	PKT_OBJECTIVE_STATUS *objective_update = (PKT_OBJECTIVE_STATUS *)packet;
	for(int i = 0; i < MAX_OBJECTIVES; ++i)
	{
		// Code to update (currently non-existing) objectives	
	}
}

/**
 * Updates the status and team details of all other players.
 *
 * The client receives a separate packet containg this information for it specifically, so
 * it ignores its own information.
 *
 * @param[in, out] 	world	The world struct holding the details to be updated.
 * @param[in] 		packet	The packet holding the update information.
 *
 * @designer Shane Spoor
 * @author Shane Spoor
 */
void client_update_status(World *world, void *packet)
{
	PKT_GAME_STATUS *status_update = (PKT_GAME_STATUS *)packet;
	unsigned int entity;
	for(int i = 0; i < MAX_PLAYERS; ++i)
	{
		if(!status_update->player_valid[i] || player_table[i] == CLIENT_PLAYER) // Don't update if they're not valid or it's our player
			continue;
		
		if(player_table[i] == UNASSIGNED)
		{
			entity 								= create_player(world, 0, 0, false);
			world->player[entity].teamNo 		= status_update->otherPlayers_teams[i];
			world->player[entity].playerNo 		= i + 1;
			player_table[i] 					= entity;
			strcpy(world->player[entity].name, status_update->otherPlayers_name[i]);
		}
		
		world->player[entity].readyStatus = status_update->readystatus[i];
	}
}

/**
 * Updates the client's player number and team details.
 *
 * The client should only receive this packet once at the beginning of each game.
 *
 * @param[out] 	world	The world struct to hold the client's information.
 * @param[in]	packet	The packet containing the client's team and player details.
 *
 * @return 	CONNECT_CODE_DENIED if the client's connection attempt was for some reason
 *			denied by the server, or CONNECT_CODE_ACCEPTED otherwise.
 *
 * @designer Shane Spoor
 * @author Shane Spoor
 */
int client_update_info(World *world, void *packet)
{
	PKT_PLAYER_CONNECT *client_info = (PKT_PLAYER_CONNECT *)packet;
	if(client_info->connect_code == CONNECT_CODE_DENIED)
		return CONNECT_CODE_DENIED;

	world->player[CLIENT_PLAYER].teamNo					= client_info->clients_team_number;
	world->player[CLIENT_PLAYER].playerNo				= client_info->clients_player_number;
	player_table[client_info->clients_player_number]	= CLIENT_PLAYER; // Or whatever the client's player entity is assigned
	
	return CONNECT_CODE_ACCEPTED;
}


