#ifndef GAMEPLAY_SYSTEMS_H
#define GAMEPLAY_SYSTEMS_H

#include <SDL2/SDL.h>

#include "../world.h"

void render_system(World* world, SDL_Surface *surface);
void movement_system(World* world, unsigned int player_entity);
void add_force(World* world, unsigned int entity, float magnitude, float dir);
void add_force_acceleration_x(MovementComponent& movement, float magnitude, float dir, float friction);
void add_force_acceleration_y(MovementComponent& movement, float magnitude, float dir, float friction);
void apply_force(World* world, unsigned int entity);
void update_system(World* world);

#endif
