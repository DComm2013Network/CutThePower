/** @ingroup Gameplay */

/** @{ */
/** @file collision.h */
/** @} */
#ifndef COLLISION_H
#define COLLISION_H

#include "../world.h"
#include "components.h"

typedef struct {
	int map_code;
	int entity_code;
	int entityID;
} CollisionData;

CollisionData collision_system(World* world, PositionComponent entity, int entityID);
int wall_collision(World* world, PositionComponent entity);
unsigned int entity_collision(World* world, PositionComponent entity, int entityID);
int tag_entity_collision(World *world, PositionComponent entity, int entityID);
int handle_target_collision(World *world, int entityIndex);
int handle_entity_collision(World *world, int entityIndex);
int handle_entity_collision(CollisionData data, World * world, int curEntityID);
void rebuild_floor(World * world, int targl);

#endif
