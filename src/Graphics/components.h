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
	SDL_Texture *playerTexture; /**< The texture with which to render the player. */
	SDL_Surface *playerSurface; /**< The surface on to which to render the player. */
	int width;                  /**< The player's height in pixels. */
	int height;                 /**< The player's width in pixels. */
} RenderPlayerComponent;

/**
 * Contains the components related to animation.
 *
 * @enum Components Contains the information for rendering an animation.
 *
 * @struct AnimationComponent
 */
typedef struct {
	
	SDL_Surface **surfaces;
	int index;
	int surface_count;
	bool triggered;
	int frames_to_skip;
	int frame_count;
	int rand_frame;
	int sound_effect;
	
} AnimationComponent;

#endif
