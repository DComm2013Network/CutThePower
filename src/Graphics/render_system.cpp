#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_Rect.h>

#include "world.h"
#include "components.h"

#define SYSTEM_MASK (COMPONENT_RENDER)

void render_system(World *world, SDL_Surface *surface){
	RenderComponent *render;
	for(entity = 0; entity < ENTITY_COUNT; entity++){
		
		if ((world->mask[entity] & SYSTEM_MASK) == SYSTEM_MASK){
			render = &(world->render[entity]);
			
			
		}
	}
}
