/**
 * [File description]
 *
 * @file world.cpp
 */

#define BUTTON_WIDTH 	400
#define BUTTON_HEIGHT 	50
#define LABEL_WIDTH		250
#define LABEL_HEIGHT	52
#define TITLE_WIDTH		750
#define TITLE_HEIGHT	150
#define KEYMAP_WIDTH	150
#define KEYMAP_HEIGHT	50
#define TEXT_WIDTH		410
#define TEXT_HEIGHT		60

#include "Input/systems.h"
#include "world.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

#include <stdlib.h>

void create_label(World *world, char *image, int x, int y, int w, int h);
void create_button(World *world, char *image, char *name, int x, int y);

/**
 * This function initializes every mask to be 0, so that there are no components.
 * 
 * @param world The world struct containing the entity masks to be zeroed.
 *
 * @designer
 * @author 
 */
void init_world(World* world) {
	int i;
	for(i = 0; i < MAX_ENTITIES; ++i) {
		world->mask[i] = COMPONENT_EMPTY;
	}
}

/**
 * This function loops through the masks of all the entities and finds the first one
 * that is not used.
 *
 * @param world 		The world struct containing all entities.
 * @param attributes 	The component mask to apply to the entity.
 *
 * @return 	The entity number if there was an entity available, or MAX_ENTITIES if all
 *			of the entities are in use.
 *
 * @designer
 * @author
 */
unsigned int create_entity(World* world, unsigned int attributes) {
	unsigned int entity;
	for(entity = 0; entity < MAX_ENTITIES; entity++) {
		if (world->mask[entity] == COMPONENT_EMPTY) {
			world->mask[entity] = attributes;
			return entity;
		}
	}
	return MAX_ENTITIES;
}

/**
 * [Brief description (one sentence)]
 *
 * [Detailed description (if necessary)]
 *
 * @param world
 * @param map
 * @param width
 * @param height
 * @param tileSize
 *
 * @return
 *
 * @designer
 * @author
 */
unsigned int create_level(World* world, uint8_t** map, int width, int height, int tileSize) {
	unsigned int entity = 0;
	int lastID = -1;
	unsigned int tempMask = 0;
	int i = 0;
	int n = 0;
	for (entity = 0; entity < MAX_ENTITIES; ++entity) {
		tempMask = world->mask[entity] & COMPONENT_LEVEL;
		if (tempMask == COMPONENT_LEVEL) {
			lastID = world->level[entity].levelID;
		}
	
		if (world->mask[entity] == COMPONENT_EMPTY) {
			lastID++;
			world->mask[entity] = COMPONENT_LEVEL;
			world->level[entity].map = (uint8_t**)malloc(sizeof(uint8_t*) * width);
			for (i = 0; i < width; i++) {
				world->level[entity].map[i] = (uint8_t*)malloc(sizeof(uint8_t) * height);
			}
			for (i = 0; i < width; i++) {
				for (n = 0; n < height; n++) {
					world->level[entity].map[i][n] = map[i][n];
				}
			}
			world->level[entity].levelID = lastID;
			world->level[entity].width = width;
			world->level[entity].height = height;
			world->level[entity].tileSize = tileSize;
			
			return entity;
		}
	}
	
	
	return MAX_ENTITIES;
}

/**
 * [Brief description (one sentence)]
 *
 * [Detailed description (if necessary)]
 *
 * @param world
 * @param x
 * @param y
 * @param controllable
 *
 * @return
 *
 * @designer
 * @author
 */
