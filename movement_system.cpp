#include "components.h"
#include "systems.h"
#include "world.h"

//This is the mask the system uses to see if it will work on the entity.
#define STANDARD_MASK (COMPONENT_POSITION | COMPONENT_MOVEMENT)
#define CONTROLLABLE_MASK (COMPONENT_POSITION | COMPONENT_COLLISION | COMPONENT_MOVEMENT)
#define INPUT_MASK (COMPONENT_INPUT)
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
				
				if (input->up) {
					temp->y -= movement->velocity;
				}
				if (input->down) {
					temp->y += movement->velocity;
				}
				if (input->left) {
					temp->x -= movement->velocity;
				}
				if (input->right) {
					temp->x += movement->velocity;
				}
				
				if ((world.mask[entity] & COLLISION_MASK) != 0) {
					/*COLLISION SYSTEM PLACEHOLDER*/
				} else {
					position->x = temp.x;
					position->y = temp.y;
				}
			}
			
		} 
	}
}
