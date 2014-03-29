/** @ingroup Graphics
 * @{ */
/** @file render_system.cpp */
/** @} */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "../world.h"
#include "components.h"
#include "systems.h"
#include "text.h"
#include "../Input/menu.h"

/*SAM**/
void make_surrounding_tiles_visible(struct fogOfWarPlayerPosition *fowp);
/******/

#define SYSTEM_MASK (COMPONENT_RENDER_PLAYER | COMPONENT_POSITION) /**< The entity must have a render player and position component
                                                                    * for processing by this system. */
#define IMAGE_WIDTH			400

                                                                    
extern SDL_Rect map_rect; /**< The rectangle containing the map. */

//const char *character_map = ".01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
//SDL_Surface *text_chars[38]; /**< we have 37 different characters. */
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
void render_player_system(World& world, SDL_Surface* surface, struct fogOfWarStruct *fow) {
	
	unsigned int entity;
	RenderPlayerComponent 	*renderPlayer;
	PositionComponent 		*position;
	TextFieldComponent		*text;
	SDL_Rect playerRect;
	
	SDL_Rect clipRect;
	
	for(entity = 0; entity < MAX_ENTITIES; entity++){
		
		if (IN_THIS_COMPONENT(world.mask[entity], SYSTEM_MASK)){
			
			position = &(world.position[entity]);
			renderPlayer = &(world.renderPlayer[entity]);			
			
			playerRect.x = position->x + map_rect.x;
			playerRect.y = position->y + map_rect.y;
			playerRect.w = renderPlayer->width;
			playerRect.h = renderPlayer->height;
			if (IN_THIS_COMPONENT(world.mask[entity], COMPONENT_COLLISION)) {
				playerRect.x -= renderPlayer->width / 2;
				playerRect.y -= renderPlayer->height / 2;
			}
			
			if (IN_THIS_COMPONENT(world.mask[entity], COMPONENT_BUTTON)) {
				
				if (world.button[entity].hovered) {
					playerRect.x -= 5;
					playerRect.y -= 5;
					playerRect.w += 10;
					playerRect.h += 10;
				}
				
			}
			
			clipRect.x = -playerRect.x;
			clipRect.y = -playerRect.y;
			clipRect.w = playerRect.w;
			clipRect.h = playerRect.h;
			
			if (clipRect.x < 0)
				clipRect.x = 0;
			
			if (clipRect.y < 0)
				clipRect.y = 0;
			
			if (clipRect.h > HEIGHT - playerRect.y)
				clipRect.h = HEIGHT - playerRect.y;
			
			if (clipRect.w > WIDTH - playerRect.x)
				clipRect.w = WIDTH - playerRect.x;
			
			SDL_BlitScaled(renderPlayer->playerSurface, &clipRect, surface, &playerRect);
			
			
			/*SAM*********************************/		
			if (IN_THIS_COMPONENT(world.mask[entity], COMPONENT_PLAYER)) {	
	
				struct fogOfWarPlayerPosition fowp;
		
				fowp.world = &world;
				fowp.pos   = position;
				fowp.fow   = fow;
	
				make_surrounding_tiles_visible(&fowp);	
			}
			/*************************************/		
			
			
			//check if a textbox.
			if (IN_THIS_COMPONENT(world.mask[entity], COMPONENT_TEXTFIELD)) {
				
				text = &(world.text[entity]);
				
				playerRect.x += 10;
				playerRect.y += 8;
				
				//TODO: Store the text in a component instead of creating it every frame.
				//Perhaps we should store it in the renderPlayer component and draw the
				//textbox if it has a text field component?
				SDL_BlitSurface(draw_small_text(text->text), NULL, surface, &playerRect);
				
				if (text->focused) {
					playerRect.x += get_small_text_width(text->text) + 1;
					
					SDL_BlitSurface(ibeam, NULL, surface, &playerRect);
				}
			}
		}
	}
}


void init_render_player_system() {
	
	/*int i;
	const int pos = strlen("assets/Graphics/screen/menu/keymap/keymap_");
	//char filename[64];
	char *filename = (char*)malloc(sizeof(char) * 128);
	
	strcpy(filename, "assets/Graphics/screen/menu/keymap/keymap_");
	
	
	for(i = 0; i < strlen(character_map); i++) {
		
		strncpy((char*)(filename + pos), &character_map[i], 1);
		strcpy((char*)(filename + pos + 1), ".png");
		
		text_chars[i] = IMG_Load(filename);
		
		if (text_chars[i] == 0) {
			printf("Error loading %c\n", character_map[i]);
		}
	}*/
	
	if ((ibeam = IMG_Load("assets/Graphics/screen/menu/ibeam.png")) == 0) {
		printf("Error loading ibeam image.\n");
	}
	//free(filename);
}
