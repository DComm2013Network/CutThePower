/*-----------------------------------------------------------------------------
--  SOURCE FILE: keyinputsystem.cpp
--
--
--  PROGRAM:    CutThePower
--
--  FUNCTIONS:  void KeyInputSystem(World *world, bool *running)
--
--
--
--  DATE:       2014/02/18
--
--  REVISIONS:      ...
--
--  DESIGNER:       Jordan Marling
--                  Cory Thomas
--                  Vincent Lau
--
--  PROGRAMMER:     Jordan Marling
-
--  NOTES:      *Features to Add*
--              - Keybinding
--              - Textfield editing
--
--
--
-----------------------------------------------------------------------------*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

#include "world.h"
#include "components.h"

#define SYSTEM_MASK (COMPONENT_COMMAND)

extern int textField;

/*-----------------------------------------------------------------------------
-   FUNCTION:
-
-   DATE:
-
-   REVISIONS:  ...
-
-   DESIGNER:   Vincent Lau
-
-   PROGRAMMER: Jordan Marling
-
-   INTERFACE:  void KeyInputSystem(World *world, bool *running)
-
-   RETURNS:    nothing.
-
-   PARAMETERS: world - pointer to WORLD (structure containing "world" information,
                                            entities/components)
-               running - state if the GAME is running or still in menu
-
-   NOTES:  Feb 18th - Textfield compatibility (typing) is not implemented yet
-           No Key-bind feature yet
-           W - Up
-           A - Left
-           S - Down
-           D - Right
-           Space - Action
-----------------------------------------------------------------------------*/
void KeyInputSystem(World *world, bool *running)
{
    int entity;
    CommandComponent *command;
    /*PositionComponent *position;
    SizeComponent *size;*/

    SDL_Event event;
    static const Uint8 *currentKeyboardState = 0, *prevKeyboardState = 0;
    static bool action = false;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }
    }

    //printf("%p\n", currentKeyboardState);

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

    //loop?
    for(entity = 0; entity < ENTITY_COUNT; entity++)
    {

        if ((world->mask[entity] & SYSTEM_MASK) == SYSTEM_MASK)
        {
            command = &(world->command[entity]);
            /*position = &(world->positions[entity]);
            size = &(world->size[entity]);*/


            command->commands[C_UP] = (currentKeyboardState[SDL_SCANCODE_W] != 0);
            command->commands[C_LEFT] = (currentKeyboardState[SDL_SCANCODE_A] != 0);
            command->commands[C_DOWN] = (currentKeyboardState[SDL_SCANCODE_S] != 0);
            command->commands[C_RIGHT] = (currentKeyboardState[SDL_SCANCODE_D] != 0);


            /*if (currentKeyboardState[SDL_SCANCODE_SPACE]) {
                if (command->commands[C_ACTION]) {
                    command->commands[C_ACTION] = false;
                }
                else {
                    command->commands[C_ACTION] = true;
                }
            }*/


            command->commands[C_ACTION] = (currentKeyboardState[SDL_SCANCODE_SPACE] != 0) && action == false;

            action = currentKeyboardState[SDL_SCANCODE_SPACE] != 0;

        }

    }
}
