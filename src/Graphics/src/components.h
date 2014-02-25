#ifndef COMPONENT_H
#define COMPONENT_H
#include <SDL2/SDL.h>
#include "graphicscomponents.h"

typedef enum{
	COMPONENT_NONE = 0,
	COMPONENT_RENDER_PLAYER = 1 << 0,
} Components;


typedef struct{
	SDL_Texture *playerTexture;
	int width;
	int height;
} RenderPlayerComponent;


#endif
