/**
 * [File description]
 *
 * @file world.cpp
 */

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
