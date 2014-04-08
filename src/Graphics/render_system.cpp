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

static void render_opponent_players(World& world, SDL_Surface *surface, FowComponent *fow, SDL_Rect map_rect);
static int opponentPlayers[32];
static int opponentPlayersCount = 0;
extern int curlevel;
#define SYSTEM_MASK (COMPONENT_RENDER_PLAYER | COMPONENT_POSITION) /**< The entity must have a render player and position component
                                                                    * for processing by this system. */

extern SDL_Rect map_rect; /**< The rectangle containing the map. */
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
 *     <li>Sam Youssef - March 25, 2014: added full support for rendering fog of war</li>
 *     <li>Sam Youssef - April 3, 2014: fog of war hides enemy team, full functionality</li>
 * </ol>
 *
 * @param[in,out] world   A reference to the world structure containing entities to render.
 * @param[in,out] surface Surface player is being rendered to.
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Mat Siwoski
 * @date Feb 14, 2024
 */
void render_player_system(World& world, SDL_Surface* surface, FowComponent *fow) {
	
	unsigned int entity;
	RenderPlayerComponent 	*renderPlayer;
	PositionComponent 	*position;
	SDL_Rect playerRect;
	
	SDL_Rect clipRect;

	opponentPlayersCount = 0;
	memset(opponentPlayers, 0, sizeof(opponentPlayers));


	for(entity = 0; entity < MAX_ENTITIES; entity++){
		
		if(IN_THIS_COMPONENT(world.mask[entity], COMPONENT_PLAYER | COMPONENT_CONTROLLABLE)) {
			fow->teamNo = world.player[entity].teamNo;
		}


		if (IN_THIS_COMPONENT(world.mask[entity], SYSTEM_MASK) && !IN_THIS_COMPONENT(world.mask[entity], COMPONENT_MENU_ITEM)){
			
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
			
			if(!IN_THIS_COMPONENT(world.mask[entity], COMPONENT_PLAYER))
			{
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
			}
			
			if(IN_THIS_COMPONENT(world.mask[entity], COMPONENT_PLAYER)) {
					

				// only render players on my team
				if ((fow->teamNo == world.player[entity].teamNo)) {	
		
					FowPlayerPosition fowp;
		
					fowp.world = &world;
					fowp.pos   = position;
					fowp.fow   = fow;		
					fowp.isControllablePlayer = IN_THIS_COMPONENT(world.mask[entity], COMPONENT_CONTROLLABLE);
					make_surrounding_tiles_visible(&fowp);

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
				}

				else {
					opponentPlayers[opponentPlayersCount++] = entity;
				}
			}
		}
	}

	render_opponent_players(world, surface, fow, map_rect);
	
	fow->tilesVisibleToControllablePlayerCount = 0;
	for(int i = 0; i < NMAXTILESINLOS; i++)
		memset(fow->tilesVisibleToControllablePlayer[i], 0, 2 * sizeof(int));
}


/**
 * Renders an opponent player contingent on his presence inside the team's visibility bubbles
 * Calls render_player_speech to provide sound effects
 *
 * Revisions:
 *     None.
 *loading
 * @param world			The world struct
 * @param surface		The surface to blit to
 * @param map_rect		A struct containing the camera offset from the map's origin
 * @param fow   		A pointer to a FogComponent struct which contains a tile map and sound effects
 * @return void.
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date April 3rd, 2014
 */
void render_opponent_players(World& world, SDL_Surface *surface, FowComponent *fow, SDL_Rect map_rect) {

	for(int entity = 0; entity < opponentPlayersCount && entity < 32; entity++) {

		PositionComponent     *position     = &(world.position    [ opponentPlayers[entity] ]);
		RenderPlayerComponent *renderPlayer = &(world.renderPlayer[ opponentPlayers[entity] ]);

		SDL_Rect playerRect;
		SDL_Rect clipRect;

		playerRect.x = position->x + map_rect.x - 20;
		playerRect.y = position->y + map_rect.y - 20 ;
		playerRect.w = renderPlayer->width;
		playerRect.h = renderPlayer->height;

		int xPos = position->x / TILE_WIDTH;
		int yPos = position->y / TILE_HEIGHT;

		//if(xPos >= 0 && yPos >= 0 && yPos < world.level[ position->level ].height && xPos < world.level[ position->level ].width)
		if(fow -> tiles[yPos][xPos].visible[ position->level ] == 0)
		{
			// show enemy player
	
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

			render_player_speech(fow, xPos, yPos);
		}
	}
}

/**
 * Render the menu to the screen.
 *
 * @param[in,out] world   A reference to the world structure containing entities to render.
 * @param[in,out] surface Surface player is being rendered to.
 *
 * @designer Jordan Marling
 *
 * @author Jordan Marling
 * @date March 12s, 2024
 */
void render_menu_system(World *world, SDL_Surface *surface) {
	
	unsigned int entity;
	RenderPlayerComponent 	*renderPlayer;
	PositionComponent 	*position;
	TextFieldComponent *text;
	SDL_Rect menu_rect;
	
	for(entity = 0; entity < MAX_ENTITIES; entity++){
		
		if (IN_THIS_COMPONENT(world->mask[entity], SYSTEM_MASK | COMPONENT_MENU_ITEM)) {
			
			position = &(world->position[entity]);
			renderPlayer = &(world->renderPlayer[entity]);
			
			
			menu_rect.x = position->x;
			menu_rect.y = position->y;
			menu_rect.w = renderPlayer->width;
			menu_rect.h = renderPlayer->height;
			
			if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_BUTTON)) {
				
				if (world->button[entity].hovered) {
					menu_rect.x -= 5;
					menu_rect.y -= 5;
					menu_rect.w += 10;
					menu_rect.h += 10;
				}
			}
			
			SDL_BlitScaled(renderPlayer->playerSurface, NULL, surface, &menu_rect);
			
			
		
			//check if a textbox.
			if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_TEXTFIELD)) {
				
				text = &(world->text[entity]);
				
				menu_rect.x += 10;
				menu_rect.y += 8;
				
				//TODO: Store the text in a component instead of creating it every frame.
				//Perhaps we should store it in the renderPlayer component and draw the
				//textbox if it has a text field component?
				SDL_BlitSurface(draw_text(text->text, MENU_FONT), NULL, surface, &menu_rect);
				
				if (text->focused) {
					menu_rect.x += get_text_width(text->text, MENU_FONT) + 1;
					SDL_BlitSurface(ibeam, NULL, surface, &menu_rect);
				}
			}	
		}
	}
}

/**
 * Render the Ibeam for chat.
 *
 * @designer Jordan Marling
 *
 * @author Mat Siwoski
 * @date March 7, 2024
 */
void init_render_player_system() {
	if ((ibeam = IMG_Load("assets/Graphics/screen/menu/ibeam.png")) == 0) {
		printf("Error loading ibeam image.\n");
	}
}
