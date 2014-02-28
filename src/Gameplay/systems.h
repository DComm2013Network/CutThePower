#ifndef GAMEPLAY_SYSTEMS_H
#define GAMEPLAY_SYSTEMS_H

#include <SDL2/SDL.h>

#include "../systems.h"
#include "../world.h"

void render_system(World& world, SDL_Surface *surface);
//void input_system(World& world, bool& running);
void movement_system(World& world);
void update_system(World& world);

#endif
