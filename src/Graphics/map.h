#ifndef MAP_H
#define MAP_H

#include <SDL2/SDL.h>
#include "../world.h"


#define TILE_WIDTH		4
#define TILE_HEIGHT		4

int map_init(World* world, char *file_map, char *tilemap);
void map_render(SDL_Surface *surface, int x, int y);

#endif
