#include "world.h"


unsigned int create_entity(World *world)
{
	unsigned int entity;
	
	for(entity = 0; entity < ENTITY_COUNT; entity++)
	{
		if (world->mask[entity] == COMPONENT_NONE)
			return entity;
	}
	
	return ENTITY_COUNT;
}

void delete_entity(World *world, unsigned int entity)
{
	world->mask[entity] = COMPONENT_NONE;
}

void init_world(World *world)
{
	unsigned int entity;
	for(entity = 0; entity < ENTITY_COUNT; entity++)
	{
		world->mask[entity] = COMPONENT_NONE;
	}
}


















