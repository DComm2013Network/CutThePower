/** @ingroup Graphics
 * @{ */
/** @file render_system.cpp */
/** @} */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../world.h"
#include "components.h"
#include "systems.h"
#include "../Input/menu.h"

#include <cstdio>
#include <cstring>

#define SYSTEM_MASK (COMPONENT_RENDER_PLAYER | COMPONENT_POSITION) /**< The entity must have a render player and position component
                                                                    * for processing by this system. */
#define IMAGE_WIDTH			150
                                                                    
extern SDL_Rect map_rect; /**< The rectangle containing the map. */

const char *character_map = ".01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
SDL_Surface *text_chars[38]; /**< we have 37 different characters. */
SDL_Surface *ibeam;

/**
 * Render a player onto the map. 
 *
 * Player is added as a texture then painted onto the surface. Multiple players can
 * be added.
 *
 * Revisions: 
 * <ol>
 *     <li>Sam Youssef - February 24, 2014: Updated the player render to get it working with multiple players</li>
 *     <li>Mateusz Siwoski - February 28, 2014: Modified to pass in the Position Component, fixed not loading the image
 *         every time the frame is drawn</li>
 *     <li>Jordan Marling/Mat Siwoski - March 6, 2014: Updated support for the camera.</li>
 * </ol>
 *
 * @param[in,out] world   A reference to the world structure containing entities to render.
 * @param[in,out] surface Surface player is being rendered to.
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Mat Siwoski
 */
void render_player_system(World& world, SDL_Surface* surface) {
	
	unsigned int entity;
	int i, j;
	RenderPlayerComponent 	*renderPlayer;
	PositionComponent 		*position;
	TextFieldComponent		*text;
	SDL_Rect playerRect;
	
	
	
	for(entity = 0; entity < MAX_ENTITIES; entity++){
		
		if (IN_THIS_COMPONENT(world.mask[entity], SYSTEM_MASK)){
			
			position = &(world.position[entity]);
			renderPlayer = &(world.renderPlayer[entity]);			
			
			playerRect.x = position->x + map_rect.x;
			playerRect.y = position->y + map_rect.y;
			playerRect.w = renderPlayer->width;
			playerRect.h = renderPlayer->height;
			
			if (IN_THIS_COMPONENT(world.mask[entity], COMPONENT_BUTTON)) {
				
				if (world.button[entity].hovered) {
					playerRect.x -= 5;
					playerRect.y -= 5;
					playerRect.w += 10;
					playerRect.h += 10;
				}
				
			}
			
			SDL_BlitScaled(renderPlayer->playerSurface, NULL, surface, &playerRect);
			
			//check if a textbox.
			if (IN_THIS_COMPONENT(world.mask[entity], COMPONENT_TEXTFIELD)) {
				
				text = &(world.text[entity]);
				
				playerRect.y += 5;
				playerRect.x -= (IMAGE_WIDTH / 2);
				
				for(i = 0; i < text->length; i++) {
					
					for(j = 0; j < strlen(character_map); j++) {
						if (character_map[j] == text->text[i])
							break;
					}
					
					playerRect.x += CHARACTER_WIDTH;
					
					//If we find a character, draw it
					if (j < strlen(character_map)) {
						
						SDL_BlitSurface(text_chars[j], NULL, surface, &playerRect);
						
					}
					
				}
				
				if (text->focused) {
					playerRect.x += CHARACTER_WIDTH;
					playerRect.y += 5;
					
					SDL_BlitSurface(ibeam, NULL, surface, &playerRect);
				}
				
			}
		}
	}
}


void init_render_player_system() {
	
	int i;
	const int pos = strlen("assets/Graphics/menu/keymap/keymap_");
	char filename[64];
	
	strcpy(filename, "assets/Graphics/menu/keymap/keymap_");
	
	
	for(i = 0; i < strlen(character_map); i++) {
		
		strncpy((char*)(filename + pos), &character_map[i], 1);
		strcpy((char*)(filename + pos + 1), ".png");
		
		text_chars[i] = IMG_Load(filename);
		
		if (text_chars[i] == 0) {
			printf("Error loading %c\n", character_map[i]);
		}
	}
	
	ibeam = IMG_Load("assets/Graphics/menu/keymap/keymap_ibeam.png");
	
}
