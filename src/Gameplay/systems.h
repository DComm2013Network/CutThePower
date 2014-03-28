/** @ingroup Gameplay */
/** @{ */
/** @file systems.h */
/** @} */
#ifndef GAMEPLAY_SYSTEMS_H
#define GAMEPLAY_SYSTEMS_H

#include <SDL2/SDL.h>

#include "../world.h"

void render_system(World* world, SDL_Surface *surface);
void movement_system(World* world, FPS fps);
void update_system(World* world);

#endif
