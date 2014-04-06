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

void make_surrounding_tiles_visible(FowPlayerPosition *fowp);
static int opponentPlayers[32];
static int opponentPlayersCount = 0;

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
 *		 <li>Sam Youssef - March 25, 2014: added full support for rendering fog of war</li>
 *		 <li>Sam Youssef - April 3, 2014: fog of war hides enemy team, full functionality</li>
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
void render_player_system(World& world, SDL_Surface* surface, FowComponent *fow) {
	
	unsigned int entity;
	RenderPlayerComponent 	*renderPlayer;
	PositionComponent 	*position;
	TextFieldComponent	*text;
	SDL_Rect playerRect;
	
	SDL_Rect clipRect;

	opponentPlayersCount = 0;
	memset(opponentPlayers, 0, sizeof(opponentPlayers));


	for(entity = 0; entity < MAX_ENTITIES; entity++){
		
		if(IN_THIS_COMPONENT(world.mask[entity], COMPONENT_PLAYER | COMPONENT_CONTROLLABLE)) {
			fow->teamNo = world.player[entity].teamNo;
		}


		if (IN_THIS_COMPONENT(world.mask[entity], SYSTEM_MASK)){
			
			position = &(world.position[entity]);
			renderPlayer = &(world.renderPlayer[entity]);
			
			if (!IN_THIS_COMPONENT(world.mask[entity], COMPONENT_MENU_ITEM)) {
				playerRect.x = position->x + map_rect.x;
				playerRect.y = position->y + map_rect.y;
			}
			else {
				playerRect.x = position->x;
				playerRect.y = position->y;
			}
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

			//check if a textbox.
			if (IN_THIS_COMPONENT(world.mask[entity], COMPONENT_TEXTFIELD)) {
				
				text = &(world.text[entity]);
				
				playerRect.x += 10;
				playerRect.y += 8;
				
				//TODO: Store the text in a component instead of creating it every frame.
				//Perhaps we should store it in the renderPlayer component and draw the
				//textbox if it has a text field component?
				SDL_BlitSurface(draw_text(text->text, MENU_FONT), NULL, surface, &playerRect);
				
				if (text->focused) {
					playerRect.x += get_text_width(text->text, MENU_FONT) + 1;
					SDL_BlitSurface(ibeam, NULL, surface, &playerRect);
				}
			}
		}
	}


	//only render enemy players that are inside the visibility circles
	for(int entity = 0; entity < opponentPlayersCount && entity < 32; entity++) {

		position 			= &(world.position    [ opponentPlayers[entity] ]);
		renderPlayer 	= &(world.renderPlayer[ opponentPlayers[entity] ]);

		playerRect.x = position->x + map_rect.x;
		playerRect.y = position->y + map_rect.y;
		playerRect.w = renderPlayer->width;
		playerRect.h = renderPlayer->height;

		time_t tm;

		int xPos = position->x / TILE_WIDTH;
		int yPos = position->y / TILE_HEIGHT;

		int vistype = fow -> tiles[yPos][xPos].visible[ position->level ];

		if(vistype == 1 || vistype == 2)
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


			// sound enemy speech if near
			if(time(&tm) - fow->copSpeech.played > 4)
			{			
				int nTilesInLos = fow -> tilesVisibleToControllablePlayerCount;
				for(int i = 0; i < nTilesInLos; i++)
				{
					if(fow -> tilesVisibleToControllablePlayer[i][0] == yPos &&
						 fow -> tilesVisibleToControllablePlayer[i][0] == xPos) 
					{		 
						Mix_PlayChannel( -1, fow->copSpeech.speech[rand() % NUMSPEECH], 0 );
						time(&fow->copSpeech.played);
					}
				}	
			}
		}
	}
	
	fow->tilesVisibleToControllablePlayerCount = 0;
	memset(fow->tilesVisibleToControllablePlayer, 0, sizeof(fow->tilesVisibleToControllablePlayer));

	
}

void init_players_speech(FowComponent *fow) {

	fow->copSpeech.speech[0] = Mix_LoadWAV("assets/Sound/speech/cop1.wav");
	fow->copSpeech.speech[1] = Mix_LoadWAV("assets/Sound/speech/cop2.wav");
	fow->copSpeech.speech[2] = Mix_LoadWAV("assets/Sound/speech/cop3.wav");
	time( &fow->copSpeech.played );
}

void render_player_speech(FowComponent *fow) {

	

}


void init_render_player_system() {
	if ((ibeam = IMG_Load("assets/Graphics/screen/menu/ibeam.png")) == 0) {
		printf("Error loading ibeam image.\n");
	}
}
