#ifndef POWERUPS_H
#define POWERUPS_H

#include "../world.h"

void powerup_system(World * world, unsigned int entityID);
void powerup_speedup(World * world, unsigned int entityID);
void powerup_speeddown(World * world, unsigned int entityID);
void powerup_beltright(World * world, unsigned int entityID);
void powerup_beltleft(World * world, unsigned int entityID);

#endif

