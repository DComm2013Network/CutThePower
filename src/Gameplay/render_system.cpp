/** @ingroup Gameplay */
/** @{ */
/** @file render_system.cpp */
/** @} */

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_pixels.h>

#include "components.h"
#include "systems.h"
#include "world.h"

#define SYSTEM_MASK (COMPONENT_RENDER | COMPONENT_POSITION) /**< The component mask necessary for the entity to be rendered. */

/**
 * Renders all eligible entities on @surface.
 *
 * @param world   A reference to the world structure.
 * @param surface Pointer to the SDL surface on which to render.
 *
 * @designer ?
 * @author   ?
 */
void render_system(World& world, SDL_Surface *surface) {
	unsigned int entity;
	PositionComponent	*position;
	RenderComponent		*render;
	
	SDL_Rect player_rect;
	
	//Redraw the background colour to black.
	SDL_FillRect(surface, NULL, 0x000000);
	
	//loop through each entity and see if the system can do work on it.
	for(entity = 0; entity < MAX_ENTITIES; ++entity) {
		if (IN_THIS_COMPONENT(SYSTEM_MASK)) {
			//get the position and render components as pointers for easy code.
			position = &(world.position[entity]);
			render = &(world.render[entity]);
			
			//create the player rectangle from the position and render components.
			player_rect.x = position->x - (render->width / 2);
			player_rect.y = position->y - (render->height / 2);
			
			player_rect.w = render->width;
			player_rect.h = render->height;
			
			//draw the player rectangle.
			SDL_FillRect(surface, &player_rect, render->colour);
			
		}
	}
}
