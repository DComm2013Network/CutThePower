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

#include "map.h"
#include "systems.h"
#include "../sound.h"


/*SAM******************************************************/
int fogOfWarHeight, fogOfWarWidth;
/**********************************************************/


SDL_Surface *map_surface = 0; /**< The surface on which to render the map. */
SDL_Rect map_rect;        /**< The rectangle containing the map. */
int w;                    /**< The map's width. */
int h;                    /**< The map's height. */
int level;                /**< The current floor. */



/*SAM*************************************************************************/
void blit_tile(SDL_Surface *surface, struct fogOfWarStruct *fow, SDL_Rect *tileRect, int y, int x, int i)
{
	SDL_BlitSurface(fow -> corners[i], NULL, surface, tileRect);
	fow -> tiles[y][x].visible[ level - 1 ] = 2;
}

void render_fog_of_war(SDL_Surface *surface, struct fogOfWarStruct *fow)
{
	int xOffset = fow -> xOffset;
	int yOffset = fow -> yOffset;

	
	int count = 0;
	
	for(int y = 0; y < fogOfWarHeight; y++)
	{
		for(int x = 0; x < fogOfWarWidth; x++)
		{
			int visible = fow -> tiles[y][x].visible[ level - 1  ];

			SDL_Rect tileRect;
			tileRect.x = fow -> tiles[y][x].rect.x - xOffset;
			tileRect.y = fow -> tiles[y][x].rect.y - yOffset;
				
			tileRect.w = TILE_WIDTH;
			tileRect.h = TILE_HEIGHT;

			switch(visible)
			{
				case 0:	fow -> tiles[y][x].visible[ level - 1 ] = 2;	break;
				
				case 1: SDL_BlitSurface(fow -> fogOfWar[count++], NULL, surface, &tileRect); break;
				case 2: SDL_BlitSurface(fow -> alphaFog[count++], NULL, surface, &tileRect); break;
			
				case 10: blit_tile(surface, fow, &tileRect, y, x, 0); break;
				case 11: blit_tile(surface, fow, &tileRect, y, x, 1); break;					
				case 12: blit_tile(surface, fow, &tileRect, y, x, 2); break;	
				case 13: blit_tile(surface, fow, &tileRect, y, x, 3); break;
				case 14: blit_tile(surface, fow, &tileRect, y, x, 4); break;
				case 15: blit_tile(surface, fow, &tileRect, y, x, 5); break;
				case 16: blit_tile(surface, fow, &tileRect, y, x, 6); break;
				case 17: blit_tile(surface, fow, &tileRect, y, x, 7); break;
				case 18: blit_tile(surface, fow, &tileRect, y, x, 8); break;
				case 19: blit_tile(surface, fow, &tileRect, y, x, 9); break;
				case 20: blit_tile(surface, fow, &tileRect, y, x, 10); break;
				case 21: blit_tile(surface, fow, &tileRect, y, x, 11); break;
			}
		}
	}
}


