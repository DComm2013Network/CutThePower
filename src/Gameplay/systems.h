#ifndef GAMEPLAY_SYSTEMS_H
#define GAMEPLAY_SYSTEMS_H

#include <SDL2/SDL.h>

#include "../world.h"

void render_system(World& world, SDL_Surface *surface);
void movement_system(World& world);
void add_force(World& world, unsigned int entity, float magnitude, float dir);
void apply_force(World& world, unsigned int entity);
void update_system(World& world);

#endif
