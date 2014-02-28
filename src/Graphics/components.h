#ifndef GRAPHIC_COMPONENT_H
#define GRAPHIC_COMPONENT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../components.h"

/*
typedef enum{
	COMPONENT_NONE = 0,
	COMPONENT_RENDER_PLAYER = 1 << 0,
} Components;
*/

typedef struct{
	SDL_Texture *playerTexture;
	int width;
	int height;
} RenderPlayerComponent;


#endif
