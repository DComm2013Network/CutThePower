#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "map.h"
#include "world.h"
#include "systems.h"

int main(int argc, char* argv[]) 
{
	SDL_Window *gWindow		= NULL;
	SDL_Surface *gSurface	= NULL;
	SDL_Event e;
	
	World world;
	unsigned int player;
	
	int running = 1;
	
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		return 1;
	} else {
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) //Set texture filtering to linear
		{
			printf( "Warning: Linear texture filtering not enabled!" );
			return 1;
		}
		if ((gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN )) == NULL)
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			return 1;
		} else {
			if((gSurface = SDL_GetWindowSurface(gWindow)) == NULL){
				printf( "Surface could not be created! SDL Error: %s\n", SDL_GetError() );
				return 1;
			} else{
				if (map_init("assets/SampleFloor.txt", "assets/tiles.txt") != 0) {
					printf("Error loading map.\n");
					return 1;
				}
			}
		}
	}
	player = create_entity(world);
	world.mask[player] = COMPONENT_RENDER_PLAYER;


	/*
	 * POSSIBLY TEMPORARY!!! - These two variables may be temporary -i.e. for the first milestone. The playerRect struct defines the position of a player. The playerFilename declares the player's starting texture.
	 * We need the gameplay people to supply at least the playerRect struct.
	 */
	SDL_Rect playerRect [ 2 ]; 
	char* playerFilename[ 2 ] = { "assets/dot.bmp", "assets/dot_2.bmp" };	

	for(int i = 0; i < 2; i++)
	{					
		playerRect[i].x = rand() % 400;
		playerRect[i].y = rand() % 400;
		playerRect[i].w = rand() % 400;
		playerRect[i].h = rand() % 400;
	}

	
	while (running)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				running = 0;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					running = 0;
				}
			}
		}

		map_render(gSurface);
		
		for(int i = 0; i < 2; i++)
		{
			/* POSSIBLY TEMPORARY!!! passing playerFilename may not be needed if gameplay gives us a complete player struct. */
			render_player_system(world, gSurface, &playerRect[i], playerFilename[i]);
		}
		SDL_UpdateWindowSurface(gWindow);
	}
	
	return 0;
}


