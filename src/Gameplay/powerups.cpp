#include "powerups.h"
#include "poweruptypes.h"

void powerup_speedup(World * world, unsigned int entityID) {
	world->powerup[entityID].captureTime = SDL_GetTicks();
	world->powerup[entityID].duration = PU_SPEED_UP_DURATION;
	world->powerup[entityID].type = PU_SPEED_UP;
	
	if (IN_THIS_COMPONENT(world->mask[entityID], COMPONENT_MOVEMENT)) {
		world->movement[entityID].maxSpeed = world->movement[entityID].defMaxSpeed * 1.5;
	}
}

void powerup_speeddown(World * world, unsigned int entityID) {
	world->powerup[entityID].captureTime = SDL_GetTicks();
	world->powerup[entityID].duration = PU_SPEED_DOWN_DURATION;
	world->powerup[entityID].type = PU_SPEED_DOWN;
	
	if (IN_THIS_COMPONENT(world->mask[entityID], COMPONENT_MOVEMENT)) {
		world->movement[entityID].maxSpeed = world->movement[entityID].defMaxSpeed * 0.5;
	}
}

void powerup_system(World * world, unsigned int entityID) {
	if (IN_THIS_COMPONENT(world->mask[entityID], COMPONENT_POWERUP)) {
		PowerUpComponent * powerup = &(world->powerup[entityID]);
		if (powerup->type != PU_NONE) {
			unsigned int current_time = SDL_GetTicks();
			
			//printf("Current: ");
			
			if (current_time >= (powerup->captureTime + powerup->duration)) {
				switch(powerup->type) {
					case PU_SPEED_UP:
						if (IN_THIS_COMPONENT(world->mask[entityID], COMPONENT_MOVEMENT)) {
							world->movement[entityID].maxSpeed = world->movement[entityID].defMaxSpeed;
						}
					break;
					case PU_SPEED_DOWN:
						if (IN_THIS_COMPONENT(world->mask[entityID], COMPONENT_MOVEMENT)) {
							world->movement[entityID].maxSpeed = world->movement[entityID].defMaxSpeed;
						}
					break;
				}
			}
		}
	}
}
