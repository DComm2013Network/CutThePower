#ifndef WORLD_H
#define WORLD_H

#include "components.h"

#define ENTITY_COUNT 1000

#define HEIGHT 600
#define WIDTH 800

typedef struct
{
	
	int 					mask[ENTITY_COUNT];
	
	MouseComponent 			mouse[ENTITY_COUNT];
	TextFieldComponent 		text[ENTITY_COUNT];
	ButtonComponent 		button[ENTITY_COUNT];
	CommandComponent 		command[ENTITY_COUNT];
	PositionComponent		position[ENTITY_COUNT];
	
} World;

unsigned int create_entity(World *world);
void delete_entity(World *world, unsigned int entity);
void init_world(World *world);

#endif
