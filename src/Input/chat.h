#ifndef CHAT_H
#define CHAT_H

#include <SDL2/SDL.h>

#include "../Graphics/text.h"
#include "../world.h"
#include "../Network/Packets.h"

#define CHAT_LINES 					6
#define CHAT_LINE_LENGTH			198

#define CHAT_LINE_DISSAPEAR_TIME	10000
#define CHAT_LINE_DISSAPEAR_LENGTH	1000

#define CHAT_LINE_HEIGHT			20
#define CHAT_LINE_GAP				4
#define CHAT_SURFACE_WIDTH			(CHAT_LINE_LENGTH * 20) - 40
#define CHAT_SURFACE_HEIGHT			(CHAT_LINES * (CHAT_LINE_HEIGHT + CHAT_LINE_GAP))

/**
 * Contains information of a chat line 
 * 
 * 
 * @struct Chat Lines
 */
typedef struct {
	char text[MAX_MESSAGE + MAX_NAME + 3];
	unsigned int start_ticks;
	int font_type;
} chat_line;

void init_chat();
void chat_add_line(const char *text, int font_type);
void chat_update();
void chat_render(SDL_Surface *surface);
unsigned int create_chat(World *world);

#endif
