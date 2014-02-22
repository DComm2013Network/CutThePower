#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "map.h"

#if 0

SDL_Surface *map_surface;
SDL_Rect map_rect;

int map_init(char *file_map, char *file_tiles) {
	
	FILE *fp_map;
	FILE *fp_tiles;
	
	int *map;
	int width, height;
	int x, y, i;
	
	SDL_Surface **tiles;
	int num_tiles;
	int pos;
	char tile_filename[64];
	
	SDL_Rect tile_rect;
	
	if ((fp_map = fopen(file_map, "r")) == 0) {
		printf("Error opening map %s\n", file_map);
		return -1;
	}
	
	if (fscanf(fp_map, "%d %d", &width, &height) != 2) {
		return -1;
	}
	
	printf("Map size: %dx%d = %d tiles\n", width, height, width * height);
	
	map = (int*)malloc(sizeof(int) * width * height);
	
	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			if (fscanf(fp_map, "%d", &map[x + (y * width)]) != 1) {
				printf("Expected more map.\n");
				return -1;
			}
		}
	}
	
	fclose(fp_map);
	
	//load tiles
	if ((fp_tiles = fopen(file_tiles, "r")) == 0) {
		printf("Error opening tile set %s\n", file_tiles);
		return -1;
	}
	
	if (fscanf(fp_tiles, "%d", &num_tiles) != 1) {
		return -1;
	}
	tiles = (SDL_Surface**)malloc(sizeof(SDL_Surface*) * num_tiles);
	
	for(i = 0; i < num_tiles; i++) {
		
		if (fscanf(fp_tiles, "%d", &pos) != 1) {
			printf("Error reading tile map index.\n");
			return -1;
		}
		
		if (fscanf(fp_tiles, "%s", (char*)&tile_filename) != 1) {
			printf("Error reading tile map filename.\n");
			return -1;
		}
		
		tiles[pos] = SDL_LoadBMP(tile_filename);
		
		if (tiles[pos] == 0) {
			printf("Error loading tile: %s\n", tile_filename);
			return -1;
		}
		
	}
	
	fclose(fp_tiles);
	
	//render to surface
	
	map_surface = SDL_CreateRGBSurface(0, width * TILE_WIDTH, height * TILE_HEIGHT, 32, 0, 0, 0, 0);
	
	tile_rect.w = TILE_WIDTH;
	tile_rect.h = TILE_HEIGHT;
	
	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			
			tile_rect.x = x * TILE_WIDTH;
			tile_rect.y = y * TILE_HEIGHT;
			
			
			SDL_BlitSurface(tiles[map[x + (width * y)]], NULL, map_surface, &tile_rect);
			
		}
	}
	
	map_rect.x = 0;
	map_rect.y = 0;
	map_rect.w = width * TILE_WIDTH;
	map_rect.h = height * TILE_HEIGHT;
	
	return 0;
}

void map_render(SDL_Surface *surface) {

	SDL_FillRect(surface, NULL, 0xFF33FF);
	
	SDL_BlitSurface(map_surface, NULL, surface, &map_rect);
	
}

#else

int *map;
int width;
int height;

SDL_Surface **tiles;

int map_init(char *file_map, char *file_tiles) {
	
	FILE *fp_map;
	FILE *fp_tiles;
	
	int x, y, i;
	
	int num_tiles;
	int pos;
	char tile_filename[64];
	
	//load map
	if ((fp_map = fopen(file_map, "r")) == 0) {
		printf("Error opening map %s\n", file_map);
		return -1;
	}
	
	if (fscanf(fp_map, "%d %d", &width, &height) != 2) {
		return -1;
	}
	
	printf("Map size: %dx%d = %d tiles\n", width, height, width * height);
	
	map = (int*)malloc(sizeof(int) * width * height);
	
	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			if (fscanf(fp_map, "%d", &map[x + (y * width)]) != 1) {
				printf("Expected more map.\n");
				return -1;
			}	
		}
	}
	
	fclose(fp_map);
	
	//load tiles
	if ((fp_tiles = fopen(file_tiles, "r")) == 0) {
		printf("Error opening tile set %s\n", file_tiles);
		return -1;
	}
	
	if (fscanf(fp_tiles, "%d", &num_tiles) != 1) {
		return -1;
	}
	
	tiles = (SDL_Surface**)malloc(sizeof(SDL_Surface*) * num_tiles);
	
	for(i = 0; i < num_tiles; i++) {
		
		if (fscanf(fp_tiles, "%d", &pos) != 1) {
			printf("Error reading tile map index.\n");
			return -1;
		}
		
		if (fscanf(fp_tiles, "%s", (char*)&tile_filename) != 1) {
			printf("Error reading tile map filename.\n");
			return -1;
		}
		
		tiles[pos] = SDL_LoadBMP(tile_filename);
		
		if (tiles[pos] == 0) {
			printf("Error loading tile: %s\n", tile_filename);
			return -1;
		}
	}
	
	fclose(fp_tiles);
	
	return 0;
}

void map_render(SDL_Surface *surface) {
	
	int x, y;
	SDL_Rect tile_rect;
	
	tile_rect.w = TILE_WIDTH;
	tile_rect.h = TILE_HEIGHT;
	
	//If there are not enough tiles vs. Screen size, put a random color up
	//SDL_FillRect(surface, NULL, 0xFF33FF);
	
	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			
			tile_rect.x = x * TILE_WIDTH;
			tile_rect.y = y * TILE_HEIGHT;
			
			SDL_BlitSurface(tiles[map[x + (width * y)]], NULL, surface, &tile_rect);
		}
	}
	
}

#endif
