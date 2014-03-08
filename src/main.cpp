#include <SDL2/SDL.h>

#include "systems.h"
#include "world.h"

int game_net_signalfd, game_net_lockfd;

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
	
	game_net_signalfd 	= eventfd(0, EFD_SEMAPHORE);
	game_net_lockfd		= eventfd(0, EFD_SEMAPHORE);

	init_client_network(send_router_fd, rcv_router_fd);
	
	while (running) {
		//client_update_system(world, rcv_router_fd[READ_END]);
		KeyInputSystem(world, &running);
		MouseInputSystem(world);
		movement_system(world);
		map_render(surface);
		render_player_system(*world, surface);
		send_system(world, send_router_fd[WRITE_END]);
		
		SDL_UpdateWindowSurface(window);
	}
	return 0;
}




