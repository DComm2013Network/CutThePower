#ifndef TRIGGERED_H
#define TRIGGERED_H

#include "world.h"

bool menu_click(World *world, unsigned int entity);
void animation_end(World *world, unsigned int entity, int ANIMATION_ID);

#endif
