/** @defgroup Graphics */
/** @ingroup Graphics */
/** @{ */
/** @file components.h */
/** @} */


#ifndef GRAPHIC_COMPONENT_H
#define GRAPHIC_COMPONENT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../components.h"

/**
 * Contains the components related to rendering.
 *
 * @enum Components Contains the information for rendering a player.
 *
 * @struct RenderPlayerComponent
 */
typedef struct{
	SDL_Surface *playerSurface; /**< The surface on to which to render the player. */
	int width;                  /**< The player's height in pixels. */
	int height;                 /**< The player's width in pixels. */
} RenderPlayerComponent;

typedef struct {
	
	char *name; //name of animation
	SDL_Surface **surfaces; //surface array
	int index; //current surface to be drawn
	int surface_count; //total amount of surfaces
	int frames_to_skip; //frames in between surface changes
	int frame_count; //current amount of frames between surface change
	int sound_effect; //sound effect to be played
	int loop; //-1 is no loop, 1 is loop
	
} Animation;

/**
 * Contains the components related to animation.
 *
 * @enum Components Contains the information for rendering an animation.
 *
 * @struct AnimationComponent
 */
typedef struct {
	
	Animation *animations; //animation array
	int animation_count; //amount of animations
	int current_animation; //current animation to be played, -1 is none
	
	int hover_animation; //id of the animation to be played while hovered over, -1 is none
	int rand_animation; //id of the animation to be played when triggered. -1 is none
	int rand_occurance; //has a 1/rand_occurance to happen once a frame. -1 is none
	int id; //this is used to trigger an event at the end of an animation, -1 is none
	
} AnimationComponent;

#endif
