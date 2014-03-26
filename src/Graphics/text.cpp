#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "text.h"
#include "../world.h"
#include "../components.h"

#define BIG_FONT				83
#define SMALL_FONT				48
#define BIG_FONT_OUTLINE		1
#define SMALL_FONT_OUTLINE		1

TTF_Font *big;
TTF_Font *small;

void render_text(World *world, unsigned int entity, const char *text, TTF_Font *font, int outline_size);

void init_fonts() {
	
	TTF_Init();
	
	big = TTF_OpenFont("assets/Graphics/font/font.ttf", BIG_FONT);
	small = TTF_OpenFont("assets/Graphics/font/font.ttf", SMALL_FONT);
	
	if (!big || !small) {
		printf("Error loading font file.\n");
	}
	
}

void render_text(World *world, unsigned int entity, const char *text, TTF_Font *font, int outline_size) {
	
	SDL_Surface *outline_surface;
	SDL_Rect size;
	SDL_Color outline = {0, 0, 0};
	SDL_Color colour = {255, 0, 0};
	int width, height;
	
	TTF_SetFontOutline(font, 0);
	
	if ((world->renderPlayer[entity].playerSurface = TTF_RenderText_Blended(font, text, colour)) == 0) {
		printf("Error rendering the big text.\n");
	}
	
	TTF_SetFontOutline(font, outline_size);
	
	if ((outline_surface = TTF_RenderText_Solid(font, text, outline)) == 0) {
		printf("Error rendering the outline of the big text.\n");
	}
	
	
	if (TTF_SizeText(font, text, &width, &height)) {
		printf("Error getting the size of the big text.\n");
	}
	
	world->position[entity].x -= (width / 2);
	world->position[entity].width = width;
	world->position[entity].height = height;
	
	world->renderPlayer[entity].width = width;
	world->renderPlayer[entity].height = height;
	
	//blit the outline onto the text
	
	size.x = 0;
	size.y = 0;
	size.w = width;
	size.h = height;
	
	SDL_BlitSurface(outline_surface, NULL, world->renderPlayer[entity].playerSurface, &size);
	
}

void render_big_text(World *world, unsigned int entity, const char *text) {
	
	render_text(world, entity, text, big, BIG_FONT_OUTLINE);
	
}

void render_small_text(World *world, unsigned int entity, const char *text) {

	render_text(world, entity, text, small, SMALL_FONT_OUTLINE);
	
}
