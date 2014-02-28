#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../world.h"
#include "components.h"

#define SYSTEM_MASK (COMPONENT_RENDER_PLAYER)

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: render_player_system
--
-- DATE: February 21, 2014
--
-- REVISIONS: Sam Youssef - February 24, 2014: Updated the player render to get it working with multiple players
--
-- DESIGNER: Jordan Marling/Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: int render_player_system(World& world, SDL_Surface* surface, SDL_Rect* playerRect, char *playerFilename)
--              World& world: World 
--				SDL_Surface* surface: Surface player is being rendered to
--				SDL_Rect* playerRect: The bounding box for the player
-				char *playerFilename: pathway for the player file
--
-- RETURNS: Returns 1 upon success.
--
-- NOTES:
-- Render a player onto the map. Player is added as a texture then painted onto the surface. Multiple players can
-- be added.
--
------------------------------------------------------------------------------------------------------------------*/

/* POSSIBLY TEMPORARY!!! passing playerFilename may not be needed if gameplay gives us a complete player struct. */
int render_player_system(World& world, SDL_Surface* surface, SDL_Rect* playerRect, char *playerFilename) {
	
	unsigned int entity;
	RenderPlayerComponent *renderPlayer;
	
	SDL_Texture* playerTexture;
	SDL_Surface* playerSurface;
	

	
	for(entity = 0; entity < MAX_ENTITIES; entity++){
		
		if ((world.mask[entity] & SYSTEM_MASK) == SYSTEM_MASK){
			
			renderPlayer = &(world.renderPlayer[entity]);			

			/* POSSIBLY TEMPORARY!!! playerFilename may be different if gameplay gives us a complete player struct. */
			playerSurface = SDL_LoadBMP(playerFilename);
			SDL_BlitSurface(playerSurface, NULL, surface, playerRect);
			
			if (playerSurface == 0) {
				printf("Error loading tile");
				return 0;
			}			
		}
	}
	
	return 1;
}
