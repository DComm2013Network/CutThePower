#ifndef COMPONENT_H
#define COMPONENT_H

#include "Gameplay/components.h"
#include "Graphics/components.h"
#include "Input/components.h"

/* Each components needs to be added here */
typedef enum {
	COMPONENT_NONE = 0,
	COMPONENT_MOUSE = 1 << 0,
	COMPONENT_TEXTFIELD = 1 << 1,
	COMPONENT_BUTTON = 1 << 2,
	COMPONENT_COMMAND = 1 << 3,
	COMPONENT_POSITION = 1 << 4
} Components;

#endif
