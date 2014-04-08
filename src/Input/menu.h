#ifndef MENU_H
#define MENU_H

#include "../world.h"

#define BIG_TEXT_WIDTH			650
#define BIG_TEXT_HEIGHT			60
#define SMALL_TEXT_WIDTH		300
#define SMALL_TEXT_HEIGHT		60
#define CHARACTER_WIDTH			48
#define CHARACTER_HEIGHT 		50
#define ANIMATED_BUTTON_WIDTH	180
#define ANIMATED_BUTTON_HEIGHT	130

void destroy_menu(World *world);
void disable_background_sound(World *world);

void create_credits_menu(World *world);
void create_keymap_menu(World *world);
void create_main_menu(World* world);
void create_options_menu(World *world);
void create_play_menu(World *world);
void create_setup_menu(World *world);
void create_bsod_menu(World *world);
void create_intro(World *world);
void create_load_screen(World *world);
void create_logo_screen(World* world);
void create_select_screen(World *world);
void create_pause_screen(World *world);

void create_van_intro(World *world, int character);


#endif
