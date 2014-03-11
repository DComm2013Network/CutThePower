#ifndef INPUT_SYSTEMS_H
#define INPUT_SYSTEMS_H

#include "../world.h"

void KeyInputSystem(World *world, bool *running);
int KeyMapInit(char *file);
void MouseInputSystem(World *world);

#endif
