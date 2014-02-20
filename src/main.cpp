#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
//#include "systems.h"
#include "world.h"

bool init(SDL_Window* gWindow, SDL_Renderer* gRenderer);

int main(void) 
{
	SDL_Window* gWindow 		= NULL; //The window we'll be rendering to
	SDL_Renderer* gRenderer 	= NULL;	//The window renderer

	init(gWindow, gRenderer);
	
}

bool init(SDL_Window* gWindow, SDL_Renderer* gRenderer){
	bool success = true; //Initialization flag
	
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	} else {
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) //Set texture filtering to linear
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}
		
		if ((gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN )) == NULL)
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		} else {
			
			if((gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED ))== NULL){
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			} else{
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				
				int imgFlags = IMG_INIT_PNG|IMG_INIT_JPG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}
	
	return success;
}
