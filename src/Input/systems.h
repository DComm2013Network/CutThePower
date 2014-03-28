#ifndef INPUT_SYSTEMS_H
#define INPUT_SYSTEMS_H

#include "../world.h"

void KeyInputSystem(World *world);
int KeyMapInit(char *file);
int KeyMapInitArray(char *file, int **commmands);
void MouseInputSystem(World *world);

#endif
