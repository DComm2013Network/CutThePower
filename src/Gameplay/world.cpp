#include "world.h"

#include <stdlib.h>

/*
 * 
 * This function initializes every mask to be 0, so that there are no components.
 * 
 */
void init_world(World& world) {
	int i;
	for(i = 0; i < MAX_ENTITIES; ++i) {
		world.mask[i] = COMPONENT_EMPTY;
	}
}

/*
 * 
 * This function loops through the masks of all the entities and finds the first one
 * that is not used.
 * 
 */
unsigned int create_entity(World& world, unsigned int attributes) {
	unsigned int entity;
	for(entity = 0; entity < MAX_ENTITIES; entity++) {
		if (world.mask[entity] == COMPONENT_EMPTY) {
			world.mask[entity] = attributes;
			return entity;
		}
	}
	return MAX_ENTITIES;
}

unsigned int create_level(World& world, int map[MAX_WIDTH][MAX_HEIGHT], int width, int height, int tileSize) {
	unsigned int entity = 0;
	int lastID = -1;
	unsigned int tempMask = 0;
	int i = 0;
	int n = 0;
	if (width <= MAX_WIDTH && height <= MAX_HEIGHT) {
		for (entity = 0; entity < MAX_ENTITIES; ++entity) {
			tempMask = world.mask[entity] & COMPONENT_LEVEL;
			if (tempMask == COMPONENT_LEVEL) {
				lastID = world.level[entity].levelID;
			}
		
			if (world.mask[entity] == COMPONENT_EMPTY) {
				lastID++;
				world.mask[entity] = COMPONENT_LEVEL;
				for (i = 0; i < width; i++) {
					for (n = 0; n < height; n++) {
						world.level[entity].map[i][n] = map[i][n];
					}
				}
				world.level[entity].levelID = lastID;
				world.level[entity].width = width;
				world.level[entity].height = height;
				world.level[entity].tileSize = tileSize;
				
				return entity;
			}
		}
	}
	
	return MAX_ENTITIES;
}

unsigned int create_player(World& world, int x, int y, InputComponent input, bool controllable) 
{
	unsigned int entity;
	PositionComponent pos;
	RenderComponent render;
	MovementComponent movement;
	ControllableComponent control;
	//InputComponent input;
	int lastID = -1;
	unsigned int tempMask = 0;
	
	//MovementComponent movement;
	//CollisionComponent collision;
	
	render.colour = 0x000000 + (rand() % 0x1000000);
	render.width = 20;
	render.height = 20;
	
	pos.x = x;
	pos.y = y;

	pos.width = 20;
	pos.height = 20;
	pos.s = 0;
	pos.level = 0;
	
	movement.id = 0;
	movement.lastDirection = 0;
	movement.acceleration = 0.001;
	movement.maxSpeed = 0.15;
	movement.movX = 0;
	movement.movY = 0;
	
	input.up = false;
	input.down = false;
	input.left = false;
	input.right = false;
	
	control.active = true;
	
	for(entity = 0; entity < MAX_ENTITIES; ++entity) {
		tempMask = world.mask[entity] & COMPONENT_POSITION;
		if (tempMask == COMPONENT_MOVEMENT) {
			lastID = world.movement[entity].id;
		}
		
		if (world.mask[entity] == COMPONENT_EMPTY) {
			lastID += 1;
			movement.id = lastID;
			if (controllable) {
				world.mask[entity] =  COMPONENT_POSITION | 
									  COMPONENT_RENDER | 
									  COMPONENT_INPUT | 
									  COMPONENT_MOVEMENT | 
									  COMPONENT_COLLISION |
									  COMPONENT_CONTROLLABLE; //| COMPONENT_MOVEMENT | COMPONENT_COLLISION;
			} else {
				world.mask[entity] =  COMPONENT_POSITION | COMPONENT_RENDER | COMPONENT_INPUT | COMPONENT_COLLISION | COMPONENT_MOVEMENT;
			}
			world.position[entity] = pos;
			world.render[entity] = render;
			world.input[entity] = input;
			world.movement[entity] = movement;
			if (controllable) {
				world.controllable[entity] = control;
			}
			//world.collision[entity] = collision;
			if (controllable) {
				world.controllable[entity] = control;
			}
			//world.collision[entity] = collision;
			return entity;
		}
	}
	return MAX_ENTITIES;
}

/*
 * 
 * Clean up is easy.
 * 
 */
void destory_entity(World& world, const unsigned int entity) {
	world.mask[entity] = COMPONENT_EMPTY;
}