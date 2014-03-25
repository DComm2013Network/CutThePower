/** @ingroup Graphics */
/** @{ */
/** @file map.h */
/** @} */

#ifndef MAP_H
#define MAP_H

#include <SDL2/SDL.h>
#include "../world.h"


#define TILE_WIDTH	40 /**< The width of a tile in pixels. */
#define TILE_HEIGHT	40 /**< The height of a tile in pixels. */

struct fogOfWarStruct
{
	int topXCorner;
	int topYCorner;
	int **revealedTiles;

	int xOffset;
	int yOffset;
};

int map_init(World* world, char *file_map, char *tilemap);
void map_render(SDL_Surface *surface, World *world, unsigned int player_entity);

#endif
