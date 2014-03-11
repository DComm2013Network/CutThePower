/** @ingroup Input */
/*@{*/

/**
 * @date 2014/02/18
 *
 * @todo Implement keybindings
 * @todo Implement textfield editing and add more useable characters
 * 
 * @file keyinputsystem.cpp
 */
/*@}*/

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

#include "../world.h"
#include "components.h"

#define SYSTEM_MASK (COMPONENT_COMMAND) /**< Entities with a command component will be processed by the system. */

int GetScancode(char *character);

extern int textField; /**< This references the textField variable in the mouseinputsystem for the currently active textfield. */
int command_keys[NUM_COMMANDS]; /**< This is the current keycodes mapped to each command. */

/**
 * Polls the keyboard for input and performs the appropriate action.
 *
 * Current player commands:
 * <ul>
 *    <li><b>W</b> - Up</li>
 *    <li><b>A</b> - Left</li>
 *    <li><b>S</b> - Down</li>
 *    <li><b>D</b> - Right</li>
 *    <li><b>Space</b> - Action</li>
 * </ul>
 *
 * @param[in, out]	world 	Pointer to WORLD (structure containing "world" information, entities/components)
 * @param[out] 		running	Whether the GAME is running or still in menu
 *
 * @designer Vincent Lau
 * @author Jordan Marling
 *
 */
void KeyInputSystem(World *world, bool *running)
{
    int entity;
    CommandComponent *command;

    SDL_Event event;
    static const Uint8 *currentKeyboardState = 0, *prevKeyboardState = 0;
    static bool action = false;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }
    }

    prevKeyboardState = currentKeyboardState;
    currentKeyboardState = SDL_GetKeyboardState(NULL);

    if (prevKeyboardState == 0)
        return;

    //If a textfield is focused
    if (textField != -1) {

        //TextFieldComponent text = world->text[textField];

        //TODO: Make this work.
        //Add other characters.
        /*for(int i = 'A'; i < 'z'; i++) {

            if (currentKeyboardState[i] &&
                !prevKeyboardState[i]) {

                //will this work?
                strcpy(text.text + text.length, SDL_GetScancodeName(SDL_GetScancodeFromName()));

            }

        }*/

    }


    for(entity = 0; entity < MAX_ENTITIES; entity++) {

        if ((world->mask[entity] & SYSTEM_MASK) == SYSTEM_MASK)
        {
            command = &(world->command[entity]);


            //command->commands[C_UP] = (currentKeyboardState[SDL_SCANCODE_W] != 0);
            //command->commands[C_LEFT] = (currentKeyboardState[SDL_SCANCODE_A] != 0);
            //command->commands[C_DOWN] = (currentKeyboardState[SDL_SCANCODE_S] != 0);
            //command->commands[C_RIGHT] = (currentKeyboardState[SDL_SCANCODE_D] != 0);

            command->commands[C_UP] = (currentKeyboardState[command_keys[C_UP]] != 0);
            command->commands[C_LEFT] = (currentKeyboardState[command_keys[C_LEFT]] != 0);
            command->commands[C_DOWN] = (currentKeyboardState[command_keys[C_DOWN]] != 0);
            command->commands[C_RIGHT] = (currentKeyboardState[command_keys[C_RIGHT]] != 0);


            command->commands[C_ACTION] = (currentKeyboardState[SDL_SCANCODE_SPACE] != 0) && action == false;

            action = currentKeyboardState[SDL_SCANCODE_SPACE] != 0;

        }

    }
}

/**
 * Loads the desired keyboard commands.
 *
 * Current player commands:
 * <ul>
 *    <li><b>C_UP</b> - Up</li>
 *    <li><b>C_LEFT</b> - Left</li>
 *    <li><b>C_DOWN</b> - Down</li>
 *    <li><b>C_RIGHT</b> - Right</li>
 *    <li><b>C_ACTION</b> - Action</li>
 * </ul>
 *
 * @param[in]		file 	The file to load the data.
 *
 * @designer Jordan Marling
 * @author Jordan Marling
 *
 */
int KeyMapInit(char *file) 
{
	
	FILE *fp;
	
	char command[64];
	char value[64];
	
	if (file == 0 || (fp = fopen(file, "r")) == 0) {
		
		command_keys[C_UP] = SDL_SCANCODE_W;
		command_keys[C_LEFT] = SDL_SCANCODE_A;
		command_keys[C_DOWN] = SDL_SCANCODE_S;
		command_keys[C_RIGHT] = SDL_SCANCODE_D;
		command_keys[C_ACTION] = SDL_SCANCODE_SPACE;
		
		printf("Error opening file: %s\n", file);
		return -1;
	}
	
	while (!feof(fp)) {
		
		if (fscanf(fp, "%s %s", command, value) != 2) {
			
			if (feof(fp)) {
				return 0;
			}
			
			printf("Error loading line.\n");
			return -1;
		}
		
		if (strcmp(command, "C_UP") == 0) { //C_UP
			command_keys[C_UP] = GetScancode(value);
		}
		else if (strcmp(command, "C_LEFT") == 0) { //C_LEFT
			command_keys[C_LEFT] = GetScancode(value);
		}
		else if (strcmp(command, "C_DOWN") == 0) { //C_DOWN
			command_keys[C_DOWN] = GetScancode(value);
		}
		else if (strcmp(command, "C_RIGHT") == 0) { //C_RIGHT
			command_keys[C_RIGHT] = GetScancode(value);
		}
		else if (strcmp(command, "C_ACTION") == 0) { //C_ACTION
			command_keys[C_ACTION] = GetScancode(value);
		}
		else {
			printf("Unable to load command: %s\n", command);
		}
	}
	
	return 0;
}


/**
 * Returns the keycode based on a character.
 *
 * @param[in]		character 	The character to be loaded
 *
 * @designer Jordan Marling
 * @author Jordan Marling
 *
 */
 int GetScancode(char *character)
 {
	if (character == 0) {
		return -1;
	}
	
	if (strcmp(character, "UP") == 0) {
		return SDL_SCANCODE_UP;
	}
	if (strcmp(character, "LEFT") == 0) {
		return SDL_SCANCODE_LEFT;
	}
	if (strcmp(character, "DOWN") == 0) {
		return SDL_SCANCODE_DOWN;
	}
	if (strcmp(character, "RIGHT") == 0) {
		return SDL_SCANCODE_RIGHT;
	}
	if (strcmp(character, "SPACE") == 0) {
		return SDL_SCANCODE_SPACE;
	}
	
	
	return SDL_GetScancodeFromName(character);
 }
