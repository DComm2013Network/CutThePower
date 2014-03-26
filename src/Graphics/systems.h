/** @ingroup Graphics */
/** @{ */
/** @file systems.h */
/** @} */
#ifndef GRAPHICS_SYSTEMS_H
#define GRAPHICS_SYSTEMS_H

#include "../world.h"
#include "map.h"

/* POSSIBLY TEMPORARY!!! passing playerFilename may not be needed if gameplay gives us a complete player struct. */
void render_player_system(World& world, SDL_Surface* surface);
void init_render_player_system();
<<<<<<< HEAD
void animation_system(World *world, unsigned int *entity, int send_router_fd[2], int rcv_router_fd[2]);
=======
void animation_system(World *world);
>>>>>>> origin/Development

int load_animation(char *filename, World *world, unsigned int entity);
void play_animation(World *world, unsigned int entity, char *animation_name);
void cancel_animation(World *world, unsigned int entity, char *animation_name);

#endif
