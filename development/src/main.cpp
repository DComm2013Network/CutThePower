#include <SDL2/SDL.h>
#include <unistd.h>

#include "systems.h"
#include "world.h"

void test(World *world);

int main(int argc, char* argv[]) 
{
	SDL_Window *window;
	SDL_Surface *surface;
	//SDL_Event e;
	
	World world;
	bool running = true;
	unsigned int entity;
	
	
	SDL_Init(SDL_INIT_VIDEO);
	
	window = SDL_CreateWindow("SDL Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	surface = SDL_GetWindowSurface(window);
	
	if (window == NULL)
	{
		printf("Error initializing the window.\n");
		return 1;
	}
	
	init_world(&world);
	
	/*entity = create_box(&world);
	world.render[entity].colour = 0xFF0000;
	world.velocity[entity].x = 1.0f;
	world.velocity[entity].y = 2.0f;*/
	
	entity = create_entity(&world);
	world.mask[entity] = COMPONENT_COMMAND;
	world.command[entity].commands[C_UP] = 0;
	world.command[entity].commands[C_LEFT] = 0;
	world.command[entity].commands[C_RIGHT] = 0;
	world.command[entity].commands[C_DOWN] = 0;
	world.command[entity].commands[C_ACTION] = 0;
	
	while (running)
	{
		/*
		//check if window is closed
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				running = false;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					running = false;
				}
			}
		}*/
		
		
		
		/*velocity_system(&world);
		collision_system(&world);
		render_system(&world, surface);*/
		
		key_input_system(&world, &running);
		mouse_input_system(&world);
		
		
		
		test(&world);
		
		//sleep(1);
		
		SDL_UpdateWindowSurface(window);
	}
	
	
	
	
	return 0;
}

void test(World *world) {
	
	int entity;
	CommandComponent *command;
	
	for(entity = 0; entity < ENTITY_COUNT; entity++) {
		
		if ((world->mask[entity] & COMPONENT_COMMAND) == COMPONENT_COMMAND) {
			
			command = &(world->command[entity]);
			
			if (command->commands[C_UP]) {
				printf("UP\n");
			}
			if (command->commands[C_LEFT]) {
				printf("LEFT\n");
			}
			if (command->commands[C_RIGHT]) {
				printf("RIGHT\n");
			}
			if (command->commands[C_DOWN]) {
				printf("DOWN\n");
			}
			if (command->commands[C_ACTION]) {
				printf("ACTION\n");
			}
			
		}
		if ((world->mask[entity] & COMPONENT_MOUSE) == COMPONENT_MOUSE) {
			
			
			
			
			
		}
		
		
	}
	
	
}







