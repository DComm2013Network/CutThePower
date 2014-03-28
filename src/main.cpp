#include <SDL2/SDL.h>

#include "systems.h"
#include "sound.h"
#include "world.h"
#include "Input/menu.h"
#include "Graphics/text.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int game_net_signalfd, game_net_lockfd;
bool running;
unsigned int player_entity;

int main(int argc, char* argv[]) {
	SDL_Window *window;
	SDL_Surface *surface;
	
	int send_router_fd[2];
	int rcv_router_fd[2];

	//create_pipe(send_router_fd);
	//create_pipe(rcv_router_fd);

	World *world = (World*)malloc(sizeof(World));
	printf("Current World size: %i\n", sizeof(World));
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	
	window = SDL_CreateWindow("Cut The Power", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if (window == NULL) {
		printf("Error initializing the window.\n");
		return 1;
	}
	surface = SDL_GetWindowSurface(window);
	
	
	init_sound();
	init_fonts();
	
	init_world(world);
	srand(time(NULL));//random initializer
	
	KeyMapInit((char*)"assets/Input/keymap.txt");
	init_render_player_system();
	
	create_main_menu(world);
	
	
	FPS fps;
	fps.init();
	running = true;
	player_entity = -1;
	
	while (running)
	{
		
		//INPUT
		KeyInputSystem(world);
		MouseInputSystem(world);
		movement_system(world, fps);
		if (player_entity < MAX_ENTITIES) {
			map_render(surface, world, player_entity);
			//send_system(world, send_router_fd[WRITE_END]);
		}
		animation_system(world);
		render_player_system(*world, surface);
		
		SDL_UpdateWindowSurface(window);
		
		//fps.limit();
		fps.update();
	}
	
	cleanup_map();
	cleanup_sound();
	cleanup_fonts();
	
	destroy_world(world);
	free(world);
	
	//SDLNet_Quit();
	SDL_Quit();
	
	printf("Exiting The Game\n");
	
	printf("Exiting The Game\n");
	
	return 0;
}