void init_fog_of_war(struct fogOfWarStruct **fow)
{
		fogOfWarWidth = 64;  // screen width (tiles/screen)
		fogOfWarHeight = 38; // screen height (tiles/screen)

		int const TOTALTILESX = 2560;
		int const TOTALTILESY = 1520;
		

		(*fow) = (struct fogOfWarStruct*)malloc(sizeof(struct fogOfWarStruct));


		// fog of war tile map
		(*fow) -> tiles = (struct fowtile**)malloc(sizeof(struct fowtile*) * TOTALTILESY);

		for(int y = 0; y < TOTALTILESY; y++)
		{
			(*fow) -> tiles[y] = (struct fowtile*)malloc(sizeof(struct fowtile) * TOTALTILESX);

			for(int x = 0; x < TOTALTILESX; x++)
			{
				for(int lev = 0; lev < NUMLEVELS; lev++)
				{
					(*fow) -> tiles[y][x].visible[ lev ] = 1;
					(*fow) -> tiles[y][x].rect.x = (x * TILE_WIDTH );
					(*fow) -> tiles[y][x].rect.y = (y * TILE_HEIGHT);
					(*fow) -> tiles[y][x].rect.w = ( TILE_WIDTH  );
					(*fow) -> tiles[y][x].rect.h = ( TILE_HEIGHT );				
				}
			}	
		}
	
	
		// array of surfaces
		(*fow) -> fogOfWar = (SDL_Surface**)malloc(sizeof(SDL_Surface*) * fogOfWarWidth * fogOfWarHeight);
		(*fow) -> alphaFog = (SDL_Surface**)malloc(sizeof(SDL_Surface*) * fogOfWarWidth * fogOfWarHeight);
			
		for(int i = 0; i < (fogOfWarHeight * fogOfWarWidth); i++)
		{	//libpng-devel-1.5.13-2
			(*fow) -> alphaFog[ i ] = SDL_CreateRGBSurface(0, TILE_WIDTH, TILE_HEIGHT, 32, 0, 0, 0, 0);
			SDL_FillRect((*fow) -> alphaFog[ i ],0,0x221122);
			SDL_SetSurfaceBlendMode((*fow)->alphaFog[ i ], SDL_BLENDMODE_BLEND);
			SDL_SetSurfaceAlphaMod ((*fow)->alphaFog[ i ] , 110);


			(*fow) -> fogOfWar[ i ] = SDL_CreateRGBSurface(0, TILE_WIDTH, TILE_HEIGHT, 32, 0, 0, 0, 0);
			SDL_FillRect((*fow) -> fogOfWar[ i ],0,0x221122);

		}
		
		(*fow)->corners[0] = IMG_Load("assets/Graphics/fow/topleft/head.png");
		(*fow)->corners[1] = IMG_Load("assets/Graphics/fow/topleft/right.png");
		(*fow)->corners[2] = IMG_Load("assets/Graphics/fow/topleft/left.png");
			
		(*fow)->corners[3] = IMG_Load("assets/Graphics/fow/botleft/head.png");
		(*fow)->corners[4] = IMG_Load("assets/Graphics/fow/botleft/right.png");
		(*fow)->corners[5] = IMG_Load("assets/Graphics/fow/botleft/left.png");
			
		(*fow)->corners[6] = IMG_Load("assets/Graphics/fow/topright/head.png");
		(*fow)->corners[7] = IMG_Load("assets/Graphics/fow/topright/left.png");
		(*fow)->corners[8] = IMG_Load("assets/Graphics/fow/topright/right.png");
		
		(*fow)->corners[9] = IMG_Load("assets/Graphics/fow/botright/head.png");
		(*fow)->corners[10] = IMG_Load("assets/Graphics/fow/botright/left.png");
		(*fow)->corners[11] = IMG_Load("assets/Graphics/fow/botright/right.png");
}

void cleanup_fog_of_war(struct fogOfWarStruct *fow)
{
	for(int i = 0; i < (fogOfWarHeight * fogOfWarWidth); i++)
	{
		SDL_FreeSurface(fow -> fogOfWar[i]);
		SDL_FreeSurface(fow -> alphaFog[i]);
	}
	
	for(int i = 0; i < 12; i++)
	{
		SDL_FreeSurface(fow -> corners[i]);
	}
}
/**************************************************************************/




/**
 * Initiates the map by loading the tiles and putting it into one large texture.
 * 
 * Tiles are loaded from the array.
 *
 * Revisions:
 *     -# March 10th - Jordan Marling: Implemented reading in the file correctly for the Stairs, 
 *    able to now set the location of the stairs & where the stairs will push the player to.
 *loading
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
	int x, y, i;
	//uint8_t** map;
	int **collision_map;
	int **map;
	//int **collision_map;loading
	
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
			
			if (strcmp(entity_type, "stair") == 0) { //stair
				
				//stair x y targetX targetY 2
				int x, y, targetX, targetY, floor;
				
				if (fscanf(fp_map, "%d %d %d %d %d", &x, &y, &targetX, &targetY, &floor) != 5) {
					printf("Error loading stair\n");
					return -1;
				}
				
				printf("mx: %d\n", x * TILE_WIDTH);
				printf("my: %d\n", y * TILE_HEIGHT);
				printf("mw: %d\n", TILE_WIDTH);
				printf("mh: %d\n", TILE_HEIGHT);
				
				create_stair(world, floor, targetX * TILE_WIDTH, targetY * TILE_HEIGHT, x * TILE_WIDTH, y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, level);
				
				//printf("Create stair entity %d\n", entity);
				
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
				
				//printf("Loading object %d [%s] %s\n", entity, animation_name, animation_filename);
				
				world->position[entity].x = x * TILE_WIDTH + TILE_WIDTH / 2;
				world->position[entity].y = y * TILE_HEIGHT + TILE_HEIGHT / 2;
				
				world->position[entity].width = w;
				world->position[entity].height = h;
				
				world->renderPlayer[entity].width = TILE_WIDTH;
				world->renderPlayer[entity].height = TILE_HEIGHT;
				
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
	
	
	
	create_level(world, collision_map, width, height, TILE_WIDTH);
	
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
	
	//create_level(world, collision_map, width, height, TILE_WIDTH);
	
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
