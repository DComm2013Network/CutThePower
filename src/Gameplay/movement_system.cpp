#include <stdio.h>
#include <math.h>
#include "components.h"
#include "systems.h"
#include "collision.h"

//This is the mask the system uses to see if it will work on the entity.
#define STANDARD_MASK (COMPONENT_POSITION | COMPONENT_MOVEMENT)
#define CONTROLLABLE_MASK (COMPONENT_POSITION | COMPONENT_MOVEMENT)
#define INPUT_MASK (COMPONENT_INPUT)
#define COLLISION_MASK (COMPONENT_COLLISION)
#define PI 3.14159265


void movement_system(World& world) {
	unsigned int entity;
	PositionComponent		*position;
	CommandComponent		*command;
	ControllableComponent 	*controllable;

	//loop through each entity and see if the system can do work on it.
	for(entity = 0; entity < MAX_ENTITIES; entity++) {
		//For controllable entities
		if (IN_THIS_COMPONENT(world.mask[entity], CONTROLLABLE_MASK)) {
			command = &(world.command[entity]);
			position = &(world.position[entity]);
			controllable = &(world.controllable[entity]);
			
			//very simple movement. This needs to be synchronized with the
			//game loop so there is no jittering on very slow systems.
			if (controllable->active) {
				PositionComponent temp;
				temp.x = position->x;
				temp.y = position->y;
				temp.width = position->width;
				temp.height = position->height;
				temp.s = position->s;
				temp.level = position->level;
				
				
				if (command->commands[C_UP]) {
					add_force(world, entity, world.movement[entity].acceleration, -90);
				}
				if (command->commands[C_DOWN]) {
					add_force(world, entity, world.movement[entity].acceleration, 90);
				}
				if (command->commands[C_LEFT]) {
					add_force(world, entity, world.movement[entity].acceleration, 180);
				}
				if (command->commands[C_RIGHT]) {
					add_force(world, entity, world.movement[entity].acceleration, 0);
				}
				
				temp.x = world.position[entity].x + world.movement[entity].movX;
				temp.y = world.position[entity].y + world.movement[entity].movY;
				
				if (IN_THIS_COMPONENT(world.mask[entity], COLLISION_MASK)) {
					if (collision_system(world, temp, entity)) {
						//here we assume that the variable 'temp' holds the correct
						//position data based on the collision that occurred.
						world.movement[entity].movX = world.movement[entity].movX - (temp.x - world.position[entity].x);
						world.movement[entity].movY = world.movement[entity].movY - (temp.y - world.position[entity].y);
					}
					//application of all of the forces this tick
					apply_force(world, entity);
					//1% slowdown per tick, means you move at 99% speed while moving,
					//but when stopping, slow down. This is all regardless of collisions.
					world.movement[entity].movX *= 1 - 0.01;
					world.movement[entity].movY *= 1 - 0.01;
				}
			}
		} 
	}
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

void apply_force(World& world, unsigned int entity) {
	world.position[entity].x += world.movement[entity].movX;
	world.position[entity].y += world.movement[entity].movY;
}

