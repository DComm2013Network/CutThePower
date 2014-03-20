/** @ingroup Gameplay */

/** @{ */
/** @file collision.h */
/** @} */

#ifndef COLLISION_H
#define COLLISION_H

#include "../world.h"
#include "components.h"

#define COLLISION_WALL		10001    /**< A collision between an entity and a wall. */
#define COLLISION_STAIR		10002    /**< A collision between an entity and a guard. */
#define COLLISION_HACKER	10003    /**< A collision between an entity and a hacker. */
#define COLLISION_GUARD		10004    /**< A collision between an entity and a guard. */

int collision_system(World &world, PositionComponent entity, int entityID);
bool wall_collision(World &world, PositionComponent entity);
bool entity_collision(World &world, PositionComponent entity, int entityID);

#endif
