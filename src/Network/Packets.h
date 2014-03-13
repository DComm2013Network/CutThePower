#ifndef PACKETS_H
#define PACKETS_H

#include <cstdint>

/*---------------------------------------------------------------------*
-- HEADER FILE: NetComm.h The definitions and declarations to be
-- to be used to communicate between the
-- client and the server
--
-- PROGRAM: Network-Client/Server
--
-- DATE: January 27, 2014
--
-- REVISIONS: none
--
-- DESIGNER: Andrew Burian
--
-- PROGRAMMER: Andrew Burian
--
-- NOTES:
-- This MUST be consistant between the client and server
----------------------------------------------------------------------*/

// Limits
#define MAX_PLAYERS 32
#define MAX_FLOORS 8
#define MAX_NAME 80
#define MAX_MESSAGE 180
#define MAX_OBJECTIVES 16

// Definitions for various game data types
typedef uint32_t floorNo_t;
typedef uint32_t playerNo_t;
typedef uint32_t teamNo_t;
typedef uint32_t status_t;
typedef float 	 pos_t;
typedef float	 vel_t;

// Connect code Definitions
#define CONNECT_CODE_ACCEPTED 0x001
#define CONNECT_CODE_DENIED 0x000

#define TIMEOUT 5000

/**
 * Encapsulates data for easy sending by the send thread.
 *
 * @property protocol The protocol (TCP or UDP) used to send this packet.
 * @property type     The type of packet to send (1 through 14).
 * @property data     The data contained in the packet.
 *
 * @struct internal_packet
 */
typedef struct{
	int protocol;
	uint32_t type;
	char * data;
}internal_packet;

#define TCP 0
#define UDP 1
#define READ 0
#define WRITE 1

#define P_NAME           1
#define P_CONNECT        2
#define G_STATUS         3
#define P_CHAT           4
#define P_CLNT_LOBBY     5
#define P_OBJCTV_LOC     6
#define P_UNDEF          7 // NOT DEFINED
#define P_OBJSTATUS      8
#define P_UNDEF2         9 // NOT DEFINED
#define P_POSUPDATE      10
#define G_ALLPOSUPDATE   11
#define P_FLOOR_MOVE_REQ 12
#define P_FLOOR_MOVE     13
#define P_TAGGING        14
#define P_KEEPALIVE      4294967295 /**< Sent from client to server or vice versa if there has been no activity for @a TIMEOUT ms. */

#define NUM_PACKETS  14

// Game Status Definitions

#define GAME_STATE_WAITING 		0x001
#define GAME_STATE_ACTIVE 		0x002
#define GAME_STATE_OVER 		0x003
#define PLAYER_STATE_READY 		0x004
#define PLAYER_STATE_WAITING 	0x005

// Special floor Definitions
#define FLOOR_LOBBY 0x000

// Packet Definitions

typedef struct pkt01{
	char client_player_name[MAX_NAME];
} PKT_PLAYER_NAME;

typedef struct pkt02{
	uint32_t	connect_code;
	playerNo_t 	clients_player_number;
	teamNo_t 	clients_team_number;
} PKT_PLAYER_CONNECT;

typedef struct pkt03{
	int	player_valid[MAX_PLAYERS];
	char 		otherPlayers_name[MAX_PLAYERS][MAX_NAME];
	teamNo_t 	otherPlayers_teams[MAX_PLAYERS];
	status_t	readystatus[MAX_PLAYERS];
} PKT_GAME_STATUS;

typedef struct pkt04{
	playerNo_t sendingPlayer_number;
	char message[MAX_MESSAGE];
} PKT_SND_CHAT;

typedef struct pkt05{
	playerNo_t	player_number;
	status_t	ready_status;
	teamNo_t	team_number;
	char 		player_name[MAX_NAME];
} pkt05;

typedef struct pkt06{
	floorNo_t	map_data[MAX_FLOORS];
	int	objective_locations[MAX_OBJECTIVES];
} pkt06;
//Packet 7: 0x0007
// << UNPURPOSED >>

typedef struct pkt08
{
	int	objectives_captured[MAX_OBJECTIVES];
	status_t	game_status;
} PKT_OBJECTIVE_STATUS;

//Packet 9: 0x0009
// << UNPURPOSED >>

typedef struct pkt10{
	floorNo_t 	floor;
	playerNo_t 	player_number;
	pos_t 		xPos;
	pos_t		yPos;
	vel_t		xVel;
	vel_t		yVel;
} PKT_POS_UPDATE;

typedef struct pkt11{
	floorNo_t 	floor;
	int	players_on_floor[MAX_PLAYERS];
	pos_t		xPos[MAX_PLAYERS];
	pos_t		yPos[MAX_PLAYERS];
	pos_t		xVel[MAX_PLAYERS];
	pos_t		yVel[MAX_PLAYERS];
} PKT_ALL_POS_UPDATE;

typedef struct pkt12{
	playerNo_t 	player_number;
	floorNo_t 	current_floor;
	floorNo_t 	desired_floor;
} PKT_FLOOR_MOVE_REQUEST;

typedef struct pkt13{
	floorNo_t new_floor;
	pos_t	xPos;
	pos_t	yPos;
} PKT_FLOOR_MOVE;

typedef struct pkt14 {
	playerNo_t	tagger_id; /* the person who tagged */
	playerNo_t  taggee_id; /* the person who got tagged */
} PKT_TAGGING;

#endif
