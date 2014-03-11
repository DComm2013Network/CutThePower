/** @ingroup Gameplay */
/** @{ */
/** @file movement_system.cpp */
/** @} */
#include "components.h"
#include "systems.h"
#include "world.h"
#include "collision.h"
#include "stdio.h"
#include <math.h>

#define STANDARD_MASK (COMPONENT_POSITION | COMPONENT_MOVEMENT)     /**< Mask for all moveable entities. */
#define CONTROLLABLE_MASK (COMPONENT_POSITION | COMPONENT_MOVEMENT) /**< Mask for moveable entities that may be controlled. */
#define INPUT_MASK (COMPONENT_INPUT)                                /**< Mask for entities that respond to input. */
#define COLLISION_MASK (COMPONENT_COLLISION)                        /**< Mask for entities that may collide with other entities. */
#define PI 3.14159265                                               /**< An approximation of pi for vector calculations. */


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


/**
 * Moves the entity one unit along its movement vector.
 *
 * @param world  A reference to the world struct.
 * @param entity The entity to move.
 *
 * @designer ?
 * @author   ?
 */
void apply_force(World& world, unsigned int entity) {
	world.position[entity].x += world.movement[entity].movX;
	world.position[entity].y += world.movement[entity].movY;
}

/**
 * Determines the inputs applied to the entity and adds forces in
 * the specified directions.
 *
 * @param world A reference to the world struct.
 *
 * @designer ?
 * @author   ?
 */
void movement_system(World& world) {
	unsigned int entity;
	PositionComponent		*position;
	InputComponent			*input;
	ControllableComponent 	*controllable;
	MovementComponent		*movement;
	
	//loop through each entity and see if the system can do work on it.
	for(entity = 0; entity < MAX_ENTITIES; entity++) {
		//For controllable entities
		if (IN_THIS_COMPONENT(CONTROLLABLE_MASK)) {
			input = &(world.input[entity]);
			position = &(world.position[entity]);
			movement = &(world.movement[entity]);
			controllable = &(world.controllable[entity]);
			
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
					add_force(world, entity, world.movement[entity].acceleration, -90);
				}
				if (input->down) {
					add_force(world, entity, world.movement[entity].acceleration, 90);
				}
				if (input->left) {
					add_force(world, entity, world.movement[entity].acceleration, 180);
				}
				if (input->right) {
					add_force(world, entity, world.movement[entity].acceleration, 0);
				}
				
				temp.x = world.position[entity].x + world.movement[entity].movX;
				temp.y = world.position[entity].y + world.movement[entity].movY;
				
				if ((world.mask[entity] & COLLISION_MASK) != 0) {
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