unsigned int create_player(World* world, int x, int y, bool controllable) {
	unsigned int entity;
	PositionComponent pos;
	RenderPlayerComponent render;
	MovementComponent movement;
	ControllableComponent control;
	CommandComponent command;
	int lastID = -1;
	unsigned int tempMask = 0;
	
	//MovementComponent movement;
	//CollisionComponent collision;
	
	render.width = 80;
	render.height = 80;
	render.playerSurface = IMG_Load("assets/Graphics/player_80px.png");
	if (!render.playerSurface) {
		printf("mat is a doof\n");
	}
	
	pos.x = x;
	pos.y = y;

	pos.width = render.width;
	pos.height = render.height;
	pos.s = 0;
	pos.level = 0;
	
	movement.id = 0;
	movement.lastDirection = 0;
	movement.acceleration = 0.3;
	movement.maxSpeed = 1.8;
	movement.movX = 0;
	movement.movY = 0;
	
	command.commands[C_UP] = false;
	command.commands[C_DOWN] = false;
	command.commands[C_LEFT] = false;
	command.commands[C_RIGHT] = false;
	
	control.active = true;
	
	for(entity = 0; entity < MAX_ENTITIES; ++entity) {
		tempMask = world->mask[entity] & COMPONENT_POSITION;
		if (tempMask == COMPONENT_MOVEMENT) {
			lastID = world->movement[entity].id;
		}
		
		if (world->mask[entity] == COMPONENT_EMPTY) {
			lastID += 1;
			movement.id = lastID;
			if (controllable) {
				world->mask[entity] =	COMPONENT_POSITION | 
										COMPONENT_RENDER_PLAYER | 
										COMPONENT_COMMAND | 
										COMPONENT_MOVEMENT | 
										COMPONENT_COLLISION |
										COMPONENT_CONTROLLABLE; //| COMPONENT_MOVEMENT | COMPONENT_COLLISION;
			} else {
				world->mask[entity] =	COMPONENT_POSITION | 
										COMPONENT_RENDER_PLAYER | 
										COMPONENT_COMMAND | 
										COMPONENT_COLLISION | 
										COMPONENT_MOVEMENT;
			}
			world->position[entity] = pos;
			world->renderPlayer[entity] = render;
			world->command[entity] = command;
			world->movement[entity] = movement;
			if (controllable) {
				world->controllable[entity] = control;
			}
			//world->collision[entity] = collision;
			if (controllable) {
				world->controllable[entity] = control;
			}
			//world->collision[entity] = collision;
			return entity;
		}
	}
	return MAX_ENTITIES;
}

/**
 * Clean up is easy.
 *
 * [Detailed description (if necessary)]
 * 
 * TODO: free all memory on heap. Memory leaks suck.
 *
 * @param world
 * @param entity
 *
 * @designer
 * @author
 */
void destroy_entity(World* world, const unsigned int entity) {
	world->mask[entity] = COMPONENT_EMPTY;
}

//TODO: IMPLEMENT
void destroy_world(World *world) {
	unsigned int entity;
	
	for(entity = 0; entity < MAX_ENTITIES; entity++) {
		destroy_entity(world, entity);
	}
}

void create_button(World *world, char *image, char *name, int x, int y) {
	
	char *new_name;
	unsigned int entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_BUTTON | COMPONENT_MOUSE);
	
	world->renderPlayer[entity].width = BUTTON_WIDTH;
	world->renderPlayer[entity].height = BUTTON_HEIGHT;
	world->renderPlayer[entity].playerSurface = IMG_Load(image);
	if (!world->renderPlayer[entity].playerSurface) {
		printf("mat is a doof\n");
	}
	
	world->position[entity].x = x;
	world->position[entity].y = y;
	world->position[entity].width = BUTTON_WIDTH;
	world->position[entity].height = BUTTON_HEIGHT;
	
	world->button[entity].prevState = false;
	world->button[entity].currentState = false;
	
	new_name = (char*)malloc(sizeof(char) * strlen(name));
	
	strcpy(new_name, name);
	
	world->button[entity].label = new_name;
}

void create_label(World *world, char *image, int x, int y, int w, int h) {
	
	unsigned int entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	world->renderPlayer[entity].width = w;
	world->renderPlayer[entity].height = h;
	world->renderPlayer[entity].playerSurface = IMG_Load(image);
	if (!world->renderPlayer[entity].playerSurface) {
		printf("mat is a doof\n");
	}
	
	world->position[entity].x = x;
	world->position[entity].y = y;
	world->position[entity].width = w;
	world->position[entity].height = h;
}

void create_textfield(World *world, char *name, int x, int y) {
	
	unsigned int entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_TEXTFIELD | COMPONENT_MOUSE);
	
	world->renderPlayer[entity].width = TEXT_WIDTH;
	world->renderPlayer[entity].height = TEXT_HEIGHT;
	world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/menu/text_field.png");
	if (!world->renderPlayer[entity].playerSurface) {
		printf("mat is a doof\n");
	}
	
	world->position[entity].x = x;
	world->position[entity].y = y;
	world->position[entity].width = TEXT_WIDTH;
	world->position[entity].height = TEXT_HEIGHT;
	
	world->text[entity].text = (char*)calloc(MAX_STRING, sizeof(char));
	world->text[entity].length = 0;
	world->text[entity].focused = false;
	world->text[entity].number = false;
	
}

