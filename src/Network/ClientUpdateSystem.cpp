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
#include "../systems.h"
#include <sys/poll.h>

extern int game_ready;
static int controllable_playerNo;
extern int game_net_signalfd, game_net_lockfd;
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
	uint64_t 	sem_buf;
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
		
		printf("Updating with packet type %u\n", type);
		switch (type) { // the cached packets minuses one from the value 
			case P_CONNECT:
				if(client_update_info(world, packet) == CONNECT_CODE_DENIED)
				{
					game_ready++;
					return; // Pass error up to someone else to deal with
				}
				break;
			case G_STATUS:
				client_update_status(world, packet);
				game_ready++;
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
				game_ready++;
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
	PKT_OBJ_LOC *obj_loc = (PKT_OBJ_LOC*) packet;
}


void client_update_floor(World *world, void *packet)
{
	PKT_FLOOR_MOVE* floor_move = (PKT_FLOOR_MOVE*)packet;
	for (int i = 0; i < MAX_ENTITIES; i++)
	{
		if (IN_THIS_COMPONENT(world->mask[i], COMPONENT_CONTROLLABLE | COMPONENT_MOVEMENT | COMPONENT_POSITION))
		{
			world->position[i].x		= floor_move->xPos;
			world->position[i].y		= floor_move->yPos;
			world->position[i].level	= floor_move->new_floor;
			break;
		}
	}
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
	bool player_found[MAX_PLAYERS];

	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		player_found[i] = false;
	}

	for (int i = 0; i < MAX_ENTITIES; i++)
	{
		if (IN_THIS_COMPONENT(world->mask[i], COMPONENT_MOVEMENT | COMPONENT_POSITION | COMPONENT_PLAYER))
		{
			for (playerNo_t j = 0; j < MAX_PLAYERS; j++) 
			{
				if(j == controllable_playerNo)
				{
					player_found[controllable_playerNo] = true;
					continue;
				}

				else if(world->player[i].playerNo == j)
				{
					world->movement[i].movX		= pos_update->xVel[j];
					world->movement[i].movY 	= pos_update->yVel[j];
					world->position[i].x		= pos_update->xPos[j];
					world->position[i].y		= pos_update->yPos[j];
					world->position[i].level	= pos_update->floor;
					player_found[j] = true;
				}
			}
		}
	}

	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		if((pos_update->players_on_floor[i] == true) && (player_found[i] == false))
		{
			printf("PLAYER CREATED: NUMBER %d\n", i);
			unsigned int player_entity = create_player(world, pos_update->xPos[i], pos_update->yPos[i], false, COLLISION_HACKER, i);
			world->mask[player_entity] |= COMPONENT_ANIMATION;
			load_animation("assets/Graphics/player/robber/rob_animation.txt", world, player_entity);
		}
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
	int player_found[MAX_PLAYERS];

	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		player_found[i] = false;
	}

	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		if((status_update->player_valid[i] == true) && (i != controllable_playerNo))
		{
			if(status_update->otherPlayers_teams[i] == ROBBERS)
			{
				printf("PLAYER CREATED: NUMBER %d\n", i);
				unsigned int player_entity = create_player(world, -50, -50, false, COLLISION_HACKER, i);
				world->mask[player_entity] |= COMPONENT_ANIMATION;
				load_animation("assets/Graphics/player/robber/rob_animation.txt", world, player_entity);
			}

			else if(status_update->otherPlayers_teams[i] == COPS)
			{
				printf("PLAYER CREATED: NUMBER %d\n", i);
				unsigned int player_entity = create_player(world, -50, -50, false, COLLISION_HACKER, i);
				world->mask[player_entity] |= COMPONENT_ANIMATION;
				load_animation("assets/Graphics/player/robber/rob_animation.txt", world, player_entity);
			}
		}
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

	for (int i = 0; i < MAX_ENTITIES; i++)
	{
		if (IN_THIS_COMPONENT(world->mask[i], COMPONENT_MOVEMENT | COMPONENT_POSITION | COMPONENT_PLAYER | COMPONENT_CONTROLLABLE))
		{
			world->player[i].teamNo					= client_info->clients_team_number;
			world->player[i].playerNo				= client_info->clients_player_number;
			controllable_playerNo 					= client_info->clients_player_number;
		}
	}

	return CONNECT_CODE_ACCEPTED;
}


