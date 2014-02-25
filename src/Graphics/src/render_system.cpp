#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "world.h"
#include "components.h"

#define SYSTEM_MASK (COMPONENT_RENDER_PLAYER)

void render_player_system(World& world, SDL_Surface* surface){
	unsigned int entity;
	RenderPlayerComponent *renderPlayer;
	SDL_Rect playerRect;
	SDL_Texture* playerTexture;
	
	for(entity = 0; entity < ENTITY_COUNT; entity++){
		
		if ((world.mask[entity] & SYSTEM_MASK) == SYSTEM_MASK){
			renderPlayer = &(world.renderPlayer[entity]);			
			
			//SDL_FillRect(surface, &playerRect, renderPlayer->colour);
		}
	}
}
