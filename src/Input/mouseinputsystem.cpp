/*-----------------------------------------------------------------------------
--  SOURCE FILE:    mouseinputsystem.cpp
--
--
--  PROGRAM:        CutThePower
--
--  FUNCTIONS:      void MouseInputSystem(World *world)
--
--
--
--  DATE:           2014/02/18
--
--  REVISIONS:      ...
--
--  DESIGNER:       Jordan Marling
--                  Cory Thomas
--                  Vincent Lau
--
--  PROGRAMMER:     Jordan Marling
--
--  NOTES:
--
--
--
--
-----------------------------------------------------------------------------*/
#include <SDL2/SDL.h>
#include "../world.h"
#include "components.h"

#define SYSTEM_MASK (COMPONENT_MOUSE)

int textField = -1;

/*-----------------------------------------------------------------------------
-   FUNCTION:
-
-   DATE:
-
-   REVISIONS:  ...
-
-   DESIGNER:   Jordan Marling
-               Cory Thomas
-               Vincent Lau
-
-   PROGRAMMER: Jordan Marling
-
-   INTERFACE:  void MouseInputSystem(World *world)
-
-   RETURNS:    nothing!!!!!
-
-   PARAMETERS: world - pointer to the world structure (contains "world" info,
                                                        entities / components)
-
-   NOTES:      Updates the mouse position for every frame. 
-               Currently not needed until text fields are implemented for menu.
-               Needed for the later "Field of View Looking" feature using mouse
-                   to rotate.
-
-----------------------------------------------------------------------------*/
void MouseInputSystem(World *world)
{
    int entity, x, y;
    static Uint32 previousState = 0;
    static Uint32 currentState = 0;
    bool rclick, lclick;
    MouseComponent *mouse;
    TextFieldComponent *text;
    ButtonComponent *button;
    PositionComponent *position;
    //SizeComponent *size;

    previousState = currentState;
    currentState = SDL_GetMouseState(&x, &y);

    lclick = (currentState & SDL_BUTTON(1)) == 0 && (previousState & SDL_BUTTON(1)) != 0;
    rclick = (currentState & SDL_BUTTON(3)) == 0 && (previousState & SDL_BUTTON(3)) != 0;

    if (lclick) {
        textField = -1;
    }

    for(entity = 0; entity < ENTITY_COUNT; entity++)
    {

        if ((world->mask[entity] & SYSTEM_MASK) == SYSTEM_MASK)
        {
            mouse = &(world->mouse[entity]);
            //size = &(world->size[entity]);

            mouse->x = x;
            mouse->y = y;
            mouse->leftClick = lclick;
            mouse->rightClick = rclick;

            if (lclick) {
                //does the entity have a text field?
                if ((world->mask[entity] & COMPONENT_TEXTFIELD) == COMPONENT_TEXTFIELD) {

                    text = &(world->text[entity]);
                    position = &(world->position[entity]);

                    if (position->x < x &&
                        position->y < y &&
                        position->width > x &&
                        position->height > y) {

                        text->focused = true;
                        textField = entity;
                    }

                }

                //does the entity have a text field?
                if ((world->mask[entity] & COMPONENT_BUTTON) == COMPONENT_BUTTON) {

                    button = &(world->button[entity]);
                    position = &(world->position[entity]);

                    button->prevState = button->currentState;

                    button->currentState =  position->x < x &&
                                            position->y < y &&
                                            position->width > x &&
                                            position->height > y;

                }
            }

        }

    }
}
