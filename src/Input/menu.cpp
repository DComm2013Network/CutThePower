#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

#include "menu.h"
#include "../world.h"
#include "../components.h"
#include "../systems.h"
#include "../sound.h"

unsigned int background = MAX_ENTITIES + 1;

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
		
		if (entity != background) {
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
void create_button(World *world, char *image, char *name, int x, int y) {
	
	char *new_name;
	unsigned int entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_BUTTON | COMPONENT_MOUSE);
	
	world->renderPlayer[entity].width = BUTTON_WIDTH;
	world->renderPlayer[entity].height = BUTTON_HEIGHT;
	world->renderPlayer[entity].playerSurface = IMG_Load(image);
	if (!world->renderPlayer[entity].playerSurface) {
		printf("Error loading image in create_button: %s\n", image);
	}
	
	world->position[entity].x = x;
	world->position[entity].y = y;
	world->position[entity].width = BUTTON_WIDTH;
	world->position[entity].height = BUTTON_HEIGHT;
	
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
void create_label(World *world, char *image, int x, int y, int w, int h) {
	
	unsigned int entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	world->renderPlayer[entity].width = w;
	world->renderPlayer[entity].height = h;
	world->renderPlayer[entity].playerSurface = IMG_Load(image);
	if (!world->renderPlayer[entity].playerSurface) {
		printf("Error loading image in create_label: %s\n", image);
	}
	
	world->position[entity].x = x;
	world->position[entity].y = y;
	world->position[entity].width = w;
	world->position[entity].height = h;
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
void create_textfield(World *world, char *name, int x, int y, const char* text, bool big) {
	
	unsigned int entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_TEXTFIELD | COMPONENT_MOUSE);
	
	world->renderPlayer[entity].width = TEXT_WIDTH;
	world->renderPlayer[entity].height = TEXT_HEIGHT;
	if(big)
	{
		world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/screen/menu/text_field.png");
	} else {
		world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/screen/menu/small_text_field.png");
	}
	if (!world->renderPlayer[entity].playerSurface) {
		printf("Error loading image in create_textfield.\n");
	}
	
	world->position[entity].x = x;
	world->position[entity].y = y;
	world->position[entity].width = TEXT_WIDTH;
	world->position[entity].height = TEXT_HEIGHT;
	
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

void create_animated_button(World *world, char* fileName, int x, int y, char* name) {
	
	char *new_name;
	unsigned int entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_ANIMATION | COMPONENT_BUTTON | COMPONENT_MOUSE);
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
		return;
	}
	
	background = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_ANIMATION);
	
	world->position[background].x = 0;
	world->position[background].y = 0;
	world->position[background].width = WIDTH;
	world->position[background].height = HEIGHT;
	
	load_animation((char*)"assets/Graphics/screen/menu/mainmenu_animation.txt", world, background);
	
	world->renderPlayer[background].width = WIDTH;
	world->renderPlayer[background].height = HEIGHT;
	
	play_music(SOUND_MUSIC_MENU_RAIN);
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
	
	create_label(world, (char*)"assets/Graphics/screen/menu/menu_label_cutthepower.png", (WIDTH / 2) - (TITLE_WIDTH / 2), (HEIGHT / 2) - 250, TITLE_WIDTH, TITLE_HEIGHT);
	
	create_button(world, (char*)"assets/Graphics/screen/menu/menu_button_play.png", (char*)"mainmenu_play", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) - 25);
	create_button(world, (char*)"assets/Graphics/screen/menu/menu_button_options.png", (char*)"mainmenu_options", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) + 50);
	create_button(world, (char*)"assets/Graphics/screen/menu/menu_button_credits.png", (char*)"mainmenu_credits", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) + 125);
	create_button(world, (char*)"assets/Graphics/screen/menu/menu_button_exit.png", (char*)"mainmenu_exit", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) + 200);
	
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
	
	create_label(world, (char*)"assets/Graphics/screen/menu/menu_label_options.png", (WIDTH / 2) - (TITLE_WIDTH / 2), (HEIGHT / 2) - 250, TITLE_WIDTH, TITLE_HEIGHT);
	
	create_button(world, (char*)"assets/Graphics/screen/menu/menu_button_soundon.png", (char*)"options_sound_on", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) + 50);
	create_button(world, (char*)"assets/Graphics/screen/menu/menu_button_keymap.png", (char*)"options_keymap", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) + 125);
	create_button(world, (char*)"assets/Graphics/screen/menu/menu_button_back.png", (char*)"options_back", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) + 200);
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
	
	create_label(world, (char*)"assets/Graphics/screen/menu/menu_label_keymap.png", (WIDTH / 2) - (TITLE_WIDTH / 2), (HEIGHT / 2) - 250, TITLE_WIDTH, TITLE_HEIGHT);
	
	create_label(world, (char*)"assets/Graphics/screen/menu/menu_label_up.png", (WIDTH / 2) - 550, (HEIGHT / 2) - 100, BUTTON_WIDTH, BUTTON_HEIGHT);
	create_label(world, (char*)"assets/Graphics/screen/menu/menu_label_down.png", (WIDTH / 2) - 550, (HEIGHT / 2) - 25, BUTTON_WIDTH, BUTTON_HEIGHT);
	create_label(world, (char*)"assets/Graphics/screen/menu/menu_label_left.png", (WIDTH / 2) - 550, (HEIGHT / 2) + 50, BUTTON_WIDTH, BUTTON_HEIGHT);
	create_label(world, (char*)"assets/Graphics/screen/menu/menu_label_right.png", (WIDTH / 2) - 550, (HEIGHT / 2) + 125, BUTTON_WIDTH, BUTTON_HEIGHT);
	create_label(world, (char*)"assets/Graphics/screen/menu/menu_label_action.png", (WIDTH / 2) - 550, (HEIGHT / 2) + 200, BUTTON_WIDTH, BUTTON_HEIGHT);
	
	//load information
	const int pos = strlen("assets/Graphics/screen/menu/keymap/keymap_");
	int *commands;
	char filename[64];
	strcpy(filename, "assets/Graphics/screen/menu/keymap/keymap_");
	
	KeyMapInitArray((char*)"assets/Input/keymap.txt", &commands);
	
	
	strcpy((char*)(filename + pos), SDL_GetScancodeName((SDL_Scancode)commands[C_UP]));
	strcpy((char*)(filename + pos + strlen(SDL_GetScancodeName((SDL_Scancode)commands[C_UP]))), ".png");
	//create_button(world, filename, "keymap_up", (WIDTH / 2) + 250, (HEIGHT / 2) - 100 + (0 * 75));
	create_textfield(world, (char*)"keymap_up", (WIDTH / 2) + 250, (HEIGHT / 2) - 100 + (0 * 75), SDL_GetScancodeName((SDL_Scancode)commands[C_UP]), 0);
	
	strcpy((char*)(filename + pos), SDL_GetScancodeName((SDL_Scancode)commands[C_DOWN]));
	strcpy((char*)(filename + pos + strlen(SDL_GetScancodeName((SDL_Scancode)commands[C_DOWN]))), ".png");
	//create_button(world, filename, "keymap_down", (WIDTH / 2) + 250, (HEIGHT / 2) - 100 + (1 * 75));
	create_textfield(world, (char*)"keymap_down", (WIDTH / 2) + 250, (HEIGHT / 2) - 100 + (1 * 75), SDL_GetScancodeName((SDL_Scancode)commands[C_DOWN]), 0);
	
	strcpy((char*)(filename + pos), SDL_GetScancodeName((SDL_Scancode)commands[C_LEFT]));
	strcpy((char*)(filename + pos + strlen(SDL_GetScancodeName((SDL_Scancode)commands[C_LEFT]))), ".png");
	//create_button(world, filename, "keymap_left", (WIDTH / 2) + 250, (HEIGHT / 2) - 100 + (2 * 75));
	create_textfield(world, (char*)"keymap_left", (WIDTH / 2) + 250, (HEIGHT / 2) - 100 + (2 * 75), SDL_GetScancodeName((SDL_Scancode)commands[C_LEFT]), 0);
	
	strcpy((char*)(filename + pos), SDL_GetScancodeName((SDL_Scancode)commands[C_RIGHT]));
	strcpy((char*)(filename + pos + strlen(SDL_GetScancodeName((SDL_Scancode)commands[C_RIGHT]))), ".png");
	//create_button(world, filename, "keymap_right", (WIDTH / 2) + 250, (HEIGHT / 2) - 100 + (3 * 75));
	create_textfield(world, (char*)"keymap_right", (WIDTH / 2) + 250, (HEIGHT / 2) - 100 + (3 * 75), SDL_GetScancodeName((SDL_Scancode)commands[C_RIGHT]), 0);
	
	strcpy((char*)(filename + pos), SDL_GetScancodeName((SDL_Scancode)commands[C_ACTION]));
	strcpy((char*)(filename + pos + strlen(SDL_GetScancodeName((SDL_Scancode)commands[C_ACTION]))), ".png");
	//create_button(world, filename, "keymap_action", (WIDTH / 2) + 250, (HEIGHT / 2) - 100 + (4 * 75));
	create_textfield(world, (char*)"keymap_action", (WIDTH / 2) + 250, (HEIGHT / 2) - 100 + (4 * 75), SDL_GetScancodeName((SDL_Scancode)commands[C_ACTION]), 0);
	
	
	create_button(world, (char*)"assets/Graphics/screen/menu/menu_button_back.png", (char*)"keymap_back", (WIDTH / 2) - (BUTTON_WIDTH / 2) + 240, (HEIGHT / 2) + 275);
	create_button(world, (char*)"assets/Graphics/screen/menu/menu_button_default.png", (char*)"keymap_default", (WIDTH / 2) - (BUTTON_WIDTH / 2) - 240, (HEIGHT / 2) + 275);
	create_button(world, (char*)"assets/Graphics/screen/menu/menu_button_save.png", (char*)"keymap_save", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) + 275);
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
	
	//init_world(world);
	
	unsigned int entity;
	
	create_main_menu_background(world);
	
	entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	world->renderPlayer[entity].width = WIDTH;
	world->renderPlayer[entity].height = HEIGHT;
	world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/screen/menu/credits.png");
	if (!world->renderPlayer[entity].playerSurface) {
		printf("mat is a doof\n");
	}
	
	world->position[entity].x = 0;
	world->position[entity].y = 0;
	world->position[entity].width = WIDTH;
	world->position[entity].height = HEIGHT;
	
	
	create_button(world, (char*)"assets/Graphics/screen/menu/menu_button_back.png", (char*)"credits_back", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) + 275);
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
	
	create_label(world, (char*)"assets/Graphics/screen/menu/menu_label_setup.png", (WIDTH / 2) - (TITLE_WIDTH / 2), (HEIGHT / 2) - 250, TITLE_WIDTH, TITLE_HEIGHT);
	
	create_label(world, (char*)"assets/Graphics/screen/menu/menu_label_username.png", (WIDTH / 2) - 550, (HEIGHT / 2) - 25, BUTTON_WIDTH, BUTTON_HEIGHT);
	create_textfield(world, (char*)"setup_username", (WIDTH / 2) - 55, (HEIGHT / 2) - 30, NULL, 1);
	
	create_label(world, (char*)"assets/Graphics/screen/menu/menu_label_serverip.png", (WIDTH / 2) - 550, (HEIGHT / 2) + 50, BUTTON_WIDTH, BUTTON_HEIGHT);
	create_textfield(world, (char*)"setup_serverip", (WIDTH / 2) - 55, (HEIGHT / 2) + 45, NULL, 1);
	
	
	create_button(world, (char*)"assets/Graphics/screen/menu/menu_button_back.png", (char*)"setup_back", (WIDTH / 2) - (BUTTON_WIDTH / 2) + 150, (HEIGHT / 2) + 275);
	create_button(world, (char*)"assets/Graphics/screen/menu/menu_button_play.png", (char*)"setup_play", (WIDTH / 2) - (BUTTON_WIDTH / 2) - 150, (HEIGHT / 2) + 275);
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
	
	entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	world->renderPlayer[entity].width = WIDTH;
	world->renderPlayer[entity].height = HEIGHT;
	world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/end/blue_screen.png");
	if (!world->renderPlayer[entity].playerSurface) {
		printf("mat is a doof\n");
	}
	
	world->position[entity].x = 0;
	world->position[entity].y = 0;
	world->position[entity].width = WIDTH;
	world->position[entity].height = HEIGHT;
	
	create_button(world, (char*)"assets/Graphics/end/press_x_to_exit.png", (char*)"bsod_exit", (WIDTH / 2) - (BUTTON_WIDTH / 2) - 400, (HEIGHT / 2) + 100);
	create_button(world, (char*)"assets/Graphics/end/press_y_to_continue.png", (char*)"bsod_continue", (WIDTH / 2) - (BUTTON_WIDTH / 2) - 400, (HEIGHT / 2) + 150);
}



