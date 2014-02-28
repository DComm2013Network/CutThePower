#include <SDL2/SDL.h>

#include "systems.h"
#include "world.h"

int main(int argc, char* argv[]) {
	SDL_Window *window;
	SDL_Surface *surface;
	
	World world;
	bool running = true;
	
	
	SDL_Init(SDL_INIT_VIDEO);
	
	window = SDL_CreateWindow("Cut The Power", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	surface = SDL_GetWindowSurface(window);
	
	if (window == NULL)
	{
		printf("Error initializing the window.\n");
		return 1;
	}
	
	init_world(&world);
	
	while (running)
	{
		
		
		
		
		
		SDL_UpdateWindowSurface(window);
	}
	
	return 0;
}




