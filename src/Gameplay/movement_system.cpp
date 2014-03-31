
/** @ingroup Gameplay */
/** @{ */
/** @file movement_system.cpp */
/** @} */
#include "../components.h"
#include "../systems.h"
#include "../world.h"
#include "collision.h"
#include "stdio.h"
#include <math.h>

//This is the mask the system uses to see if it will work on the entity.
#define STANDARD_MASK (COMPONENT_POSITION | COMPONENT_MOVEMENT)									/**< Mask for all moveable entities. */
#define CONTROLLABLE_MASK (COMPONENT_POSITION | COMPONENT_MOVEMENT | COMPONENT_CONTROLLABLE)	/**< Mask for moveable entities that may be controlled. */
#define INPUT_MASK (COMPONENT_INPUT)															/**< Mask for entities that respond to input. */
#define COLLISION_MASK (COMPONENT_COLLISION)													/**< Mask for entities that may collide with other entities. */
#define PI 3.14159265		
#define DIRECTION_RIGHT	1
#define DIRECTION_LEFT	2
#define DIRECTION_UP	3
#define DIRECTION_DOWN	4																	/**< An approximation of pi for vector calculations. */

extern int floor_change_flag;
extern int send_router_fd[];
extern unsigned int player_entity;
extern int network_ready;
extern unsigned int *player_table;
/**
 * Adds a vector to the entity's movement vector.
 *
 * If the resultant movement vector's magnitude is greater than the entity's maximum
 * allowable speed, the 
 *
 * @param world     A reference to the world structure.
 * @param entity    The entity to whose movement vector the force is added.
 * @param magnitude The magnitude of the force vector.
 * @param dir       The direction of the force vector.
 *
 * @designer Clark Allenby
 * @author   Clark Allenby
 */
void add_force(World* world, unsigned int entity, float magnitude, float dir) {
	if ((world->mask[entity] & STANDARD_MASK) == STANDARD_MASK) {
		world->movement[entity].movX += cos(dir * PI / 180) * magnitude;
		world->movement[entity].movY += sin(dir * PI / 180) * magnitude;
		if (sqrt(pow(world->movement[entity].movX, 2) + pow(world->movement[entity].movY, 2)) > world->movement[entity].maxSpeed) {
			float tx = world->movement[entity].movX;
			float ty = world->movement[entity].movY;
			world->movement[entity].movX *= world->movement[entity].maxSpeed / sqrt(pow(tx, 2) + pow(ty, 2));
			world->movement[entity].movY *= world->movement[entity].maxSpeed / sqrt(pow(tx, 2) + pow(ty, 2));
		}
	}
}

void apply_force_x(World* world, unsigned int entity, PositionComponent* temp, FPS fps) {
	temp->x += world->movement[entity].movX * (FPS_MAX / fps.getFPS());
}

void remove_force_x(World* world, unsigned int entity, PositionComponent* temp, FPS fps) {
	temp->x -= world->movement[entity].movX * (FPS_MAX / fps.getFPS());
}

void apply_deceleration_x(World* world, unsigned int entity) {
	world->movement[entity].movX *= 1 - world->movement[entity].friction;
}

void apply_force_y(World* world, unsigned int entity, PositionComponent* temp, FPS fps) {
	temp->y += world->movement[entity].movY * (FPS_MAX / fps.getFPS());
}

void remove_force_y(World* world, unsigned int entity, PositionComponent* temp, FPS fps) {
	temp->y -= world->movement[entity].movY * (FPS_MAX / fps.getFPS());
}

void apply_deceleration_y(World* world, unsigned int entity) {
	world->movement[entity].movY *= 1 - world->movement[entity].friction;
}

void handle_x_collision(World* world, unsigned int entity, PositionComponent* temp, unsigned int entity_number, unsigned int tile_number, FPS fps) {
	switch(entity_number) {
		case COLLISION_SOLID:
			remove_force_x(world, entity, temp, fps);
			world->movement[entity].movX = 0;
			break;
		case COLLISION_HACKER:
			remove_force_x(world, entity, temp, fps);
			world->movement[entity].movX = 0;
			break;
		case COLLISION_GUARD:
			remove_force_x(world, entity, temp, fps);
			world->movement[entity].movX = 0;
			break;
	}
	
	switch(tile_number) {
		case COLLISION_WALL:
			remove_force_x(world, entity, temp, fps);
			world->movement[entity].movX = 0;
			break;
		default:
			if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_CONTROLLABLE)) {
				apply_deceleration_x(world, entity);
			}
			break;
	}
}

