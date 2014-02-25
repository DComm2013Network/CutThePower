#ifndef WORLD_H
#define WORLD_H

#include <SDL2/SDL.h>

#include "components.h"
#include "level.h"

//Maximum entities that will be used.
#define MAX_ENTITIES 10

#define IN_THIS_COMPONENT(x) ((world.mask[entity] & x) == x)

//This contains all of the entities' components and their respective component masks.
typedef struct {
	unsigned int mask[MAX_ENTITIES];

	PositionComponent		position[MAX_ENTITIES];
	RenderComponent			render[MAX_ENTITIES];
	InputComponent			input[MAX_ENTITIES];
	MovementComponent		movement[MAX_ENTITIES];
	CollisionComponent		collision[MAX_ENTITIES];
	ControllableComponent	controllable[MAX_ENTITIES];
	LevelComponent			level[MAX_ENTITIES];
} World;

void init_world(World& world);
unsigned int create_entity(World& world);
unsigned int create_player(World& world, int x, int y, InputComponent input, bool controllable);
unsigned int create_level(World& world, int map[MAX_WIDTH][MAX_HEIGHT], int width, int height, int tileSize);
void destory_entity(World& world, const unsigned int entity);

#endif
