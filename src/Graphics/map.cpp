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
#include "systems.h"
#include "../sound.h"

#define MAP_SECTION_WIDTH 25
#define MAP_SECTION_HEIGHT 25

SDL_Surface *map_surface; /**< The surface on which to render the map. */
SDL_Rect map_rect;        /**< The rectangle containing the map. */
int w;                    /**< The map's width. */
int h;                    /**< The map's height. */
int level;                /**< The current floor. */


void load_map_section(int **map, SDL_Surface **tiles, int startX, int startY, int map_width, int map_height, SDL_Surface **map_surface);

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
	//uint8_t** map;
	int **collision_map;
	int **map;
	//int **collision_map;
	
	//char entity_type[64];
	char *entity_type = (char*)malloc(sizeof(char) * 128);
	int entity_count;
	
	SDL_Surface **tiles;
	int *collision;
	int num_tiles;
	int pos = 0;
	//char tile_filename[64];
	char *tile_filename = (char*)malloc(sizeof(char) * 128);
	
	SDL_Rect tile_rect;
	
	//load tiles
	if ((fp_tiles = fopen(file_tiles, "r")) == 0) {
		printf("Error opening tile set %s\n", file_tiles);
		return -1;
	}
	
	if (fscanf(fp_tiles, "%d", &num_tiles) != 1) {
		printf("Cannot find tile number\n");
		return -1;
	}
	
	if ((tiles = (SDL_Surface**)malloc(sizeof(SDL_Surface*) * num_tiles)) == 0) {
		printf("Error mallocing tile surfaces\n");
		return -1;
	}
	if ((collision = (int*)malloc(sizeof(int) * num_tiles)) == 0) {
		printf("Error mallocing tile surfaces\n");
		return -1;
	}
	
	for(i = 0; i < num_tiles; i++) {
		
		if (fscanf(fp_tiles, "%d", &pos) != 1) {
			printf("Error reading tile map index.\n");
			return -1;
		}
		
		if (pos >= num_tiles) {
			printf("Trying to write to tiles outside of memory\n");
			return -1;
		}
		
		if (fscanf(fp_tiles, "%s", (char*)tile_filename) != 1) {
			printf("Error reading tile map filename.\n");
			return -1;
		}
		if (fscanf(fp_tiles, "%d", &collision[pos]) != 1){
			printf("Error reading tile map collision.\n");
			return -1;	
		}
		
		tiles[pos] = IMG_Load(tile_filename);
		
		if (tiles[pos] == NULL) {
			printf("Error loading tile: %s\n", tile_filename);
			return -1;
		}
		
	}
	
	fclose(fp_tiles);
	
	//LOAD MAP
	
	if ((fp_map = fopen(file_map, "r")) == 0) {
		printf("Error opening map %s\n", file_map);
		return -1;
	}
	
	if (fscanf(fp_map, "%d %d %d", &level, &width, &height) != 3) {
		return -1;
	}
	
	if ((map = (int**)malloc(sizeof(int*) * width)) == NULL) {
		printf("malloc failed\n");
	}
	if ((collision_map = (int**)malloc(sizeof(int*) * width)) == NULL) {
		printf("malloc failed\n");
	}
	
	for (i = 0; i < width; i++) {
		if ((map[i] = (int*)malloc(sizeof(int) * height)) == NULL) {
			printf("malloc failed\n");
		}
		if ((collision_map[i] = (int*)malloc(sizeof(int) * height)) == NULL) {
			printf("malloc failed\n");
		}
	}
	
	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			
			if (fscanf(fp_map, "%d", &map[x][y]) != 1) {
				printf("Expected more map.\n");
				return -1;
			}
			
			if (map[x][y] >= num_tiles) {
				printf("Using tile %u that is bigger than %d\n", map[x][y], num_tiles);
			}
			
			collision_map[x][y] = collision[map[x][y]];
		}
	}
	
	if (fscanf(fp_map, "%d", &entity_count) == 1) {
		
		for(i = 0; i < entity_count; i++) {
			
			if (fscanf(fp_map, "%s", (char*)entity_type) != 1) {
				printf("Entity type error: %s\n", file_map);
				return -1;
			}
			
			//printf("Found entity: %s\n", entity_type);
			
			if (strcmp(entity_type, "stair") == 0) { //stair
				fscanf(fp_map, "%d %d %d %d %d", &a, &a, &a, &a, &a);
			}
			else if (strcmp(entity_type, "object") == 0) { //animated objects
				
				unsigned int entity;
				int x, y;
				char *animation_name = (char*)malloc(sizeof(char) * 64);
				char *animation_filename = (char*)malloc(sizeof(char) * 64);
				
				if (fscanf(fp_map, "%d %d %s %s", &x, &y, animation_filename, animation_name) != 4) {
					printf("Error loading object!\n");
					return -1;
				}
				
				printf("Loading object [%s] %s\n", animation_name, animation_filename);
				
				
				entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_ANIMATION | COMPONENT_COLLISION);
				
				world->position[entity].x = TILE_WIDTH * x;
				world->position[entity].y = TILE_HEIGHT * y;
				
				world->position[entity].width = TILE_WIDTH;
				world->position[entity].height = TILE_HEIGHT;
				
				world->renderPlayer[entity].width = TILE_WIDTH;
				world->renderPlayer[entity].height = TILE_HEIGHT;
				
				world->collision[entity].type = COLLISION_SOLID;
				world->collision[entity].active = true;
				world->collision[entity].radius = 1;
				
				load_animation(animation_filename, world, entity);
				play_animation(&(world->animation[entity]), animation_name);
				
				free(animation_name);
				free(animation_filename);
				
			}
			else if (strcmp(entity_type, "sound") == 0) {
				
				int sound_id;
				
				if (fscanf(fp_map, "%d", &sound_id) != 1) {
					printf("Error loading sound!\n");
					return -1;
				}
				
				play_music(sound_id);
				
				printf("Playing sound %d\n", sound_id);
				
			}
			else {
				printf("Did not deal with the entity type: %s\n", entity_type);
				break;
				//return -1;
			}
		}
	}
	
	fclose(fp_map);
	
	
	//render to surface
	
	map_surface = SDL_CreateRGBSurface(0, width * TILE_WIDTH, height * TILE_HEIGHT, 32, 0, 0, 0, 0);
	
	if (map_surface == 0) {
		printf("error making map surface.\n");
		return -1;
	}
	
	SDL_FillRect(map_surface, NULL, 0xFF0000);
	
	tile_rect.w = TILE_WIDTH;
	tile_rect.h = TILE_HEIGHT;
	
	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			
			tile_rect.x = x * TILE_WIDTH;
			tile_rect.y = y * TILE_HEIGHT;
			
			SDL_BlitSurface(tiles[map[x][y]], NULL, map_surface, &tile_rect);
			
		}
	}
	
	//load_map_section(map, tiles, 0, 0, width * TILE_WIDTH, height * TILE_HEIGHT, &map_surface);
	
	for(i = 0; i < num_tiles; i++) {
		SDL_FreeSurface(tiles[i]);
	}
	
	map_rect.x = 0;
	map_rect.y = 0;
	w = map_rect.w = width * TILE_WIDTH;
	h = map_rect.h = height * TILE_HEIGHT;
	
	
	
	//create_level(world, collision_map, width, height, TILE_WIDTH);
	
	for (i = 0; i < width; i++) {
		free(map[i]);
		free(collision_map[i]);
	}
	free(map);
	free(collision_map);
	
	
	free(tiles);
	free(collision);
	
	free(entity_type);
	free(tile_filename);
	
	/*for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			for (int n = 0; n < num_tiles; n++) {
				if (tileTypes[n] == map[x][y]) {
					map[x][y] = collisionTypes[n];
					break;
				}
			}
		}
	}*/
	
	create_level(world, collision_map, width, height, TILE_WIDTH);
	
	//free(tileTypes);
	//free(collisionTypes);

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
	
	if (map_surface == 0) {
		printf("Map surface isn't initialized!\n");
		return;
	}
	
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



