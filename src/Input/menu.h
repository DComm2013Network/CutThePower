#ifndef MENU_H
#define MENU_H

#include "../world.h"

#define BUTTON_WIDTH 		400
#define BUTTON_HEIGHT 		50
#define LABEL_WIDTH			250
#define LABEL_HEIGHT		52
#define TITLE_WIDTH			750
#define TITLE_HEIGHT		150
#define KEYMAP_WIDTH		150
#define KEYMAP_HEIGHT		50
#define TEXT_WIDTH			600
#define TEXT_HEIGHT			60
#define CHARACTER_WIDTH		35
#define CHARACTER_HEIGHT 	50

void destroy_menu(World *world);

void create_credits_menu(World *world);
void create_keymap_menu(World *world);
void create_main_menu(World* world);
void create_options_menu(World *world);
void create_play_menu(World *world);
void create_setup_menu(World *world);
void create_bsod_menu(World *world);
void create_intro(World *world);

#endif
