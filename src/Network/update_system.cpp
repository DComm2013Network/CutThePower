#include <stdlib.h>
#include <string.h>
#include "Packets.h"
#include "systems.h"

/**
 * TODO:	Rearrange the switch statement to put the most likely packets first. 
 *			This would be, in order: 11, 8, 4, 7 (?), and we'll have to decide the rest later
 */


static unsigned int *player_table = malloc(sizeof(unsigned int) * MAX_PLAYERS);
memset(player_table, MAX_ENTITIES + 1, MAX_PLAYERS * sizeof(unsigned int)); 

void update_system(World *world) {
	
	void* packet;
	uint32_t type;
	
	packet = read_data(0 /* the pipe fd... wherever that will come from, lol */, &type);
	
	if(!packet)
		return; // might need to check errno here and log error if there was one
	
	switch (type) {
		case 2:
		{
			PKT_PLAYER_CONNECT *client_info = (PKT_PLAYER_CONNECT *)packet;
			if(client_info->connect_code == CONNECT_CODE_DENIED)
				return; // Pass this info to someone as well; next milestone's problem

			player_table[client_info->clients_player_number] = CLIENT_PLAYER; // Or whatever the client's player entity is assigned
			world->player[CLIENT_PLAYER].teamNo = client_info->clients_team_number;
			break;
		}
		case 3:
		{
			PKT_GAME_STATUS *status_update = (PKT_GAME_STATUS *)packet;
			unsigned int entity;
			for(int i = 0; i < MAX_PLAYERS; ++i)
			{
				if(!status_update->player_valid[i] || player_table[i] == CLIENT_PLAYER) // Don't update if they're not valid or it's our player
					continue;
				
				if(player_table[i] == MAX_ENTITIES + 1) // I.e., they haven't yet been initialised
				{
					entity 							= create_player(world, 0, 0, false);
					world->player[entity].teamNo 	= status_update->otherPlayers_teams[i];
					world->player[entity].playerNo 	= i + 1;
					player_table[i] 				= entity;
					strcpy(world->player[entity].name, status_update->otherPlayers_name[i]);
				}
				
				world.player[entity].readyStatus = status_update->readystatus[i];
			}
		}
			break;
		case 4:
			// Chat data
			break;
		case 6:
			// Map info
			break;
		case 8:
			// Objective update (game_status is significant here)
			break;
		case 11:
		{
			PKT_ALL_POS_UPDATE *pos_update = (PKT_ALL_POS_UPDATE *)packet;
			for (int i = 0; i < MAX_PLAYERS; i++)
			{
				if (!pos_update->players_on_floor[i] || player_table[i] == CLIENT_PLAYER) // If they're not on this floor or it's our player
					continue;

				world.movement[player_table[i]].movX	= pos_update->xVel[i];
				world.movement[player_table[i]].movY 	= pos_update->yVel[i];
				world.position[player_table[i]].x		= pos_update->x;
				world.position[player_table[i]].y		= pos_update->y
				world.position[player_table[i]].s		= pos_update->s;
				world.position[player_table[i]].level	= pos_update->floor;
			}
		}
			break;
		
		// Should never receive a packet outside the above range (the rest are unpurposed or client->server packets); 
		// discard it (and maybe log an error) if we get one
		case 1:
		case 5:
		case 7:
		case 9:
		case 10:
		case 12:
		default:
			break;
	}
	
	// Free the arrays and packets
}


