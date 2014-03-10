#ifndef WORLD_H
#define WORLD_H

#include <SDL2/SDL.h>

#include "components.h"

#define WIDTH 1280
#define HEIGHT 768

//Maximum entities that will be used.
#define MAX_ENTITIES 256

#define IN_THIS_COMPONENT(mask, x) ((mask & x) == x)

//This contains all of the entities' components and their respective component masks.
typedef struct {
	unsigned int 			mask[MAX_ENTITIES];
	PositionComponent		position[MAX_ENTITIES];
	CommandComponent		command[MAX_ENTITIES];
	MovementComponent		movement[MAX_ENTITIES];
	CollisionComponent		collision[MAX_ENTITIES];
	ControllableComponent	controllable[MAX_ENTITIES];
	LevelComponent			level[MAX_ENTITIES];
	MouseComponent			mouse[MAX_ENTITIES];
	TextFieldComponent		text[MAX_ENTITIES];
	ButtonComponent			button[MAX_ENTITIES];
	RenderPlayerComponent	renderPlayer[MAX_ENTITIES];
	PlayerComponent			player[MAX_ENTITIES];
	TagComponent			tag[MAX_ENTITIES];
} World;

void init_world(World* world);
unsigned int create_entity(World* world, unsigned int attributes);
unsigned int create_player(World* world, int x, int y, bool controllable);
unsigned int create_level(World* world, uint8_t** map, int width, int height, int tileSize);
void destroy_entity(World* world, const unsigned int entity);

#endif
