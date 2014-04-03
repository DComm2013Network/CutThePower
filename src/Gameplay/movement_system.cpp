
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

void apply_deceleration_x(World* world, unsigned int entity, FPS fps) {
	world->movement[entity].movX *= 1 - world->movement[entity].friction * (FPS_MAX / fps.getFPS());
}

void apply_ice_deceleration_x(World* world, unsigned int entity, float deceleration, FPS fps) {
	world->movement[entity].movX *= 1 - deceleration * (FPS_MAX / fps.getFPS());
}

void apply_force_y(World* world, unsigned int entity, PositionComponent* temp, FPS fps) {
	temp->y += world->movement[entity].movY * (FPS_MAX / fps.getFPS());
}

void remove_force_y(World* world, unsigned int entity, PositionComponent* temp, FPS fps) {
	temp->y -= world->movement[entity].movY * (FPS_MAX / fps.getFPS());
}

void apply_deceleration_y(World* world, unsigned int entity, FPS fps) {
	world->movement[entity].movY *= 1 - world->movement[entity].friction * (FPS_MAX / fps.getFPS());
}

void apply_ice_deceleration_y(World* world, unsigned int entity, float deceleration, FPS fps) {
	world->movement[entity].movY *= 1 - deceleration * (FPS_MAX / fps.getFPS());
}
/* SPECIAL TILES */
void add_force_acceleration_x(MovementComponent& movement, float magnitude, float dir, float friction) {

	movement.movX += cos(dir * PI / 180) * magnitude;
	movement.movY += sin(dir * PI / 180) * magnitude;

	if (sqrt(pow(movement.movX, 2) + pow(movement.movY, 2)) > movement.maxSpeed) {
		float tx = movement.movX;
		float ty = movement.movY;
		movement.movX *= 1 - friction;
		movement.movY *= movement.maxSpeed / sqrt(pow(tx, 2) + pow(ty, 2));
	}
}
/* SPECIAL TILES */
void add_force_acceleration_y(MovementComponent& movement, float magnitude, float dir, float friction) {

	movement.movX += cos(dir * PI / 180) * magnitude;
	movement.movY += sin(dir * PI / 180) * magnitude;

	if (sqrt(pow(movement.movX, 2) + pow(movement.movY, 2)) > movement.maxSpeed) {
		float tx = movement.movX;
		float ty = movement.movY;
		movement.movX *= movement.maxSpeed / sqrt(pow(tx, 2) + pow(ty, 2));
		movement.movY *= 1 - friction;
	}
}

void handle_x_collision(World* world, unsigned int entity, PositionComponent* temp, unsigned int entity_number, unsigned int tile_number, FPS fps) {

	switch(entity_number) {
		case COLLISION_SOLID:
		case COLLISION_HACKER:
		case COLLISION_GUARD:
		case COLLISION_TARGET:
			remove_force_x(world, entity, temp, fps);
			world->movement[entity].movX = 0;
			break;
			/* SPECIAL TILES*/
		case COLLISION_BELTLEFT:
			//add_force(movement, 0.47, 180);
			add_force_acceleration_x(world->movement[entity], 0.47, 180, -0.000000000000000000001);
			break;
		case COLLISION_BELTRIGHT:
			add_force(world, entity, 0.47, 0);
			//add_force_acceleration_x(movement, 0.47, 0, -0.000000000000000000001);
			break;
		default:
			break;
		}
			/*SPECIAL TILES*/
	switch(tile_number) {
		case COLLISION_WALL:
			remove_force_x(world, entity, temp, fps);
			world->movement[entity].movX = 0;
			break;
			/* SPECIAL TILES*/
		case COLLISION_BELTRIGHT:
			add_force(world, entity, 0.47, 0);
			//add_force_acceleration_x(movement, 0.47, 0, -0.000000000000000000001);
			break;
		case COLLISION_BELTLEFT:
			//add_force(movement, 0.47, 180);
			add_force_acceleration_x(world->movement[entity], 0.47, 180, -0.000000000000000000001);
			break;
		case COLLISION_ICE:
			apply_ice_deceleration_x(world, entity, 0.005, fps);
			break;
			/* SPECIAL TILES*/
		default:
			if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_CONTROLLABLE)) {
				apply_deceleration_x(world, entity, fps);
			}
			break;
	}
}

