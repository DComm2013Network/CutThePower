/** @ingroup Gameplay */
/** @{ */

/**
 * Contains functions for detecting and handling collisions.
 *
 * @todo Implement stair collision detection.
 * @file collision_system.cpp
 */
/** @} */
#include "components.h"
#include "systems.h"
#include "../world.h"
#include "level.h"
#include "collision.h"
#include <stdio.h>

#define COLLISION_MASK (COMPONENT_COLLISION) /**< Indicates that an entity can collide with other entities. */
#define LEVEL_MASK (COMPONENT_LEVEL)         /**< Indicates that an entity has levels (floors). */

/**
 * This is the main wrapper function for all other collision checking functions.
 *
 * @param[in,out] world    A reference to the world structure.
 * @param[in]     entity   The temporary position entity used for checking for collisions. 
 * @param[in]     entityID The current entity being checked based on its position in the world struct
 *
 * @return <ul>
 *             <li>Returns 0 for no collision</li>
 *             <li>Returns COLLISION_WALL for a collision with a wall</li>
 *             <li>Returns COLLISION_HACKER for a collision with a hacker</li>
 *             <li>Returns COLLISION_GUARD for a collision with a guard</li>
 *             <li>Returns COLLISION_STAIR for a collision with a stair</li>
 *         </ul>
 *
 * @designer Joshua Campbell
 * @author   Joshua campbell
 */
int collision_system(World &world, PositionComponent entity, int entityID) {
	if (wall_collision(world, entity)) {
		return COLLISION_WALL;
	}
	if (entity_collision(world, entity, entityID)) {
		return COLLISION_HACKER;
	}
	return 0;
}

/**
 * Checks for collisions with walls on the map.
 *
 * @param[in,out] world    A reference to the world structure.
 * @param[in]     entity   The temporary position entity used for checking for collisions. 
 *
 * @return True if a collision occurred, or false if there was no collision.
 *
 * @designer Joshua Campbell
 * @author   Joshua campbell
 */
bool wall_collision(World &world, PositionComponent entity) {
	int i = 0;
	int curlevel = -1;
	int eposx = 0, eposy = 0;
	for (i = 0; i < MAX_ENTITIES; i++) {
		if (((world.mask[i] & LEVEL_MASK) != 0) && world.level[i].levelID == entity.level) {
			curlevel = i;
			break;
		}
	}
	
	if (curlevel == -1) {
		//perror("No Level found\n");
		return false;
	}
	
	eposx = entity.x / world.level[curlevel].tileSize;
	eposy = entity.y / world.level[curlevel].tileSize;
	if (world.level[curlevel].map[eposx][eposy] == L_WALL) {
		return true;
	}
	return false;
}

/**
 * Checks for collisions with stairs on the map.
 *
 * @param[in,out] world    A reference to the world structure.
 * @param[in]     entity   The temporary position entity used for checking for collisions. 
 *
 * @return True if the entity collided with a set of stairs, false if they did not.
 *
 * @designer Joshua Campbell
 * @author   Joshua campbell
 */
bool stair_collision(World &world, PositionComponent entity) {
	return false;
}

/**
 * Checks if there's a collision between two entities on the map.
 *
 * @param[in,out] world    A reference to the world structure.
 * @param[in]     entity   The temporary position entity used for checking for collisions. 
 * @param[in]     entityID The current entity being checked based on its position in the world struct
 *
 * @return True if a collision occurred, or false if there was no collision.
 *
 * @designer Joshua Campbell
 * @author   Joshua campbell
 */
bool entity_collision(World &world, PositionComponent entity, int entityID) {
	int i = 0;
	
	for (i = 0; i < MAX_ENTITIES; i++) {
		if (i != entityID && (world.mask[i] & COLLISION_MASK) != 0) {
			if (entity.x + entity.width > world.position[i].x &&
				entity.x < world.position[i].x + world.position[i].width &&
				entity.y + entity.height > world.position[i].y &&
				entity.y < world.position[i].y + world.position[i].height) {
				return true;
			}
		}
	}
	

	return false;
}
