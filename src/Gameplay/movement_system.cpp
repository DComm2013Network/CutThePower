
/** @ingroup Gameplay */
/** @{ */
/** @file movement_system.cpp */
/** @} */
#include "../components.h"
#include "../systems.h"
#include "../world.h"
#include "../Gameplay/collision.h"
#include "stdio.h"
#include <math.h>

//This is the mask the system uses to see if it will work on the entity.
#define STANDARD_MASK (COMPONENT_POSITION | COMPONENT_MOVEMENT)									/**< Mask for all moveable entities. */
#define CONTROLLABLE_MASK (COMPONENT_POSITION | COMPONENT_MOVEMENT | COMPONENT_CONTROLLABLE)	/**< Mask for moveable entities that may be controlled. */
#define INPUT_MASK (COMPONENT_INPUT)															/**< Mask for entities that respond to input. */
#define COLLISION_MASK (COMPONENT_COLLISION)													/**< Mask for entities that may collide with other entities. */
#define PI 3.14159265																			/**< An approximation of pi for vector calculations. */

extern int floor_change_flag;
extern int send_router_fd[];
extern unsigned int player_entity;
extern int network_ready;
extern unsigned int *player_table;

bool handle_collision_target(World *world, int entityIndex) {
	if (!world->objective[entityIndex].status) {
		world->objective[entityIndex].status = true;
		return true;
	}
	return false;
}

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
 * @designer ?
 * @author   ?
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

void add_force(MovementComponent& movement, float magnitude, float dir) {

	movement.movX += cos(dir * PI / 180) * magnitude;
	movement.movY += sin(dir * PI / 180) * magnitude;

	if (sqrt(pow(movement.movX, 2) + pow(movement.movY, 2)) > movement.maxSpeed) {
		float tx = movement.movX;
		float ty = movement.movY;
		movement.movX *= movement.maxSpeed / sqrt(pow(tx, 2) + pow(ty, 2));
		movement.movY *= movement.maxSpeed / sqrt(pow(tx, 2) + pow(ty, 2));
	}
}

/**
 * Moves the entity one unit along its movement vector.
 *
 * @param world  A reference to the world struct.
 * @param entity The entity to move.
 *
 * @designer ?
 * @author   ?
 */
void apply_force(World& world, unsigned int entity, FPS fps) {
	world.position[entity].x += world.movement[entity].movX * (FPS_MAX / fps.getFPS());
	world.position[entity].y += world.movement[entity].movY * (FPS_MAX / fps.getFPS());
}

void apply_forcex(PositionComponent& position, MovementComponent movement, FPS fps) {
	position.x += movement.movX * (FPS_MAX / fps.getFPS());
}

void remove_forcex(PositionComponent& position, MovementComponent movement, FPS fps) {
	position.x -= movement.movX * (FPS_MAX / fps.getFPS());
}

void apply_forcey(PositionComponent& position, MovementComponent movement, FPS fps) {
	position.y += movement.movY * (FPS_MAX / fps.getFPS());
}

void remove_forcey(PositionComponent& position, MovementComponent movement, FPS fps) {
	position.y -= movement.movY * (FPS_MAX / fps.getFPS());
}


void apply_deceleration(World* world, unsigned int entity) {
	world->movement[entity].movX *= 1 - world->movement[entity].friction;
	world->movement[entity].movY *= 1 - world->movement[entity].friction;
}

void apply_deceleration_x(MovementComponent &movement) {
	movement.movX *= 1 - movement.friction;
}

void apply_deceleration_y(MovementComponent &movement) {
	movement.movY *= 1 - movement.friction;
}

/* ------ MANUAL FRICTION X ------ */
void apply_deceleration_x(MovementComponent &movement, float friction) {
	movement.movX *= 1 - friction;
}

/* ------ MANUAL FRICTION Y ------ */
void apply_deceleration_y(MovementComponent &movement, float friction) {
	movement.movY *= 1 - friction;
}