void create_main_menu(World* world) {
	
	init_world(world);
	
	unsigned int entity;
	
	entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	world->renderPlayer[entity].width = WIDTH;
	world->renderPlayer[entity].height = HEIGHT;
	world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/menu/MainMenu.png");
	if (!world->renderPlayer[entity].playerSurface) {
		printf("mat is a doof\n");
	}
	
	world->position[entity].x = 0;
	world->position[entity].y = 0;
	world->position[entity].width = WIDTH;
	world->position[entity].height = HEIGHT;
	
	create_label(world, "assets/Graphics/menu/menu_label_cutthepower.png", (WIDTH / 2) - (TITLE_WIDTH / 2), (HEIGHT / 2) - 250, TITLE_WIDTH, TITLE_HEIGHT);
	
	create_button(world, "assets/Graphics/menu/menu_button_play.png", "mainmenu_play", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) - 25);
	create_button(world, "assets/Graphics/menu/menu_button_options.png", "mainmenu_options", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) + 50);
	create_button(world, "assets/Graphics/menu/menu_button_credits.png", "mainmenu_credits", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) + 125);
	create_button(world, "assets/Graphics/menu/menu_button_exit.png", "mainmenu_exit", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) + 200);
	
}

void create_options_menu(World *world) {
	
	init_world(world);
	
	unsigned int entity;
	
	entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	world->renderPlayer[entity].width = WIDTH;
	world->renderPlayer[entity].height = HEIGHT;
	world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/menu/MainMenu.png");
	if (!world->renderPlayer[entity].playerSurface) {
		printf("mat is a doof\n");
	}
	
	world->position[entity].x = 0;
	world->position[entity].y = 0;
	world->position[entity].width = WIDTH;
	world->position[entity].height = HEIGHT;
	
	create_label(world, "assets/Graphics/menu/menu_label_options.png", (WIDTH / 2) - (TITLE_WIDTH / 2), (HEIGHT / 2) - 250, TITLE_WIDTH, TITLE_HEIGHT);
	
	create_button(world, "assets/Graphics/menu/menu_button_soundoff.png", "options_sound_off", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) + 50);
	create_button(world, "assets/Graphics/menu/menu_button_keymap.png", "options_keymap", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) + 125);
	create_button(world, "assets/Graphics/menu/menu_button_back.png", "options_back", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) + 200);
}

void create_keymap_menu(World *world) {
	
	init_world(world);
	
	unsigned int entity;
	
	entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	world->renderPlayer[entity].width = WIDTH;
	world->renderPlayer[entity].height = HEIGHT;
	world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/menu/MainMenu.png");
	if (!world->renderPlayer[entity].playerSurface) {
		printf("mat is a doof\n");
	}
	
	world->position[entity].x = 0;
	world->position[entity].y = 0;
	world->position[entity].width = WIDTH;
	world->position[entity].height = HEIGHT;
	
	create_label(world, "assets/Graphics/menu/menu_label_keymap.png", (WIDTH / 2) - (TITLE_WIDTH / 2), (HEIGHT / 2) - 250, TITLE_WIDTH, TITLE_HEIGHT);
	
	create_label(world, "assets/Graphics/menu/menu_label_up.png", (WIDTH / 2) - 550, (HEIGHT / 2) - 100, BUTTON_WIDTH, BUTTON_HEIGHT);
	create_label(world, "assets/Graphics/menu/menu_label_down.png", (WIDTH / 2) - 550, (HEIGHT / 2) - 25, BUTTON_WIDTH, BUTTON_HEIGHT);
	create_label(world, "assets/Graphics/menu/menu_label_left.png", (WIDTH / 2) - 550, (HEIGHT / 2) + 50, BUTTON_WIDTH, BUTTON_HEIGHT);
	create_label(world, "assets/Graphics/menu/menu_label_right.png", (WIDTH / 2) - 550, (HEIGHT / 2) + 125, BUTTON_WIDTH, BUTTON_HEIGHT);
	create_label(world, "assets/Graphics/menu/menu_label_action.png", (WIDTH / 2) - 550, (HEIGHT / 2) + 200, BUTTON_WIDTH, BUTTON_HEIGHT);
	
	//load information
	const int pos = strlen("assets/Graphics/menu/keymap/keymap_");
	int *commands;
	char filename[64];
	strcpy(filename, "assets/Graphics/menu/keymap/keymap_");
	
	KeyMapInitArray("assets/Input/keymap.txt", &commands);
	
	
	strcpy((char*)(filename + pos), SDL_GetScancodeName((SDL_Scancode)commands[C_UP]));
	strcpy((char*)(filename + pos + strlen(SDL_GetScancodeName((SDL_Scancode)commands[C_UP]))), ".png");
	create_button(world, filename, "keymap_up", (WIDTH / 2) + 250, (HEIGHT / 2) - 100 + (0 * 75));
	
	strcpy((char*)(filename + pos), SDL_GetScancodeName((SDL_Scancode)commands[C_DOWN]));
	strcpy((char*)(filename + pos + strlen(SDL_GetScancodeName((SDL_Scancode)commands[C_DOWN]))), ".png");
	create_button(world, filename, "keymap_down", (WIDTH / 2) + 250, (HEIGHT / 2) - 100 + (1 * 75));
	
	strcpy((char*)(filename + pos), SDL_GetScancodeName((SDL_Scancode)commands[C_LEFT]));
	strcpy((char*)(filename + pos + strlen(SDL_GetScancodeName((SDL_Scancode)commands[C_LEFT]))), ".png");
	create_button(world, filename, "keymap_left", (WIDTH / 2) + 250, (HEIGHT / 2) - 100 + (2 * 75));
	
	strcpy((char*)(filename + pos), SDL_GetScancodeName((SDL_Scancode)commands[C_RIGHT]));
	strcpy((char*)(filename + pos + strlen(SDL_GetScancodeName((SDL_Scancode)commands[C_RIGHT]))), ".png");
	create_button(world, filename, "keymap_right", (WIDTH / 2) + 250, (HEIGHT / 2) - 100 + (3 * 75));
	
	strcpy((char*)(filename + pos), SDL_GetScancodeName((SDL_Scancode)commands[C_ACTION]));
	strcpy((char*)(filename + pos + strlen(SDL_GetScancodeName((SDL_Scancode)commands[C_ACTION]))), ".png");
	create_button(world, filename, "keymap_action", (WIDTH / 2) + 250, (HEIGHT / 2) - 100 + (4 * 75));
	
	
	create_button(world, "assets/Graphics/menu/menu_button_back.png", "keymap_back", (WIDTH / 2) - (BUTTON_WIDTH / 2) + 150, (HEIGHT / 2) + 275);
	create_button(world, "assets/Graphics/menu/menu_button_default.png", "keymap_default", (WIDTH / 2) - (BUTTON_WIDTH / 2) - 150, (HEIGHT / 2) + 275);
}


