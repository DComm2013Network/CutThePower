#ifndef MAP_H
#define MAP_H

#include <SDL2/SDL.h>

/*#define WIDTH 			1280
#define HEIGHT 			768*/

#define TILE_WIDTH		4
#define TILE_HEIGHT		4

int map_init(char *map, char *tilemap);
void map_render(SDL_Surface *surface);

#endif