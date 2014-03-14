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

#define SYSTEM_MASK (COMPONENT_MOUSE) /**< Entities must have a mouse component to be processed by this system. */

int textField = -1;

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
void MouseInputSystem(World *world, unsigned int *player_entity, bool *running)
{
    int entity, e, x, y;
    static Uint32 previousState = 0;
    static Uint32 currentState = 0;
    bool rclick, lclick, text_field_pressed = false;
    MouseComponent *mouse;
    TextFieldComponent *text;
    ButtonComponent *button;
    PositionComponent *position;

    previousState = currentState;
    currentState = SDL_GetMouseState(&x, &y);

    lclick = (currentState & SDL_BUTTON(1)) == 0 && (previousState & SDL_BUTTON(1)) != 0;
    rclick = (currentState & SDL_BUTTON(3)) == 0 && (previousState & SDL_BUTTON(3)) != 0;

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
					
					//MAIN MENU
					if (strcmp(button->label, "mainmenu_play") == 0) {
						
						destroy_menu(world);
						
						//map_init(world, "assets/Graphics/lobby/lobby.txt", "assets/Graphics/lobby/lobby_tiles.txt");
						//map_init(world, "assets/Graphics/SampleFloor.txt", "assets/Graphics/tiles_lobby.txt");
						
						//*player_entity = create_player(world, 600, 600, true);
						
						create_setup_menu(world);
						
						return;
					}
					else if (strcmp(button->label, "mainmenu_options") == 0) {
						
						destroy_menu(world);
						
						create_options_menu(world);
						return;
					}
					else if (strcmp(button->label, "mainmenu_credits") == 0) {
						
						destroy_menu(world);
						
						create_credits_menu(world);
						
						return;
					}
					else if (strcmp(button->label, "mainmenu_exit") == 0) {
						destroy_world(world);

						*running = false;
						return;
					}
					
					//OPTIONS
					else if (strcmp(button->label, "options_back") == 0) {
						
						destroy_menu(world);
						
						create_main_menu(world);
						
						return;
					}
					else if (strcmp(button->label, "options_sound_off") == 0) {
						
						world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/menu/menu_button_soundon.png");
						world->button[entity].label = "options_sound_on";
						
						enable_sound(true);
						play_music(SOUND_MUSIC_MENU_RAIN);
						
					}
					else if (strcmp(button->label, "options_sound_on") == 0) {
						
						world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/menu/menu_button_soundoff.png");
						world->button[entity].label = "options_sound_off";
						
						enable_sound(false);
						
					}
					else if (strcmp(button->label, "options_keymap") == 0) {
						
						destroy_menu(world);
						
						create_keymap_menu(world);
						
						return;
					}
					
					//KEYMAP
					else if (strcmp(button->label, "keymap_back") == 0) {
						
						destroy_menu(world);
						
						create_options_menu(world);
						
						return;
					}
					
					//CREDITS
					else if (strcmp(button->label, "credits_back") == 0) {
						
						destroy_menu(world);
						
						create_main_menu(world);
						
						return;
					}
					
					//SETUP
					else if (strcmp(button->label, "setup_back") == 0) {
						
						destroy_menu(world);
						
						create_main_menu(world);
						
						return;
					}
					else if (strcmp(button->label, "setup_play") == 0) {
						
						destroy_world(world);
						
						stop_music();
						stop_effect();
						
						//map_init(world, "assets/Graphics/map/map_01/map01.txt", "assets/Graphics/map/map_01/map01_tiles.txt");
						//map_init(world, "assets/Graphics/lobby/lobby.txt", "assets/Graphics/lobby/lobby_tiles.txt");
						//map_init(world, "assets/Graphics/SampleFloor.txt", "assets/Graphics/tiles_lobby.txt");
						
						#if 0
						
						map_init(world, "assets/Graphics/map/map_01/map01.txt", "assets/Graphics/map/map_01/map01_tiles.txt");
						*player_entity = create_player(world, 600, 600, true, COLLISION_HACKER);
											
						world->mask[*player_entity] |= COMPONENT_ANIMATION;
						
						load_animation("assets/Graphics/player/robber/rob_animation.txt", world, *player_entity);
						#else
						create_intro(world);
						#endif
						
						return;
					}
					
					//BSOD
					else if (strcmp(button->label, "bsod_exit") == 0) {
						
						destroy_world(world);
						
						exit(0);
					}
					else if (strcmp(button->label, "bsod_continue") == 0) {
						
						destroy_menu(world);
						
						create_main_menu(world);
						return;
						
					}
					
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

