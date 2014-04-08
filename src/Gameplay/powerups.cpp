/** @ingroup Gameplay */
/** @{ */
/** @file powerups.cpp */
/** @} */
#include "powerups.h"
#include "poweruptypes.h"

/**
 * Sets the player powerup to the speed up powerup.
 * 
 * @param[in,out] world    A pointer to the world structure.
 * @param[in] entityID     The player entityID number.
 *
 * @designer Joshua Campbell
 * @author   Joshua Campbell
 */
void powerup_speedup(World * world, unsigned int entityID) {
	world->powerup[entityID].captureTime = SDL_GetTicks();
	world->powerup[entityID].duration = PU_SPEED_UP_DURATION;
	world->powerup[entityID].type = PU_SPEED_UP;
	
	if (IN_THIS_COMPONENT(world->mask[entityID], COMPONENT_MOVEMENT)) {
		world->movement[entityID].maxSpeed = world->movement[entityID].defMaxSpeed * 1.5;
	}
}

/**
 * Sets the player powerup to the speed down powerup.
 * 
 * @param[in,out] world    A pointer to the world structure.
 * @param[in] entityID     The player entityID number.
 *
 * @designer Joshua Campbell
 * @author   Joshua Campbell
 */
void powerup_speeddown(World * world, unsigned int entityID) {
	world->powerup[entityID].captureTime = SDL_GetTicks();
	world->powerup[entityID].duration = PU_SPEED_DOWN_DURATION;
	world->powerup[entityID].type = PU_SPEED_DOWN;
	
	if (IN_THIS_COMPONENT(world->mask[entityID], COMPONENT_MOVEMENT)) {
		world->movement[entityID].maxSpeed = world->movement[entityID].defMaxSpeed * 0.5;
	}
}

/**
 * Sets the player powerup to the belt left power up.
 * 
 * @param[in,out] world    A pointer to the world structure.
 * @param[in] entityID     The player entityID number.
 *
 * @designer Ramzi 
 * @author   Ramzi 
 */
void powerup_beltleft(World * world, unsigned int entityID) {
	world->powerup[entityID].captureTime = SDL_GetTicks();
	world->powerup[entityID].duration = PU_BELT_DURATION;
	world->powerup[entityID].type = PU_BELTLEFT;
	world->player[entityID].tilez = TILE_BELT_LEFT;
}

/**
 * Sets the player powerup to the belt right power up.
 * 
 * @param[in,out] world    A pointer to the world structure.
 * @param[in] entityID     The player entityID number.
 *
 * @designer Ramzi 
 * @author   Ramzi 
 */
void powerup_beltright(World * world, unsigned int entityID) {
	world->powerup[entityID].captureTime = SDL_GetTicks();
	world->powerup[entityID].duration = PU_BELT_DURATION;
	world->powerup[entityID].type = PU_BELTRIGHT;
	world->player[entityID].tilez = TILE_BELT_RIGHT;
}

/**
 * Checks if the powerup has expired and if it has, it sets the current
 * player powerup to no power up.
 * 
 * @param[in,out] world    A pointer to the world structure.
 * @param[in] entityID     The player entityID number.
 *
 * @designer Joshua Campbell
 * @author   Joshua Campbell, Ramzi
 */
void powerup_system(World * world, unsigned int entityID) {
	if (IN_THIS_COMPONENT(world->mask[entityID], COMPONENT_POWERUP)) {
		PowerUpComponent * powerup = &(world->powerup[entityID]);
		if (powerup->type != PU_NONE) {
			unsigned int current_time = SDL_GetTicks();
			
			if (current_time >= (powerup->captureTime + powerup->duration)) {
				switch(powerup->type) {
					case PU_SPEED_UP:
						world->powerup[entityID].captureTime = 0;
						world->powerup[entityID].duration = 0;
						world->powerup[entityID].type = PU_NONE;
						if (IN_THIS_COMPONENT(world->mask[entityID], COMPONENT_MOVEMENT)) {
							world->movement[entityID].maxSpeed = world->movement[entityID].defMaxSpeed;
						}
					break;
					case PU_SPEED_DOWN:
						world->powerup[entityID].captureTime = 0;
						world->powerup[entityID].duration = 0;
						world->powerup[entityID].type = PU_NONE;
						if (IN_THIS_COMPONENT(world->mask[entityID], COMPONENT_MOVEMENT)) {
							world->movement[entityID].maxSpeed = world->movement[entityID].defMaxSpeed;
						}
					break;
					case PU_BELTLEFT:
						world->powerup[entityID].captureTime = 0;
						world->powerup[entityID].duration = 0;
						world->powerup[entityID].type = PU_NONE;
						world->player[entityID].tilez = 0;
					break;
					case PU_BELTRIGHT:
						world->powerup[entityID].captureTime = 0;
						world->powerup[entityID].duration = 0;
						world->powerup[entityID].type = PU_NONE;
						world->player[entityID].tilez = 0;
					break;
				}
			}
		}
	}
}
