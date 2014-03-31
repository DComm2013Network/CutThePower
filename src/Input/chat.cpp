
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../Network/Packets.h"
#include "chat.h"
#include "../Graphics/text.h"
#include "menu.h"

chat_line chat_text[CHAT_LINES];
int start_text = 0;
int end_text = 0;
SDL_Surface *chat_surface;

void init_chat() {
	
	//masking
	Uint32 rmask, gmask, bmask, amask;

    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
	
	chat_surface = SDL_CreateRGBSurface(0, CHAT_SURFACE_WIDTH, CHAT_SURFACE_HEIGHT, 32, rmask, gmask, bmask, amask);
	
}

void chat_add_line(const char *text){
		
	if(text == NULL)
		return;

	strncpy((char*)chat_text[end_text].text, text, CHAT_LINE_LENGTH);
	chat_text[end_text].start_ticks = SDL_GetTicks();
	
	end_text++;
	//if the last position is beyond the length, go to the beginning and append there next.
	if (end_text >= CHAT_LINES)
	{
		end_text = 0;
	}

	//If the last position overlaps the first position, move the first position pointer to the next value.
	if (start_text == end_text) 
	{
		start_text++;
		if (start_text >= CHAT_LINES)
		{
			start_text = 0;
		}
	}
}

void chat_update() {
	
	int i, index;
	
	Uint8 alpha;
	float alpha_percentage;
	
	SDL_Surface *temp_text;
	SDL_Rect text_rect;
	
	text_rect.x = 0;
	text_rect.w = CHAT_SURFACE_WIDTH;
	text_rect.h = CHAT_LINE_HEIGHT;
	
	//set blend mode to none to overwrite.
	//SDL_SetSurfaceBlendMode(chat_surface, SDL_BLENDMODE_NONE);
	SDL_FillRect(chat_surface, NULL, SDL_MapRGBA(chat_surface->format, 0, 0, 0, 0));
	SDL_SetSurfaceBlendMode(chat_surface, SDL_BLENDMODE_BLEND);
	SDL_SetSurfaceAlphaMod(chat_surface, 255);
	
	for(i = 0, index = start_text; i <= CHAT_LINES; i++, index++) 
	{

		if (index >= CHAT_LINES)
		{
			index = 0;
		}

		if (index == end_text)
		{
			break;
		}
		
		alpha_percentage = ((float)(SDL_GetTicks() - chat_text[index].start_ticks) - CHAT_LINE_DISSAPEAR_TIME) / CHAT_LINE_DISSAPEAR_LENGTH;
		
		if (alpha_percentage < 0)
			alpha_percentage = 0;
		else if (alpha_percentage > 1)
			alpha_percentage = 1;
		
		//flip the alpha because 0 is transparent and 255 is opaque
		alpha_percentage *= -1;
		alpha_percentage += 1;
		alpha = (Uint8)(alpha_percentage * 255);
		
		
		temp_text = draw_small_text(chat_text[index].text);
		SDL_SetSurfaceAlphaMod(temp_text, alpha);
		
		text_rect.y = i * (CHAT_LINE_HEIGHT + CHAT_LINE_GAP);
		
		//set blend mode for transparency
		//SDL_SetSurfaceBlendMode(temp_text, SDL_BLENDMODE_BLEND);
		SDL_BlitSurface(temp_text, NULL, chat_surface, &text_rect);
		SDL_FreeSurface(temp_text);
	}
}

void chat_render(SDL_Surface *surface) {
	
	SDL_Rect rect;
	
	rect.x = 40;
	rect.y = (HEIGHT - CHAT_SURFACE_HEIGHT - 100);
	rect.w = CHAT_SURFACE_WIDTH;
	rect.h = CHAT_SURFACE_HEIGHT;
	
	chat_update();
	//SDL_FillRect(surface, NULL, 0xff0000);
	//SDL_SetSurfaceAlphaMod(chat_surface, 255);
	SDL_BlitSurface(chat_surface, NULL, surface, &rect);
	
}

unsigned int create_chat(World *world) {
	
	unsigned int entity = create_entity(world, COMPONENT_MENU_ITEM | COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_TEXTFIELD | COMPONENT_MOUSE);
	
	
	world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/screen/menu/text_field.png");
	
	world->renderPlayer[entity].width = CHAT_SURFACE_WIDTH;
	world->renderPlayer[entity].height = SMALL_TEXT_HEIGHT;
	world->position[entity].width = CHAT_SURFACE_WIDTH;
	world->position[entity].height = SMALL_TEXT_HEIGHT;
	world->text[entity].max_length = 64;

	if (!world->renderPlayer[entity].playerSurface) {
		printf("Error loading image in create_textfield.\n");
	}
	
	world->position[entity].x = 40;
	world->position[entity].y = HEIGHT - 80;
	
	world->text[entity].name = (char*)calloc(strlen("chat_text") + 1, sizeof(char));
	strcpy(world->text[entity].name, "chat_text");
	world->text[entity].text = (char*)calloc(MAX_MESSAGE + MAX_NAME + 2, sizeof(char));

	
	world->text[entity].length = 0;

	world->text[entity].focused = true;
	world->text[entity].number = false;

	world->text[entity].max_length = MAX_MESSAGE + MAX_NAME + 2;//stuff
	
	return entity;
}
