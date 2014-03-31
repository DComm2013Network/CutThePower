#include <SDL2/SDL.h>
#include "systems.h"
#include "sound.h"
#include "Network/Packets.h"
#include "world.h"
#include "Input/menu.h"
#include "Graphics/text.h"
#include "Input/chat.h"

#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>

bool running;
unsigned int player_entity;
int send_router_fd[2];
int rcv_router_fd[2];
int game_net_signalfd;
int network_ready = 0;
int window_width = WIDTH;
int window_height = HEIGHT;
SDL_Window *window;

int main(int argc, char* argv[]) {
	SDL_Surface *surface;

	SDL_Renderer *renderer;
	SDL_Texture *surface_texture;
	
	int send_router_fd[2];
	int rcv_router_fd[2];

	create_pipe(send_router_fd);
	create_pipe(rcv_router_fd);

	World *world = (World*)malloc(sizeof(World));
	printf("Current World size: %i\n", sizeof(World));
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	
	window = SDL_CreateWindow("Cut The Power", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		printf("Error initializing the window.\n");
		return 1;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
	surface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);

	init_chat();
	init_sound();
	init_fonts();
	
	init_world(world);
	srand(time(NULL));//random initializer
	KeyMapInit("assets/Input/keymap.txt");
	init_render_player_system();

	//create_main_menu(world);
	
	unsigned int begin_time = SDL_GetTicks();

	create_logo_screen(world);

	FPS fps;
	fps.init();

	running = true;
	player_entity = -1;

	//chat_add_line("Hello I am Jordan0!");

	while (running)
	{
		unsigned int current_time;
		//INPUT
		KeyInputSystem(world);
		MouseInputSystem(world);
		movement_system(world, fps, send_router_fd[WRITE]);

		if (player_entity < MAX_ENTITIES) {
			map_render(surface, world, player_entity);
		}
		animation_system(world);
		render_player_system(*world, surface);
		chat_render(surface);
		
		surface_texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, surface_texture, NULL, NULL);
		
		SDL_DestroyTexture(surface_texture);
		SDL_RenderPresent(renderer);

		if(network_ready)
		{
			current_time = SDL_GetTicks();
			if((current_time - begin_time) >= (1000/SEND_FREQUENCY))
			{
				begin_time = SDL_GetTicks();
				send_location(world, send_router_fd[WRITE]);
			}
			client_update_system(world, rcv_router_fd[READ]);
		}

		fps.limit();
		fps.update();
	}
	
	cleanup_map();
	cleanup_sound();
	cleanup_fonts();
	
	destroy_world(world);
	free(world);
	
	//SDLNet_Quit();
	IMG_Quit();
	SDL_Quit();
	
	printf("Exiting The Game\n");
	
	return 0;
}
