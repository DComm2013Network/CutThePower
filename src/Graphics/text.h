#ifndef GRAPHICS_TEXT_H
#define GRAPHICS_TEXT_H

#include <SDL2/SDL.h>
#include "../world.h"

#define	TITLE_FONT			0
#define MENU_FONT			1
#define CHAT_FONT			2
#define PLAYER_FONT			3
#define OTHER_TEAM_FONT		4
#define SERVER_FONT			5

void init_fonts();
void cleanup_fonts();
void render_text(World *world, unsigned int entity, const char *text, int font_type);
SDL_Surface *draw_text(const char *text, int font_type);
int get_text_width(const char *text, int font_type);
int get_text_height(const char *text, int font_type);

#endif
