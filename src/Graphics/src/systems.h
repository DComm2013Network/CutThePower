#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "world.h"

/* POSSIBLY TEMPORARY!!! passing playerFilename may not be needed if gameplay gives us a complete player struct. */
void render_player_system(World& world, SDL_Surface* surface, SDL_Rect* playerRect, char *playerFilename);

#endif