void handle_y_collision(World* world, unsigned int entity, PositionComponent* temp, unsigned int entity_number, unsigned int tile_number, FPS fps) {
	switch(entity_number) {
		case COLLISION_SOLID:
		case COLLISION_HACKER:
		case COLLISION_GUARD:
		case COLLISION_TARGET:
			remove_force_y(world, entity, temp, fps);
			world->movement[entity].movY = 0;
			break;
	}
	
	switch(tile_number) {
		case COLLISION_WALL:
			remove_force_y(world, entity, temp, fps);
			world->movement[entity].movY = 0;
			break;
			/* SPECIAL TILES */
		case COLLISION_BELTDOWN:
			add_force(world, entity, 0.47, 90);
			break;
		case COLLISION_BELTUP:
			add_force(world, entity, 0.47, -90);
			break;
		case COLLISION_ICE:
			apply_ice_deceleration_y(world, entity, 0.005, fps);
			break;
			/* SPECIAL TILES */
		default:
			if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_CONTROLLABLE)) {
				apply_deceleration_y(world, entity, fps);
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
		case 4:
			map_init(world, "assets/Graphics/map/map_04/map04.txt", "assets/Graphics/map/map_04/tiles.txt");
			break;
		case 5:
			map_init(world, "assets/Graphics/map/map_05/map05.txt", "assets/Graphics/map/map_05/tiles.txt");
			break;
		case 6:
			map_init(world, "assets/Graphics/map/map_06/map06.txt", "assets/Graphics/map/map_06/tiles.txt");
			break;
		case 7:
			map_init(world, "assets/Graphics/map/map_07/map07.txt", "assets/Graphics/map/map_07/tiles.txt");
			break;
		case 8:
			map_init(world, "assets/Graphics/map/map_08/map08.txt", "assets/Graphics/map/map_08/tiles.txt");
			break;
		case 9:
			map_init(world, "assets/Graphics/map/map_09/map09.txt", "assets/Graphics/map/map_09/tiles.txt");
			break;
	}
	for (unsigned int i = 0; i < MAX_ENTITIES; i++) {
		if (IN_THIS_COMPONENT(world->mask[i], COMPONENT_LEVEL)) {
			world->level[i].levelID = targl;
			break;
		}
	}
}

void handle_entity_collision(World* world, unsigned int entity, unsigned int entity_number, unsigned int tile_number, unsigned int hit_entity) {
	switch(entity_number) {
		case COLLISION_STAIR:
			if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_CONTROLLABLE) && (world->collision[entity].type == COLLISION_HACKER || world->collision[entity].type == COLLISION_GUARD)) {
				int targx = world->wormhole[hit_entity].targetX;
				int targy = world->wormhole[hit_entity].targetY;
				int targl = world->wormhole[hit_entity].targetLevel;

				world->player[player_entity].tilez = rand() % 2 + 1;
				move_request(world, send_router_fd[WRITE], targl, targx, targy);
				if(!network_ready)
				{
					rebuild_floor(world, targl);
					return;
				}
				floor_change_flag = 1;
			}
			break;
	}
	if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_CONTROLLABLE) && world->controllable[entity].active) {
		tag_player(world, entity);
		capture_objective(world, entity);
	}
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
		if(IN_THIS_COMPONENT(world->mask[entity], COMPONENT_STILE))
		{
			switch(world->tile[entity].type)
			{
				case TILE_BELT_RIGHT:
					play_animation(world, entity, (char*)"speed_right");
				break;

				case TILE_BELT_LEFT:
					play_animation(world, entity, (char*)"speed_left");
				break;
			}
		}	

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
				/* SPECIAL TILES */
				unsigned int tile;
				if(command->commands[C_TILE]){
					key_pressed = true;
					if(world->player[entity].tilez == TILE_BELT_RIGHT){
						tile = create_stile(world, TILE_BELT_RIGHT, world->position[entity].x, world->position[entity].y, world->position[entity].level);
					}
					else if(world->player[entity].tilez == TILE_BELT_LEFT){
						tile = create_stile(world, TILE_BELT_LEFT, world->position[entity].x, world->position[entity].y, world->position[entity].level);
					}
					send_tiles(world, tile, send_router_fd[WRITE]);
				}
				/* SPECIAL TILES */
				
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

