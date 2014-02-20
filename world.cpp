#include "world.h"

/*
 * 
 * This function initializes every mask to be 0, so that there are no components.
 * 
 */
void init_world(World& world)
{
	
	int i;
	
	for(i = 0; i < MAX_ENTITIES; ++i)
	{
		world.mask[i] = COMPONENT_EMPTY;
	}
	
}

/*
 * 
 * This function loops through the masks of all the entities and finds the first one
 * that is not used.
 * 
 */
unsigned int create_entity(World& world) 
{
	
	unsigned int entity;
	
	for(entity = 0; entity < MAX_ENTITIES; ++entity) 
	{
		if (world.mask[entity] == COMPONENT_EMPTY) 
		{
			return entity;
		}
	}
	
	return MAX_ENTITIES;
}

unsigned int create_level(World& world, int **map, int width, int height, int tileSize) {
	unsigned int entity = 0;
	LevelComponent level;
	
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

unsigned int create_player(World& world, int x, int y, InputComponent input) 
{
	
	unsigned int entity;
	PositionComponent pos;
	RenderComponent render;
	MovementComponent movement;
	int lastID = -1;
	unsigned int tempMask = 0;
	//MovementComponent movement;
	//CollisionComponent collision;
	
	render.colour = 0xFF0000;
	render.width = 20;
	render.height = 20;
	
	pos.x = x;
	pos.y = y;
	
	movement.id = 0;
	movement.lastDirection = 0;
	movement.velocity = 2;
	movement.accelleration = 0;
	
	for(entity = 0; entity < MAX_ENTITIES; ++entity) 
	{
		tempMask = world.mask[entity] & COMPONENT_POSITION;
		if (tempMask == COMPONENT_MOVEMENT) {
			lastID = world.movement[entity].id;
		}
		
		if (world.mask[entity] == COMPONENT_EMPTY) 
		{
			lastID += 1;
			movement.id = lastID;
			world.mask[entity] =  COMPONENT_POSITION | COMPONENT_RENDER | COMPONENT_INPUT | COMPONENT_MOVEMENT; //| COMPONENT_MOVEMENT | COMPONENT_COLLISION;
			world.position[entity] = pos;
			world.render[entity] = render;
			world.input[entity] = input;
			world.movement[entity] = movement;
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
