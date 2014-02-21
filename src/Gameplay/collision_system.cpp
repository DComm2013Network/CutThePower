#include "components.h"
#include "systems.h"
#include "world.h"
#include "level.h"
#include "collision.h"

#define COLLISION_MASK (COMPONENT_COLLISION)

int collision_system(World &world, PositionComponent entity) {
	if (wall_collision(world, entity)) {
		return COLLISION_WALL;
	}
	return 0;
}

bool wall_collision(World &world, PositionComponent entity) {
	int i = 0;
	int curlevel = -1;
	int eposx = 0, eposy = 0;
	for (i = 0; i < MAX_ENTITIES; i++) {
		if (world.level[i].levelID == entity.level) {
			curlevel = i;
		}
	}
	eposx = entity.x / world.level[curlevel].tileSize;
	eposy = entity.y / world.level[curlevel].tileSize;
	if (world.level[curlevel].map[eposx][eposy] == L_WALL) {
		return true;
	}
	return false;
}

bool stair_collision(World &world, PositionComponent entity) {
	return false;
}

bool movable_collision(World &world, PositionComponent entity) {
	
	int i = 0;	
	
	return false;
}
