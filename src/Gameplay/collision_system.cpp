#include "components.h"
#include "systems.h"
#include "world.h"
#include "level.h"
#include "collision.h"

#define COLLISION_MASK (COMPONENT_COLLISION)
#define WITHIN_MAP(x1, y1, x2, y2) ((x1 < world.level[curlevel].width) && (x1 >= 0) && (y1 < world.level[curlevel].height) && (y1 >= 0) && (x2 < world.level[curlevel].width) && (x2 >= 0) && (y2 < world.level[curlevel].height) && (y2 >= 0))

int collision_system(World &world, PositionComponent entity) {
	if (wall_collision(world, entity)) {
		return COLLISION_WALL;
	}
	return 0;
}

bool wall_collision(World &world, PositionComponent entity) {
	int i = 0;
	int curlevel = -1;
	int lx = 0, rx = 0, ty = 0, by = 0;
	for (i = 0; i < MAX_ENTITIES; i++) {
		if (world.mask[i] & COMPONENT_LEVEL) {
			if (world.level[i].levelID == entity.level) {
				curlevel = i;
				break;
			}
		}
	}
	lx = (int)floor((entity.x - entity.width / 2) / world.level[curlevel].tileSize);
	rx = (int)floor((entity.x + entity.width / 2) / world.level[curlevel].tileSize);
	ty = (int)floor((entity.y - entity.height / 2) / world.level[curlevel].tileSize);
	by = (int)floor((entity.y + entity.height / 2) / world.level[curlevel].tileSize);
	
	//printf("ts: %i, x: %f, y: %f, lx: %i, rx: %i, ty: %i, by: %i\n", world.level[curlevel].tileSize, entity.x, entity.y, lx, rx, ty, by);
	if (!WITHIN_MAP(lx, ty, rx, by)) {
		return true;
	}
	//printf("%i, %i\n",world.level[curlevel].map[0][0], curlevel);
	if (world.level[curlevel].map[lx][ty] == L_WALL || world.level[curlevel].map[lx][by] == L_WALL
		|| world.level[curlevel].map[rx][ty] == L_WALL || world.level[curlevel].map[rx][by] == L_WALL) {
		return true;
	}
	return false;
}

bool stair_collision(World &world, PositionComponent entity) {
	return false;
}

bool movable_collision(World &world, PositionComponent entity) {
	return false;
}
