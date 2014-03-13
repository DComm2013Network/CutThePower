/** @ingroup Graphics */
/** @{ */
/**
 *  Description
 * 	@file map.cpp
 **/
/** @} */

 
 
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "map.h"

SDL_Surface *map_surface; /**< The surface on which to render the map. */
SDL_Rect map_rect;        /**< The rectangle containing the map. */
int w;                    /**< The map's width. */
int h;                    /**< The map's height. */
int level;                /**< The current floor. */

/**
 * Initiates the map by loading the tiles and putting it into one large texture.
 * 
 * Tiles are loaded from the array.
 *
 * Revisions:
 *     -# March 10th - Jordan Marling: Implemented reading in the file correctly for the Stairs, 
 *    able to now set the location of the stairs & where the stairs will push the player to.
 *
 * @param[out] world      The world struct in which to store the map.
 * @param[in]  file_map   The pathway for the map.
 * @param[in]  file_tiles Pathway for the tiles.
 * @return 0 on success, -1 on failure.
 * 
 * @designer Jordan Marling
 * @designer Mat Siwoski
 * @author Mat Siwoski
 *
 * @date February 26, 2014
 */

int map_init(World* world, char *file_map, char *file_tiles) {
	
	FILE *fp_map;
	FILE *fp_tiles;
	
	int width, height;
	int x, y, i, a;
	uint32_t** map;
	
	char entity_type[64];
	int entity_count;
	
	SDL_Surface **tiles;
	int num_tiles;
	int pos;
	char tile_filename[64];
	int collision;
	int *tileTypes;
	int *collisionTypes;
	
	SDL_Rect tile_rect;
	
	
	if ((fp_map = fopen(file_map, "r")) == 0) {
		printf("Error opening map %s\n", file_map);
		return -1;
	}
	
	if (fscanf(fp_map, "%d %d %d", &level, &width, &height) != 3) {
		return -1;
	}
	
	printf("Map size: %dx%d = %d tiles\n", width, height, width * height);
	
	if ((map = (uint32_t**)malloc(sizeof(uint32_t*) * width)) == NULL) {
		printf("malloc failed\n");
	}
	
	for (int i = 0; i < width; i++) {
		if ((map[i] = (uint32_t*)malloc(sizeof(uint32_t) * height)) == NULL) {
			printf("malloc failed\n");
		}
	}
	
	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			if (fscanf(fp_map, "%u", &map[x][y]) != 1) {
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

	tileTypes = (int*)malloc(sizeof(int) * num_tiles);
	collisionTypes = (int*)malloc(sizeof(int) * num_tiles);
	
	for(i = 0; i < num_tiles; i++) {
		
		if (fscanf(fp_tiles, "%d", &pos) != 1) {
			printf("Error reading tile map index.\n");
			return -1;
		}
		
		if (fscanf(fp_tiles, "%s", (char*)&tile_filename) != 1) {
			printf("Error reading tile map filename.\n");
			return -1;
		}
		if (fscanf(fp_tiles, "%d", &collision) != 1){
			printf("Error reading tile map collision.\n");
			return -1;	
		}
		
		tiles[pos] = SDL_LoadBMP(tile_filename);
		tileTypes[i] = pos;
		collisionTypes[i] = collision;
		
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
	w = map_rect.w = width * TILE_WIDTH;
	h = map_rect.h = height * TILE_HEIGHT;
	
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			for (int n = 0; n < num_tiles; n++) {
				if (tileTypes[n] == map[x][y]) {
					map[x][y] = collisionTypes[n];
					break;
				}
			}
		}
	}
	
	create_level(world, map, width, height, TILE_WIDTH);
	
	for (int i = 0; i < width; i++) {
		free(map[i]);
	}
	free(map);
	free(tileTypes);
	free(collisionTypes);

	return 0;
}

/**
 * Add the surface to the main surface of the window.
 * 
 * Fills the remaining space with a pink color to ensure that
 * there are no errors in tiles. If so, pink background will show.
 * 
 * Revisions:
 *     -# March 6th, 2014 - Added Camera support for the map. 
 *
 * @param[in,out] surface         The surface for the window.
 * @param[in]     playerXPosition The player's x-coordinate.
 * @param[in]     playerYPosition The player's y-coordinate.
 *
 * @designer Mat Siwoski
 * @designer Jordan Marling
 * @author Mat Siwoski
 * @author Jordan Marling
 *
 * @date February 26, 2014
 */
void map_render(SDL_Surface *surface, World *world, unsigned int player_entity) {
	
	SDL_Rect tempRect;
	
	int playerXPosition = world->position[player_entity].x;
	int playerYPosition = world->position[player_entity].y;
	int playerWidth = world->position[player_entity].width;
	int playerHeight = world->position[player_entity].height;
	
	
	
	SDL_FillRect(surface, NULL, 0xFF33FF);
		
	map_rect.x = (WIDTH/2) -( playerXPosition + playerWidth / 2 );
	map_rect.y = (HEIGHT/2) - ( playerYPosition + playerHeight / 2 );
	
	if(map_rect.y + h < HEIGHT){
		map_rect.y = HEIGHT - h;
	}
	if (map_rect.x + w < WIDTH){
		map_rect.x = WIDTH - w ;
	}	
	if( map_rect.x > 0 )
	{ 
		map_rect.x = 0;
	}
	if( map_rect.y > 0 )
	{
		map_rect.y = 0;
	}
	
	if (map_rect.x + w < WIDTH) {
		map_rect.x += (WIDTH / 2) - (w / 2);
	}
	if (map_rect.y + h < HEIGHT) {
		map_rect.y += (HEIGHT / 2) - (h / 2);
	}
	
	
	
	
	tempRect.x = map_rect.x;
	tempRect.y = map_rect.y;
	tempRect.w = map_rect.w;
	tempRect.h = map_rect.h;
	
	SDL_BlitSurface(map_surface, NULL, surface, &tempRect);
	
}
