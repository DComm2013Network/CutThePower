#include "../components.h"
#include "../systems.h"
#include "../world.h"
#include "../Gameplay/collision.h"
#include "stdio.h"
#include <math.h>

//This is the mask the system uses to see if it will work on the entity.
#define STANDARD_MASK (COMPONENT_POSITION | COMPONENT_MOVEMENT)
#define CONTROLLABLE_MASK (COMPONENT_POSITION | COMPONENT_MOVEMENT)
#define INPUT_MASK (COMPONENT_INPUT)
#define COLLISION_MASK (COMPONENT_COLLISION)
#define PI 3.14159265

extern int floor_change_flag;
extern int send_router_fd[];
extern int rcv_router_fd[];
extern unsigned int player_entity;
extern int network_ready;
int targl = 0;


int handle_collision_target(World *world, int entityIndex) {
	if (world->collision[entityIndex].timer < world->collision[entityIndex].timerMax) {
		world->collision[entityIndex].timer += 2;
	} else {
		world->collision[entityIndex].active = false;
		world->collision[entityIndex].type = COLLISION_EMPTY;
		//world->mask[entityIndex] = COMPONENT_EMPTY;
	}
	return COLLISION_TARGET;
}

void add_force(World& world, unsigned int entity, float magnitude, float dir) {
	if ((world.mask[entity] & STANDARD_MASK) == STANDARD_MASK) {
		world.movement[entity].movX += cos(dir * PI / 180) * magnitude;
		world.movement[entity].movY += sin(dir * PI / 180) * magnitude;
		if (sqrt(pow(world.movement[entity].movX, 2) + pow(world.movement[entity].movY, 2)) > world.movement[entity].maxSpeed) {
			float tx = world.movement[entity].movX;
			float ty = world.movement[entity].movY;
			world.movement[entity].movX *= world.movement[entity].maxSpeed / sqrt(pow(tx, 2) + pow(ty, 2));
			world.movement[entity].movY *= world.movement[entity].maxSpeed / sqrt(pow(tx, 2) + pow(ty, 2));
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

void apply_force(World& world, unsigned int entity) {
	world.position[entity].x += world.movement[entity].movX;
	world.position[entity].y += world.movement[entity].movY;
}

void apply_forcex(PositionComponent& position, MovementComponent movement) {
	position.x += movement.movX;
}

void remove_forcex(PositionComponent& position, MovementComponent movement) {
	position.x -= movement.movX;
}

void apply_forcey(PositionComponent& position, MovementComponent movement) {
	position.y += movement.movY;
}

void remove_forcey(PositionComponent& position, MovementComponent movement) {
	position.y -= movement.movY;
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

void handle_x_collision(CollisionData data, PositionComponent& position, MovementComponent &movement) {
	switch(data.entity_code) {
	case COLLISION_SOLID:
		remove_forcex(position, movement);
		movement.movX = 0;
		break;
	default:
		break;
	}

	switch(data.map_code) {
	case COLLISION_WALL:
		remove_forcex(position, movement);
		movement.movX = 0;
		break;
	case COLLISION_BELTRIGHT:
	    add_force(movement, 0.4, 0);
	    break;
	case COLLISION_BELTLEFT:
	    add_force(movement, 0.4, 180);
	    break;
	case COLLISION_ICE:
	    apply_deceleration_x(movement, 0.01);
	    break;
	default:
		apply_deceleration_x(movement);
		break;
	}
}

void handle_y_collision(CollisionData data, PositionComponent& position, MovementComponent &movement) {
	switch(data.entity_code) {
	case COLLISION_SOLID:
		remove_forcey(position, movement);
		movement.movY = 0;
		break;
	default:
		break;
	}

	switch(data.map_code) {
	case COLLISION_WALL:
		remove_forcey(position, movement);
		movement.movY = 0;
		break;
	case COLLISION_BELTDOWN:
	    add_force(movement, 0.4, 90);
	    break;
	case COLLISION_BELTUP:
	    add_force(movement, 0.4, -90);
	    break;
	case COLLISION_ICE:
	    apply_deceleration_y(movement, 0.001);
	    break;
	default:
		apply_deceleration_y(movement);
		break;
	}
}

void rebuild_floor(World * world)
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

void handle_entity_collision(CollisionData data, World * world, int curEntityID) {
	switch(data.entity_code) {
	case COLLISION_TARGET:
		if (world->collision[curEntityID].type == COLLISION_HACKER) {
			handle_collision_target(world, data.entityID);
		}
		break;
	case COLLISION_STAIR:
		if (world->collision[curEntityID].type == COLLISION_HACKER || world->collision[curEntityID].type == COLLISION_GUARD) {
			int targx = world->wormhole[data.entityID].targetX;
			int targy = world->wormhole[data.entityID].targetY;
			targl = world->wormhole[data.entityID].targetLevel;
			
			move_request(world, send_router_fd[WRITE_END], targl, targx, targy);

		 	floor_change_flag = 1;
		}
		break;
	case COLLISION_HACKER:
		if (world->collision[curEntityID].type == COLLISION_GUARD) {
			//world->mask[data.entityID] = COMPONENT_EMPTY;
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
}

void movement_system(World* world, int sendpipe) {
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
				int goffsetW = 0;//(position->width);
				int goffsetH = 0;//(position->height);
				temp.x = position->x - goffsetW;
				temp.y = position->y - goffsetH;
				temp.width = position->width;
				temp.height = position->height;
				temp.level = position->level;
				bool moved = false;
				
				if (command->commands[C_ACTION]) {
					
				}
				if (command->commands[C_UP]) {
					add_force(world, entity, world->movement[entity].acceleration, -90);
					play_animation(world, entity, (char*)"up");
					moved = true;
				}
				else {
					cancel_animation(world, entity, (char*)"up");
				}
				if (command->commands[C_DOWN]) {
					add_force(world, entity, world->movement[entity].acceleration, 90);
					play_animation(world, entity, (char*)"down");
					moved = true;
				}
				else {
					cancel_animation(world, entity, (char*)"down");
				}
				if (command->commands[C_LEFT]) {
					add_force(world, entity, world->movement[entity].acceleration, 180);
					play_animation(world, entity, (char*)"left");
					moved = true;
				}
				else {
					cancel_animation(world, entity, (char*)"left");
				}
				if (command->commands[C_RIGHT]) {
					add_force(world, entity, world->movement[entity].acceleration, 0);
					play_animation(world, entity, (char*)"right");
					moved = true;
				}
				else {
					cancel_animation(world, entity, (char*)"right");
				}

				CollisionData data;
				if (IN_THIS_COMPONENT(world->mask[entity], COLLISION_MASK)) {

					apply_forcex(temp, *movement);
					data = collision_system(world, temp, entity);
					handle_x_collision(data, temp, *movement);
					apply_forcey(temp, *movement);
					data = collision_system(world, temp, entity);
					handle_y_collision(data, temp, *movement);
					position->x = temp.x + goffsetW;
					position->y = temp.y + goffsetH;
					handle_entity_collision(data, world, entity);
				}
				else {
					position->x = temp.x + goffsetW;
					position->y = temp.y + goffsetH;
				}
				if (position->level == 0) {
					if (position->x < 240) {
						send_status_ready(world, sendpipe, 1);
					}
					else if (position->x > 1000) {
						send_status_ready(world, sendpipe, 2);
					}
					else {
						//send_status_ready(world, sendpipe, 0);
					}
				}
				if(moved && !floor_change_flag)
					send_location(world, sendpipe);
			}
		}
	} 
}


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

void apply_force(World* world, unsigned int entity) {
	world->position[entity].x += world->movement[entity].movX;
	world->position[entity].y += world->movement[entity].movY;
}

