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

#define DIRECTION_RIGHT	1
#define DIRECTION_LEFT	2
#define DIRECTION_UP	3
#define DIRECTION_DOWN	4
#define TAG_DISTANCE	5

extern int send_router_fd[];

bool spacebar_collision(World* world, unsigned int entity, unsigned int** collision_list, unsigned int* num_collisions);
void cleanup_spacebar_collision(unsigned int** collision_list);

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
	
	if (yt < world->level[level].height && yt > 0 && yb < world->level[level].height && yb > 0) {
		for (int i = 0; i < xdts; i++) {
			if (xl + i * world->level[level].tileSize < world->level[level].width &&
				xl + i * world->level[level].tileSize >= 0) {
				if (world->level[level].map[xl + i * world->level[level].tileSize][yt] == L_WALL) {
					*tile_number = COLLISION_WALL;
					return;
				}
			}
			if (xr - i * world->level[level].tileSize < world->level[level].width &&
				xr - i * world->level[level].tileSize >= 0) {
				if (world->level[level].map[xr - i * world->level[level].tileSize][yb] == L_WALL) {
					*tile_number = COLLISION_WALL;
					return;
				}
			}
		}
	}
	if (xl < world->level[level].width && xl > 0 && xr < world->level[level].width && xr > 0) {
		for (int i = 0; i < ydts; i++) {
			if (yt + i * world->level[level].tileSize < world->level[level].height &&
				yt + i * world->level[level].tileSize >= 0) {
				if (world->level[level].map[xr][yt + i * world->level[level].tileSize] == L_WALL) {
					*tile_number = COLLISION_WALL;
					return;
				}
			}
			if (yb - i * world->level[level].tileSize < world->level[level].height &&
				yb - i * world->level[level].tileSize >= 0) {
				if (world->level[level].map[xl][yb - i * world->level[level].tileSize] == L_WALL) {
					*tile_number = COLLISION_WALL;
					return;
				}
			}
		}
	}
	
	
	// debug statement: printf("xl: %i, xr: %i, yt: %i, yb: %i, xdts: %i, ydts: %i\n", xl, xr, yt, yb, xdts, ydts);
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
	unsigned int i = 0;
	
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

/**
 * Checks if a player is stuck in another player and moves the other player
 * out of the current player.
 *
 * @param[in,out] world      		A pointer to the world structure.
 * @param[in]     curEntityID   	The entity being checked for collisions with. 
 * @param[in]     otherEntityID     The temporary position to be applied later.
 *
 * @designer Joshua Campbell
 * @author   Joshua campbell
 */
void anti_stuck_system(World *world, unsigned int curEntityID, int otherEntityID) {
	int leftDist = 0;
	int rightDist = 0;
	int upDist = 0;
	int downDist = 0;

	if (otherEntityID < 0) {
		return;
	}

	rightDist = abs(world->position[otherEntityID].x + world->position[otherEntityID].width - world->position[curEntityID].x); //right side of other player
	leftDist = abs(world->position[curEntityID].x + world->position[curEntityID].width - world->position[otherEntityID].x); //left side of other player
	upDist = abs(world->position[curEntityID].y + world->position[curEntityID].height - world->position[otherEntityID].y); //bottom of other player
	downDist = abs(world->position[otherEntityID].y + world->position[otherEntityID].height - world->position[curEntityID].y); //top of other player
	
	if (rightDist <= leftDist && rightDist <= upDist && rightDist <= downDist && !(world->command[curEntityID].commands[C_LEFT])) {
		world->position[otherEntityID].x = world->position[curEntityID].x - world->position[otherEntityID].width - 1;
	} else if (leftDist <= rightDist && leftDist <= upDist && leftDist <= downDist && !(world->command[curEntityID].commands[C_RIGHT])) {
		world->position[otherEntityID].x = world->position[curEntityID].x + world->position[curEntityID].width + 1;
	} else if (upDist <= leftDist && upDist <= rightDist && upDist <= downDist && !(world->command[curEntityID].commands[C_DOWN])) {
		world->position[otherEntityID].y = world->position[curEntityID].y + world->position[curEntityID].height + 1;
	} else if (downDist <= leftDist && downDist <= upDist && downDist <= rightDist && !(world->command[curEntityID].commands[C_UP])) {
		world->position[otherEntityID].y = world->position[curEntityID].y - world->position[otherEntityID].height - 1;
	}
}

/**
 * Checks if a hacker is in distance of a guard so that they can be tagged.
 *
 * @param[in,out] world      		A pointer to the world structure.
 * @param[in]     currentEntityID   The entity being checked for collisions with. 
 *
 * @designer Joshua Campbell
 * @author   Joshua campbell
 */
