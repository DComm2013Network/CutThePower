#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_pixels.h>

#include "components.h"
#include "systems.h"
#include "world.h"

//This is the mask the system uses to see if it will work on the entity.
#define SYSTEM_MASK (COMPONENT_RENDER | COMPONENT_POSITION)

void render_system(World& world, SDL_Surface *surface) 
{
	
	unsigned int entity;
	PositionComponent	*position;
	RenderComponent		*render;
	
	SDL_Rect player_rect;
	
	//Redraw the background colour to black.
	SDL_FillRect(surface, NULL, 0x000000);
	
	//loop through each entity and see if the system can do work on it.
	for(entity = 0; entity < MAX_ENTITIES; ++entity) 
	{
		
		if ((world.mask[entity] & SYSTEM_MASK) == SYSTEM_MASK) 
		{
			
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
