/** @ingroup Input */
/*@{*/

/**
 * @date 2014/02/18
 *
 * 
 * @file chat.cpp
 */
/*@}*/
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

/**
 * Initializes the chat surface that is drawn on.
 *
 *
 * @designer Jordan Marling
 * @author Jordan Marling
 *
 */
void init_chat() {
	//masking
	Uint32 rmask, gmask, bmask, amask;

    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
	
	chat_surface = SDL_CreateRGBSurface(0, CHAT_SURFACE_WIDTH, CHAT_SURFACE_HEIGHT, 32, rmask, gmask, bmask, amask);
	
}

/**
 * Adds a line to the circular buffer of text to be drawn to the screen.
 *
 *
 * @param[in]		text The text that is drawn to the screen
 * @param[in] 		font_type The type of font that is drawn
 *
 * @designer Jordan Marling
 * @author Jordan Marling
 *
 */
void chat_add_line(const char *text, int font_type) {
		
	if(text == NULL) {
		return;
	}

	strncpy((char*)chat_text[end_text].text, text, CHAT_LINE_LENGTH);
	chat_text[end_text].start_ticks = SDL_GetTicks();
	chat_text[end_text].font_type = font_type;
	
	end_text++;
	//if the last position is beyond the length, go to the beginning and append there next.
	if (end_text >= CHAT_LINES) {
		end_text = 0;
	}

	//If the last position overlaps the first position, move the first position pointer to the next value.
	if (start_text == end_text) {
		start_text++;
		if (start_text >= CHAT_LINES) {
			start_text = 0;
		}
	}
}

/**
 * Updates the surface with the text currently in the circular buffer.
 *
 *
 *
 * @designer Jordan Marling
 * @author Jordan Marling
 *
 */
void chat_update() {
	
	int i, index;
	
	Uint8 alpha;
	float alpha_percentage;
	
	SDL_Surface *temp_text;
	SDL_Rect text_rect;
	
	text_rect.x = 0;
	text_rect.w = CHAT_SURFACE_WIDTH;
	text_rect.h = CHAT_LINE_HEIGHT;
	
	//clear the text rectangle
	SDL_FillRect(chat_surface, NULL, SDL_MapRGBA(chat_surface->format, 0, 0, 0, 0));
	SDL_SetSurfaceBlendMode(chat_surface, SDL_BLENDMODE_BLEND);
	
	//draw each of the lines
	for(i = 0, index = start_text; i <= CHAT_LINES; i++, index++) {

		if (index >= CHAT_LINES) {
			index = 0;
		}

		if (index == end_text) {
			break;
		}
		
		if (strlen(chat_text[i].text) == 0) {
			continue;
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
		
		temp_text = draw_text(chat_text[index].text, chat_text[index].font_type);
		
		SDL_PixelFormat* fmt = temp_text->format;
		
		//change the alpha value of the text for cool effect.
		if (alpha < 255) {
			
			for(int y = 0; y < temp_text->h; y++) {
				for(int x = 0; x < temp_text->w; x++) {
					
					Uint32* pixel_ptr = (Uint32 *)((Uint8 *)temp_text->pixels + y * temp_text->pitch + x * fmt->BytesPerPixel);

					// Get the old pixel components.
					Uint8 r, g, b, a;
					SDL_GetRGBA( *pixel_ptr, fmt, &r, &g, &b, &a );
					
					if (alpha < a)
						a = alpha;
					
					// Set the pixel with the new alpha.
					*pixel_ptr = SDL_MapRGBA( fmt, r, g, b, a );
					
				}
			}
			
		}
		
		text_rect.y = i * (CHAT_LINE_HEIGHT + CHAT_LINE_GAP);
		
		SDL_BlitSurface(temp_text, NULL, chat_surface, &text_rect);
		SDL_FreeSurface(temp_text);
	}
}

/**
 * Renders the chat to the main surface.
 *
 * @param[in, out]	surface The surface to draw the text to.
 *
 * @designer Jordan Marling
 * @author Jordan Marling
 *
 */
void chat_render(SDL_Surface *surface) {
	
	SDL_Rect rect;
	
	rect.x = 40;
	rect.y = (HEIGHT - CHAT_SURFACE_HEIGHT - 50);
	rect.w = CHAT_SURFACE_WIDTH;
	rect.h = CHAT_SURFACE_HEIGHT;
	
	chat_update();
	//SDL_FillRect(surface, NULL, 0xff0000);
	//SDL_SetSurfaceAlphaMod(chat_surface, 255);
	SDL_BlitSurface(chat_surface, NULL, surface, &rect);
	
}
/**
 * Creates the text field for the user to type text into the chat.
 *
 *
 * @param[in, out]	world 	Pointer to WORLD (structure containing "world" information, entities/components)
 *
 * @designer Jordan Marling
 * @author Jordan Marling
 *
 */
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
	
	world->position[entity].x = 20;
	world->position[entity].y = HEIGHT - 80;
	
	world->text[entity].name = (char*)calloc(strlen("chat_text") + 1, sizeof(char));
	strcpy(world->text[entity].name, "chat_text");
	world->text[entity].text = (char*)calloc(MAX_MESSAGE + MAX_NAME + 3, sizeof(char));

	
	world->text[entity].length = 0;

	world->text[entity].focused = true;
	world->text[entity].number = false;

	world->text[entity].max_length = MAX_MESSAGE + MAX_NAME + 3;//stuff
	
	return entity;
}
