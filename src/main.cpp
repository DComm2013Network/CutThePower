#include <SDL2/SDL.h>

#include "systems.h"
#include "world.h"
#define NETWORKON
/*
===============================================
	DEFINE NETWORKOFF TO DISABLE NETWORKING
===============================================
*/

int game_net_signalfd;
int network_ready = 0;

int main(int argc, char* argv[]) {
	SDL_Window *window;
	SDL_Surface *surface;
	int send_router_fd[2];
	int rcv_router_fd[2];

	create_pipe(send_router_fd);
	create_pipe(rcv_router_fd);

	World *world = (World*)malloc(sizeof(World));
	printf("Current World size: %i\n", sizeof(World));
	bool running = true;
	
	SDL_Init(SDL_INIT_VIDEO);
	
	window = SDL_CreateWindow("Cut The Power", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	surface = SDL_GetWindowSurface(window);
		
	if (window == NULL) {
		printf("Error initializing the window.\n");
		return 1;
	}
	
	init_world(world);
	
	map_init(world, "assets/Graphics/SampleFloor.txt", "assets/Graphics/tiles.txt");
		
	create_player(world, 50, 50, true);

	#ifndef NETWORKOFF
		game_net_signalfd 	= eventfd(0, EFD_SEMAPHORE);

		init_client_network(send_router_fd, rcv_router_fd);
		send_intialization(world, send_router_fd[WRITE_END]);
	#endif
	
	while (running) {
		KeyInputSystem(world, &running);
		MouseInputSystem(world);
		movement_system(world, send_router_fd[WRITE_END]);
		map_render(surface);
		render_player_system(*world, surface);

		#ifndef NETWORKOFF
			send_location(world, send_router_fd[WRITE_END]);
			client_update_system(world, rcv_router_fd[READ_END]);
		#endif

		SDL_UpdateWindowSurface(window);
	}
	return 0;
}




