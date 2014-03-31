#ifndef CHAT_H
#define CHAT_H

#include <SDL2/SDL.h>

#include "../world.h"
#include "../Network/Packets.h"

#define CHAT_LINES 					5
#define CHAT_LINE_LENGTH			64

#define CHAT_LINE_DISSAPEAR_TIME	5000
#define CHAT_LINE_DISSAPEAR_LENGTH	1000

#define CHAT_LINE_HEIGHT			40
#define CHAT_LINE_GAP				4
#define CHAT_SURFACE_WIDTH			(CHAT_LINE_LENGTH * 40)
#define CHAT_SURFACE_HEIGHT			(CHAT_LINES * (CHAT_LINE_HEIGHT + CHAT_LINE_GAP))

typedef struct {
	char text[MAX_MESSAGE + MAX_NAME + 2];
	unsigned int start_ticks;
} chat_line;

void init_chat();
void chat_add_line(const char *text);
void chat_update();
void chat_render(SDL_Surface *surface);
unsigned int create_chat(World *world);

#endif
