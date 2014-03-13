#ifndef GAMEPLAY_SYSTEMS_H
#define GAMEPLAY_SYSTEMS_H

#include <SDL2/SDL.h>
#include "../Network/SendSystem.h"

#include "../world.h"

void render_system(World* world, SDL_Surface *surface);
void movement_system(World* world, int sendpipe);
void add_force(World* world, unsigned int entity, float magnitude, float dir);
void apply_force(World* world, unsigned int entity);
void update_system(World* world);

#endif
