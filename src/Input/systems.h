#ifndef INPUT_SYSTEMS_H
#define INPUT_SYSTEMS_H

#include "../world.h"

void KeyInputSystem(World *world);
int KeyMapInit(const char *file);
int KeyMapInitArray(const char *file, int **commmands);
void MouseInputSystem(World *world);
void wait(World* world, const unsigned int entity);

#endif
