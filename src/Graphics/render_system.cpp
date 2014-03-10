#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../world.h"
#include "components.h"

#define SYSTEM_MASK (COMPONENT_RENDER_PLAYER | COMPONENT_POSITION)
extern SDL_Rect map_rect;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: render_player_system
--
-- DATE: February 21, 2014
--
-- REVISIONS: Sam Youssef - February 24, 2014: Updated the player render to get it working with multiple players
--			  Mateusz Siwoski - February 28, 2014: Modified to pass in the Position Component, fixed not loading the image
--													every time the frame is drawn
--			  Jordan Marling/Mat Siwoski - March 6, 2014: Updated support for the camera.
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
void render_player_system(World& world, SDL_Surface* surface) {
	
	unsigned int entity;
	RenderPlayerComponent 	*renderPlayer;
	PositionComponent 		*position;
	SDL_Rect playerRect;
	
	for(entity = 0; entity < MAX_ENTITIES; entity++){
		
		if (IN_THIS_COMPONENT(world.mask[entity], SYSTEM_MASK)){
			
			position = &(world.position[entity]);
			renderPlayer = &(world.renderPlayer[entity]);			
			
			playerRect.x = position->x + map_rect.x;
			playerRect.y = position->y + map_rect.y;
			playerRect.w = renderPlayer->width;
			playerRect.h = renderPlayer->height;
			SDL_BlitSurface(renderPlayer->playerSurface, NULL, surface, &playerRect);
		}
	}
}
