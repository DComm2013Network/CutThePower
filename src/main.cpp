#include <SDL2/SDL.h>
#include "systems.h"
#include "sound.h"
#include "world.h"
#include "Input/menu.h"
#include "Graphics/text.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#define FPS_MAX 60

bool running;
unsigned int player_entity;
int send_router_fd[2];
int rcv_router_fd[2];
int game_net_signalfd;

int network_ready = 0;
int send_ready = 0;
int game_ready = 0;


/*SAM**************************/
extern void render_fog_of_war	( SDL_Surface *surface, FowComponent *fow );
extern void init_fog_of_war  	( FowComponent **fow );
extern void cleanup_fog_of_war( FowComponent  *fow );
/******************************/


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

	create_pipe(send_router_fd);
	create_pipe(rcv_router_fd);

	World *world = (World*)malloc(sizeof(World));
	printf("Current World size: %i\n", sizeof(World));
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	
	window = SDL_CreateWindow("Cut The Power", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
	
	if (window == NULL) {
		printf("Error initializing the window.\n");
		return 1;
	}
	surface = SDL_GetWindowSurface(window);
	
	
	init_sound();
	init_fonts();
	
	init_world(world);
	srand(time(NULL));//random initializer
	
	KeyMapInit("assets/Input/keymap.txt");
	init_render_player_system();

	
	create_main_menu(world);
	
	FPS fps;
	fps.init();

	running = true;
	player_entity = -1;
	
	
	
	/*SAM********************************/
	FowComponent *fow;
	
	init_fog_of_war(&fow);
	/************************************/
	


	while (running)
	{
		
		//INPUT
		KeyInputSystem(world);
		MouseInputSystem(world);
		movement_system(world, send_router_fd[WRITE_END]);
		if (player_entity < MAX_ENTITIES) {
			map_render(surface, world, player_entity);
			//send_system(world, send_router_fd[WRITE_END]);
		}
				
		/*SAM****************/
		render_fog_of_war(surface, fow);
		/*****************/

		animation_system(world);

		render_player_system(*world, surface, fow);


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
	
	
	cleanup_fog_of_war(fow);
	cleanup_map();
	cleanup_sound();
	cleanup_fonts();
	
	destroy_world(world);
	free(world);
	
	//SDLNet_Quit();
	SDL_Quit();
	
	printf("Exiting The Game\n");
	
	return 0;
}




