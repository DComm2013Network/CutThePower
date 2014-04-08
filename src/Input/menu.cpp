/** @ingroup Input */
/*@{*/

/**
 * @date 2014/02/18
 *
 * 
 * @file menu.cpp
 */
/*@}*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

#include "../Graphics/text.h"
#include "menu.h"
#include "../world.h"
#include "../components.h"
#include "../systems.h"
#include "../sound.h"
#include "../Network/network_systems.h"

unsigned int background = MAX_ENTITIES + 1;
unsigned int background_music = -1;

/**
 * Destroys every entity in the world except for the background of the menu.
 *
 * @param world The world struct
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void destroy_menu(World *world) {
	unsigned int entity;
	
	for(entity = 0; entity < MAX_ENTITIES; entity++) {
		
		if (entity != background && IN_THIS_COMPONENT(world->mask[entity], COMPONENT_MENU_ITEM)) {
			destroy_entity(world, entity);
		}
		
	}
}

/**
 * Creates a button entity to be displayed on a menu.
 *
 * @param world The world struct
 * @param image The string of the image to be displayed as the button
 * @param x The x coordinate of the button
 * @param y The y coordinate of the button.
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void create_button(World *world, const char *text, const char *name, int x, int y) {
	
	char *new_name;
	unsigned int entity = create_entity(world, COMPONENT_MENU_ITEM | COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_BUTTON | COMPONENT_MOUSE);

	world->position[entity].x = x;
	world->position[entity].y = y;
	
	render_text(world, entity, text, MENU_FONT);
	
	world->button[entity].prevState = false;
	world->button[entity].currentState = false;
	world->button[entity].hovered = false;
	
	new_name = (char*)malloc(sizeof(char) * strlen(name) + 1);
	
	strcpy(new_name, name);
	
	world->button[entity].label = new_name;
}

/**
 * Creates a label entity to be displayed on a menu.
 *
 * @param world The world struct
 * @param image The string of the image to be displayed as the button
 * @param x The x coordinate of the button
 * @param y The y coordinate of the button.
 * @param w The width of the label.
 * @param h The height of the label.
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void create_label(World *world, const char *text, int x, int y) {
	
	unsigned int entity = create_entity(world, COMPONENT_MENU_ITEM | COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	render_text(world, entity, text, MENU_FONT);
	
	world->position[entity].x = x;
	world->position[entity].y = y;
}

/**
 * Creates a title entity to be displayed on a menu.
 * 
 * This centers the title in the width of the screen.
 *
 * @param world The world struct
 * @param image The string of the image to be displayed as the button
 * @param x The x coordinate of the button
 * @param y The y coordinate of the button.
 * @param w The width of the label.
 * @param h The height of the label.
 *
 * @designer Jordan Marling
 * @designer Vincent Lau
 *
 * @author Jordan Marling
 */
