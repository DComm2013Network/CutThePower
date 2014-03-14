#include "../components.h"
#include "../systems.h"
#include "../world.h"
#include "level.h"
#include "collision.h"
#include <stdio.h>
#include <math.h>

#define COLLISION_MASK (COMPONENT_COLLISION)
#define LEVEL_MASK (COMPONENT_LEVEL)


/*--------------------------------------------------------------------------------------------------------
--FUNCTION: collision_system
--
--DESIGNER: Joshua Campbell
--
--PROGRAMMERS: Joshua Campbell
--
--REVISIONS: none
--
--PARAMATERS:
--	 World &world - the world structure
--	 PositionComponent entity - the temporary position entity used for checking for collisions
--	 int entityID - the current entity being checked based on its position in the world struct
--
--RETURNS:
--	 Returns 0 for no collision
--	 Returns COLLISION_WALL for a collision with a wall
--	 Returns COLLISION_HACKER for a collision with a hacker
--	 Returns COLLISION_GUARD for a collision with a guard
--	 Returns COLLISION_STAIR for a collision with a stair
--
--NOTES:
--This is the main wrapper function for all other collision checking functions.
--------------------------------------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------------------------------------
--FUNCTION: wall_collision
--
--DESIGNER: Joshua Campbell
--
--PROGRAMMERS: Joshua Campbell
--
--REVISIONS: none
--
--PARAMATERS:
--	 World &world - the world structure
--	 PositionComponent entity - the temporary position entity used for checking for collisions
--
--RETURNS:
--	 Returns true if a collision occurred
--	 Returns false if there was no collision
--
--NOTES:
--Checks for collisions with walls on the map.
--------------------------------------------------------------------------------------------------------*/
int wall_collision(World *world, PositionComponent entity) {
	int i = 0;
	int curlevel = -1;
	int eposx = 0, eposy = 0;
	for (i = 0; i < MAX_ENTITIES; i++) {
		if (((world->mask[i] & LEVEL_MASK) != 0) && world->level[i].levelID == entity.level) {
			curlevel = i;
			break;
		}
	}

	if (curlevel == -1) {
		return false;
	}

	eposx = (entity.x) / world->level[curlevel].tileSize;
	eposy = (entity.y) / world->level[curlevel].tileSize;
	int tsize = world->level[curlevel].tileSize;
	int tiletoplayerfactor = entity.width / tsize;
	if (tiletoplayerfactor < 0) {
		tiletoplayerfactor = 1;
	}
	for (int x = eposx - 2 * tiletoplayerfactor; x <= eposx + 2 * tiletoplayerfactor && x < world->level[curlevel].width; x++) {
		if (x < 0) {
			x = 0;
		} 
		for (int y = eposy - 2 * tiletoplayerfactor; y <= eposy + 2 * tiletoplayerfactor && y < world->level[curlevel].height; y++) {
			if (y < 0) {
				y = 0;
			}
		
			int mapx = x;
			int mapy = y;
			if (world->level[curlevel].map[mapx][mapy] == L_WALL) {
				if (entity.x + entity.width - 1 > tsize * mapx + 1 &&
					entity.x + 1 < mapx * tsize + tsize - 1 &&
					entity.y + entity.height - 1 > mapy * tsize + 1 &&
					entity.y + 1 < mapy * tsize + tsize - 1) {
					return COLLISION_WALL;
				}
			}
		}
	}

	return COLLISION_EMPTY;
}


/*--------------------------------------------------------------------------------------------------------
--FUNCTION: stair_collision
--
--DESIGNER: Joshua Campbell
--
--PROGRAMMERS: Joshua Campbell
--
--REVISIONS: none
--
--PARAMATERS:
--	 World &world - the world structure
--	 PositionComponent entity - the temporary position entity used for checking for collisions
--
--RETURNS:
--	 Returns 0 for no collision
--	 Returns COLLISION_WALL for a collision with a wall
--	 Returns COLLISION_HACKER for a collision with a hacker
--	 Returns COLLISION_GUARD for a collision with a guard
--	 Returns COLLISION_STAIR for a collision with a stair
--
--NOTES:
--Checks for collisions with stairs on the map.
--------------------------------------------------------------------------------------------------------*/
bool stair_collision(World &world, PositionComponent entity) {
	return false;
}

/*--------------------------------------------------------------------------------------------------------
--FUNCTION: entity_collision
--
--DESIGNER: Joshua Campbell
--
--PROGRAMMERS: Joshua Campbell
--
--REVISIONS: none
--
--PARAMATERS:
--	 World &world - the world structure
--	 PositionComponent entity - the temporary position entity used for checking for collisions
--	 int entityID - the current entity being checked based on its position in the world struct
--
--RETURNS:
--	 Returns true if a collision occurred
--	 Returns false if there was no collision
--
--NOTES:
--This checks if there is a collision between two entities on the map.
--------------------------------------------------------------------------------------------------------*/
int entity_collision(World *world, PositionComponent entity, int entityID) {
	int i = 0;

	//entity.x += entity.width / 2;
	//entity.y += entity.height / 2;
	for (i = 0; i < MAX_ENTITIES; i++) {
		if (i != entityID && (world->mask[i] & COLLISION_MASK) != 0) {
			if (entity.x + entity.width -1 > world->position[i].x - world->collision[i].radius + 1 &&
				entity.x  + 1< world->position[i].x + world->position[i].width + world->collision[i].radius - 1&&
				entity.y + entity.height -1 > world->position[i].y - world->collision[i].radius + 1&&
				entity.y  + 1< world->position[i].y + world->position[i].height + world->collision[i].radius - 1
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
