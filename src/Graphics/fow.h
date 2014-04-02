#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include "../world.h"


#define NUMLEVELS   8
#define NUMCORNERS 18

typedef struct FowTile
{
	SDL_Rect rect;
	int visible[NUMLEVELS];
} FowTile;


typedef struct FowComponent
{
	FowTile **tiles;
	int xOffset;
	int yOffset;
	SDL_Surface **fogOfWar;
	SDL_Surface **alphaFog;
	SDL_Surface  *corners[NUMCORNERS];
	teamNo_t teamNo;
} FowComponent;


typedef struct FowPlayerPosition
{
	World *world;
	FowComponent *fow;
	PositionComponent *pos;
} FowPlayerPosition;

#endif
