#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include "../world.h"

#define NUMLEVELS 8

typedef struct FowTile
{
	SDL_Rect rect;
	int visible[NUMLEVELS];
} FowTile;


typedef struct FowComponent
{
	struct fowtile **tiles;
	int xOffset;
	int yOffset;
	SDL_Surface **fogOfWar;
	SDL_Surface **alphaFog;
	SDL_Surface  *corners[12];
} FowComponent;


typedef struct FowPlayerPosition
{
	World *world;
	FowComponent *fow;
	PositionComponent *pos;
} FowPlayerPosition;

#endif
