/**
 *  Description
 * 	@file map.cpp
 **/
 
 
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "world.h"

int level;
SDL_Surface *map_surface;
SDL_Rect map_rect;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: map_init
--
-- DATE: February 26, 2014
--
-- REVISIONS: 
--
-- DESIGNER: Jordan Marling/Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: int map_init(char *file_map, char *file_tiles)
--              char *file_map: the pathway for the map
--				char *file_tiles: pathway for the tiles
--
-- RETURNS: 	returns 0 upon success
--			
--
-- NOTES:
-- Initiates the map by loading the tiles and putting it into one large texture.
-- Tiles are being loaded from the array. 
--
------------------------------------------------------------------------------------------------------------------*/
int map_init(World* world, char *file_map, char *file_tiles) {
	
	FILE *fp_map;
	FILE *fp_tiles;
	
	int width, height;
	int x, y, i, a;
	uint8_t** map;
	
	char entity_type[64];
	int entity_count;
	
	SDL_Surface **tiles;
	int num_tiles;
	int pos;
	char tile_filename[64];
	
	SDL_Rect tile_rect;
	
	
	if ((fp_map = fopen(file_map, "r")) == 0) {
		printf("Error opening map %s\n", file_map);
		return -1;
	}
	
	if (fscanf(fp_map, "%d %d %d", &level, &width, &height) != 3) {
		printf("Error getting map information: %s\n", file_map);
		return -1;
	}
	
	printf("Map level %d: %dx%d = %d tiles\n", level, width, height, width * height);
	
	if ((map = (uint8_t**)malloc(sizeof(uint8_t*) * width)) == NULL) {
		printf("malloc failed\n");
	}
	for (int i = 0; i < width; i++) {
		if ((map[i] = (uint8_t*)malloc(sizeof(uint8_t) * height)) == NULL) {
			printf("malloc failed\n");
		}
	}
	
	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			if (fscanf(fp_map, "%ud", &map[x][y]) != 1) {
				printf("Expected more map.\n");
				return -1;
			}
		}
	}
	
	if (fscanf(fp_map, "%d", &entity_count) == 1) {
		
		for(i = 0; i < entity_count; i++) {
			
			if (fscanf(fp_map, "%s", &entity_type) != 1) {
				printf("Entity type error: %s\n", file_map);
				return -1;
			}
			
			//printf("Found entity: %s\n", entity_type);
			
			if (strcmp(entity_type, "stair") == 0) { //stair
				//TODO: Create the stairs properly.
				fscanf(fp_map, "%d %d %d %d %d", &a, &a, &a, &a, &a);
			}
			else {
				printf("Did not deal with the entity type: %s\n", entity_type);
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
			
			SDL_BlitSurface(tiles[map[x][y]], NULL, map_surface, &tile_rect);
			
		}
	}
	
	map_rect.x = 0;
	map_rect.y = 0;
	map_rect.w = width * TILE_WIDTH;
	map_rect.h = height * TILE_HEIGHT;
	
	create_level(world, map, width, height, TILE_WIDTH);
	for (int i = 0; i < width; i++) {
		free(map[i]);
	}
	free(map);
	return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: map_render
--
-- DATE: February 26, 2014
--
-- REVISIONS: 
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: void map_render(SDL_Surface *surface
--              SDL_Surface *surface: the surface for window.
--
-- RETURNS: 
--
-- NOTES:
-- Add the surface to the main surface of the window. Fills the remaining space with a pink color to ensure that
-- there are no errors in tiles. If so, pink background will show.
--
------------------------------------------------------------------------------------------------------------------*/
void map_render(SDL_Surface *surface) {

	SDL_FillRect(surface, NULL, 0xFF33FF);
	
	SDL_BlitSurface(map_surface, NULL, surface, &map_rect);
	
}