void handle_y_collision(World* world, unsigned int entity, PositionComponent* temp, unsigned int entity_number, unsigned int tile_number, FPS fps) {
	switch(entity_number) {
		case COLLISION_SOLID:
			remove_force_y(world, entity, temp, fps);
			world->movement[entity].movY = 0;
			break;
		case COLLISION_HACKER:
			remove_force_y(world, entity, temp, fps);
			world->movement[entity].movY = 0;
			break;
		case COLLISION_GUARD:
			remove_force_y(world, entity, temp, fps);
			world->movement[entity].movY = 0;
			break;
	}
	
	switch(tile_number) {
		case COLLISION_WALL:
			remove_force_y(world, entity, temp, fps);
			world->movement[entity].movY = 0;
			break;
		default:
			if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_CONTROLLABLE)) {
				apply_deceleration_y(world, entity);
			}
			break;
	}
}
/**
 * Recreates the environment with the map specified without deleting the characters.
 *			
 * @param[in, out]  world  	game world, searched for updates
 * @param[out] 		targl 	game map to load (floor level)
 *
 * @return  void
 *
 * @designer    Ramzi Chennafi
 * @author      Ramzi Chennafi
 */
void rebuild_floor(World* world, int targl)
{
	destroy_world_not_player(world);
    switch (targl) {
		case 0:
			map_init(world, "assets/Graphics/map/map_00/map00.txt", "assets/Graphics/map/map_00/tiles.txt");
			break;
		case 1:
			map_init(world, "assets/Graphics/map/map_01/map01.txt", "assets/Graphics/map/map_01/tiles.txt");
			break;
		case 2:
			map_init(world, "assets/Graphics/map/map_02/map02.txt", "assets/Graphics/map/map_02/tiles.txt");
			break;
		case 3:
			map_init(world, "assets/Graphics/map/map_03/map03.txt", "assets/Graphics/map/map_03/tiles.txt");
			break;
	}
	for (unsigned int i = 0; i < MAX_ENTITIES; i++) {
		if (IN_THIS_COMPONENT(world->mask[i], COMPONENT_LEVEL)) {
			world->level[i].levelID = targl;
			break;
		}
	}
}

int handle_entity_collision(World* world, unsigned int entity, unsigned int entity_number, unsigned int tile_number, unsigned int hit_entity) {
	switch(entity_number) {
		case COLLISION_TARGET:
			if (world->collision[entity].type == COLLISION_HACKER && world->command[entity].commands[C_ACTION]) {
				if (!world->objective[hit_entity].status) {
					printf("You captured an objective! You is the best!\n");
					world->objective[hit_entity].status = true;
				}
			}
			break;
		case COLLISION_STAIR:
			if (world->collision[entity].type == COLLISION_HACKER || world->collision[entity].type == COLLISION_GUARD) {
				int targx = world->wormhole[hit_entity].targetX;
				int targy = world->wormhole[hit_entity].targetY;
				int targl = world->wormhole[hit_entity].targetLevel;
			
				move_request(world, send_router_fd[WRITE], targl, targx, targy);
				if(!network_ready)
				{
					rebuild_floor(world, targl);
					return 1;
				}
				floor_change_flag = 1;
			}
			break;
	}
	if (world->collision[entity].type == COLLISION_GUARD && world->command[entity].commands[C_ACTION]) {
		unsigned int entity_number_local = 0;
		unsigned int hit_entity_local = 0;
		PositionComponent position;
		position.x = world->position[entity].x;
		position.y = world->position[entity].y;
		position.width = 60;
		position.height = 60;
		position.level = world->position[entity].level;
		entity_collision(world, entity, position, &entity_number_local, &hit_entity_local);
		if (world->collision[hit_entity_local].type == COLLISION_HACKER) {
			send_tag(world, send_router_fd[WRITE], world->player[hit_entity_local].playerNo);
		}
	}
	return entity_number;
}

/**
 * Determines the inputs applied to the entity and adds forces in
 * the specified directions.
 *
 * @param world A reference to the world struct.
 * @param fps current frame rate.
 *
 * @designer ?
 * @author   ?
 */
