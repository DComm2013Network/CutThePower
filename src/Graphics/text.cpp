/** @ingroup Graphics
 * @{ */
/** @file text.cpp */
/** @} */
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "text.h"
#include "../world.h"
#include "../components.h"

#define BIG_FONT_SIZE			83
#define BIG_FONT_OUTLINE		3

#define SMALL_FONT_SIZE			42
#define SMALL_FONT_OUTLINE		2

#define CHAT_FONT_SIZE			16
#define CHAT_FONT_OUTLINE		1

TTF_Font *big_font;
TTF_Font *small_font;
TTF_Font *chat_font;

int get_font(int font_type, TTF_Font **font, int *outline_size);

/**
 * Loads in the font files used.
 *
 *
 * @designer Jordan Marling
 * @designer Cory Thomas
 * @designer Vincent Lau
 *
 * @author Jordan Marling
 */
void init_fonts() {
	
	TTF_Init();
	
	big_font = TTF_OpenFont("assets/Graphics/font/font.ttf", BIG_FONT_SIZE);
	small_font = TTF_OpenFont("assets/Graphics/font/font.ttf", SMALL_FONT_SIZE);
	chat_font = TTF_OpenFont("assets/Graphics/font/font.ttf", CHAT_FONT_SIZE);
	
	if (!big_font || !small_font || !chat_font) {
		printf("Error loading font file.\n");
	}
	
}

void cleanup_fonts() {
	
	TTF_CloseFont(big_font);
	TTF_CloseFont(small_font);
	TTF_CloseFont(chat_font);
	
}

/**
 * Returns a surface with the text drawn to it.
 *
 * @param text The text to be drawn
 * @param font The font to use while drawing the text
 * @param outline_size The size of the black border around the text in pixels
 *
 * @designer Jordan Marling
 * @designer Cory Thomas
 * @designer Vincent Lau
 *
 * @author Vincent Lau
 * @author Jordan Marling
 */
SDL_Surface *draw_text(const char *text, int font_type) {
	
	SDL_Surface *outline_surface;
	SDL_Surface *text_surface;
	TTF_Font *font;
	int outline_size;
	SDL_Color outline = {0, 0, 0};
	SDL_Color colour = {255, 0, 0};
	
	if (text == 0 || strlen(text) == 0)
		return 0;
	
	if (get_font(font_type, &font, &outline_size) == 0) {
		printf("Error loading font type in draw_text: %d\n", font_type);
	}
	
	switch(font_type) {
		case PLAYER_FONT:
			colour = { 0xFF, 0xFF, 0xFF };
			break;
			
		case OTHER_TEAM_FONT:
			colour = { 0x88, 0x88, 0x88 };
			break;
		
		case SERVER_FONT:
			colour = { 0xFF, 0xFF, 0x00 };
			break;
		
	}
	
	TTF_SetFontOutline(font, outline_size);
	if ((text_surface = TTF_RenderText_Blended(font, text, outline)) == 0) {
		printf("Error rendering the text.\n");
	}
	
	TTF_SetFontOutline(font, 0);
	if ((outline_surface = TTF_RenderText_Blended(font, text, colour)) == 0) {
		printf("Error rendering the outline of the big text.\n");
	}
	
	//blit the outline onto the text
	SDL_BlitSurface(outline_surface, NULL, text_surface, NULL);
	SDL_FreeSurface(outline_surface);
	
	return text_surface;
}

/**
 * Draws text to an entity and updates the width/height values accordingly
 *
 * @param world The world structure that holds all the entities
 * @param entity The entity to draw the text to
 * @param text The text to be drawn
 * @param font The font to be used
 * @param outline_size The size of the black border around the text
 *
 * @designer Jordan Marling
 * @designer Cory Thomas
 * @designer Vincent Lau
 *
 * @author Vincent Lau
 * @author Jordan Marling
 */
void render_text(World *world, unsigned int entity, const char *text, int font_type) {
	
	int width = get_text_width(text, font_type);
	int height = get_text_height(text, font_type);
	
	world->position[entity].x -= (width / 2);
	world->position[entity].width = width;
	world->position[entity].height = height;
	
	world->renderPlayer[entity].width = width;
	world->renderPlayer[entity].height = height;
	world->renderPlayer[entity].playerSurface = draw_text(text, font_type);
	
}

int get_font(int font_type, TTF_Font **font, int *outline_size) {
	
	switch(font_type) {
		
		case TITLE_FONT:
			*font = big_font;
			*outline_size = BIG_FONT_OUTLINE;
			break;
		
		case MENU_FONT:
			*font = small_font;
			*outline_size = SMALL_FONT_OUTLINE;
			break;
		
		case PLAYER_FONT:
		case SERVER_FONT:
		case OTHER_TEAM_FONT:
		case CHAT_FONT:
			*font = chat_font;
			*outline_size = CHAT_FONT_OUTLINE;
			break;
		
		default: //did not find the font.
			printf("Did not find font %d\n", font_type);
			return 0;
			break;
	}
	
	return 1;
}

/**
 * Returns the width of a string drawn
 *
 * @param text The text to be used
 *
 * @designer Vincent Lau
 *
 * @author Vincent Lau
 */
int get_text_width(const char *text, int font_type) {
	
	int width, height;
	
	TTF_Font *font;
	int outline_size;
	
	if (get_font(font_type, &font, &outline_size) == 0) {
		printf("Error loading font type in get_text_width: %d\n", font_type);
	}
	
	
	if (TTF_SizeText(font, text, &width, &height)) {
		printf("Error getting the size of the big text.\n");
	}
	
	return width;
}

/**
 * Returns the height of a string drawn
 *
 * @param text The text to be used
 *
 * @designer Vincent Lau
 *
 * @author Vincent Lau
 */
int get_text_height(const char *text, int font_type) {
	
	int width, height;
	
	TTF_Font *font;
	int outline_size;
	
	if (get_font(font_type, &font, &outline_size) == 0) {
		printf("Error loading font type in get_text_width: %d\n", font_type);
	}
	
	
	if (TTF_SizeText(font, text, &width, &height)) {
		printf("Error getting the size of the big text.\n");
	}
	
	return height;
}