int check_tag_collision(World* world, unsigned int currentEntityID) {
	PositionComponent entity;
	int lastDirection = 0;

	lastDirection = world->movement[currentEntityID].lastDirection;

	entity.height = world->position[currentEntityID].height;
	entity.width = world->position[currentEntityID].width;
	entity.level = world->position[currentEntityID].level;
	entity.x = world->position[currentEntityID].x;
	entity.y = world->position[currentEntityID].y;

	for (unsigned int i = 0; i < MAX_ENTITIES; i++) {
		if (i != currentEntityID && (world->mask[i] & COLLISION_MASK) != 0) {
			switch(lastDirection) {
				case DIRECTION_RIGHT:
					if (entity.x + entity.width -1 + TAG_DISTANCE > world->position[i].x + 1 &&
					entity.x  + 1 < world->position[i].x + world->position[i].width - 1 &&
					entity.y + entity.height -1 > world->position[i].y + 1 &&
					entity.y  + 1 < world->position[i].y + world->position[i].height - 1
					&& world->collision[i].active && world->position[i].level == entity.level) {
						return i;
					}
				break;
				case DIRECTION_LEFT:
					if (entity.x + entity.width -1 > world->position[i].x + 1 &&
					entity.x  + 1 - TAG_DISTANCE < world->position[i].x + world->position[i].width - 1 &&
					entity.y + entity.height -1 > world->position[i].y + 1 &&
					entity.y  + 1< world->position[i].y + world->position[i].height - 1
					&& world->collision[i].active && world->position[i].level == entity.level) {
						return i;
					}
				break;
				case DIRECTION_UP:
					if (entity.x + entity.width -1 > world->position[i].x + 1 &&
					entity.x  + 1 < world->position[i].x + world->position[i].width - 1 &&
					entity.y + entity.height -1 + TAG_DISTANCE > world->position[i].y + 1 &&
					entity.y  + 1 < world->position[i].y + world->position[i].height - 1
					&& world->collision[i].active && world->position[i].level == entity.level) {
						return i;
					}
				break;
				case DIRECTION_DOWN:
					if (entity.x + entity.width -1 > world->position[i].x + 1 &&
					entity.x  + 1 < world->position[i].x + world->position[i].width - 1 &&
					entity.y + entity.height -1 > world->position[i].y + 1 &&
					entity.y  + 1 - TAG_DISTANCE < world->position[i].y + world->position[i].height - 1
					&& world->collision[i].active && world->position[i].level == entity.level) {
						return i;
					}
				break;
				default:
					if (entity.x + entity.width -1 > world->position[i].x + 1 &&
					entity.x  + 1 < world->position[i].x + world->position[i].width - 1 &&
					entity.y + entity.height -1 > world->position[i].y + 1 &&
					entity.y  + 1 < world->position[i].y + world->position[i].height - 1
					&& world->collision[i].active && world->position[i].level == entity.level) {
						return i;
					}
				break;
			}
		}
	}

	return -1;
}

/**
 * Checks if a the tag key was pressed and if the current player is a guard.
 * If a player was tagged, the tag data is sent to the server.
 *
 * @param[in,out] world      		A pointer to the world structure.
 * @param[in]     entity   			The player entity number. 
 *
 * @designer 
 * @author   
 */
bool tag_player(World* world, unsigned int entity) {
	if (world->collision[entity].type == COLLISION_GUARD && world->command[entity].commands[C_ACTION]) {
		unsigned int* collision_list = NULL;
		unsigned int num_collisions;
		if (spacebar_collision(world, entity, &collision_list, &num_collisions)) {
			unsigned int i;
			for (i = 0; i < num_collisions; i++) {
				if (world->collision[collision_list[i]].type == COLLISION_HACKER) {
					printf("tagged\n");
					send_tag(world, send_router_fd[WRITE], world->player[collision_list[i]].playerNo);
				}
			}
			cleanup_spacebar_collision(&collision_list);
		}
	}
	return false;
}

/**
 * Checks if a hacker is in the vicinity of an objective and if the action key is pressed.
 * If so, the capture objective data is sent to the server.
 *
 * @param[in,out] world      		A pointer to the world structure.
 * @param[in]     entity   			The player entity number.
 *
 * @designer  
 * @author    
 */
bool capture_objective(World* world, unsigned int entity) {
	if (world->collision[entity].type == COLLISION_HACKER && world->command[entity].commands[C_ACTION]) {
		unsigned int* collision_list = NULL;
		unsigned int num_collisions;
		bool captured = false;
		if (spacebar_collision(world, entity, &collision_list, &num_collisions)) {
			unsigned int i;
			for (i = 0; i < num_collisions; i++) {
				if (world->collision[collision_list[i]].type == COLLISION_TARGET && world->objective[collision_list[i]].status == 1) {
					printf("You captured an objective[%u] %u! You is the best!\n", collision_list[i], world->objective[collision_list[i]].objectiveID);
					world->objective[collision_list[i]].status = 2;
					captured = true;
					play_animation(world, collision_list[i], "captured");
				}
			}
			cleanup_spacebar_collision(&collision_list);
		}
		if (captured) {
			send_objectives(world, send_router_fd[WRITE]);
		}
	}
	return false;
}

