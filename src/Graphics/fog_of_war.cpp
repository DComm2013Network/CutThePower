/** @ingroup Graphics */
/** @{ */
/**
 *  Description
 * 	@file fog_of_war.cpp
 **/
/** @} */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#include "map.h"

#define L_EMPTY			1
#define L_WALL			2

#define COLLISION_MASK (COMPONENT_COLLISION)
#define LEVEL_MASK (COMPONENT_LEVEL)


extern SDL_Rect map_rect;

int subOne(int n);
int addOne(int n);

int set_visibility_type (struct fogOfWarPlayerPosition *fowp, int yDel, int xDel, int visibility);
int is_wall_collision(World *world, PositionComponent newposition);

int fogOfWarHeight;
int fogOfWarWidth;

extern int level;


void blit_tile(SDL_Surface *surface, struct fogOfWarStruct *fow, SDL_Rect *tileRect, int y, int x, int i)
{
	SDL_BlitSurface(fow -> corners[i], NULL, surface, tileRect);
	fow -> tiles[y][x].visible[ level ] = 2;
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
			int visible = fow -> tiles[y][x].visible[ level  ];

			SDL_Rect tileRect;
			tileRect.x = fow -> tiles[y][x].rect.x - xOffset;
			tileRect.y = fow -> tiles[y][x].rect.y - yOffset;
				
			tileRect.w = TILE_WIDTH;
			tileRect.h = TILE_HEIGHT;

			switch(visible)
			{
				case 0:	fow -> tiles[y][x].visible[ level ] = 2;	break;
				
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
			SDL_FillRect((*fow) -> fogOfWar[ i ],0,0x000000);

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


/*** visibility settings ***/
void make_surrounding_tiles_visible(struct fogOfWarPlayerPosition *fowp)
{
	fowp -> fow -> xOffset = -map_rect.x;
	fowp -> fow -> yOffset = -map_rect.y;
	

	#define LEFT -1
	#define RGHT +1
	#define TOP	 -1
	#define BOT	 +1

	set_visibility_type(fowp, 0, 0, 0);

	int ttl = 1;
	int ttr = 1;
	int llt = 1;
	int rrt = 1;
	
	int bbl = 1;
	int bbr = 1;
	int llb = 1;
	int rrb = 1;

	int l;
	int r;
	int b;
	int t;

	set_visibility_type(fowp, TOP + TOP,  0, 0);

	if( ! (t = set_visibility_type(fowp, TOP,  0, 0))) {
		ttl = 0;
		ttr = 0;
	}
	
	
	if((b = set_visibility_type(fowp, BOT,  0, 0))) {	
		set_visibility_type(fowp, BOT + BOT,  0, 0);
	}
	else {
		bbl = 0;
		bbr = 0;
	}


	if((l = set_visibility_type(fowp, 0,  LEFT, 0))) {
		set_visibility_type(fowp, 0, LEFT + LEFT, 0);
	}
	else {
		llt = 0;
		llb = 0;
	}
	
	
	if((r = set_visibility_type(fowp, 0, RGHT, 0))) {
		set_visibility_type(fowp,  0, RGHT + RGHT, 0);
	}
	else {
		rrt = 0;
		rrb = 0;
	}
	
	
	if(set_visibility_type(fowp, TOP, LEFT, 0)) {
	
		if( l && t )
			set_visibility_type(fowp, TOP + TOP, LEFT + LEFT, 10);
		
		if( ttl )	set_visibility_type(fowp, TOP + TOP, LEFT, 11);
		if( llt ) set_visibility_type(fowp, TOP, LEFT + LEFT, 12);

	}
			
	
	if(set_visibility_type(fowp, BOT, LEFT, 0)) {
	
		if( l && b )
			set_visibility_type(fowp, BOT + BOT, LEFT + LEFT, 13);
		
		if( bbl ) set_visibility_type(fowp, BOT + BOT, LEFT, 14);
		if( llb ) set_visibility_type(fowp, BOT, LEFT + LEFT, 15);
	}
			
			
	if(set_visibility_type(fowp, TOP, RGHT, 0)) {
	
		if( r && t )
			set_visibility_type(fowp, TOP + TOP, RGHT + RGHT, 16);
		
		if( ttr ) set_visibility_type(fowp, TOP + TOP, RGHT, 17);
		if( rrt ) set_visibility_type(fowp, TOP, (RGHT + RGHT), 18);
	}


	if(set_visibility_type(fowp, BOT, RGHT, 0)) {
	
		if( r && b )
			set_visibility_type(fowp, BOT + BOT, RGHT + RGHT, 19);
		
		if( bbr ) set_visibility_type(fowp, BOT + BOT, RGHT, 20);
		if( rrb ) set_visibility_type(fowp, BOT, RGHT + RGHT, 21);
	}
}


int get_tile_pos(int pos, int delta)
{
	int absd = (delta > 0) ? delta : (-1) * delta;
	
	for(int i = 0; i < absd; i++)
		pos = (delta < 0) ? subOne(pos) : addOne(pos);
		
	return pos;
}

int *get_visibility_type(PositionComponent *newposition, struct fogOfWarPlayerPosition *fowp, int yDel, int xDel)
{
	struct fogOfWarStruct *fow 	 = fowp -> fow;
	PositionComponent     *pos 	 = fowp -> pos;

	int xPos = pos->x / TILE_WIDTH;
	int yPos = pos->y / TILE_HEIGHT;
	
	int y = get_tile_pos(yPos, yDel);
	int x = get_tile_pos(xPos, xDel);
	
	newposition->x 	   = x * TILE_WIDTH;
	newposition->y 	   = y * TILE_HEIGHT;
	newposition->height = pos -> height;
	newposition->width  = pos -> width;
	newposition->level  = pos -> level;

	return &fow -> tiles[y][x].visible[ pos->level ];
}

int set_visibility_type(struct fogOfWarPlayerPosition *fowp, int yDel, int xDel, int visibility)
{

	World *world = fowp -> world;
	PositionComponent newposition;

 	(*get_visibility_type(&newposition, fowp, yDel, xDel)) = visibility;
	
	if( is_wall_collision(world, newposition) == COLLISION_WALL ) return 0;
	return 1;
}


int is_wall_collision(World *world, PositionComponent newposition)
{
	int curlevel = -1;
	
	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (((world->mask[i] & LEVEL_MASK) != 0) && world->level[i].levelID == newposition.level) {
			curlevel = i;
			break;
		}
	}
	
	if (curlevel == -1) {
		return false;
	}
	
	int x = newposition.x / world->level[curlevel].tileSize;
	int y = newposition.y / world->level[curlevel].tileSize;
	
	if (world->level[curlevel].map[x][y] == L_WALL) 
	{
			return COLLISION_WALL;
	}
	
	return COLLISION_EMPTY;
}


int addOne(int n)
{
	return ++n;
}

int subOne(int n)
{
	if (n == 0) return n;
	return --n;
}
