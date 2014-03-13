#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../../world.h"
#include "../network_systems.h"
#include "../Packets.h"
#include "../GameplayCommunication.h"

int main()
{
	World world;
	int pipes[2];
	const char *names[MAX_PLAYERS]= {
									"interstellar rancher",
									"differentiated forging",
									"sorrier mercenary",
									"dissociated fledgling",
									"associative bluebell",
									"lonelier patriotism",
									"psychic jerking",
									"studded cucumber",
									"traditionalist glisten",
									"cardiac prier",
									"revived humbug",
									"sneakier twinkle",
									"fuzziest elastic",
									"aforesaid juggling",
									"puny homeless",
									"vertebrate highbrow",
									"dryer airmail",
									"frayed cacao",
									"amethyst gala",
									"infinitive prier",
									"ripened thatching",
									"disgraceful farming",
									"readier holiness",
									"articulating oyster",
									"fruity carat",
									"panicked gremlin",
									"voiced augment",
									"tortuous whiff",
									"irreverent inmate",
									"rainy hesitancy",
									"blinding fluster",
									"tearful eyesore"
								};
	
	PKT_PLAYER_CONNECT 	conn_good 	= {CONNECT_CODE_ACCEPTED, 0, 0};
	PKT_PLAYER_CONNECT 	conn_fail 	= {CONNECT_CODE_DENIED, 0, 0};

	PKT_GAME_STATUS		stat_client	= {{0}, {0}, {0}, {0}};
	PKT_GAME_STATUS		stat_all	= {{0}, {0}, {0}, {0}};
	
	PKT_ALL_POS_UPDATE	pos_1 = {5, {0}, {0}, {0}, {0}, {0}};
	PKT_ALL_POS_UPDATE	pos_2 = {3, {0}, {0}, {0}, {0}, {0}};

	if(pipe(pipes) == -1)
		return -1;
	
	init_world(world);
	
	stat_client.player_valid[CLIENT_PLAYER] = 1;
	strcpy(stat_client.otherPlayers_name[CLIENT_PLAYER], names[CLIENT_PLAYER]);
	memset(stat_all.player_valid, 1, sizeof(stat_all.player_valid[0]) * MAX_PLAYERS);
	memset(stat_all.readystatus, 1, sizeof(stat_all.readystatus[0]) * MAX_PLAYERS);
	
	for(int i = 0; i < MAX_PLAYERS; ++i)
	{
		stat_all.otherPlayers_teams[i] = (teamNo_t)(i % 2);
		strcpy(stat_all.otherPlayers_name[i], names[i]);
	}
	
	write_packet(pipes[1], (uint32_t)2, (void *)&conn_good);
	write_packet(pipes[1], (uint32_t)2, (void *)&conn_fail);
	write_packet(pipes[1], (uint32_t)3, (void *)&stat_client);
	write_packet(pipes[1], (uint32_t)3, (void *)&stat_all);
	write_packet(pipes[1], (uint32_t)11, (void *)&pos_1);
	write_packet(pipes[1], (uint32_t)11, (void *)&pos_2);
	
	client_update_system(&world, pipes[0]);
	client_update_system(&world, pipes[0]);
	client_update_system(&world, pipes[0]);
	client_update_system(&world, pipes[0]);
	client_update_system(&world, pipes[0]);
	client_update_system(&world, pipes[0]);
	
	for(int i = 0; i < MAX_PLAYERS; ++i)
	{
		printf("Name: %s\n", world.player[i].name);
		printf("Number: %u\n", world.player[i].playerNo);
		printf("Team number: %u\n", world.player[i].teamNo);
		printf("Ready status: %s\n", (world.player[i].readyStatus ? "ready": "not ready"));
		printf("\n");
	}

	return 0;
}

