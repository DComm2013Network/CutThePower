#ifndef GRAPHICS_TEXT_H
#define GRAPHICS_TEXT_H

#include <SDL2/SDL.h>
#include "../world.h"

void init_fonts();
void render_big_text(World *world, unsigned int entity, const char *text);
void render_small_text(World *world, unsigned int entity, const char *text);

#endif
