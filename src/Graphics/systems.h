/** @ingroup Graphics */
/** @{ */
/** @file systems.h */
/** @} */
#ifndef GRAPHICS_SYSTEMS_H
#define GRAPHICS_SYSTEMS_H

#include "../world.h"
#include "map.h"

void render_player_system(World& world, SDL_Surface* surface, FowComponent *fow);
void render_menu_system(World *world, SDL_Surface *surface);
void init_render_player_system();
void animation_system(World *world);
void cutscene_system(World *world);

int load_animation(const char *filename, World *world, unsigned int entity);
void play_animation(World *world, unsigned int entity, const char *animation_name);
void cancel_animation(World *world, unsigned int entity);

unsigned int load_cutscene(const char *filename, World *world, int id);

#endif
