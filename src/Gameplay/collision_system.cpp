/** @ingroup Gameplay */
/** @{ */

/**
 * Contains functions for detecting and handling collisions.
 *
 * @todo Implement stair collision detection.
 * @file collision_system.cpp
 */
/** @} */
#include "../components.h"
#include "../systems.h"
#include "../world.h"
#include "level.h"
#include "collision.h"
#include <stdio.h>
#include <math.h>

#define COLLISION_MASK (COMPONENT_COLLISION) /**< Indicates that an entity can collide with other entities. */
#define LEVEL_MASK (COMPONENT_LEVEL) /**< Indicates that an entity has levels (floors). */


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
CollisionData collision_system(World *world, PositionComponent entity, int entityID) {
	int entityIndex = -1;
	CollisionData data;
	data.map_code = COLLISION_EMPTY;
	data.entity_code = COLLISION_EMPTY;
	data.entityID = entityIndex;

	data.map_code = wall_collision(world, entity);

	if ((entityIndex = entity_collision(world, entity, entityID)) != -1) {
		data.entity_code = handle_entity_collision(world, entityIndex);
		data.entityID = entityIndex;
	}
	return data;
}

/**
 * Checks for collisions with walls on the map.
 *
 * @param[in,out] world    A reference to the world structure.
 * @param[in]     entity   The temporary position entity used for checking for collisions. 
 *
 * @return True if a collision occurred, or false if there was no collision.
 *
 * @designer Joshua Campbell & Clark Allenby
 * @author   Joshua campbell & Clark Allenby
 */
int wall_collision(World *world, PositionComponent entity) {
	int i = 0;
	int curlevel = -1;
	int xl, xr, yt, yb;
	int xdts, ydts;
	for (i = 0; i < MAX_ENTITIES; i++) {
		if (((world->mask[i] & LEVEL_MASK) != 0) && world->level[i].levelID == entity.level) {
			curlevel = i;
			break;
		}
	}

	if (curlevel == -1) {
		return false;
	}
	
	xl = (entity.x - entity.width / 2) / world->level[curlevel].tileSize;
	xr = (entity.x + entity.width / 2) / world->level[curlevel].tileSize;
	yt = (entity.y - entity.height / 2) / world->level[curlevel].tileSize;
	yb = (entity.y + entity.height / 2) / world->level[curlevel].tileSize;
	
	xdts = ceil((float)entity.width / (float)world->level[curlevel].tileSize);
	ydts = ceil((float)entity.height / (float)world->level[curlevel].tileSize);
	
	
	
	// debug statement: printf("xl: %i, xr: %i, yt: %i, yb: %i, xdts: %i, ydts: %i\n", xl, xr, yt, yb, xdts, ydts);
	if (yt < world->level[curlevel].height && yt > 0 &&
		yb < world->level[curlevel].height && yb > 0) {
		for (int i = 0; i < xdts; i++) {
			if (xl + i * world->level[curlevel].tileSize < world->level[curlevel].width &&
				xl + i * world->level[curlevel].tileSize > 0 &&
				world->level[curlevel].map[xl + i * world->level[curlevel].tileSize][yt] == L_WALL) {
				return COLLISION_WALL;
			}
			if (xr - i * world->level[curlevel].tileSize < world->level[curlevel].width &&
				xr - i * world->level[curlevel].tileSize > 0 &&
				world->level[curlevel].map[xr - i * world->level[curlevel].tileSize][yb] == L_WALL) {
				return COLLISION_WALL;
			}
		}
	}
	if (xl < world->level[curlevel].width && xl > 0 &&
		xr < world->level[curlevel].width && xr > 0) {
		for (int i = 0; i < ydts; i++) {
			if (yt + i * world->level[curlevel].tileSize < world->level[curlevel].height &&
				yt + i * world->level[curlevel].tileSize > 0 &&
				world->level[curlevel].map[xr][yt + i * world->level[curlevel].tileSize] == L_WALL) {
				return COLLISION_WALL;
			}
			if (yb - i * world->level[curlevel].tileSize < world->level[curlevel].height &&
				yb - i * world->level[curlevel].tileSize > 0 &&
				world->level[curlevel].map[xl][yb - i * world->level[curlevel].tileSize] == L_WALL) {
				return COLLISION_WALL;
			}
		}
	}

	return COLLISION_EMPTY;
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
int entity_collision(World *world, PositionComponent entity, int entityID) {
	int i = 0;

	//entity.x += entity.width / 2;
	//entity.y += entity.height / 2;
	for (i = 0; i < MAX_ENTITIES; i++) {
		if (i != entityID && IN_THIS_COMPONENT(world->mask[i], COLLISION_MASK)) {
			if (entity.x + entity.width / 2 - 1 > world->position[i].x - world->position[i].width / 2 + 1 &&
				entity.x - entity.width / 2 + 1 < world->position[i].x + world->position[i].width / 2 - 1 &&
				entity.y + entity.height / 2 - 1 > world->position[i].y - world->position[i].height / 2 + 1 &&
				entity.y - entity.height / 2 + 1 < world->position[i].y + world->position[i].height / 2 + 1
				&& world->collision[i].active) {
				return i;
			}
		}
	}

	return -1;
}

int tag_entity_collision(World *world, PositionComponent entity, int entityID) {
	int i = 0;
	int range = 5;
	//entity.x += entity.width / 2;
	//entity.y += entity.height / 2;
	for (i = 0; i < MAX_ENTITIES; i++) {
		if (i != entityID && IN_THIS_COMPONENT(world->mask[i], COLLISION_MASK)) {
			if (entity.x + entity.width / 2 - (1 + range) > world->position[i].x - world->position[i].width / 2 + (1 + range) &&
				entity.x - entity.width / 2 + (1 + range) < world->position[i].x + world->position[i].width / 2 - (1 + range) &&
				entity.y + entity.height / 2 - (1 + range) > world->position[i].y - world->position[i].height / 2 + (1 + range) &&
				entity.y - entity.height / 2 + (1 + range) < world->position[i].y + world->position[i].height / 2 + (1 + range)
				&& world->collision[i].active) {
				return i;
			}
		}
	}

	return -1;
}

int handle_entity_collision(World *world, int entityIndex) {
	if (world->collision[entityIndex].active) {
		return world->collision[entityIndex].type;
	} else {
		return COLLISION_UNKNOWN;
	}
}
