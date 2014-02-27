#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <SDL2/SDL.h>

#include "world.h"

void render_system(World& world, SDL_Surface *surface);
void input_system(World& world, bool& running);
void movement_system(World& world);

#endif
