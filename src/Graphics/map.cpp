/** @ingroup Graphics */
/** @{ */
/**
 *  Description
 * 	@file map.cpp
 **/
/** @} */

 

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "systems.h"
#include "../sound.h"


SDL_Surface *map_surface = 0; /**< The surface on which to render the map. */
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
 * 	   - March  24th - Tim Kim: Corrected the code to free
 *loading
 * @param[out] world      The world struct in which to store the map.
 * @param[in]  file_map   The pathway for the map.
 * @param[in]  file_tiles Pathway for the tiles.
 * @return 0 on success, -1 on failure.
 * 
 * @designer Jordan Marling
 * @designer Mat Siwoski
 * 
 * @author Mat Siwoski
 * @author Tim Kim
 * 
 * @date February 26, 2014
 */

int map_init(World* world, const char *file_map, const char *file_tiles) {
	
	FILE *fp_map;
	FILE *fp_tiles;
	
	int width, height;
	int x, y, i;
	int **collision_map;
	int **map;

	char *entity_type = (char*)malloc(sizeof(char) * 128);
	int entity_count;
	
	
	SDL_Surface **tiles;
	int *collision;
	int num_tiles;
	int pos = 0;
	char *tile_filename = (char*)malloc(sizeof(char) * 128);
	
	SDL_Rect tile_rect;
	
	if (map_surface != 0) {
		SDL_FreeSurface(map_surface);
	}
	
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
	
	printf("Map load size %d %d\n", width, height);
	
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
			
			if (strcmp(entity_type, "stair") == 0 || strcmp(entity_type, "stairs") == 0) { //stairs
				
				//stair x y targetX targetY 2
				int x, y, floor;
				float targetX, targetY;
				char dir;

				if (fscanf(fp_map, "%d %d %f %f %d %c", &x, &y, &targetX, &targetY, &floor, &dir) != 6) {
					printf("Error loading stair\n");
					return -1;
				}
				switch (dir) { // make the hitboxes for the stairs
					case 'l':
						create_stair(world, floor, targetX * TILE_WIDTH + TILE_WIDTH / 2, targetY * TILE_HEIGHT + TILE_HEIGHT / 2, x * TILE_WIDTH + TILE_WIDTH / 2 - 5, y * TILE_HEIGHT + TILE_HEIGHT / 2, 4, 4, level);
						create_block(world, x * TILE_WIDTH + 7, y * TILE_HEIGHT + TILE_HEIGHT / 2, 10, TILE_HEIGHT - 4, floor);
						break;
					case 'r':
						create_stair(world, floor, targetX * TILE_WIDTH + TILE_WIDTH / 2, targetY * TILE_HEIGHT + TILE_HEIGHT / 2, x * TILE_WIDTH + TILE_WIDTH / 2 + 5, y * TILE_HEIGHT + TILE_HEIGHT / 2, 4, 4, level);
						create_block(world, x * TILE_WIDTH + TILE_WIDTH - 7, y * TILE_HEIGHT + TILE_HEIGHT / 2, 10, TILE_HEIGHT - 4, floor);
						break;
					case 'u':
						create_stair(world, floor, targetX * TILE_WIDTH + TILE_WIDTH / 2, targetY * TILE_HEIGHT + TILE_HEIGHT / 2, x * TILE_WIDTH + TILE_WIDTH / 2, y * TILE_HEIGHT + TILE_HEIGHT / 2 - 5, 4, 4, level);
						create_block(world, x * TILE_WIDTH + TILE_WIDTH / 2, y * TILE_HEIGHT + 7, TILE_WIDTH - 4, 10, floor);
						break;
					case 'd':
						create_stair(world, floor, targetX * TILE_WIDTH + TILE_WIDTH / 2, targetY * TILE_HEIGHT + TILE_HEIGHT / 2, x * TILE_WIDTH + TILE_WIDTH / 2, y * TILE_HEIGHT + TILE_HEIGHT / 2 + 5, 4, 4, level);
						create_block(world, x * TILE_WIDTH + TILE_WIDTH / 2, y * TILE_HEIGHT + TILE_HEIGHT - 7, TILE_WIDTH - 4, 10, floor);
						break;
				}
			}
			else if (strcmp(entity_type, "object") == 0) { //animated objects
				
				unsigned int entity;
				float x, y;
				int w, h;
				char *animation_name = (char*)malloc(sizeof(char) * 64);
				char *animation_filename = (char*)malloc(sizeof(char) * 64);
				
				if (fscanf(fp_map, "%f %f %d %d %s %s", &x, &y, &w, &h, animation_filename, animation_name) != 6) {
					printf("Error loading object!\n");
					return -1;
				}
				
				entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_ANIMATION | COMPONENT_COLLISION);
				
				//printf("Loading object %d (%f, %f) [%s] %s\n", entity, x, y, animation_name, animation_filename);
				
				world->position[entity].x = x * TILE_WIDTH + TILE_WIDTH / 2;
				world->position[entity].y = y * TILE_HEIGHT + TILE_HEIGHT / 2;
				
				world->position[entity].width = w;
				world->position[entity].height = h;
				
				world->renderPlayer[entity].width = w;
				world->renderPlayer[entity].height = h;
				
				world->collision[entity].id = 0;
				world->collision[entity].type = COLLISION_SOLID;
				world->collision[entity].timer = 0;
				world->collision[entity].timerMax = 0;
				world->collision[entity].active = true;
				world->collision[entity].radius = 0;
				
				load_animation(animation_filename, world, entity);
				play_animation(world, entity, animation_name);
				
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
				
				//printf("Playing sound %d\n", sound_id);
				
			}
			else if (strcmp(entity_type, "objective") == 0) {
				
				float x, y;
				int w, h;
				unsigned int id, entity = -1;
				char *animation_filename = (char*)malloc(sizeof(char) * 64);
				
				if (fscanf(fp_map, "%f %f %d %d %u %s", &x, &y, &w, &h, &id, animation_filename) != 6) {
					printf("Error loading objective!\n");
					return -1;
				}
				
				entity = create_objective(world, x * TILE_WIDTH + TILE_WIDTH / 2, y * TILE_HEIGHT + TILE_HEIGHT / 2, w, h, id, level);
				
				if (entity >= MAX_ENTITIES) {
					printf("exceeded max entities.\n");
					return -1;
				}
				
				world->mask[entity] |= COMPONENT_ANIMATION | COMPONENT_RENDER_PLAYER;
				
				world->renderPlayer[entity].width = w;
				world->renderPlayer[entity].height = h;
				
				load_animation(animation_filename, world, entity);
				play_animation(world, entity, "not_captured");
				
				//printf("Loaded objective: %u\n", entity);
				
				free(animation_filename);
			}
			else if (strcmp(entity_type, "powerup") == 0) {
				float x, y;
				int w, h, type;
				unsigned int entity = -1;
				char *animation_filename = (char*)malloc(sizeof(char) * 128);
				
				if (fscanf(fp_map, "%f %f %d %d %d %s", &x, &y, &w, &h, &type, animation_filename) != 6) {
					printf("Error loading powerup!\n");
					return -1;
				}
				entity = create_powerup(world, x * TILE_WIDTH + TILE_WIDTH / 2, y * TILE_HEIGHT + TILE_HEIGHT / 2, w, h, type, level);
				
				if (entity >= MAX_ENTITIES) {
					printf("exceeded max entities.\n");
					return -1;
				}
				world->mask[entity] |= COMPONENT_ANIMATION | COMPONENT_RENDER_PLAYER;
				world->renderPlayer[entity].width = w;
				world->renderPlayer[entity].height = h;
				
				load_animation(animation_filename, world, entity);
				play_animation(world, entity, "bounce");
				
				free(animation_filename);
			}
			else if (strcmp(entity_type, "chair") == 0) { //animated objects
				
				unsigned int entity;
				float x, y;
				int w, h;
				char *animation_name = (char*)malloc(sizeof(char) * 64);
				char *animation_filename = (char*)malloc(sizeof(char) * 64);
				
				if (fscanf(fp_map, "%f %f %d %d %s %s", &x, &y, &w, &h, animation_filename, animation_name) != 6) {
					printf("Error loading chair!\n");
					return -1;
				}
				
				entity = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_ANIMATION);
				
				//printf("Loading object %d (%f, %f) [%s] %s\n", entity, x, y, animation_name, animation_filename);
				
				world->position[entity].x = x * TILE_WIDTH;
				world->position[entity].y = y * TILE_HEIGHT;
				
				world->position[entity].width = w;
				world->position[entity].height = h;
				
				world->renderPlayer[entity].width = w;
				world->renderPlayer[entity].height = h;
				
				load_animation(animation_filename, world, entity);
				play_animation(world, entity, animation_name);
				
				free(animation_name);
				free(animation_filename);
				
			}
			else {
				printf("Did not deal with the entity type: %s\n", entity_type);
				break;
			}
		}
	}
	
	fclose(fp_map);
	//cleanup_map();

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
	
	for(i = 0; i < num_tiles; i++) {
		SDL_FreeSurface(tiles[i]);
	}
	
	map_rect.x = 0;
	map_rect.y = 0;
	w = map_rect.w = width * TILE_WIDTH;
	h = map_rect.h = height * TILE_HEIGHT;
	
	
	create_level(world, collision_map, width, height, TILE_WIDTH, level);

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

	return 0;
}

void cleanup_map() {
	
	if (map_surface != 0) {
		SDL_FreeSurface(map_surface);
	}
	
}

/**
 * Add the surface to the main surface of the window.
 * 
 * Fills the remaining space with a pink (black) color to ensure that
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
 * @designer Robin Hsieh
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
	
	SDL_FillRect(surface, NULL, 0x000000);
	
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

