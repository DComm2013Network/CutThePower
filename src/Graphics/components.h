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
 * @enum Components
typedef enum{
	COMPONENT_NONE = 0,               /**< The entity has no components.
	COMPONENT_RENDER_PLAYER = 1 << 0, /**< The entity can be rendered.
} Components;
*/

/**
 * Contains the information for rendering a player.
 *
 * @struct RenderPlayerComponent
 */
typedef struct{
	SDL_Texture *playerTexture; /**< The texture with which to render the player. */
	SDL_Surface* playerSurface; /**< The surface on to which to render the player. */
	int width;                  /**< The player's height in pixels. */
	int height;                 /**< The player's width in pixels. */
} RenderPlayerComponent;


#endif
