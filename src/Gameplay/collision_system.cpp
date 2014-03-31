/** @ingroup Gameplay */
/** @{ */

/**
 * Contains functions for detecting collisions.
 *
 * @file collision_system.cpp
 */
/** @} */
#include "../components.h"
#include "../systems.h"
#include "../world.h"
#include "collision.h"
#include "level.h"
#include <stdio.h>
#include <math.h>

#define COLLISION_MASK (COMPONENT_COLLISION) /**< Indicates that an entity can collide with other entities. */
#define LEVEL_MASK (COMPONENT_LEVEL) /**< Indicates that an entity has levels (floors). */

/**
 * This is the main wrapper function for all other collision checking functions.
 *
 * @param[in,out] world      A pointer to the world structure.
 * @param[in]     entity     The temporary position entity used for checking for collisions. 
 * @param[in,out] temp       The temporary position that will be applied later.
 * @param[out] entity_number The collision type of the entity that was hit (if any).
 * @param[out] tile_number   The collision type of the tile that was hit (if any).
 * @param[out] hit_entity    The identifier of the entity that was hit (if any).
 *
 * @designer Clark Allenby & Joshua Campbell
 * @author   Clark Allenby
 */
void collision_system(World* world, unsigned int entity, PositionComponent* temp, unsigned int* entity_number, unsigned int* tile_number, unsigned int* hit_entity) {
	wall_collision(world, *temp, tile_number);
	entity_collision(world, entity, *temp, entity_number, hit_entity);
}

/**
 * Checks for collisions with walls on the map.
 *
 * @param[in,out] world       A pointer to the world structure.
 * @param[in]     temp        The temporary position used for checking for collisions.
 * @param[out]    tile_number The collision type of the tile that was hit (if any). 
 *
 * @designer Clark Allenby & Joshua Campbell
 * @author   Clark Allenby
 */
void wall_collision(World* world, PositionComponent temp, unsigned int* tile_number) {
	int i = 0;
	int level = -1;
	int xl, xr, yt, yb;
	int xdts, ydts;
	
	for (i = 0; i < MAX_ENTITIES; i++) {
		if (((world->mask[i] & LEVEL_MASK) != 0) && world->level[i].levelID == temp.level) {
			level = i;
			break;
		}
	}

	if (level == -1) {
		*tile_number = COLLISION_UNKNOWN;
		return;
	}
	
	xl = (temp.x - temp.width / 2) / world->level[level].tileSize;
	xr = (temp.x + temp.width / 2) / world->level[level].tileSize;
	yt = (temp.y - temp.height / 2) / world->level[level].tileSize;
	yb = (temp.y + temp.height / 2) / world->level[level].tileSize;
	
	xdts = ceil((float)temp.width / (float)world->level[level].tileSize);
	ydts = ceil((float)temp.height / (float)world->level[level].tileSize);
	
	for (int i = 0; i < xdts; i++) {
		if (world->level[level].map[xl + i * world->level[level].tileSize][yt] == L_WALL) {
			*tile_number = COLLISION_WALL;
			return;
		}
		if (world->level[level].map[xr - i * world->level[level].tileSize][yb] == L_WALL) {
			*tile_number = COLLISION_WALL;
			return;
		}
	}
	for (int i = 0; i < ydts; i++) {
		if (world->level[level].map[xr][yt + i * world->level[level].tileSize] == L_WALL) {
			*tile_number = COLLISION_WALL;
			return;
		}
		if (world->level[level].map[xl][yb - i * world->level[level].tileSize] == L_WALL) {
			*tile_number = COLLISION_WALL;
			return;
		}
	}

	*tile_number = COLLISION_EMPTY;
}

/**
 * Checks if there's a collision between two entities on the map.
 *
 * @param[in,out] world      A pointer to the world structure.
 * @param[in]     entity     The entity being checked for collisions with. 
 * @param[in]     temp       The temporary position to be applied later.
 * @param[out] entity_number The collision type of the entity that was hit (if any).
 * @param[out] hit_entity    The identifier of the entity that was hit (if any).
 *
 * @designer Joshua Campbell & Clark Allenby
 * @author   Joshua campbell & Clark Allenby
 */
void entity_collision(World* world, unsigned int entity, PositionComponent temp, unsigned int* entity_number, unsigned int* hit_entity) {
	int i = 0;
	
	for (i = 0; i < MAX_ENTITIES; i++) {
		if (i != entity && IN_THIS_COMPONENT(world->mask[i], COLLISION_MASK)) {
			if (temp.x + temp.width / 2 - 1 > world->position[i].x - world->position[i].width / 2 + 1 &&
				temp.x - temp.width / 2 + 1 < world->position[i].x + world->position[i].width / 2 - 1 &&
				temp.y + temp.height / 2 - 1 > world->position[i].y - world->position[i].height / 2 + 1 &&
				temp.y - temp.height / 2 + 1 < world->position[i].y + world->position[i].height / 2 + 1
				&& world->collision[i].active) {
				*entity_number = world->collision[i].type;
				*hit_entity = i;
				return;
			}
		}
	}
	
	*entity_number = COLLISION_EMPTY;
	*hit_entity = MAX_ENTITIES;
}

