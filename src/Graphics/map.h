/** @ingroup Graphics */
/** @{ */
/** @file map.h */
/** @} */

#ifndef MAP_H
#define MAP_H

#include <SDL2/SDL.h>
#include "../world.h"
#include "fow.h"

#define TILE_WIDTH	40 /**< The width of a tile in pixels. */
#define TILE_HEIGHT	40 /**< The height of a tile in pixels. */



int map_init(World* world, const char *file_map, const char *tilemap);
void cleanup_map();
void map_render(SDL_Surface *surface, World *world, unsigned int player_entity);

#endif
