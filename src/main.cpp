#include <SDL2/SDL.h>

#include "systems.h"
#include "sound.h"
#include "world.h"
#include "Input/menu.h"

#include <stdlib.h>
#include <time.h>

int game_net_signalfd, game_net_lockfd;

int main(int argc, char* argv[]) {
	SDL_Window *window;
	SDL_Surface *surface;
	int send_router_fd[2];
	int rcv_router_fd[2];
	unsigned int entity = -1;

	create_pipe(send_router_fd);
	create_pipe(rcv_router_fd);

	World *world = (World*)malloc(sizeof(World));
	printf("Current World size: %i\n", sizeof(World));
	bool running = true;
	
	//SDL_Init(SDL_INIT_VIDEO);
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	
	window = SDL_CreateWindow("Cut The Power", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	surface = SDL_GetWindowSurface(window);
	
	if (window == NULL) {
		printf("Error initializing the window.\n");
		return 1;
	}
	
	init_sound();
	
	init_world(world);
	srand(time(NULL));//random initializer
	
	//map_init(world, "assets/Graphics/lobby/lobby.txt", "assets/Graphics/lobby/lobby_tiles.txt");
	//map_init(world, "assets/Graphics/lobby.txt", "assets/Graphics/tiles_lobby.txt");
	
	KeyMapInit("assets/Input/keymap.txt");
	init_render_player_system();
	//unsigned int entity = create_player(world, 600, 600, true);
	
	create_main_menu(world);
	//create_bsod_menu(world);
	
	while (running)
	{
		
		//INPUT
		KeyInputSystem(world, &running);
		MouseInputSystem(world, &entity);
		movement_system(world);
		if (entity < MAX_ENTITIES) {
			map_render(surface, world, entity);
			//send_system(world, send_router_fd[WRITE_END]);
		}
		animation_system(world);
		render_player_system(*world, surface);
		
		SDL_UpdateWindowSurface(window);
	}
	return 0;
}




