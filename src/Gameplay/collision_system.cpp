#include "../components.h"
#include "../systems.h"
#include "../world.h"
#include "../Gameplay/level.h"
#include "../Gameplay/collision.h"
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
int collision_system(World &world, PositionComponent entity, int entityID) {
if (wall_collision(world, entity)) {
return COLLISION_WALL;
}
/*if (wall_collision2(world, entity, cd, input)) {
return COLLISION_WALL;
}*/
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
		return false;
	}

	eposx = (entity.x) / world.level[curlevel].tileSize;
	eposy = (entity.y) / world.level[curlevel].tileSize;
	int tsize = world.level[curlevel].tileSize;
	for (int x = eposx - 2; x <= eposx + 2 && x < world.level[curlevel].width; x++) {
		if (x < 0) {
			x = 0;
	} 
	for (int y = eposy - 2; y <= eposy + 2 && y < world.level[curlevel].height; y++) {
		if (y < 0) {
			y = 0;
		}
		
		int mapx = x;
		int mapy = y;
		if (world.level[curlevel].map[mapx][mapy] == L_WALL) {
			if (entity.x + entity.width >= tsize * mapx &&
				entity.x <= mapx * tsize + tsize &&
				entity.y + entity.height >= mapy * tsize &&
				entity.y <= mapy * tsize + tsize) {
				return true;
			}
		}
		}
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