void movement_system(World* world, FPS fps, int sendpipe) {
	unsigned int entity;
	PositionComponent		*position;
	CommandComponent		*command;
	ControllableComponent 	*controllable;
	MovementComponent		*movement;

	//loop through each entity and see if the system can do work on it.
	for(entity = 0; entity < MAX_ENTITIES; entity++) {
		//For controllable entities
		if (IN_THIS_COMPONENT(world->mask[entity], CONTROLLABLE_MASK)) {
			command = &(world->command[entity]);
			position = &(world->position[entity]);
			controllable = &(world->controllable[entity]);
			movement = &(world->movement[entity]);
			
			//very simple movement. This needs to be synchronized with the
			//game loop so there is no jittering on very slow systems.
			if (controllable->active == true) {
				PositionComponent temp;
				temp.x = position->x;
				temp.y = position->y;
				temp.width = position->width;
				temp.height = position->height;
				temp.level = position->level;
				unsigned int entity_number = 0;
				unsigned int tile_number = 0;
				unsigned int hit_entity = 0;
				bool key_pressed = false;
				
				if (command->commands[C_UP]) {
					key_pressed = true;
					world->movement[entity].lastDirection = DIRECTION_UP;
					add_force(world, entity, world->movement[entity].acceleration, -90);
				}
				
				if (command->commands[C_DOWN]) {
					key_pressed = true;
					world->movement[entity].lastDirection = DIRECTION_DOWN;
					add_force(world, entity, world->movement[entity].acceleration, 90);
				}
				
				if (command->commands[C_LEFT]) {
					key_pressed = true;
					world->movement[entity].lastDirection = DIRECTION_LEFT;
					add_force(world, entity, world->movement[entity].acceleration, 180);
				}
				
				if (command->commands[C_RIGHT]) {
					key_pressed = true;
					world->movement[entity].lastDirection = DIRECTION_RIGHT;
					add_force(world, entity, world->movement[entity].acceleration, 0);
				}
				
				if (IN_THIS_COMPONENT(world->mask[entity], COLLISION_MASK)) {
					
					collision_system(world, entity, &temp, &entity_number, &tile_number, &hit_entity);
					if (hit_entity != -1 && (entity_number == COLLISION_HACKER || entity_number== COLLISION_GUARD)) {
						anti_stuck_system(world, entity, hit_entity);
					}

					apply_force_x(world, entity, &temp, fps);
					collision_system(world, entity, &temp, &entity_number, &tile_number, &hit_entity);
					handle_x_collision(world, entity, &temp, entity_number, tile_number, fps);
					
					apply_force_y(world, entity, &temp, fps);
					collision_system(world, entity, &temp, &entity_number, &tile_number, &hit_entity);
					handle_y_collision(world, entity, &temp, entity_number, tile_number, fps);
					
					handle_entity_collision(world, entity, entity_number, tile_number, hit_entity);
				 }
				
				position->x = temp.x;
				position->y = temp.y;
				
				if (movement->movX > 0 && abs(movement->movX) > abs(movement->movY)) {
					play_animation(world, entity, "right");
				}
				else if (movement->movX < 0 && abs(movement->movX) > abs(movement->movY)) {
					play_animation(world, entity, "left");
				}
				else if (movement->movY > 0 && abs(movement->movY) > abs(movement->movX)) {
					play_animation(world, entity, "down");
				}
				else if (movement->movY < 0 && abs(movement->movY) > abs(movement->movX)) {
					play_animation(world, entity, "up");
				}
				else if (movement->movX > 0.15 && abs(movement->movX) == abs(movement->movY)) {
					play_animation(world, entity, "right");
				}
				else if (movement->movX < -0.15 && abs(movement->movX) == abs(movement->movY)) {
					play_animation(world, entity, "left");
				}
				else if (movement->movX > 0.15 && abs(movement->movX) == abs(movement->movY)) {
					play_animation(world, entity, "right");
					//diagonal - up/right and down/right
				}
				else if (movement->movX < -0.15 && abs(movement->movX) == abs(movement->movY)) {
					play_animation(world, entity, "left");
					//diagonal - up/left and down/left
				}
				else {
					cancel_animation(world, entity);
				}
				
				if (position->level == 0) {
					if (position->x < 240) {
						send_status(world, sendpipe, 1, PLAYER_STATE_READY);
					}
					else if (position->x > 1000) {
						send_status(world, sendpipe, 2, PLAYER_STATE_READY);
					}
					else {
						send_status(world, sendpipe, 0, PLAYER_STATE_WAITING);
					}
				}
			}
		}
		else if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_POSITION | COMPONENT_MOVEMENT | COMPONENT_COLLISION)) {
			command = &(world->command[entity]);
			position = &(world->position[entity]);
			controllable = &(world->controllable[entity]);
			movement = &(world->movement[entity]);
			PositionComponent temp;
			
			temp.x = position->x;
			temp.y = position->y;
			temp.width = position->width;
			temp.height = position->height;
			temp.level = position->level;
			unsigned int entity_number = 0;
			unsigned int tile_number = 0;
			unsigned int hit_entity = 0;
			
			
			apply_force_x(world, entity, &temp, fps);
			collision_system(world, entity, &temp, &entity_number, &tile_number, &hit_entity);
			handle_x_collision(world, entity, &temp, entity_number, tile_number, fps);
			
			apply_force_y(world, entity, &temp, fps);
			collision_system(world, entity, &temp, &entity_number, &tile_number, &hit_entity);
			handle_y_collision(world, entity, &temp, entity_number, tile_number, fps);
			
			position->x = temp.x;
			position->y = temp.y;
			
			handle_entity_collision(world, entity, entity_number, tile_number, hit_entity);

			switch(world->movement[entity].lastDirection) {
				case DIRECTION_RIGHT:
					if (world->movement[entity].movX != 0) {
						play_animation(world, entity, "right");
					} else {
						cancel_animation(world, entity);
					}
					break;
				case DIRECTION_LEFT:
					if (world->movement[entity].movX != 0) {
						play_animation(world, entity, "left");
					} else {
						cancel_animation(world, entity);
					}
					break;
				case DIRECTION_UP:
					if (world->movement[entity].movY != 0) {
						play_animation(world, entity, "down");
					} else {
						cancel_animation(world, entity);
					}
					break;
				case DIRECTION_DOWN:
					if (world->movement[entity].movY != 0) {
						play_animation(world, entity, "up");
					} else {
						cancel_animation(world, entity);
					}
					break;
			}
		}
	}
}

