#include "components.h"
#include "systems.h"
#include "world.h"
#include "collision.h"
#include "stdio.h"

#include <math.h>

//This is the mask the system uses to see if it will work on the entity.
#define STANDARD_MASK (COMPONENT_POSITION | COMPONENT_MOVEMENT)
#define CONTROLLABLE_MASK (COMPONENT_POSITION | COMPONENT_MOVEMENT | COMPONENT_INPUT)
#define COLLISION_MASK (COMPONENT_COLLISION)
#define SPEED 1
#define PI 3.14159265

void apply_force(World& world, unsigned int entity, float magnitude, float dir) {
	float newx = 0;
	float newy = 0;
	if ((world.mask[entity] & STANDARD_MASK) == STANDARD_MASK) {
		if (world.movement[entity].velocity < world.movement[entity].maxVelocity) {
			newx = cos(world.movement[entity].movementRot * PI / 180) * world.movement[entity].velocity;
			newy = sin(world.movement[entity].movementRot * PI / 180) * world.movement[entity].velocity;
			//printf("newx: %i, newy: %i, rot: %f, vel: %f\n", newx, newy, world.movement[entity].movementRot, world.movement[entity].velocity);
			newx += cos(dir * PI / 180) * magnitude;
			newy += sin(dir * PI / 180) * magnitude;
			//printf("newx: %i, newy: %i, rot: %f, vel: %f\n", newx, newy, dir, magnitude);
			world.movement[entity].velocity = sqrt(pow(newx, 2) + pow(newy, 2));
			//printf("vel: %f\n", world.movement[entity].velocity);
			if (world.movement[entity].velocity > 0) {
				world.movement[entity].movementRot = dir;//(asin(newy / world.movement[entity].velocity) * 180 / PI);
				world.movement[entity].movementRot = dir;//(acos(newx / world.movement[entity].velocity) * 180 / PI);
			}
		} else {
			world.movement[entity].velocity = world.movement[entity].maxVelocity;
		}
	}
}

void movement_system(World& world) {
	unsigned int entity;
	PositionComponent		*position;
	InputComponent			*input;
	ControllableComponent 	*controllable;
	MovementComponent		*movement;
	//CollisionComponent		*collision;
	
	//loop through each entity and see if the system can do work on it.
	for(entity = 0; entity < MAX_ENTITIES; entity++) {
		//For controllable entities
		if (IN_THIS_COMPONENT(CONTROLLABLE_MASK)) {
			input = &(world.input[entity]);
			position = &(world.position[entity]);
			movement = &(world.movement[entity]);
			controllable = &(world.controllable[entity]);
			//collision = &(world.collision[entity]);
			
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
				
				movement->lastDirection = 0;
				if (input->up) {
					apply_force(world, entity, world.movement[entity].acceleration, -90);
				}
				if (input->down) {
					apply_force(world, entity, world.movement[entity].acceleration, 90);
				}
				if (input->left) {
					apply_force(world, entity, world.movement[entity].acceleration, 180);
				}
				if (input->right) {
					apply_force(world, entity, world.movement[entity].acceleration, 0);
				}
				
				temp.x = world.position[entity].x + cos(world.movement[entity].movementRot * PI / 180) * world.movement[entity].velocity;
				temp.y = world.position[entity].y + sin(world.movement[entity].movementRot * PI / 180) * world.movement[entity].velocity;
				if ((world.mask[entity] & COLLISION_MASK) != 0) {
					if (collision_system(world, temp) == false) {
						/*if ((movement->lastDirection & (0x01 | 0x04)) == (0x01 & 0x04)) {
							apply_force(world, entity, world.movement[entity].acceleration, -135);
						}
						if ((movement->lastDirection & (0x01 | 0x08)) == (0x01 & 0x08)) {
							apply_force(world, entity, world.movement[entity].acceleration, -45);
						}
						if ((movement->lastDirection & (0x02 | 0x04)) == (0x02 & 0x04)) {
							apply_force(world, entity, world.movement[entity].acceleration, 135);
						}
						if ((movement->lastDirection & (0x02 | 0x08)) == (0x02 & 0x08)) {
							apply_force(world, entity, world.movement[entity].acceleration, 45);
						}*/
						//printf("sped: %f, rot: %f\n", world.movement[entity].velocity, world.movement[entity].movementRot);
						world.position[entity].x = temp.x;
						world.position[entity].y = temp.y;
						world.movement[entity].velocity *= 0.99;
						/*if (world.movement[entity].velocity < 0.01) {
							world.movement[entity].velocity = 0;
						}*/
					}
					else {
						world.movement[entity].velocity = 0;
					}
				} else {
					position->x = temp.x;
					position->y = temp.y;
				}
			}
			
		} 
	}
}