void create_title(World *world, const char *text, int x, int y) {
	
	unsigned int entity = create_entity(world, COMPONENT_MENU_ITEM | COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	world->position[entity].x = x;
	world->position[entity].y = y;
	
	render_text(world, entity, text, TITLE_FONT);
}

/**
 * Creates a textfield entity to be displayed on a menu.
 * 
 * The text field is a text box that keeps track of user input.
 *
 * @param world The world struct
 * @param name The name of the textfield.
 * @param x The x coordinate of the button
 * @param y The y coordinate of the button.
 * @param text text in the field when loaded
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 * @author Cory Thomas
 */
void create_textfield(World *world, const char *name, int x, int y, const char* text, bool big) {
	
	unsigned int entity = create_entity(world, COMPONENT_MENU_ITEM | COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_TEXTFIELD | COMPONENT_MOUSE);
	
	if(big) {
		world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/screen/menu/text_field.png");
		
		world->renderPlayer[entity].width = BIG_TEXT_WIDTH;
		world->renderPlayer[entity].height = BIG_TEXT_HEIGHT;
		world->position[entity].width = BIG_TEXT_WIDTH;
		world->position[entity].height = BIG_TEXT_HEIGHT;
		world->text[entity].max_length = MAX_STRING;
		
	} else {
		world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/screen/menu/small_text_field.png");
		
		world->renderPlayer[entity].width = SMALL_TEXT_WIDTH;
		world->renderPlayer[entity].height = SMALL_TEXT_HEIGHT;
		world->position[entity].width = SMALL_TEXT_WIDTH;
		world->position[entity].height = SMALL_TEXT_HEIGHT;
		world->text[entity].max_length = MAX_KEYMAP_STRING;
	}
	if (!world->renderPlayer[entity].playerSurface) {
		printf("Error loading image in create_textfield.\n");
	}
	
	world->position[entity].x = x;
	world->position[entity].y = y;
	
	world->text[entity].name = (char*)calloc(strlen(name) + 1, sizeof(char));
	strcpy(world->text[entity].name, name);
	world->text[entity].text = (char*)calloc(MAX_STRING + 1, sizeof(char));

	if(text != NULL)
	{
		for(int i = 0; text[i]; i++)
		{
			world->text[entity].text[i] = toupper(text[i]);
		}
		world->text[entity].length = strlen(text);
		
	} else {
		world->text[entity].length = 0;
	}

	world->text[entity].focused = false;
	world->text[entity].number = false;
	
}

void create_animated_button(World *world, const char* fileName, int x, int y, const char* name) {
	
	char *new_name;
	unsigned int entity = create_entity(world, COMPONENT_MENU_ITEM | COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_ANIMATION | COMPONENT_BUTTON | COMPONENT_MOUSE);
	world->position[entity].x = x;
	world->position[entity].y = y;
	world->position[entity].width = ANIMATED_BUTTON_WIDTH;
	world->position[entity].height = ANIMATED_BUTTON_HEIGHT;
	
	world->renderPlayer[entity].width = ANIMATED_BUTTON_WIDTH;
	world->renderPlayer[entity].height = ANIMATED_BUTTON_HEIGHT;
	
	load_animation(fileName, world, entity);
	
	world->button[entity].prevState = false;
	world->button[entity].currentState = false;
	world->button[entity].hovered = false;
	
	new_name = (char*)malloc(sizeof(char) * strlen(name) + 1);
	
	strcpy(new_name, name);
	
	world->button[entity].label = new_name;
}

/**
 * Creates an entity that serves as the background image on the menus.
 *
 * @param world The world struct
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void create_main_menu_background(World *world) {
	
	if (background < MAX_ENTITIES) {
		world->animation[background].animations[world->animation[background].rand_animation].sound_enabled = true;
		play_music(background_music);
		return;
	}
	
	background = create_entity(world, COMPONENT_MENU_ITEM | COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_ANIMATION);
	
	world->position[background].x = 0;
	world->position[background].y = 0;
	world->position[background].width = WIDTH;
	world->position[background].height = HEIGHT;
	
	load_animation("assets/Graphics/screen/menu/mainmenu_animation.txt", world, background);
	
	world->renderPlayer[background].width = WIDTH;
	world->renderPlayer[background].height = HEIGHT;
	
	//Load and play music
	background_music = load_music("assets/Sound/menu/sound_menu_bg.wav");
	play_music(background_music);
}

void disable_background_sound(World *world) {
	if (background < MAX_ENTITIES) {
		world->animation[background].animations[world->animation[background].rand_animation].sound_enabled = false;
	}
}

/**
 * Creates the logo screen
 *
 * @param world The world struct
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void create_logo_screen(World* world) {
	
	unsigned int entity;
	
	//create black background
	entity = create_entity(world, COMPONENT_MENU_ITEM | COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	world->position[entity].x = 0;
	world->position[entity].y = 0;
	world->position[entity].width = WIDTH;
	world->position[entity].height = HEIGHT;
	
	world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/screen/logo/load.png");
	if (world->renderPlayer[entity].playerSurface == 0) {
		printf("Error loading logo background\n");
	}
	
	world->renderPlayer[entity].width = WIDTH;
	world->renderPlayer[entity].height = HEIGHT;
	
	//create animation
	entity = create_entity(world, COMPONENT_MENU_ITEM | COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_ANIMATION);
	
	world->position[entity].x = 440;
	world->position[entity].y = 334;
	world->position[entity].width = 400;
	world->position[entity].height = 100;
	
	load_animation("assets/Graphics/screen/logo/animation.txt", world, entity);
	
	world->animation[entity].id = 2;
	
	world->renderPlayer[entity].width = 400;
	world->renderPlayer[entity].height = 100;
	
	play_animation(world, entity, "load");
}


/**
 * Creates the main menu
 * 
 * This menu has buttons to go to the setup menu, options menu, credits menu
 * and exit the game.
 *
 * @param world The world struct
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void create_main_menu(World* world) {
	
	create_main_menu_background(world);
	
	create_title(world, "CUT THE POWER", (WIDTH / 2), (HEIGHT / 2) - 250);
	
	create_button(world, "PLAY", "mainmenu_play", (WIDTH / 2), (HEIGHT / 2) - 25);
	create_button(world, "OPTIONS", "mainmenu_options", (WIDTH / 2), (HEIGHT / 2) + 50);
	create_button(world, "CREDITS", "mainmenu_credits", (WIDTH / 2), (HEIGHT / 2) + 125);
	create_button(world, "EXIT", "mainmenu_exit", (WIDTH / 2), (HEIGHT / 2) + 200);
	
}

/**
 * Creates the options menu
 * 
 * This menu allows you to turn on and off sound and gives you
 * a button to go to the keymap menu.
 *
 * @param world The world struct
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void create_options_menu(World *world) {
	
	create_main_menu_background(world);
	
	create_title(world, "OPTIONS", (WIDTH / 2), (HEIGHT / 2) - 250);
	
	create_button(world, "SOUND ON", "options_sound_on", (WIDTH / 2), (HEIGHT / 2) - 25);
	create_button(world, "KEYMAP", "options_keymap", (WIDTH / 2), (HEIGHT / 2) + 50);
	create_button(world, "FULLSCREEN OFF", "options_fullscreen_off", (WIDTH / 2), (HEIGHT / 2) + 125);
	create_button(world, "BACK", "options_back", (WIDTH / 2), (HEIGHT / 2) + 200);
}

/**
 * Creates the keymap menu.
 * 
 * This menu allows you to change your key bindings.
 *
 * @param world The world struct
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 * @author Cory Thomas
 */
void create_keymap_menu(World *world) {
	
	create_main_menu_background(world);
	
	create_title(world, "KEYMAP", (WIDTH / 2), (HEIGHT / 2) - 250);
	
	create_label(world, "UP", (WIDTH / 2) - 350, (HEIGHT / 2) - 100);
	create_label(world, "DOWN", (WIDTH / 2) - 350, (HEIGHT / 2) - 25);
	create_label(world, "LEFT", (WIDTH / 2) - 350, (HEIGHT / 2) + 50);
	create_label(world, "RIGHT", (WIDTH / 2) - 350, (HEIGHT / 2) + 125);
	create_label(world, "ACTION", (WIDTH / 2) - 350, (HEIGHT / 2) + 200);
	
	//load information
	const int pos = strlen("assets/Graphics/screen/menu/keymap/keymap_");
	int *commands;
	char filename[64];
	strcpy(filename, "assets/Graphics/screen/menu/keymap/keymap_");
	
	KeyMapInitArray((char*)"assets/Input/keymap.txt", &commands);
	
	
	strcpy((char*)(filename + pos), SDL_GetScancodeName((SDL_Scancode)commands[C_UP]));
	strcpy((char*)(filename + pos + strlen(SDL_GetScancodeName((SDL_Scancode)commands[C_UP]))), ".png");
	create_textfield(world, "keymap_up", (WIDTH / 2), (HEIGHT / 2) - 100 + (0 * 75), SDL_GetScancodeName((SDL_Scancode)commands[C_UP]), 0);
	
	strcpy((char*)(filename + pos), SDL_GetScancodeName((SDL_Scancode)commands[C_DOWN]));
	strcpy((char*)(filename + pos + strlen(SDL_GetScancodeName((SDL_Scancode)commands[C_DOWN]))), ".png");
	create_textfield(world, "keymap_down", (WIDTH / 2), (HEIGHT / 2) - 100 + (1 * 75), SDL_GetScancodeName((SDL_Scancode)commands[C_DOWN]), 0);
	
	strcpy((char*)(filename + pos), SDL_GetScancodeName((SDL_Scancode)commands[C_LEFT]));
	strcpy((char*)(filename + pos + strlen(SDL_GetScancodeName((SDL_Scancode)commands[C_LEFT]))), ".png");
	create_textfield(world, "keymap_left", (WIDTH / 2), (HEIGHT / 2) - 100 + (2 * 75), SDL_GetScancodeName((SDL_Scancode)commands[C_LEFT]), 0);
	
	strcpy((char*)(filename + pos), SDL_GetScancodeName((SDL_Scancode)commands[C_RIGHT]));
	strcpy((char*)(filename + pos + strlen(SDL_GetScancodeName((SDL_Scancode)commands[C_RIGHT]))), ".png");
	create_textfield(world, "keymap_right", (WIDTH / 2), (HEIGHT / 2) - 100 + (3 * 75), SDL_GetScancodeName((SDL_Scancode)commands[C_RIGHT]), 0);
	
	strcpy((char*)(filename + pos), SDL_GetScancodeName((SDL_Scancode)commands[C_ACTION]));
	strcpy((char*)(filename + pos + strlen(SDL_GetScancodeName((SDL_Scancode)commands[C_ACTION]))), ".png");
	create_textfield(world, "keymap_action", (WIDTH / 2), (HEIGHT / 2) - 100 + (4 * 75), SDL_GetScancodeName((SDL_Scancode)commands[C_ACTION]), 0);
	
	
	create_button(world, "BACK", "keymap_back", (WIDTH / 2) + 300, (HEIGHT / 2) + 275);
	create_button(world, "DEFAULT", "keymap_default", (WIDTH / 2) - 300, (HEIGHT / 2) + 275);
	create_button(world, "SAVE", "keymap_save", (WIDTH / 2), (HEIGHT / 2) + 275);
}

/**
 * Creates the credits menu.
 * 
 * This menu displays everybody who worked on the project.
 *
 * @param world The world struct
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void create_credits_menu(World *world) {
	
	unsigned int entity;
	
	create_main_menu_background(world);
	
	entity = create_entity(world, COMPONENT_MENU_ITEM | COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	world->renderPlayer[entity].width = WIDTH;
	world->renderPlayer[entity].height = HEIGHT;
	world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/screen/menu/credits.png");
	if (!world->renderPlayer[entity].playerSurface) {
		printf("Error loading credits menu\n");
	}
	
	world->position[entity].x = 0;
	world->position[entity].y = 0;
	world->position[entity].width = WIDTH;
	world->position[entity].height = HEIGHT;
	
	
	create_button(world, "BACK", "credits_back", (WIDTH / 2), (HEIGHT / 2) + 275);
}

/**
 * Creates the Setup menu.
 * 
 * In this menu you input your username and server IP.
 *
 * @param world The world struct
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void create_setup_menu(World *world) {
	
	create_main_menu_background(world);
	
	create_title(world, "SETUP", (WIDTH / 2), (HEIGHT / 2) - 250);
	
	create_label(world, "USERNAME", (WIDTH / 2) - 550, (HEIGHT / 2) - 25);
	create_textfield(world, "setup_username", (WIDTH / 2) - 100, (HEIGHT / 2) - 30, "", 1);
	
	create_label(world, "SERVER IP", (WIDTH / 2) - 550, (HEIGHT / 2) + 50);
	create_textfield(world, "setup_serverip", (WIDTH / 2) - 100, (HEIGHT / 2) + 45, "192.168.0.49", 1);
	
	
	create_button(world, "BACK", "setup_back", (WIDTH / 2) + 150, (HEIGHT / 2) + 275);
	create_button(world, "PLAY", "setup_play", (WIDTH / 2) - 150, (HEIGHT / 2) + 275);
}

/**
 * Creates the Blue Screen Of Death menu.
 * 
 * In this menu you press x to exit or y to go to the main menu.
 *
 * @param world The world struct
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void create_bsod_menu(World *world) {
	
	unsigned int entity;
	
	entity = create_entity(world, COMPONENT_MENU_ITEM | COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	world->renderPlayer[entity].width = WIDTH;
	world->renderPlayer[entity].height = HEIGHT;
	world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/end/blue_screen.png");
	if (!world->renderPlayer[entity].playerSurface) {
		printf("Error loading BSOD image.\n");
	}
	
	world->position[entity].x = 0;
	world->position[entity].y = 0;
	world->position[entity].width = WIDTH;
	world->position[entity].height = HEIGHT;
	
	//create_button(world, "assets/Graphics/end/press_x_to_exit.png", "bsod_exit", (WIDTH / 2) - (BUTTON_WIDTH / 2) - 400, (HEIGHT / 2) + 100);
	//create_button(world, "assets/Graphics/end/press_y_to_continue.png", "bsod_continue", (WIDTH / 2) - (BUTTON_WIDTH / 2) - 400, (HEIGHT / 2) + 150);
}


/**
 * Creates the cutscene screen for the start of the game (night scene)
 * 
 * Cannot interact with this screen. 
 *
 * @param world The world struct
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void create_intro(World *world) {
	
	destroy_world(world);
	
	load_cutscene("assets/Graphics/cutscene/game_intro/intro_cutscene.txt", world, CUTSCENE_GAME_INTRO);
	load_cutscene("assets/Graphics/cutscene/game_intro/box_cutscene.txt", world, -1);
}

/**
 * Creates the loading screen
 * 
 * Cannot interact with the game in this screen
 *
 * @param world The world struct
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void create_load_screen(World *world) {
	
	unsigned int entity;
	
	entity = create_entity(world, COMPONENT_MENU_ITEM | COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_ANIMATION);
	
	world->position[entity].x = 0;
	world->position[entity].y = 0;
	world->position[entity].width = WIDTH;
	world->position[entity].height = HEIGHT;
	
	load_animation("assets/Graphics/screen/loading/load_animation.txt", world, entity);
	
	world->animation[entity].id = 1;
	
	world->renderPlayer[entity].width = WIDTH;
	world->renderPlayer[entity].height = HEIGHT;

	play_animation(world, entity, "load");
}

/**
 * Creates the screen for character selection with all members of the Cut the Power
 * development team.
 * 
 * Clicking a character will bring up the Name/Server screen.
 *
 * @param world The world struct
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void create_select_screen(World *world) {
	
	unsigned int entity;
	
	entity = create_entity(world, COMPONENT_MENU_ITEM | COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	world->position[entity].x = 0;
	world->position[entity].y = 0;
	world->position[entity].width = WIDTH;
	world->position[entity].height = HEIGHT;
	if ((world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/screen/menu/select/select.png")) == NULL){
		printf("Unable to find select image\n");
	}

	create_animated_button(world, "assets/Graphics/screen/menu/select/abhishek/abhishek_animation.txt", 150, 94, "menu_select_abhishek");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/aman/aman_animation.txt", 350, 94, "menu_select_aman");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/andrew/andrew_animation.txt", 550, 94, "menu_select_andrew");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/chris/chris_animation.txt", 750, 94, "menu_select_chris");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/clark/clark_animation.txt", 950, 94, "menu_select_clark");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/cory/cory_animation.txt", 150, 244, "menu_select_cory");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/damien/damien_animation.txt", 350, 244, "menu_select_damien");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/german/german_animation.txt", 550, 244, "menu_select_german");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/ian/ian_animation.txt", 750, 244, "menu_select_ian");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/jordan/jordan_animation.txt", 950, 244, "menu_select_jordan");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/josh/josh_animation.txt", 150, 394, "menu_select_josh");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/konst/konst_animation.txt", 350, 394, "menu_select_konst");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/mat/mat_animation.txt", 550, 394, "menu_select_mat");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/ramzi/ramzi_animation.txt", 750, 394, "menu_select_ramzi");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/robin/robin_animation.txt", 950, 394, "menu_select_robin");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/sam/sam_animation.txt", 150, 544, "menu_select_sam");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/shane/shane_animation.txt", 350, 544, "menu_select_shane");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/tim/tim_animation.txt", 550, 544, "menu_select_tim");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/vincent/vincent_animation.txt", 750, 544, "menu_select_vincent");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/random/random_animation.txt", 950, 544, "menu_select_random");	
	create_animated_button(world, "assets/Graphics/screen/menu/select/ian/secret/sparkle_animation.txt", 1150, 0, "menu_select_albert");	
	
	
	world->renderPlayer[entity].width = WIDTH;
	world->renderPlayer[entity].height = HEIGHT;
}

/**
 * Draws a small menu on top of the game when in the game.
 * 
 * Has the same menu as the Options menu minus keybinding
 *
 * @param world The world struct
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void create_pause_screen(World *world) {
	
	unsigned int entity;
	
	entity = create_entity(world, COMPONENT_MENU_ITEM | COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	const int w = 650;
	const int h = 600;
	
	world->position[entity].x = (WIDTH / 2) - (w / 2);
	world->position[entity].y = (HEIGHT / 2) - (h / 2);
	world->position[entity].width = w;
	world->position[entity].height = h;
	
	if ((world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/screen/pause/background.png")) == NULL){
		printf("Unable to load pause image\n");
	}
	
	world->renderPlayer[entity].width = w;
	world->renderPlayer[entity].height = h;
	
	
	
	create_title(world, "OPTIONS", (WIDTH / 2), (HEIGHT / 2) - 250);
	
	create_button(world, "SOUND ON", "ingame_sound_on", (WIDTH / 2), (HEIGHT / 2) - 25);
	create_button(world, "FULLSCREEN OFF", "ingame_fullscreen_off", (WIDTH / 2), (HEIGHT / 2) + 50);
	create_button(world, "BACK", "ingame_back", (WIDTH / 2), (HEIGHT / 2) + 125);
	create_button(world, "EXIT TO MENU", "ingame_exit", (WIDTH / 2), (HEIGHT / 2) + 200);
}

/**
 * Creates the cutscene screen for the introduction to the game
 * 
 * Cannot interact with the game in this screen.
 *
 * @param world The world struct
 * @param character The character selected by the user to render in this cutscene
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void create_van_intro(World *world, int character) {
	
	unsigned int entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	const int w = 1280;
	const int h = 768;
	
	world->position[entity].x = 0;//(WIDTH / 2) - (w / 2);
	world->position[entity].y = 0;//(HEIGHT / 2) - (h / 2);
	world->position[entity].width = w;
	world->position[entity].height = h;
	
	if ((world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/cutscene/van_intro/background_enter.png")) == NULL){
		printf("Unable to load van background image\n");
	}
	
	world->renderPlayer[entity].width = w;
	world->renderPlayer[entity].height = h;
	
	
	
	if (load_cutscene("assets/Graphics/cutscene/van_intro/van_cutscene.txt", world, -1) == MAX_ENTITIES) {
		printf("Error loading van cutscene!\n");
		return;
	}
	
	if ((entity = load_cutscene("assets/Graphics/cutscene/van_intro/player_cutscene.txt", world, CUTSCENE_VAN_INTRO)) == MAX_ENTITIES) {
		printf("Error loading player cutscene!\n");
		return;
	}
	
	setup_character_animation(world, character, entity);
	
}
