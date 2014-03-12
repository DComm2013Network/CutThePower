#include <SDL2/SDL.h>

#include "systems.h"
#include "world.h"

int main(int argc, char* argv[]) {
	SDL_Window *window = NULL;
	SDL_Surface *surface = NULL;

	World *world = (World*)malloc(sizeof(World));
	printf("Current World size: %i\n", sizeof(World));
	bool running = true;
	
	SDL_Init(SDL_INIT_VIDEO);
	
	window = SDL_CreateWindow("Cut The Power", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	surface = SDL_GetWindowSurface(window);
	
	if (window == NULL)
	{
		printf("Error initializing the window.\n");
		return 1;
	}
	
	init_world(world);
	
	map_init(world, "assets/Graphics/map01/map01.txt", "assets/Graphics/map01/tiles.txt");
	unsigned int entity = create_player(world, 600, 600, true);
		
	while (running)
	{
		
		//INPUT
		KeyInputSystem(world, &running);
		MouseInputSystem(world);
		movement_system(world);
		map_render(surface, world->position[entity].x, world->position[entity].y);
		render_player_system(*world, surface);
		
		SDL_UpdateWindowSurface(window);
	}
	return 0;
}




