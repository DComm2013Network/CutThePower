#ifndef GRAPHICS_TEXT_H
#define GRAPHICS_TEXT_H

#include <SDL2/SDL.h>
#include "../world.h"

void init_fonts();
SDL_Surface *draw_big_text(const char *text);
SDL_Surface *draw_small_text(const char *text);
void render_big_text(World *world, unsigned int entity, const char *text);
void render_small_text(World *world, unsigned int entity, const char *text);
int get_small_text_width(const char *text);

#endif
