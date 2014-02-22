#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "map.h"
int main(int argc, char* argv[]) 
{
	SDL_Window *gWindow		= NULL;
	SDL_Surface *gSurface	= NULL;
	SDL_Event e;
	
	int running = 1;
	
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		return 1;
	} else {
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) //Set texture filtering to linear
		{
			printf( "Warning: Linear texture filtering not enabled!" );
			return 1;
		}
		if ((gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN )) == NULL)
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			return 1;
		} else {
			if((gSurface = SDL_GetWindowSurface(gWindow)) == NULL){
				printf( "Surface could not be created! SDL Error: %s\n", SDL_GetError() );
				return 1;
			} else{
				if (map_init("assets/SampleFloor.txt", "assets/tiles.txt") != 0) {
					printf("Error loading map.\n");
					return 1;
				}
			}
		}
	}
	
	while (running)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				running = 0;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					running = 0;
				}
			}
		}
		map_render(gSurface);
		SDL_UpdateWindowSurface(gWindow);
	}
	
	return 0;
}


