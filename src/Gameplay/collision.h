#ifndef COLLISION_H
#define COLLISION_H

#include "../world.h"
#include "components.h"

#define COLLISION_WALL		10001
#define COLLISION_STAIR		10002
#define COLLISION_HACKER	10003
#define COLLISION_GUARD		10004

int collision_system(World* world, PositionComponent entity, int entityID);
bool wall_collision(World* world, PositionComponent entity);
bool entity_collision(World* world, PositionComponent entity, int entityID);

#endif