/**
 * Checks if there is collidable object in the vicinity of the current player.
 * Returns a list of all collidable objects and the number of collisions.
 *
 * @param[in,out] world      		A pointer to the world structure.
 * @param[in,out] entity   			The player entity number.
 * @param[in,out] collision_list    The list of all nearby collisions.
 * @param[in,out] num_collisions    The number of nearby collisions.
 *
 * @designer 
 * @author   
 */
bool spacebar_collision(World* world, unsigned int entity, unsigned int** collision_list, unsigned int* num_collisions) {
	unsigned int i = 0;
	PositionComponent position;
	position.x = world->position[entity].x;
	position.y = world->position[entity].y;
	position.width = 60;
	position.height = 60;
	position.level = world->position[entity].level;
	
	*num_collisions = 0;
	
	for (i = 0; i < MAX_ENTITIES; i++) {
		if (i != entity && IN_THIS_COMPONENT(world->mask[i], COLLISION_MASK)) {
			if (position.x + position.width / 2 - 1 > world->position[i].x - world->position[i].width / 2 + 1 &&
				position.x - position.width / 2 + 1 < world->position[i].x + world->position[i].width / 2 - 1 &&
				position.y + position.height / 2 - 1 > world->position[i].y - world->position[i].height / 2 + 1 &&
				position.y - position.height / 2 + 1 < world->position[i].y + world->position[i].height / 2 + 1
				&& world->collision[i].active) {
				if ((*collision_list = (unsigned int*)realloc(*collision_list, sizeof(unsigned int) * ((*num_collisions) + 1))) == NULL) {
					return false;
				}
				(*collision_list)[*num_collisions] = i;
				(*num_collisions)++;
			}
		}
	}
	
	return ((*num_collisions) > 0);
}

/**
 * Frees up the memory taken up by the collision list.
 * 
 * @param[in,out] collision_list    The list of all nearby collisions.
 *
 * @designer 
 * @author   
 */
void cleanup_spacebar_collision(unsigned int** collision_list) {
	if (*collision_list != NULL) {
		free(*collision_list);
		*collision_list = NULL;
	}
}

/**
 * Creates a speed belt on the map.
 * 
 * @param[in,out] world    A pointer to the world structure.
 * @param[in] type    	   The tile type.
 * @param[in] xPos    	   The tile xpos.
 * @param[in] yPos         The tile ypos.
 * @param[in] level        The level that the tile is on.
 *
 * @designer 
 * @author   
 */
unsigned int create_stile(World * world, int type, int xPos, int yPos, int level)
{
	if(type != 1 && type != 2)
	{
		return -1;
	}

	unsigned int speed_tile = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_ANIMATION | COMPONENT_COLLISION | COMPONENT_STILE);
	
	int x = xPos / TILE_WIDTH;
	int y = yPos / TILE_HEIGHT;
	
	world->position[speed_tile].x = (x * TILE_WIDTH) + (TILE_WIDTH / 2);
	world->position[speed_tile].y = (y * TILE_HEIGHT) + (TILE_HEIGHT / 2);
	world->position[speed_tile].level = level;
	
	world->position[speed_tile].width = TILE_WIDTH;
	world->position[speed_tile].height = TILE_HEIGHT;
	
	world->renderPlayer[speed_tile].width = TILE_WIDTH;
	world->renderPlayer[speed_tile].height = TILE_HEIGHT;
	
	world->collision[speed_tile].id = 0;
	world->collision[speed_tile].timer = 0;
	world->collision[speed_tile].timerMax = 0;
	world->collision[speed_tile].active = true;
	world->collision[speed_tile].radius = 0;
	
	switch(type)
	{
		case TILE_BELT_RIGHT:
			world->tile[speed_tile].type = TILE_BELT_RIGHT;
			world->collision[speed_tile].type = COLLISION_BELTRIGHT;
			load_animation("assets/Graphics/objects/tiles/speed_right/speed_right_animation.txt", world, speed_tile);
			play_animation(world, speed_tile, (char*)"speed_right");
			break;

		case TILE_BELT_LEFT:
			world->tile[speed_tile].type = TILE_BELT_RIGHT;
			world->collision[speed_tile].type = COLLISION_BELTLEFT;
			load_animation("assets/Graphics/objects/tiles/speed_left/speed_left_animation.txt", world, speed_tile);
			play_animation(world, speed_tile, (char*)"speed_left");
			break;

	}

	world->tile[speed_tile].start_time = SDL_GetTicks();
	return speed_tile;
}
