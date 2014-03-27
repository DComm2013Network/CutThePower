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

#define SYSTEM_MASK (COMPONENT_RENDER_PLAYER | COMPONENT_POSITION) /**< The entity must have a render player and position component
                                                                    * for processing by this system. */
#define IMAGE_WIDTH			400
                                                                    
extern SDL_Rect map_rect; /**< The rectangle containing the map. */

const char *character_map = ".01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
SDL_Surface *text_chars[38]; /**< we have 37 different characters. */
SDL_Surface *ibeam;

int subOne(int n);
int addOne(int n);
void makeVisible(struct fogOfWarStruct *fow, PositionComponent *position);

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
	unsigned int i, j;
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

	
	/*SAM*********************************/
	makeVisible(fow, position);
	/*************************************/

}

/*SAM******************************/
void makeVisible(struct fogOfWarStruct *fow, PositionComponent *position)
{

/*******************
 5 | 5 | 3 | 5 | 5 |
-------------------
 5 | 4 | 2 | 3 | 5 |
-------------------
 3 | 2 | 1 | 2 | 3 |
-------------------
 5 | 4 | 2 | 4 | 5 |
-------------------
 5 | 5 | 3 | 5 | 5 |
*******************/


	int xPos = (double)position->x / TILE_WIDTH;
	int yPos = (double)position->y / TILE_HEIGHT;

	int by = addOne(yPos);
	int ty = subOne(yPos);
	int lx = subOne(xPos);
	int rx = addOne(xPos);
	
	fow -> xOffset = -map_rect.x;
	fow -> yOffset = -map_rect.y;
	
	
	// standing on it
	fow -> tiles[yPos][xPos].visible = 1; // 1


	// immediate top & bottom
	fow -> tiles[ty][xPos].visible = 1; // 2 // top
	fow -> tiles[by][xPos].visible = 1; // 2 // bottom

	// immediate left & right
	fow -> tiles[yPos][rx].visible = 1; // 2 // right
	fow -> tiles[yPos][lx].visible = 1; // 2 // left
	
	
	// far top & bottom
	fow -> tiles[subOne(ty)][xPos].visible = 1; // 3 // top
	fow -> tiles[addOne(by)][xPos].visible = 1; // 3 // bottom
		
	// far left & right
	fow -> tiles[yPos][ addOne(rx) ].visible = 1; // 3 // right
	fow -> tiles[yPos][ subOne(lx) ].visible = 1; // 3 // left
	
	
	// immediate right corners
	fow -> tiles[ty][rx].visible = 1; // 4 // top right
	fow -> tiles[by][rx].visible = 1; // 4 // bottom right

	// immediate left corners
	fow -> tiles[ty][lx].visible = 1; // 4 // top left
	fow -> tiles[by][lx].visible = 1; // 4 // bottom right

	// top left arc
	fow -> tiles[subOne(ty)][subOne(lx)].visible = 11; // 5 // arrow head
	fow -> tiles[subOne(ty)][ lx ].visible = 12; // 5 // arrow right
	fow -> tiles[ ty ][subOne(lx)].visible = 13; // 5 // arrow left


	// bottom left arc
	fow -> tiles[addOne(by)][subOne(lx)].visible = 14; // 5 // arrow head
	fow -> tiles[addOne(by)][ lx ].visible = 16; // 5 // arrow left
	fow -> tiles[ by ][subOne(lx)].visible = 15; // 5 // arrow right
		

	// top right arc
	fow -> tiles[subOne(ty)][addOne(rx)].visible = 17; // 5 // arrow head
	fow -> tiles[ ty ][addOne(rx)].visible = 18; // 5 // arrow right
	fow -> tiles[subOne(ty)][ rx ].visible = 19; // 5 // arrow left
	
	// bottom right arc
	fow -> tiles[addOne(by)][addOne(rx)].visible = 20; // 5 // arrow head
	fow -> tiles[ by ][addOne(rx)].visible = 21; // 5 // arrow right
	fow -> tiles[addOne(by)][ rx ].visible = 22; // 5 // arrow left
}

int addOne(int n)
{
	return ++n;
}

int subOne(int n)
{
	if (n == 0) return n;
	else
	return --n;
}
/**********************************/


void init_render_player_system() {
	
	int i;
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
	}
	
	ibeam = IMG_Load("assets/Graphics/screen/menu/keymap/keymap_ibeam.png");
	free(filename);
}
