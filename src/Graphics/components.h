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
 * Stores each of the animation ID's. This is used for the triggering events
 */
typedef enum {
	
	ANIMATION_LOADING_SCREEN = 1,
	ANIMATION_FADE_TO_BLACK = 3
	
} AnimationID;

/**
 * Stores each of the cutscene ID's. This is used for the triggering events
 */
typedef enum {
	
	CUTSCENE_VAN_INTRO = 1,
	CUTSCENE_GAME_INTRO = 2
	
} CutsceneID;

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

/**
 * Contains an individual animation for an animation component.
 *
 * @enum Components Contains the information for a single animation.
 *
 * @struct Animation
 */
typedef struct {
	
	char *name; //name of animation
	SDL_Surface **surfaces; //surface array
	int index; //current surface to be drawn
	int surface_count; //total amount of surfaces
	unsigned int ms_to_skip; //milliseconds in between surface changes
	unsigned int ms_last; //current amount of frames between surface change
	unsigned int sound_effect; //sound effect to be played
	bool sound_enabled; //if the sound effect is enabled or not.
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
	int id; //this is used to trigger an event at the end of an animation, -1 is none
	int rand_animation; //id of the animation to be played when triggered. -1 is none
	int rand_occurance_min; //the minimum time delay for a random animation
	int rand_occurance_max; //the maximum time delay for a random animation
	unsigned int last_random_occurance; //the last time the random animation was played
	unsigned int next_random_occurance; //the next time the random animation is played
} AnimationComponent;


/**
 * Contains an individual animation for a part in the sequence of a cutscene.
 *
 * @enum Components Contains the information for a single part of a cutscene.
 *
 * @struct CutsceneSection
 */
typedef struct {
	
	float x_end;
	float y_end;
	unsigned int total_ms;
	unsigned int start_ms;
	char *animation_name;
	
} CutsceneSection;

/**
 * Contains cutscene information for a single entity.
 *
 * @enum Components Contains the information for a single entities cutscene.
 *
 * @struct CutsceneComponent
 */
typedef struct {
	
	CutsceneSection *sections;
	int num_sections;
	int current_section;
	
	int id;
	
	float xpos, ypos;
	
	
} CutsceneComponent;

#endif
