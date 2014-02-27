#ifndef WORLD_H
#define WORLD_H

#include "components.h"

#define ENTITY_COUNT 1000

#define HEIGHT 768
#define WIDTH 1280

typedef struct
{
	
	int 					mask[ENTITY_COUNT];
	RenderPlayerComponent	renderPlayer[ENTITY_COUNT];

} World;

unsigned int create_entity(World& world);
void delete_entity(World& world, unsigned int entity);
void init_world(World& world);

#endif