void load_map_section(int **map, SDL_Surface **tiles, int startX, int startY, int map_width, int map_height, SDL_Surface **map_surface) {
	
	int x, y;
	SDL_Rect tile_rect;
	
	//cap the size to make sure the surface is not too big.
	if (startX + MAP_SECTION_WIDTH < map_width) {
		map_width = startX + MAP_SECTION_WIDTH;
	}
	if (startY + MAP_SECTION_HEIGHT < map_height) {
		map_height = startY + MAP_SECTION_HEIGHT;
	}
	
	printf("Making map sized: %dx%d\n", map_width - startX, map_height - startY);
	
	*map_surface = 0;
	
	//create a new surface with the desired size.
	*map_surface = SDL_CreateRGBSurface(0, (map_width - startX) * TILE_WIDTH, (map_height - startY) * TILE_HEIGHT, 32, 0, 0, 0, 0);

	printf("Made surface!\n");
	printf("Size: %dx%d\n", map_width * TILE_WIDTH, map_height * TILE_HEIGHT);

	if (*map_surface == 0) {
		printf("error making map surface.\n");
		return;
	}

	tile_rect.w = TILE_WIDTH;
	tile_rect.h = TILE_HEIGHT;
	
	
	printf("filling!\n");
	SDL_FillRect(*map_surface, NULL, 0xFF0000);
	printf("filled!\n");
	
	for(y = startY; y < map_height; y++) {
		for(x = startX; x < map_width; x++) {
			
			tile_rect.x = (x * TILE_WIDTH) - startX;
			tile_rect.y = (y * TILE_HEIGHT) - startY;
			
			SDL_BlitSurface(tiles[map[x][y]], NULL, *map_surface, &tile_rect);
			
		}
	}
	
}


