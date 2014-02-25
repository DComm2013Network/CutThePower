#include "components.h"
#include "systems.h"
#include "world.h"
#include "collision.h"
#include "stdio.h"

//This is the mask the system uses to see if it will work on the entity.
#define STANDARD_MASK (COMPONENT_POSITION | COMPONENT_MOVEMENT)
#define CONTROLLABLE_MASK (COMPONENT_POSITION | COMPONENT_MOVEMENT)
#define INPUT_MASK (COMPONENT_INPUT)
#define COLLISION_MASK (COMPONENT_COLLISION)
#define SPEED 1

void movement_system(World& world) 
{
	
	unsigned int entity;
	PositionComponent		*position;
	InputComponent			*input;
	ControllableComponent 	*controllable;
	MovementComponent		*movement;
	CollisionComponent		*collision;
	
	for(entity = 0; entity < MAX_ENTITIES; ++entity) {
		if ((world.mask[entity] & INPUT_MASK) == INPUT_MASK) {
			input = &(world.input[entity]);
			break;
		}
	}
	
	//loop through each entity and see if the system can do work on it.
	for(entity = 0; entity < MAX_ENTITIES; ++entity) 
	{
		//For controllable entities
		if ((world.mask[entity] & CONTROLLABLE_MASK) == CONTROLLABLE_MASK) 
		{
			position = &(world.position[entity]);
			movement = &(world.movement[entity]);
			controllable = &(world.controllable[entity]);
			collision = &(world.collision[entity]);
			
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
				
				
				
				if (input->up && !input->down) {
					temp.y -= movement->velocity;
					if ((world.mask[entity] & COLLISION_MASK) != 0) {
						if (collision_system(world, temp, entity) == 0) {
							position->x = temp.x;
							position->y = temp.y;
						} else {
							temp.y += movement->velocity;
						}
					} else {
						position->x = temp.x;
						position->y = temp.y;
					}
					
				}
				
				
				
				if (input->down && !input->up) {
					temp.y += movement->velocity;
					if ((world.mask[entity] & COLLISION_MASK) != 0) {
						if (collision_system(world, temp, entity) == 0) {
							position->x = temp.x;
							position->y = temp.y;
						} else {
							temp.y -= movement->velocity;
						}
					} else {
						position->x = temp.x;
						position->y = temp.y;
					}
					
				}
				
				
				
				
				if (input->left && !input->right) {
					temp.x -= movement->velocity;
					if ((world.mask[entity] & COLLISION_MASK) != 0) {
						if (collision_system(world, temp, entity) == 0) {
							position->x = temp.x;
							position->y = temp.y;
						} else {
							temp.x += movement->velocity;
						}
					} else {
						position->x = temp.x;
						position->y = temp.y;
					}
					
				}
				
				
				
				if (input->right &&!input->left) {
					temp.x += movement->velocity;
					if ((world.mask[entity] & COLLISION_MASK) != 0) {
						if (collision_system(world, temp, entity) == 0) {
							position->x = temp.x;
							position->y = temp.y;
						} else {
							temp.x -= movement->velocity;
						}
					} else {
						position->x = temp.x;
						position->y = temp.y;
					}
				}
				
				/*if ((world.mask[entity] & COLLISION_MASK) != 0) {
					if (collision_system(world, temp, entity) == 0) {
						position->x = temp.x;
						position->y = temp.y;
					}
				} else {
					position->x = temp.x;
					position->y = temp.y;
				}*/
			}
			
		} 
	}
}
