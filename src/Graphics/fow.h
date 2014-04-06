#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <ctime>
#include "../world.h"


#define NUMLEVELS   9

#define L_EMPTY		1
#define L_WALL		2

#define COLLISION_MASK (COMPONENT_COLLISION)
#define LEVEL_MASK (COMPONENT_LEVEL)

#define CLEAR_VIS	  0
#define TRANSP_VIS 	1
#define OPAQUE_VIS 	2
		
#define NUMSPEECH   3

#define OPAQUE_FOG_COLOUR 0x000000
#define TRANSP_FOG_COLOUR 0x221122

typedef struct PlayerSpeech
{
	Mix_Chunk *speech[NUMSPEECH];
	time_t played;
		
} PlayerSpeech;

typedef struct FowTile
{
	SDL_Rect rect;
	int visible[NUMLEVELS];
	int currentLOS;
} FowTile;


typedef struct FowComponent
{
	FowTile **tiles;
	int xOffset;
	int yOffset;
	SDL_Surface **fogOfWar;
	SDL_Surface **alphaFog;
	teamNo_t teamNo;
	
	PlayerSpeech copSpeech;
	
} FowComponent;


typedef struct FowPlayerPosition
{
	World *world;
	FowComponent *fow;
	PositionComponent *pos;
} FowPlayerPosition;

void render_fog_of_war_system(SDL_Surface *surface, FowComponent *fow);
void init_fog_of_war_system  (FowComponent **fow);
void cleanup_fog_of_war      (FowComponent  *fow);
void reset_fog_of_war				 (FowComponent  *fow);

void init_players_speech		 (FowComponent  *fow);
#endif
