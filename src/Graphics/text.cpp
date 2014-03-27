#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "text.h"
#include "../world.h"
#include "../components.h"

#define BIG_FONT				83
#define SMALL_FONT				42
#define BIG_FONT_OUTLINE		3
#define SMALL_FONT_OUTLINE		2

TTF_Font *big;
TTF_Font *small;

void render_text(World *world, unsigned int entity, const char *text, TTF_Font *font, int outline_size);

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
	
	big = TTF_OpenFont("assets/Graphics/font/font.ttf", BIG_FONT);
	small = TTF_OpenFont("assets/Graphics/font/font.ttf", SMALL_FONT);
	
	if (!big || !small) {
		printf("Error loading font file.\n");
	}
	
}

void cleanup_fonts() {
	
	TTF_CloseFont(big);
	TTF_CloseFont(small);
	
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
SDL_Surface *draw_text_surface(const char *text, TTF_Font *font, int outline_size) {
	
	SDL_Surface *outline_surface;
	SDL_Surface *text_surface;
	SDL_Rect size;
	SDL_Color outline = {0, 0, 0};
	SDL_Color colour = {255, 0, 0};
	
	if (text == 0 || strlen(text) == 0)
		return 0;
	
	TTF_SetFontOutline(font, outline_size);
	
	if ((text_surface = TTF_RenderText_Blended(font, text, outline)) == 0) {
		printf("Error rendering the text.\n");
	}
	
	TTF_SetFontOutline(font, 0);
	
	if ((outline_surface = TTF_RenderText_Blended(font, text, colour)) == 0) {
		printf("Error rendering the outline of the big text.\n");
	}
	
	//blit the outline onto the text
	
	size.x = 0;
	size.y = 0;
	size.w = 0;
	size.h = 0;
	
	SDL_BlitSurface(outline_surface, NULL, text_surface, &size);
	
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
void render_text(World *world, unsigned int entity, const char *text, TTF_Font *font, int outline_size) {
	
	int width, height;
	
	
	if (TTF_SizeText(font, text, &width, &height)) {
		printf("Error getting the size of the big text.\n");
	}
	
	world->position[entity].x -= (width / 2);
	world->position[entity].width = width;
	world->position[entity].height = height;
	
	world->renderPlayer[entity].width = width;
	world->renderPlayer[entity].height = height;
	world->renderPlayer[entity].playerSurface = draw_text_surface(text, font, outline_size);
	
}

/**
 * Draws big text to an entity
 *
 * @param world The world structure that holds all the entities
 * @param entity The entity to draw the text to
 * @param text The text to be drawn
 *
 * @designer Jordan Marling
 * @designer Cory Thomas
 * @designer Vincent Lau
 *
 * @author Jordan Marling
 */
void render_big_text(World *world, unsigned int entity, const char *text) {
	
	render_text(world, entity, text, big, BIG_FONT_OUTLINE);
	
}

/**
 * Draws small text to an entity
 *
 * @param world The world structure that holds all the entities
 * @param entity The entity to draw the text to
 * @param text The text to be drawn
 *
 * @designer Jordan Marling
 * @designer Cory Thomas
 * @designer Vincent Lau
 *
 * @author Jordan Marling
 */
void render_small_text(World *world, unsigned int entity, const char *text) {

	render_text(world, entity, text, small, SMALL_FONT_OUTLINE);
	
}

/**
 * Draws big text to a surface
 *
 * @param text The text to be drawn
 *
 * @designer Vincent Lau
 *
 * @author Vincent Lau
 */
SDL_Surface *draw_big_text(const char *text) {
	
	return draw_text_surface(text, big, BIG_FONT_OUTLINE);
	
}

/**
 * Draws small text to a surface
 *
 * @param text The text to be drawn
 *
 * @designer Vincent Lau
 *
 * @author Vincent Lau
 */
SDL_Surface *draw_small_text(const char *text) {
	
	return draw_text_surface(text, small, SMALL_FONT_OUTLINE);
	
}

/**
 * Returns the width of a string drawn with small text
 *
 * @param text The text to be used
 *
 * @designer Vincent Lau
 *
 * @author Vincent Lau
 */
int get_small_text_width(const char *text) {
	
	int width, height;
	
	if (TTF_SizeText(small, text, &width, &height)) {
		printf("Error getting the size of the big text.\n");
	}
	
	return width;
}
