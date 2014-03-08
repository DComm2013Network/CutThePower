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
	
	init_world(world);
	
	create_player(world, 50, 50, true);
	
	game_net_signalfd 	= eventfd(0, EFD_SEMAPHORE);
	game_net_lockfd		= eventfd(0, EFD_SEMAPHORE);

	init_client_network(send_router_fd, rcv_router_fd);
	
	while (running)
	{
		
		//INPUT
		KeyInputSystem(&world, &running);
		MouseInputSystem(&world);
		movement_system(world);
		render_system(world, surface);
		//map_render(surface);
		send_system(&world, send_router_fd[WRITE_END]);
		update_system(&world, rcv_router_fd[READ_END])
		
		SDL_UpdateWindowSurface(window);
	}
	return 0;
}




