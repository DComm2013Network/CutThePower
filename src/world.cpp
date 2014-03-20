/**
 * [File description]
 *
 * @file world.cpp
 */

#define ANIMATION_AMOUNT 6

#include "world.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

#include <stdlib.h>
#include <stdio.h>

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
unsigned int create_level(World* world, int** map, int width, int height, int tileSize) {
	
	unsigned int entity = 0;
	int lastID = -1;
	int i = 0;
	int n = 0;
	
	entity = create_entity(world, COMPONENT_LEVEL);
	
	lastID++;
	world->level[entity].map = (int**)malloc(sizeof(int*) * width);
	for (i = 0; i < width; i++) {
		world->level[entity].map[i] = (int*)malloc(sizeof(int) * height);
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
unsigned int create_player(World* world, int x, int y, bool controllable, int collisiontype) {
	unsigned int entity;
	PositionComponent pos;
	RenderPlayerComponent render;
	MovementComponent movement;
	ControllableComponent control;
	CommandComponent command;
	CollisionComponent collision;

	int lastID = -1;
	unsigned int tempMask = 0;
	
	//MovementComponent movement;
	//CollisionComponent collision;
	
	render.width = 40;
	render.height = 40;
	//render.playerSurface = IMG_Load("assets/Graphics/player_80px.png");
	//render.playerSurface = IMG_Load("assets/Graphics/hacker_down.png");
	//if (!render.playerSurface) {
	//	printf("mat is a doof\n");
	//}
	
	pos.x = x;
	pos.y = y;

	pos.width = render.width;
	pos.height = render.height;
	pos.level = 0;
	
	movement.id = 0;
	movement.lastDirection = 0;
	movement.acceleration = 0.5;
	movement.maxSpeed = 2.0;
	movement.movX = 0;
	movement.movY = 0;
	movement.friction = 0.1;
	
	command.commands[C_UP] = false;
	command.commands[C_DOWN] = false;
	command.commands[C_LEFT] = false;
	command.commands[C_RIGHT] = false;
	
	control.active = true;

	collision.id = 0;
	collision.type = collisiontype;
	collision.timer = 0;
	collision.timerMax = 0;
	collision.active = true;
	collision.radius = 0;
	
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
			world->collision[entity] = collision;
			
			if (controllable) {
				world->controllable[entity] = control;
			}

			return entity;
		}
	}
	return MAX_ENTITIES;
}

//creates a target for the hackers
unsigned int create_target(World* world, int x, int y, int collisiontype) {
	unsigned int entity;
	PositionComponent pos;
	RenderPlayerComponent render;
	CollisionComponent collision;

	int lastID = -1;
	unsigned int tempMask = 0;
	
	render.width = 20;
	render.height = 20;
	render.playerSurface = SDL_LoadBMP("assets/Graphics/target.bmp");
	if (!render.playerSurface) {
		printf("mat is a doof\n");
	}
	
	pos.x = x;
	pos.y = y;

	pos.width = render.width;
	pos.height = render.height;
	pos.level = 0;
	
	collision.id = 0;
	collision.type = collisiontype;
	collision.timer = 0;
	collision.timerMax = 1000;
	collision.active = true;
	collision.radius = 50;
	
	for(entity = 0; entity < MAX_ENTITIES; ++entity) {
		tempMask = world->mask[entity] & COMPONENT_POSITION;
		if (tempMask == COMPONENT_MOVEMENT) {
			lastID = world->collision[entity].id;
		}
		
		if (world->mask[entity] == COMPONENT_EMPTY) {
			lastID += 1;
			collision.id = lastID;

			world->mask[entity] =	COMPONENT_POSITION | 
									COMPONENT_RENDER_PLAYER | 
									COMPONENT_COLLISION;

			world->position[entity] = pos;
			world->renderPlayer[entity] = render;
			world->collision[entity] = collision;
			
			return entity;
		}
	}
	return MAX_ENTITIES;
}

/**
 * Creates a stair entity 
 *
 * Creates and sets the parameters necessary for the creation of a stair.
 *
 * @param world
 * @param targetLevel - level the stair will go too.
 * @param targetX - x location on the target level the player will be placed.
 * @param targetY - y location on the target level the player will be placed.
 * @param x - x position of the stair.
 * @param y - y position of the stair.
 * @param width - width of the stair.
 * @param height - height of the stair.
 * @param level - level the stair is on.
 *
 * @return the entity id of the newly created stair
 *
 * @designer JOSH & IAN
 * @author JOSH & IAN
 */
unsigned int create_stair(World* world, int targetLevel, int targetX, int targetY, int x, int y, int width, int height, int level) {
	unsigned int entity;
	PositionComponent pos;
	WormholeComponent target;
	RenderPlayerComponent render;
	CollisionComponent collision;

	int lastID = -1;
	unsigned int tempMask = 0;
	
	render.width = 40;
	render.height = 40;
	render.playerSurface = SDL_LoadBMP("assets/Graphics/lobby/stair.bmp");
	if (!render.playerSurface) {
		printf("mat is a doof\n");
	}
	
	pos.x = x;
	pos.y = y;

	pos.width = render.width;
	pos.height = render.height;
	pos.level = 0;
	
	target.targetLevel = targetLevel;
    target.targetX = targetX;
    target.targetY = targetY;
    
	collision.id = 0;
	collision.type = COLLISION_SOLID;
	collision.timer = 0;
	collision.timerMax = 600;
	collision.active = true;
	collision.radius = 0;
	
	for(entity = 0; entity < MAX_ENTITIES; ++entity) {
		tempMask = world->mask[entity] & COMPONENT_POSITION;
		if (tempMask == COMPONENT_MOVEMENT) {
			lastID = world->collision[entity].id;
		}
		
		if (world->mask[entity] == COMPONENT_EMPTY) {
			lastID += 1;
			collision.id = lastID;

			world->mask[entity] =	COMPONENT_POSITION | 
									COMPONENT_RENDER_PLAYER | 
									COMPONENT_COLLISION |

									COMPONENT_WORMHOLE;

			world->position[entity] = pos;
			world->renderPlayer[entity] = render;
			world->wormhole[entity] = target;
			world->collision[entity] = collision;
			
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

	int i, j;
	
	world->mask[entity] = COMPONENT_EMPTY;
	
	if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_ANIMATION)) {
		
		for(i = 0; i < world->animation[entity].animation_count; i++) {
			
			free(world->animation[entity].animations[i].name);
			
			for(j = 0; j < world->animation[entity].animations[i].surface_count; j++) {
				
				SDL_FreeSurface(world->animation[entity].animations[i].surfaces[j]);
				
				//printf("Free frame!\n");
				
			}
		}
		free(world->animation[entity].animations);
	}
	else if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_RENDER_PLAYER)) {
		
		//printf("POINTER: %p\n", world->renderPlayer[entity].playerSurface);
		
		//if (world->renderPlayer[entity].playerSurface != NULL)
			//SDL_FreeSurface(world->renderPlayer[entity].playerSurface);
		
	}
}

/**
 * Deletes all entities in the world.
 *
 * @param world The world struct
 *
 * @designer Jordan Marling
 *
 * @author Jordan Marling
 */
void destroy_world(World *world) {
	unsigned int entity;
	
	for(entity = 0; entity < MAX_ENTITIES; entity++) {
		destroy_entity(world, entity);
	}
}



