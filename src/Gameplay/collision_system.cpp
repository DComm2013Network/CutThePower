#include "components.h"
#include "systems.h"
#include "../world.h"
#include "level.h"
#include "collision.h"
#include <stdio.h>

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
--				World &world - the world structure
--				PositionComponent entity - the temporary position entity used for checking for collisions
--				int entityID - the current entity being checked based on its position in the world struct
--
--RETURNS:
--				Returns 0 for no collision
--				Returns COLLISION_WALL for a collision with a wall
--				Returns COLLISION_HACKER for a collision with a hacker
--				Returns COLLISION_GUARD for a collision with a guard
--				Returns COLLISION_STAIR for a collision with a stair
--
--NOTES:
--This is the main wrapper function for all other collision checking functions.
--------------------------------------------------------------------------------------------------------*/
int collision_system(World &world, PositionComponent entity, int entityID) {
	if (wall_collision(world, entity)) {
		return COLLISION_WALL;
	}
	if (entity_collision(world, entity, entityID)) {
		return COLLISION_HACKER;
	}
	return 0;
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
--				World &world - the world structure
--				PositionComponent entity - the temporary position entity used for checking for collisions
--
--RETURNS:
--				Returns true if a collision occurred
--				Returns false if there was no collision
--
--NOTES:
--Checks for collisions with walls on the map.
--------------------------------------------------------------------------------------------------------*/
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
--				World &world - the world structure
--				PositionComponent entity - the temporary position entity used for checking for collisions
--
--RETURNS:
--				Returns 0 for no collision
--				Returns COLLISION_WALL for a collision with a wall
--				Returns COLLISION_HACKER for a collision with a hacker
--				Returns COLLISION_GUARD for a collision with a guard
--				Returns COLLISION_STAIR for a collision with a stair
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
--				World &world - the world structure
--				PositionComponent entity - the temporary position entity used for checking for collisions
--				int entityID - the current entity being checked based on its position in the world struct
--
--RETURNS:
--				Returns true if a collision occurred
--				Returns false if there was no collision
--
--NOTES:
--This checks if there is a collision between two entities on the map.
--------------------------------------------------------------------------------------------------------*/
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
