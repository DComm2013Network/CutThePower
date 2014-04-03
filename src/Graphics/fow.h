#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include "../world.h"


#define NUMLEVELS   9
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

void render_fog_of_war_system(SDL_Surface *surface, FowComponent *fow);
void init_fog_of_war_system  (FowComponent **fow);
void cleanup_fog_of_war      (FowComponent  *fow);

#endif
