#ifndef COMPONENT_H
#define COMPONENT_H

#include "Gameplay/components.h"
#include "Graphics/components.h"
#include "Input/components.h"

/* Each components needs to be added here */
typedef enum {
	COMPONENT_EMPTY = 0,
	COMPONENT_MOUSE = 1 << 0,
	COMPONENT_TEXTFIELD = 1 << 1,
	COMPONENT_BUTTON = 1 << 2,
	COMPONENT_COMMAND = 1 << 3,
	COMPONENT_POSITION = 1 << 4,
	COMPONENT_LEVEL = 1 << 5,
	COMPONENT_COLLISION = 1 << 6,
	COMPONENT_MOVEMENT = 1 << 7,
	COMPONENT_CONTROLLABLE = 1 << 8,
	COMPONENT_RENDER_PLAYER = 1 << 9,
	COMPONENT_PLAYER = 1 << 10,
	COMPONENT_TAG = 1 << 11,
	COMPONENT_ANIMATION = 1 << 12,
	COMPONENT_WORMHOLE = 1 << 13,
	COMPONENT_OBJECTIVE = 1 << 14,
	COMPONENT_MENU_ITEM = 1 << 15,
	COMPONENT_STILE = 1 << 16,
	COMPONENT_POWERUP = 1 << 17,
	COMPONENT_CUTSCENE = 1 << 18
} Components;

#endif
