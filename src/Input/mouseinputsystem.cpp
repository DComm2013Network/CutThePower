/** @ingroup Input */
/*@{*/

/**@file mouseinputsystem.cpp*/
/*@}*/

#include <SDL2/SDL.h>

#include "../world.h"
#include "components.h"
#include "../systems.h"
#include "menu.h"
#include "../Graphics/map.h"
#include "../sound.h"
#include "../triggered.h"

#define SYSTEM_MASK (COMPONENT_MOUSE) /**< Entities must have a mouse component to be processed by this system. */
#define ANIMATION_MASK (COMPONENT_ANIMATION | COMPONENT_POSITION)

int textField = -1;
extern int window_width, window_height;

/**
 * Updates the mouse position for every frame. 
 *
 * Used to click on buttons, focus text fields and handles click events.
 *
 * @param world Pointer to the world structure (contains "world" info, entities / components)
 *
 * @designer Jordan Marling
 * @designer Cory Thomas
 * @designer Vincent Lau
 *
 * @author Jordan Marling
 */
void MouseInputSystem(World *world)
{
    int entity, e, x, y;
    static Uint32 previousState = 0;
    static Uint32 currentState = 0;
    bool rclick, lclick, text_field_pressed = false;
    MouseComponent *mouse;
    TextFieldComponent *text;
    ButtonComponent *button;
    PositionComponent *position;
    AnimationComponent *animation;

    previousState = currentState;
    currentState = SDL_GetMouseState(&x, &y);

    lclick = (currentState & SDL_BUTTON(1)) == 0 && (previousState & SDL_BUTTON(1)) != 0;
    rclick = (currentState & SDL_BUTTON(3)) == 0 && (previousState & SDL_BUTTON(3)) != 0;

	//scale the mouse to fit the screen size so the mouse works on window resize.
	x = (int)(x * (float)WIDTH / window_width);
	y = (int)(y * (float)HEIGHT / window_height);
	
	


    if (lclick) {
        textField = -1;
    }

    for(entity = 0; entity < MAX_ENTITIES; entity++)
    {

        if ((world->mask[entity] & SYSTEM_MASK) == SYSTEM_MASK)
        {
            mouse = &(world->mouse[entity]);

            mouse->x = x;
            mouse->y = y;
            mouse->leftClick = lclick;
            mouse->rightClick = rclick;

			//does the entity have a text field?
			if ((world->mask[entity] & COMPONENT_TEXTFIELD) == COMPONENT_TEXTFIELD) {

				text = &(world->text[entity]);
				position = &(world->position[entity]);

				if (position->x < x &&
					position->y < y &&
					position->x + position->width > x &&
					position->y + position->height > y &&
					lclick) {

					text->focused = true;
					textField = entity;
					
					text_field_pressed = true;
					
					for(e = 0; e < MAX_ENTITIES; e++) {
						
						if (e != entity &&
							(world->mask[e] & COMPONENT_TEXTFIELD) == COMPONENT_TEXTFIELD) {
							world->text[e].focused = false;
						}
						
					}
				}

			}

			if ((world->mask[entity] & COMPONENT_BUTTON) == COMPONENT_BUTTON) {

				button = &(world->button[entity]);
				position = &(world->position[entity]);

				button->prevState = button->currentState;

				
				button->hovered =  position->x < mouse->x &&
									position->y < mouse->y &&
									position->x + position->width > mouse->x &&
									position->y + position->height > mouse->y;
				
				button->currentState = button->hovered && lclick;
				
				if (button->currentState == true &&
					button->prevState == false) {
					
					if (menu_click(world, entity)) {
						break;
					}
					
				}
				
			}

        }
		
		//trigger animations on hover
		if ((world->mask[entity] & ANIMATION_MASK) == ANIMATION_MASK) {
			
			position = &(world->position[entity]);
			animation = &(world->animation[entity]);
			
			if (animation->hover_animation > -1 && animation->current_animation == -1) {
				
				if (position->x < x && position->y < y &&
					position->x + position->width > x &&
					position->y + position->height > y) {
					
					
					animation->current_animation = animation->hover_animation;
					
				}
			}
			
			
		}
		
    }
    
    if (lclick &&
		text_field_pressed == false &&
		textField != -1) {
		
		world->text[textField].focused = false;
		textField = -1;
		
	}
}

