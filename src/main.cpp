#include <SDL2/SDL.h>
#include "systems.h"
#include "sound.h"
#include "world.h"
#include "Input/menu.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>


#define FPS_MAX 30

int network_ready = 0;
int send_ready = 0;
int game_ready = 0;
class FPS {
private:
	float max_frame_ticks;
	Uint32 last_ticks;
	int fps;
	int numFrames;
	Uint32 startTime;

	Uint32 current_ticks;
	Uint32 target_ticks;

public:
	void init() {
		startTime = SDL_GetTicks();
		max_frame_ticks = (1000.0/(float)FPS_MAX) + 0.00001;
		fps = 0;
		last_ticks = SDL_GetTicks();
		numFrames = 0; 
	}

	void limit() {
		fps++;
		target_ticks = last_ticks + Uint32(fps * max_frame_ticks);
		current_ticks = SDL_GetTicks();

		if (current_ticks < target_ticks) {
			SDL_Delay(target_ticks - current_ticks);
			current_ticks = SDL_GetTicks();
		}

		if (current_ticks - last_ticks >= 1000) {
			fps = 0;
			last_ticks = SDL_GetTicks();
		}
	}

	void update() {
		numFrames++;
		float display_fps = ( numFrames/(float)(SDL_GetTicks() - startTime) )*1000;
		//printf("%f\n", display_fps);
		if (numFrames >= (100.0 / ((double)60 / FPS_MAX))) {
			startTime = SDL_GetTicks();
			numFrames = 0;
		}
	}
};

int main(int argc, char* argv[]) {
	SDL_Window *window;
	SDL_Surface *surface;
	unsigned int entity = -1;
	int send_router_fd[2];
	int rcv_router_fd[2];
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
	//map_init(world, "assets/Graphics/map/map_01/map01.txt", "assets/Graphics/map/map_01/map01_tiles.txt");
	//entity = create_player(world, 600, 600, true, COLLISION_HACKER);
						
	//world->mask[entity] |= COMPONENT_ANIMATION;
	
	//load_animation("assets/Graphics/player/robber/rob_animation.txt", world, entity);
	
	FPS fps;
	fps.init();

	while (running)
	{
		
		//INPUT
		KeyInputSystem(world, &running);
		MouseInputSystem(world, &entity, &running);
		movement_system(world, send_router_fd[WRITE_END]);
		if (entity < MAX_ENTITIES) {
			map_render(surface, world, entity);
		}
		animation_system(world, &entity, send_router_fd, rcv_router_fd);
		render_player_system(*world, surface);
		
		////NETWORK CODE
		if(network_ready)
		{
			client_update_system(world, rcv_router_fd[READ_END]);
		}
		////NETWORK CODE

		SDL_UpdateWindowSurface(window);
		
		fps.limit();
		fps.update();
	}
	
	cleanup_sound();
	
	return 0;
}