void create_credits_menu(World *world) {
	
	init_world(world);
	
	unsigned int entity;
	
	entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	world->renderPlayer[entity].width = WIDTH;
	world->renderPlayer[entity].height = HEIGHT;
	world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/menu/credits.png");
	if (!world->renderPlayer[entity].playerSurface) {
		printf("mat is a doof\n");
	}
	
	world->position[entity].x = 0;
	world->position[entity].y = 0;
	world->position[entity].width = WIDTH;
	world->position[entity].height = HEIGHT;
	
	
	create_button(world, "assets/Graphics/menu/menu_button_back.png", "credits_back", (WIDTH / 2) - (BUTTON_WIDTH / 2), (HEIGHT / 2) + 275);
}


void create_setup_menu(World *world) {
	
	init_world(world);
	
	unsigned int entity;
	
	entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION);
	
	world->renderPlayer[entity].width = WIDTH;
	world->renderPlayer[entity].height = HEIGHT;
	world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/menu/MainMenu.png");
	if (!world->renderPlayer[entity].playerSurface) {
		printf("mat is a doof\n");
	}
	
	world->position[entity].x = 0;
	world->position[entity].y = 0;
	world->position[entity].width = WIDTH;
	world->position[entity].height = HEIGHT;
	
	create_label(world, "assets/Graphics/menu/menu_label_setup.png", (WIDTH / 2) - (TITLE_WIDTH / 2), (HEIGHT / 2) - 250, TITLE_WIDTH, TITLE_HEIGHT);
	
	create_label(world, "assets/Graphics/menu/menu_label_username.png", (WIDTH / 2) - 550, (HEIGHT / 2) - 25, BUTTON_WIDTH, BUTTON_HEIGHT);
	create_textfield(world, "setup_username", (WIDTH / 2) - 55, (HEIGHT / 2) - 30);
	
	create_label(world, "assets/Graphics/menu/menu_label_serverip.png", (WIDTH / 2) - 550, (HEIGHT / 2) + 50, BUTTON_WIDTH, BUTTON_HEIGHT);
	create_textfield(world, "setup_serverip", (WIDTH / 2) - 55, (HEIGHT / 2) + 45);
	
	
	create_button(world, "assets/Graphics/menu/menu_button_back.png", "setup_back", (WIDTH / 2) - (BUTTON_WIDTH / 2) + 150, (HEIGHT / 2) + 275);
	create_button(world, "assets/Graphics/menu/menu_button_play.png", "setup_play", (WIDTH / 2) - (BUTTON_WIDTH / 2) - 150, (HEIGHT / 2) + 275);
}
