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
-
-- PROGRAMMER: Andrew Burian
--
-- NOTES:
-- This MUST be consistant between the client and server
----------------------------------------------------------------------*/

// Limits
#define MAX_PLAYERS          32
#define MAX_FLOORS           8
#define MAX_NAME             15
#define MAX_MESSAGE          180
#define MAX_OBJECTIVES       32
#define OBJECTIVES_PER_FLOOR 4

// Connect code Definitions
#define CONNECT_CODE_ACCEPTED 0x001
#define CONNECT_CODE_DENIED   0x000

#define TIMEOUT 		5000	
#define SEND_FREQUENCY 	60
#define MAXIP 			20
#define TCP   			0
#define UDP   			1
#define READ  			0
#define WRITE 1
#define NEEDS_UPDATE	1
#define UPDATED			2

#define P_NAME           1
#define P_CONNECT        2
#define G_STATUS         3
#define P_CHAT           4
#define P_READY_STAT     5
#define P_SPECIAL_TILE   6
#define P_UNDEF          7 // NOT DEFINED
#define P_OBJSTATUS      8
#define P_KEEPALIVE      9 /**< Sent from client to server or vice versa if there has been no activity for @a TIMEOUT ms. */
#define P_POSUPDATE      10
#define G_ALLPOSUPDATE   11
#define P_FLOOR_MOVE_REQ 12
#define P_FLOOR_MOVE     13
#define P_TAGGING        14
#define P_MIN_POS        15
#define P_MIN_POS_ALL    16

#define NUM_PACKETS  16

#define ABHISHEK 		0
#define AMAN     		1
#define ANDREW   		2
#define CHRIS    		3
#define CLARK    		4
#define CORY     		5
#define DAMIEN   		6
#define GERMAN   		7
#define IAN      		8
#define JORDAN   		9
#define JOSH     		10
#define KONST    		11
#define MAT      		12
#define RAMZI    		13
#define ROBIN    		14
#define SAM      		15
#define SHANE    		16
#define TIM      		17
#define VINCENT  		18
#define JOSH_ALT1		19
#define IAN_ALT1		20
#define AMAN_ALT1		21
#define ANDREW_ALT1		22
#define CORY_ALT1		23
#define DAMIEN_ALT1		24
#define JORDAN_ALT1		25
#define MAT_ALT1		26
#define RAMZI_ALT1		27
#define SAM_ALT1		28
#define TIM_ALT1		29
#define CHRIS_ALT1		30
#define SHANE_ALT1		31
#define ROBIN_ALT1		32
#define VINCENT_ALT1	33
#define KONST_ALT1		34
#define ABHISHEK_ALT1	35
#define GERMAN_ALT1		36
#define ALBERT  		37
#define MAT_ALT2  		38
#define ABHISHEK_ALT2	39
#define ABHISHEK_ALT3	40
#define ABHISHEK_ALT4	41
#define ABHISHEK_ALT5	42
#define ALBERT_ALT1 	43
#define CLARK_ALT1 		44
// Game Status Definitions

// Game Status Definitions
#define GAME_STATE_WAITING 0x001 // Waiting for PLAYER_STATE_READY by all players (LOBBY)
#define GAME_STATE_ACTIVE  0x002 // Game engine running
#define GAME_STATE_OVER    0x003

#define GAME_TEAM1_WIN 0x006
#define GAME_TEAM2_WIN 0x007

#define PLAYER_STATE_READY   0x004
#define PLAYER_STATE_WAITING 0x005

#define COPS    1
#define ROBBERS	2
// Special floor Definitions
#define FLOOR_LOBBY 0x000

#define OBJECTIVE_NONE  0
#define OBJECTIVE_UNCAP 1
#define OBJECTIVE_CAP   2

/**
  * @property type     The type of packet to send (1 through 14).
 * @property data     The data contained in the packet.
 *
 * @struct internal_packet
 */
typedef struct intpkt{
	int protocol;
	uint32_t type;
	char * data;
}internal_packet;

// Definitions for various game data types
typedef uint32_t floorNo_t;
typedef uint32_t playerNo_t;
typedef uint32_t teamNo_t;
typedef uint32_t status_t;
typedef uint32_t character_t;
typedef uint32_t pos_t;
typedef uint32_t tile_t;
typedef float	 vel_t;

// Packet Definitions

typedef struct pkt01{
	char 		client_player_name[MAX_NAME];
	character_t selectedCharacter;
} PKT_PLAYER_NAME;

typedef struct pkt02{
	uint32_t	connect_code;
	playerNo_t 	clients_player_number;
	teamNo_t 	clients_team_number;
	char 		name[MAX_NAME];
} PKT_PLAYER_CONNECT;

typedef struct pkt03{
	uint32_t	player_valid[MAX_PLAYERS];
	char 		otherPlayers_name[MAX_PLAYERS][MAX_NAME];
	teamNo_t 	otherPlayers_teams[MAX_PLAYERS];
	character_t characters[MAX_PLAYERS];
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
} PKT_READY_STATUS;

typedef struct pkt06{
	playerNo_t playerNo;
    floorNo_t floor;
    pos_t xPos;
    pos_t yPos;
    tile_t tile;
} PKT_SPECIAL_TILE;
//Packet 7: 0x0007
// << UNPURPOSED >>

typedef struct pkt08
{
	status_t	objectives_captured[MAX_OBJECTIVES];
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
	vel_t		xVel[MAX_PLAYERS];
	vel_t		yVel[MAX_PLAYERS];
} PKT_ALL_POS_UPDATE;

typedef struct pkt12{
	playerNo_t 	player_number;
	floorNo_t 	current_floor;
	floorNo_t 	desired_floor;
	pos_t 		desired_xPos;
	pos_t		desired_yPos;
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

typedef struct pkt15 {
	uint32_t data;
	uint16_t vel;
} PKT_POS_UPDATE_MIN;

typedef struct pkt16 {
	uint8_t  floor;
	uint32_t players_on_floor;
	uint32_t xPos[11];
	uint32_t yPos[11];
	uint16_t vel[32];
} PKT_ALL_POS_UPDATE_MIN;

#endif
