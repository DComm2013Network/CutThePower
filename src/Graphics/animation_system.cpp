/** @ingroup Graphics
 * @{ */
/** @file animation_system.cpp */
/** @} */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../world.h"
#include "components.h"
#include "systems.h"
#include "../sound.h"

#include <stdlib.h>

#define SYSTEM_MASK (COMPONENT_RENDER_PLAYER | COMPONENT_ANIMATION) /**< The entity must have a animation and render component */


/**
 * Updates animations
 *
 * Used to draw animations. This component determines which stage the animation
 * is at and updates the render player component accordingly so no special system
 * is needed for animations vs. static images.
 * 
 * The animation can also be triggered at a random time, and can also trigger a sound effect.
 *
 * @param world Pointer to the world structure (contains "world" info, entities / components)
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void animation_system(World *world) {
	
	unsigned int entity;
	AnimationComponent 		*animation;
	RenderPlayerComponent 	*renderPlayer;
	
	for(entity = 0; entity < MAX_ENTITIES; entity++){
		
		if (IN_THIS_COMPONENT(world->mask[entity], SYSTEM_MASK)){
			
			animation = &(world->animation[entity]);
			renderPlayer = &(world->renderPlayer[entity]);
			
			if (animation->triggered) {
				
				if (animation->index == 0 && animation->frame_count == 0 && animation->sound_effect > -1) {
					
					play_effect(animation->sound_effect);
					
				}
				
				animation->frame_count++;
				
				if (animation->frame_count > animation->frames_to_skip) {
					
					animation->frame_count = 0;
					
					renderPlayer->playerSurface = animation->surfaces[animation->index];
					
					animation->index++;
					if (animation->index >= animation->surface_count) {
						
						animation->index = 0;
						animation->triggered = false;
						
					}
				}
				
			}
			else { //do random stuff
				
				if (animation->rand_frame < 0)
					continue;
				
				if (rand() % animation->rand_frame == 0) {
					animation->triggered = true;
				}
				
				
			}
			
		}
	}
}