void create_intro(World *world) {
	
	unsigned int entity;
	
	destroy_world(world);
	
	entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_ANIMATION);
	
	world->position[entity].x = 0;
	world->position[entity].y = 0;
	world->position[entity].width = WIDTH;
	world->position[entity].height = HEIGHT;
	
	load_animation((char*)"assets/Graphics/screen/intro/intro_animation.txt", world, entity);
	
	world->animation[entity].id = 0;
	
	world->renderPlayer[entity].width = WIDTH;
	world->renderPlayer[entity].height = HEIGHT;
	
	play_animation(world, entity, (char*)"intro");
}

void create_load_screen(World *world) {
	
	unsigned int entity;
	
	entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_ANIMATION);
	
	world->position[entity].x = 0;
	world->position[entity].y = 0;
	world->position[entity].width = WIDTH;
	world->position[entity].height = HEIGHT;
	
	load_animation((char*)"assets/Graphics/screen/loading/load_animation.txt", world, entity);
	
	world->animation[entity].id = 1;
	
	world->renderPlayer[entity].width = WIDTH;
	world->renderPlayer[entity].height = HEIGHT;
	
	
	play_animation(world, entity, (char*)"load");
}

void create_select_screen(World *world) {
	
	unsigned int entity;
	
	entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	world->position[entity].x = 0;
	world->position[entity].y = 0;
	world->position[entity].width = WIDTH;
	world->position[entity].height = HEIGHT;
	if ((world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/screen/menu/select/select.png")) == NULL){
		printf("Unable to find select image\n");
	}

	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/abhishek/abhishek_animation.txt", 150, 94, (char*)"menu_select_abhishek");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/aman/aman_animation.txt", 350, 94, (char*)"menu_select_aman");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/andrew/andrew_animation.txt", 550, 94, (char*)"menu_select_andrew");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/chris/chris_animation.txt", 750, 94, (char*)"menu_select_chris");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/clark/clark_animation.txt", 950, 94, (char*)"menu_select_clark");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/cory/cory_animation.txt", 150, 244, (char*)"menu_select_cory");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/damien/damien_animation.txt", 350, 244, (char*)"menu_select_damien");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/german/german_animation.txt", 550, 244, (char*)"menu_select_german");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/ian/ian_animation.txt", 750, 244, (char*)"menu_select_ian");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/jordan/jordan_animation.txt", 950, 244, (char*)"menu_select_jordan");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/josh/josh_animation.txt", 150, 394, (char*)"menu_select_josh");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/konst/konst_animation.txt", 350, 394, (char*)"menu_select_konst");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/mat/mat_animation.txt", 550, 394, (char*)"menu_select_mat");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/ramzi/ramzi_animation.txt", 750, 394, (char*)"menu_select_ramzi");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/robin/robin_animation.txt", 950, 394, (char*)"menu_select_robin");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/sam/sam_animation.txt", 150, 544, (char*)"menu_select_sam");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/shane/shane_animation.txt", 350, 544, (char*)"menu_select_shane");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/tim/tim_animation.txt", 550, 544, (char*)"menu_select_tim");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/vincent/vincent_animation.txt", 750, 544, (char*)"menu_select_vincent");	
	create_animated_button(world, (char*)"assets/Graphics/screen/menu/select/random/random_animation.txt", 950, 544, (char*)"menu_select_random");	
	
	
	
	world->renderPlayer[entity].width = WIDTH;
	world->renderPlayer[entity].height = HEIGHT;
}


