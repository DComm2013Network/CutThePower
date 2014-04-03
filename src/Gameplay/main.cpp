#include <stdio.h>
#include <SDL2/SDL.h>
#include <time.h>

#include "systems.h"
#include "world.h"
#include "level.h"

#define HEIGHT 600
#define WIDTH 800

/*
 * 
 * This is a simple demo that illustrates the use of component based systems. The code in here
 * was hacked up in an hour and should be improved on a lot.
 * In this demo I have created 3 components and 3 systems that do work on the components.
 * 
 * Use the WASD keys to move the square.
 * 
 * An "entity" in the context of this small demo is just an integer. The integer is an index in
 * an array to lookup a group of components that are related. When the components are stored a mask
 * is created for easy lookup of entities inside systems.
 * 
 * Mask explaination:
 * 	(masks are binary)
 * 
 * Each component ID:
 * PositionComponent: 	001
 * RenderComponent:		010
 * InputComponent:		100
 * 
 * If an entity has a position and render component, we OR these together giving the mask:
 * 		011
 * 
 * In the next example an entity has all 3 components, the mask is 111
 * If a system wants to find an entity with a PositionComponent and RenderComponent, It creates a lookup mask
 * of 011. It then performs the lookup:
 * 
 * ((Entity Mask) & (System Lookup Mask)) == (System Lookup Mask)
 * 111 & 011 = 011
 * 011 = 011
 * 
 * This shows that the entity will be worked on by the system.
 * 
 * 
 * 
 * Components:
 * 		A component is just data. There is no functions inside of them. Systems should be the only thing
 * 		that manipulates the data.
 * 
 * Position Component:
 * 		Contains an x coordinate and y coordinate
 * 
 * Render Component:
 * 		Contains the width, height and colour of the square
 * 
 * Input Component:
 * 		The input state, in this case boolean values for up, left, right and down.
 * 
 * 
 * Systems:
 * 		Each system loops through the list of "entities" and checks the mask to see if it has the
 * 		desired components. If it does the system does work on those components.
 * 
 * Input System:
 * 		This system keeps track of all of the input in the game in one place. If the exit button
 * 		on the window is pressed it handles it (In a bad way but this is an example).
 * 		Each input component stores input information relevant to the entity. This can also be used by
 * 		an AI system for example to simulate input information and create AI players.
 * 
 * Movement System:
 * 		This system takes the input from the input component and updates the position components values
 * 		respectively. When the input component shows the "up" key is pressed, the position moves up.
 * 
 * Render System:
 * 		This system renders the entity to the screen at the desired position with the render settings in
 * 		the render component.
 * 
 * In SDL it is set up so there is 1 window that you create, which has a surface that is
 * drawn on it. You can have as many surfaces as you want, and swap them in and out as needed.
 * This is used because in AAA games one surface is being drawn while another is being
 * rendered to for performance. I think we only need to use 1 surface for our simple 2D game.
 * 
 * 
 */
int main() {
	SDL_Window *window;
	SDL_Surface *surface;

	World world;
	bool running = true;
	InputComponent input;
	
	//initialize video drawing
	SDL_Init(SDL_INIT_VIDEO);
	
	window = SDL_CreateWindow("Gameplay", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	surface = SDL_GetWindowSurface(window);
	
	if (window == NULL)	{
		printf("Error initializing the window.\n");
		return 1;
	}
	
	srand(time(NULL));
	
	init_world(world);
	
	int map[MAX_WIDTH][MAX_HEIGHT];
		
	for (int i = 0; i < 40; i++) {
		for (int n = 0; n < 30; n++) {
			map[i][n] = 0;
		}
	}
	for (int i = 0; i < 40; i++) {
		map[i][0] = 1;
		map[i][29] = 1;
	}
	for (int i = 0; i < 30; i++) {
		map[0][i] = 1;
		map[39][i] = 1;
	}
	//This is the creation of the player entity. We should put this into a separate function
	//create_player(world, 55, 55, true);
	
	/*LEVEL(5, 5) = L_WALL;*/
	int level = create_level(world, map, 40, 30, 20);
	
	
	//world.level[level].map[3][6] = L_WALL;
	
	//This is the creation of the player entity. We should put this into a separate function
	create_player(world, rand() % 740 + 20, rand() % 540 + 20, input, true);
	/*for (int i = 0; i < 199; i++) {
		create_player(world, rand() % 740 + 20, rand() % 540 + 20, input, false);
	}*/
	
	//In the game loop we update each system in a logical order.
	while(running) {
		input_system(world, running);
		movement_system(world);
		render_system(world, surface);
		
		SDL_UpdateWindowSurface(window);
	}
	
	SDL_DestroyWindow(window);
	
	SDL_Quit();
	
	return 0;
}
