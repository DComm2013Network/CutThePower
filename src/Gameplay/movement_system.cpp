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

void movement_system(World* world) {
	unsigned int entity;
	PositionComponent		*position;
	CommandComponent		*command;
	ControllableComponent 	*controllable;
	MovementComponent	 *movement;

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
				temp.s = position->s;
				temp.level = position->level;
				
				if (command->commands[C_UP]) {
					add_force(world, entity, world->movement[entity].acceleration, -90);
				}
				if (command->commands[C_DOWN]) {
					add_force(world, entity, world->movement[entity].acceleration, 90);
				}
				if (command->commands[C_LEFT]) {
					add_force(world, entity, world->movement[entity].acceleration, 180);
				}
				if (command->commands[C_RIGHT]) {
					add_force(world, entity, world->movement[entity].acceleration, 0);
				}

				int code = 0;
				if (IN_THIS_COMPONENT(world->mask[entity], COLLISION_MASK)) {
					apply_forcex(temp, *movement);
					if (code = collision_system(*world, temp, entity)) {
						remove_forcex(temp, *movement);
						movement->movX = 0;
					} else {
						movement->movX *= 1 - 0.1;
					}
					apply_forcey(temp, *movement);
					if (code = collision_system(*world, temp, entity)) {
						remove_forcey(temp, *movement);
						movement->movY = 0;
					} else {
						movement->movY *= 1 - 0.1;
					}

				}	
				position->x = temp.x;
				position->y = temp.y;
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

