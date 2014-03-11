


void update_system(World& world) {
	
	void* packet;
	uint32_t type;
	
	packet = read_data(0, &type);
	
	switch (type) {
		case 1:
			//shouldnt happen
			break;
		case 2:
			//not implemented yet (with gameplay)
			break;
		case 3: {
				PKT_GAME_STATUS* pkt03 = packet;
				for (int i = 0; i < MAX_PLAYERS; i++) {
					if (pkt03->player_valid[i]) {
						for (int j = 0; j < MAX_ENTITIES; j++) {
							if (world.mask[j] & PLAYER_COMPONENT) {
								if (world.player[j].playerNo == i) {
									strcpy(world.player[j].name, pkt03->otherPlayers_name[i]);
									world.player[j].teamNo = pkt03->otherPlayers_teams[i];
									world.player[j].readyStatus = pkt03->readystatus[i];
								}
							}
						}
					}
				}
			}
			break;
		case 4:
			//chat data
			break;
		case 5:
			//lobby thing
			//	-Shane
			break;
		case 6:
			
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			//should never receive this packet, for it is sent by the client
			break;
		case 11: {
				PKT_ALL_POS_UPDATE* pkt12 = packet;
				for (int i = 0; i < MAX_PLAYERS; i++) {
					if (pkt12->players_on_floor[i]) {
						for (int j = 0; j < MAX_ENTITIES; j++) {
							if (world.mask[j] & (POSITION_COMPONENT | MOVEMENT_COMPONENT | PLAYER_COMPONENT) == (POSITION_COMPONENT & MOVEMENT_COMPONENT | PLAYER_COMPONENT)) {
								if (world.player[j].playerNo == i) {
									
								}
							}
						}
					}
				}
			}
			break;
		default:
			break;
	}
}



