#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <ctime>
#include "../world.h"


#define NUMLEVELS 10

#define L_EMPTY 1
#define L_WALL  2

#define COLLISION_MASK (COMPONENT_COLLISION)
#define LEVEL_MASK (COMPONENT_LEVEL)

#define CLEAR_VIS  0
#define TRANSP_VIS 1
#define OPAQUE_VIS 2
		
#define NUMSPEECHCOP    5
#define NUMSPEECHROB		2
#define NMAXTILESINLOS 37

#define OPAQUE_FOG_COLOUR 0x000000
#define TRANSP_FOG_COLOUR 0x221122



typedef struct PlayerSpeech
{
	Mix_Chunk *rob[NUMSPEECHROB];
	Mix_Chunk *cop[NUMSPEECHCOP];
	time_t played;
		
} PlayerSpeech;


typedef struct FowTile
{
	SDL_Rect rect;
	int visible[NUMLEVELS];
} FowTile;


typedef struct BlockedTiles
{
	int coords[7][7];
} BlockedTiles;


typedef struct FowComponent
{
	FowTile **tiles;
	int xOffset;
	int yOffset;
	SDL_Surface **fogOfWar;
	SDL_Surface **alphaFog;
	teamNo_t teamNo;
	
	BlockedTiles bt[7][7];

	PlayerSpeech speech;
	int tilesVisibleToControllablePlayer[NMAXTILESINLOS][2];
	int tilesVisibleToControllablePlayerCount;
	
} FowComponent;


typedef struct FowPlayerPosition
{
	World *world;
	FowComponent *fow;
	PositionComponent *pos;
	
	int isControllablePlayer;
	
} FowPlayerPosition;

void render_fog_of_war_system(SDL_Surface *surface, FowComponent *fow);
void init_fog_of_war_system  (FowComponent **fow);
void cleanup_fog_of_war      (FowComponent  *fow);
void reset_fog_of_war        (FowComponent  *fow);
void init_players_speech     (FowComponent  *fow);
void render_player_speech    (FowComponent *fow, int xPos, int yPos);

void make_surrounding_tiles_visible (FowPlayerPosition *fowp);
#endif