void handle_x_collision(World* world, CollisionData data, PositionComponent& position, MovementComponent &movement, unsigned int entity, FPS fps) {
	switch(data.entity_code) {
	case COLLISION_SOLID:
		remove_forcex(position, movement, fps);
		movement.movX = 0;
		break;
	case COLLISION_HACKER:
		remove_forcex(position, movement, fps);
		movement.movX = 0;
		break;
	case COLLISION_GUARD:
		remove_forcex(position, movement, fps);
		movement.movX = 0;
		break;
	default:
		break;
	}

	switch(data.map_code) {
	case COLLISION_WALL:
		remove_forcex(position, movement, fps);
		movement.movX = 0;
		break;
	case COLLISION_BELTRIGHT:
	    add_force(movement, 0.4, 0);
	    break;
	case COLLISION_BELTLEFT:
	    add_force(movement, 0.4, 180);
	    break;
	case COLLISION_ICE:
	    apply_deceleration_x(movement, movement.friction);
	    break;
	default:
		if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_CONTROLLABLE)) {
			apply_deceleration_x(movement);
		}
		break;
	}
}

void handle_y_collision(World* world, CollisionData data, PositionComponent& position, MovementComponent &movement, unsigned int entity, FPS fps) {
	switch(data.entity_code) {
	case COLLISION_SOLID:
		remove_forcey(position, movement, fps);
		movement.movY = 0;
		break;
	case COLLISION_HACKER:
		remove_forcey(position, movement, fps);
		movement.movY = 0;
		break;
	case COLLISION_GUARD:
		remove_forcey(position, movement, fps);
		movement.movY = 0;
		break;
	default:
		break;
	}

	switch(data.map_code) {
	case COLLISION_WALL:
		remove_forcey(position, movement, fps);
		movement.movY = 0;
		break;
	case COLLISION_BELTDOWN:
	    add_force(movement, 0.4, 90);
	    break;
	case COLLISION_BELTUP:
	    add_force(movement, 0.4, -90);
	    break;
	case COLLISION_ICE:
	    apply_deceleration_y(movement, movement.friction);
	    break;
	default:
		if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_CONTROLLABLE)) {
			apply_deceleration_y(movement);
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
void rebuild_floor(World * world, int targl)
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

int handle_entity_collision(CollisionData data, World * world, int curEntityID) {
	switch(data.entity_code) {
	case COLLISION_TARGET:
		if (world->collision[curEntityID].type == COLLISION_HACKER && world->command[curEntityID].commands[C_ACTION]) {
			if (handle_collision_target(world, data.entityID)) {
				printf("You captured a target! You is the best!\n");
			}
		}
		break;
	case COLLISION_STAIR:
		if (world->collision[curEntityID].type == COLLISION_HACKER || world->collision[curEntityID].type == COLLISION_GUARD) {
			int targx = world->wormhole[data.entityID].targetX;
			int targy = world->wormhole[data.entityID].targetY;
			int targl = world->wormhole[data.entityID].targetLevel;
			
			move_request(world, send_router_fd[WRITE], targl, targx, targy);
			if(!network_ready)
			{
				rebuild_floor(world, targl);
				return 1;
			}
			floor_change_flag = 1;
		}
		break;
	case COLLISION_HACKER:
		if (world->collision[curEntityID].type == COLLISION_GUARD) 
		{
		// 	if(world->command[curEntityID].commands[C_ACTION])
		// 	{
		// 		send_tag(world, send_router_fd[WRITE], world->player[data.entityID].playerNo);
		// 	}
		}
		break; 
	case COLLISION_GUARD:
		if (world->collision[curEntityID].type == COLLISION_HACKER) {
			//world->mask[curEntityID] = COMPONENT_EMPTY;
		}
		break;
	default:
		break;
	}
	return data.entity_code;
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
	CollisionComponent		*collision;

	//loop through each entity and see if the system can do work on it.
	for(entity = 0; entity < MAX_ENTITIES; entity++) {
		//For controllable entities
		if (IN_THIS_COMPONENT(world->mask[entity], CONTROLLABLE_MASK)) {
			command = &(world->command[entity]);
			position = &(world->position[entity]);
			controllable = &(world->controllable[entity]);
			movement = &(world->movement[entity]);
			collision = &(world->collision[entity]);
			
			//very simple movement. This needs to be synchronized with the
			//game loop so there is no jittering on very slow systems.
			if (controllable->active == true) {
				PositionComponent temp;
				temp.x = position->x;
				temp.y = position->y;
				temp.width = position->width;
				temp.height = position->height;
				temp.level = position->level;
				bool moved = false;
				int collisionType = -1;
				
				if (command->commands[C_UP]) {
					add_force(world, entity, world->movement[entity].acceleration, -90);

					play_animation(world, entity, "up");
				}
				else {
					cancel_animation(world, entity);
				}
				if (command->commands[C_DOWN]) {
					add_force(world, entity, world->movement[entity].acceleration, 90);
					play_animation(world, entity, "down");
				}
				else {
					cancel_animation(world, entity);
				}
				if (command->commands[C_LEFT]) {
					add_force(world, entity, world->movement[entity].acceleration, 180);
					play_animation(world, entity, "left");
				}
				else {
					cancel_animation(world, entity);
				}
				if (command->commands[C_RIGHT]) {
					add_force(world, entity, world->movement[entity].acceleration, 0);
					play_animation(world, entity, "right");
				}
				else {
					cancel_animation(world, entity);
				}

				CollisionData data;
				if (IN_THIS_COMPONENT(world->mask[entity], COLLISION_MASK)) {
					PositionComponent p;
					//temp.x += movement->movX;
					apply_forcex(temp, *movement, fps);
					data = collision_system(world, temp, entity);
					handle_x_collision(world, data, temp, *movement, entity, fps);
					//temp.y += movement->movY;
					apply_forcey(temp, *movement, fps);
					data = collision_system(world, temp, entity);
					handle_y_collision(world, data, temp, *movement, entity, fps);
					collisionType = handle_entity_collision(data, world, entity);
					
					p.x = position->x;
					p.y = position->y;
					p.width = 60;
					p.height = 60;
					p.level = position->level;
					int e = entity_collision(world, p, entity);
					if(e != -1)
					{
						if (world->collision[entity].type == COLLISION_GUARD && world->command[entity].commands[C_ACTION] && world->collision[e].type == COLLISION_HACKER) {
							printf("%d", world->player[e].playerNo);
							printf("You captured a hacker\n");
							send_tag(world, sendpipe, world->player[e].playerNo);
						}
					}
				 }
				
				position->x = temp.x;
				position->y = temp.y;
				
				if (movement->movX > 0 && abs(movement->movX) > abs(movement->movY)) {
					play_animation(world, entity, "right");
					//right
				}
				else if (movement->movX < 0 && abs(movement->movX) > abs(movement->movY)) {
					play_animation(world, entity, "left");
					//left
				}
				else if (movement->movY > 0 && abs(movement->movY) > abs(movement->movX)) {
					play_animation(world, entity, "down");
					//down
				}
				else if (movement->movY < 0 && abs(movement->movY) > abs(movement->movX)) {
					play_animation(world, entity, "up");
					//up
				}
				else {
					//none
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
						//send_status(world, sendpipe, 0, PLAYER_STATE_WAITING);
					}
				}
			}
		}
		else if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_POSITION | COMPONENT_MOVEMENT | COMPONENT_COLLISION)) {
			command = &(world->command[entity]);
			position = &(world->position[entity]);
			controllable = &(world->controllable[entity]);
			movement = &(world->movement[entity]);
			collision = &(world->collision[entity]);
			CollisionData data;
			PositionComponent temp;
			temp.x = position->x;
			temp.y = position->y;
			temp.width = position->width;
			temp.height = position->height;
			temp.level = position->level;
			
			//temp.x += movement->movX;
			apply_forcex(temp, *movement, fps);
			data = collision_system(world, temp, entity);
			handle_x_collision(world, data, temp, *movement, entity, fps);
			
			//temp.y += movement->movY;
			apply_forcey(temp, *movement, fps);
			data = collision_system(world, temp, entity);
			handle_y_collision(world, data, temp, *movement, entity, fps);
			
			position->x = temp.x;
			position->y = temp.y;
			
			handle_entity_collision(data, world, entity);
		}
	}
}

