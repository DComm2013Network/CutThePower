#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "world.h"
#include "components.h"

#define SYSTEM_MASK (COMPONENT_RENDER_PLAYER)

/* POSSIBLY TEMPORARY!!! passing playerFilename may not be needed if gameplay gives us a complete player struct. */
int render_player_system(World& world, SDL_Surface* surface, SDL_Rect* playerRect, char *playerFilename) {
	
	unsigned int entity;
	RenderPlayerComponent *renderPlayer;
	
	SDL_Texture* playerTexture;
	SDL_Surface* playerSurface;
	

	
	for(entity = 0; entity < ENTITY_COUNT; entity++){
		
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
