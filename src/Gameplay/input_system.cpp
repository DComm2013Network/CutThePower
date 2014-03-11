/** @ingroup Gampelay */
/** @{ */
/** @file input_system.cpp */
/** @} */

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

#include "components.h"
#include "systems.h"
#include "world.h"
#include <stdio.h>

#define SYSTEM_MASK (COMPONENT_INPUT) /**< This is the mask the system uses to see if it will work on the entity. */

bool up = false;    /**< Indicates that the key for "up" was pressed. */
bool left = false;  /**< Indicates that the key for "left" was pressed. */
bool right = false; /**< Indicates that the key for "right" was pressed. */
bool down = false;  /**< Indicates that the key for "down" was pressed. */

/**
 * Checks for pending events and handles them (if any). 
 *
 * The way input is done in SDL is that you poll events.
 * The PollEvent function is non-blocking and returns 1 if 
 * there is a pending event or 0 if no events.
 * The reason we loop through is because there could be more than
 * 1 event in the queue and we want to get all of them.
 *
 * @param world   A reference to the world structure.
 * @param running Whether the game is running.
 *
 * @designer ?
 * @author ?
 */
void input_system(World& world, bool& running) {
	unsigned int entity;
	InputComponent *input;
	
	SDL_Event e;
	
	//set keys that are pressed and unset them if they are released.
	while(SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT)	{
			running = false;
		}
		else if (e.type == SDL_KEYDOWN) {
			
			SDL_Keycode pressed = e.key.keysym.sym;
			
			switch(pressed) {
				case SDLK_COMMA:
					up = true;
					break;
				case SDLK_a:
					left = true;
					break;
				case SDLK_e:
					right = true;
					break;
				case SDLK_o:
					down = true;
					break;
			}
		}
		else if (e.type == SDL_KEYUP) {
			SDL_Keycode pressed = e.key.keysym.sym;
			
			switch(pressed) {
				case SDLK_COMMA:
					up = false;
					break;
				case SDLK_a:
					left = false;
					break;
				case SDLK_e:
					right = false;
					break;
				case SDLK_o:
					down = false;
					break;
			}
		}
	}
	
	//loop through each entity and see if the system can do work on it.
	for(entity = 0; entity < MAX_ENTITIES; ++entity) {
		if (IN_THIS_COMPONENT(SYSTEM_MASK)) {
			input = &(world.input[entity]);
			
			input->up = up;
			input->left = left;
			input->right = right;
			input->down = down;
			
			
		}
	}
	
}
