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

#define L_EMPTY		1
#define L_WALL		2

#define COLLISION_MASK (COMPONENT_COLLISION)
#define LEVEL_MASK (COMPONENT_LEVEL)

#define CLEAR_VIS	0
#define TRANSP_VIS 	1
#define OPAQUE_VIS 	2
		

#define OPAQUE_FOG_COLOUR 0x000000
#define TRANSP_FOG_COLOUR 0x221122

#define TOP2LEFT2 	10
#define TOP2LEFT 	11
#define TOPLEFT2 	12

#define BOT2LEFT2 	13
#define BOT2LEFT	14
#define BOTLEFT2 	15

#define TOP2RGHT2 	16
#define TOP2RGHT 	17
#define TOPRGHT2 	18

#define BOT2RGHT2 	19
#define BOT2RGHT 	20
#define BOTRGHT2 	21


extern SDL_Rect map_rect;

int subOne(int n);
int addOne(int n);

int set_visibility_type(FowPlayerPosition *fowp, int yDel, int xDel, int newvis);
int is_wall_collision(World *world, PositionComponent newposition);

PositionComponent set_newposition(PositionComponent *pos, int yDel, int xDel);

int fogOfWarHeight;
int fogOfWarWidth;

extern int level;
extern teamNo_t player_team;

/**
 * Blits a fog of war "corner" tile to the larger window surface.
 * 
 * The tile surface to blit is defined in corners[].
 * The relative (NOT absolute in x and y coords !) position of the tile is defined in the tile map.
 *
 * Revisions:
 *     None.
 *loading
 * @param surface 	The window surface.
 * @param fow   		A pointer to a fogOfWarStruct. Includes the corners[] array and the two-dimensional tile map.
 * @param tileRect  A pointer to an SDL rect. Defines the tile's position and dimensions.
 * @param y					The y coordinate of the tile in the tile map.
 * @param x					The x coordinate of the tile in the tile map.
 * #param i					The index of the tile surface in the corners array.
 *
 * @return void.
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date March 29, 2014
 */
void blit_tile(SDL_Surface *surface, FowComponent *fow, SDL_Rect *tileRect, int y, int x, int i)
{
	SDL_BlitSurface(fow -> corners[i], NULL, surface, tileRect);
	fow -> tiles[y][x].visible[ level ] = TRANSP_VIS;
}



/**
 * Renders all fog of war tiles to the window surface:
 * 
 * Loops through the tile map to blit all the tiles that need blitting.
 *
 * Revisions:
 *     None.
 *loading
 * @param surface 	The window surface.
 * @param fow   		A pointer to a fogOfWarStruct. Contains a reference to the tile's visibility, i.e. whether or not it should be blitted.
 *
 * @return void.
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date March 29, 2014
 */
void render_fog_of_war_system(SDL_Surface *surface, FowComponent *fow)
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

			if(fow -> teamNo == COPS)
			{
				switch(visible)
				{
					case CLEAR_VIS:	fow -> tiles[y][x].visible[ level ] = TRANSP_VIS;	break;
					case OPAQUE_VIS: SDL_BlitSurface(fow -> alphaFog[count++], NULL, surface, &tileRect); break;
					case TRANSP_VIS: SDL_BlitSurface(fow -> alphaFog[count++], NULL, surface, &tileRect); break;
				}
			}
			
			else if(fow -> teamNo == ROBBERS)
			{
				switch(visible)
				{
					case CLEAR_VIS:	fow -> tiles[y][x].visible[ level ] = TRANSP_VIS;	break;
					case OPAQUE_VIS: SDL_BlitSurface(fow -> fogOfWar[count++], NULL, surface, &tileRect); break;
					case TRANSP_VIS: SDL_BlitSurface(fow -> alphaFog[count++], NULL, surface, &tileRect); break;
				}
			}
			

			if(fow -> teamNo == ROBBERS || fow -> teamNo == COPS)
			{
				switch(visible)
				{		
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
					case 22: blit_tile(surface, fow, &tileRect, y, x, 12); break;
					case 23: blit_tile(surface, fow, &tileRect, y, x, 13); break;
					case 24: blit_tile(surface, fow, &tileRect, y, x, 14); break;
					case 25: blit_tile(surface, fow, &tileRect, y, x, 15); break;
					case 26: blit_tile(surface, fow, &tileRect, y, x, 16); break;
					case 27: blit_tile(surface, fow, &tileRect, y, x, 17); break;
				}
			}
		}
	}
}



/**
 * Allocates memory for each component of the fogOfWarStruct.
 * 
 * Sets the default positions in absolute coords of all fog tiles.
 * Sets the visibility of each fog tile at each level to opaque.
 *
 * Initializes opaque fog surfaces
 * Initializes transparent fog surfaces
 * Initializes corner-vision fog surfaces
 *
 * Revisions:
 *     None.
 *loading
 * @param fow   		A double pointer to a null fogOfWarStruct which is to be initialized
 *
 * @return void.
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date March 29, 2014
 */
void init_fog_of_war_system(FowComponent **fow)
{
		fogOfWarWidth = 64;  // screen width (tiles/screen)
		fogOfWarHeight = 38; // screen height (tiles/screen)

		int const TOTALTILESX = 2560;
		int const TOTALTILESY = 1520;
		

		(*fow) = (FowComponent*)malloc(sizeof(FowComponent));


		// fog of war tile map
		(*fow) -> tiles = (FowTile**)malloc(sizeof(FowTile*) * TOTALTILESY);

		for(int y = 0; y < TOTALTILESY; y++)
		{
			(*fow) -> tiles[y] = (FowTile*)malloc(sizeof(FowTile) * TOTALTILESX);

			for(int x = 0; x < TOTALTILESX; x++)
			{
				for(int lev = 0; lev < NUMLEVELS; lev++)
				{
					(*fow) -> tiles[y][x].visible[ lev ] = OPAQUE_VIS;
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
		{	
			(*fow) -> alphaFog[ i ] = SDL_CreateRGBSurface(0, TILE_WIDTH, TILE_HEIGHT, 32, 0, 0, 0, 0);
			SDL_FillRect((*fow) -> alphaFog[ i ], 0, TRANSP_FOG_COLOUR);
			
			
			SDL_SetSurfaceBlendMode((*fow)->alphaFog[ i ], SDL_BLENDMODE_BLEND);
			SDL_SetSurfaceAlphaMod ((*fow)->alphaFog[ i ] , 102);


			(*fow) -> fogOfWar[ i ] = SDL_CreateRGBSurface(0, TILE_WIDTH, TILE_HEIGHT, 32, 0, 0, 0, 0);
			SDL_FillRect((*fow) -> fogOfWar[ i ], 0, OPAQUE_FOG_COLOUR);

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
		(*fow)->corners[12] = IMG_Load("assets/Graphics/fow/intersects/intersect_1.png");
		(*fow)->corners[13] = IMG_Load("assets/Graphics/fow/intersects/intersect_2.png");
		(*fow)->corners[14] = IMG_Load("assets/Graphics/fow/intersects/intersect_3.png");
		(*fow)->corners[15] = IMG_Load("assets/Graphics/fow/intersects/intersect_4.png");
		(*fow)->corners[16] = IMG_Load("assets/Graphics/fow/intersects/intersect_5.png");
		(*fow)->corners[17] = IMG_Load("assets/Graphics/fow/intersects/intersect_6.png");

}



/**
 * Frees the fow struct and its components' allocated heap memories
 *
 * Revisions:
 *     None.
 *loading
 * @param fow   		A double pointer to a null fogOfWarStruct which is to be freed
 *
 * @return void.
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date March 29, 2014
 */
void cleanup_fog_of_war(FowComponent *fow)
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





/**
 * Makes the tiles surrounding the player (by a radius of two) visible.
 *
 * Handles walls by removing the line of sight behind them. 
 *
 * Revisions:
 *     None.
 *loading
 * @param fow   		A double pointer to a null fogOfWarStruct which contains the tile map
 *
 * @return void.
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date March 29, 2014
 */
void make_surrounding_tiles_visible(FowPlayerPosition *fowp)
{
	fowp -> fow -> xOffset = -map_rect.x;
	fowp -> fow -> yOffset = -map_rect.y;
	

	#define LEFT -1
	#define RGHT +1
	#define TOP  -1
	#define BOT  +1

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


	if( ! (t = set_visibility_type(fowp, TOP,  0, CLEAR_VIS))) {

		set_visibility_type(fowp, TOP, 0, CLEAR_VIS);

		PositionComponent newposition;
		newposition = set_newposition(fowp -> pos, TOP, LEFT);
		
		if(is_wall_collision(fowp -> world, newposition) == COLLISION_WALL) set_visibility_type(fowp, TOP,  LEFT, CLEAR_VIS);
		
		newposition = set_newposition(fowp -> pos, TOP, RGHT);
		if(is_wall_collision(fowp -> world, newposition) == COLLISION_WALL) set_visibility_type(fowp, TOP,  RGHT, CLEAR_VIS);
	
		ttl = 0;
		ttr = 0;
	}
	else {
		if( set_visibility_type(fowp, TOP + TOP,  0, CLEAR_VIS))
			set_visibility_type(fowp, TOP + TOP + TOP, 0, TRANSP_VIS);
	}
	
	
	if((b = set_visibility_type(fowp, BOT,  0, CLEAR_VIS))) {	
		if(set_visibility_type(fowp, BOT + BOT,  0, CLEAR_VIS))
			set_visibility_type(fowp, BOT + BOT + BOT, 0, TRANSP_VIS);
	}
	else {
		bbl = 0;
		bbr = 0;
	}


	if((l = set_visibility_type(fowp, 0,  LEFT, CLEAR_VIS))) {
		if(set_visibility_type(fowp, 0, LEFT + LEFT, CLEAR_VIS))
			set_visibility_type(fowp, 0, LEFT + LEFT + LEFT, TRANSP_VIS);
	}
	else {	
		llt = 0;
		llb = 0;
	}
	
	
	if((r = set_visibility_type(fowp, 0, RGHT, CLEAR_VIS))) {
		if(set_visibility_type(fowp, 0, RGHT + RGHT, CLEAR_VIS))
			set_visibility_type(fowp, 0, RGHT + RGHT + RGHT, TRANSP_VIS);
	}
	else {	
		rrt = 0;
		rrb = 0;
	}
	
	
	if(t && l && set_visibility_type(fowp, TOP, LEFT, CLEAR_VIS)) {
	
		if( l && t )
			set_visibility_type(fowp, TOP + TOP, LEFT + LEFT, TOP2LEFT2);
		
		if( ttl )	
			set_visibility_type(fowp, TOP + TOP, LEFT, TOP2LEFT);
				
		if( llt ) 
			set_visibility_type(fowp, TOP, LEFT + LEFT, TOPLEFT2);

	}
			
	
	if(b && l && set_visibility_type(fowp, BOT, LEFT, CLEAR_VIS)) {
	
		if( l && b )
			set_visibility_type(fowp, BOT + BOT, LEFT + LEFT, BOT2LEFT2);
		
		if( bbl ) {
			if(set_visibility_type(fowp, BOT + BOT, LEFT, BOT2LEFT))
				set_visibility_type(fowp, BOT + BOT + BOT, LEFT, TRANSP_VIS);			
		}
		if( llb ) {
			if(set_visibility_type(fowp, BOT, LEFT + LEFT, BOTLEFT2))
				set_visibility_type(fowp, BOT, LEFT + LEFT + LEFT, TRANSP_VIS);
		}
	}
			
			
	if(t && r && set_visibility_type(fowp, TOP, RGHT, CLEAR_VIS)) {
	
		if( r && t )
			set_visibility_type(fowp, TOP + TOP, RGHT + RGHT, TOP2RGHT2);
		
		if( ttr ) 
			if( set_visibility_type(fowp, TOP + TOP, RGHT, TOP2RGHT))
					set_visibility_type(fowp, TOP + TOP + TOP, RGHT, TRANSP_VIS);
					
		if( rrt ) 
			if( set_visibility_type(fowp, TOP, RGHT + RGHT, TOPRGHT2))
					set_visibility_type(fowp, TOP, RGHT + RGHT + RGHT, TRANSP_VIS);
	}


	if(b && r && set_visibility_type(fowp, BOT, RGHT, CLEAR_VIS)) {
	
		if( r && b )
			set_visibility_type(fowp, BOT + BOT, RGHT + RGHT, BOT2RGHT2);
		
		if( bbr ) 
			if( set_visibility_type(fowp, BOT + BOT, RGHT, BOT2RGHT))
				set_visibility_type(fowp, BOT + BOT + BOT, RGHT, TRANSP_VIS);
				
		if( rrb ) 
			if( set_visibility_type(fowp, BOT, RGHT + RGHT, BOTRGHT2))
				set_visibility_type(fowp, BOT, RGHT + RGHT + RGHT, TRANSP_VIS);
	}
}



/**
 * Sets the relative tile position (relative to other tiles)
 *
 * Revisions:
 *     None.
 *loading
 * @param pos   		The relative position along the x or y axis
 * @param delta			The needed offset
 *
 * @return void.
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date March 29, 2014
 */
int set_tile_pos(int pos, int delta)
{
	int absd = (delta > 0) ? delta : (-1) * delta;
	
	for(int i = 0; i < absd; i++)
		pos = (delta < 0) ? subOne(pos) : addOne(pos);
		
	return pos;
}


/**
 * Fills a PositionComponent struct with a tile position that will be checked for wall collision
 *
 * Revisions:
 *     None.
 *loading
 * @param pos   		The relative position along the x or y axis
 * @param yDel			The offset along y axis
 * @param xDel			The offset along x axis
 *
 * @return PositionComponent. The filled-in struct
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date March 29, 2014
 */
PositionComponent set_newposition(PositionComponent *pos, int yDel, int xDel)
{
	PositionComponent newposition;


	int xPos = pos->x / TILE_WIDTH;
	int yPos = pos->y / TILE_HEIGHT;
	
	int y = set_tile_pos(yPos, yDel);
	int x = set_tile_pos(xPos, xDel);
	
	
	newposition.x 	   = x * TILE_WIDTH;
	newposition.y 	   = y * TILE_HEIGHT;
	newposition.height = pos -> height;
	newposition.width  = pos -> width;
	newposition.level  = pos -> level;

	return newposition;
}



/**
 * Gets the current visibility of a tile around the player's position -- e.g. opaque, transparent, etc.
 *
 * Revisions:
 *     None.
 *loading
 * @param newposition	A pointer to a PositionComponent struct which will hold the new position
 * @param fow   			A double pointer to a fogOfWarStruct which contains the player's current position
 * @param yDel				Offset along y axis
 * @param xDel				Offset along x axis
 *
 * @return void.
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date March 29, 2014
 */
int *get_visibility_type(PositionComponent *newposition, FowPlayerPosition *fowp, int yDel, int xDel)
{
	FowComponent *fow = fowp -> fow;
	PositionComponent *pos = fowp -> pos;


	int xPos = pos->x / TILE_WIDTH;
	int yPos = pos->y / TILE_HEIGHT;
	
	int y = set_tile_pos(yPos, yDel);
	int x = set_tile_pos(xPos, xDel);

	*newposition =  set_newposition(pos, yDel, xDel);

	return &fow -> tiles[y][x].visible[ pos->level ];
}


/**
 * Checks a tile's visibility type to see if it's a corner tile.
 *
 * Revisions:
 *     None.
 *loading
 * @param n		a tile's visibility type
 *
 * @return int. 1 if corner. 0 otherwise.
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date March 29, 2014
 */
int iscorner(int n)
{
	int arr[] = { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21 };
	for(int i = 0; i < 12; i++) if(arr[i] == n) return 1;
	
	return 0;
}

/**
 * Sets a tile's visibility.
 *
 * Revisions:
 *     None.
 *loading
 * @param fow   			A double pointer to a null fogOfWarStruct
 * @param yDel				The y offset (from the player's position) of the tile
 * @param xDel				The x offset (from the player's position) of the tile
 * @param visibility	The new tile visibility
 *
 * @return void.
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date March 29, 2014
 */
int set_visibility_type(FowPlayerPosition *fowp, int yDel, int xDel, int newvis)
{

	World *world = fowp -> world;
	PositionComponent newposition;

	int *vis = get_visibility_type(&newposition, fowp, yDel, xDel);

	if(*vis == 0 || newvis == 0) {
		*vis = CLEAR_VIS;
	}
	
	else if(iscorner(*vis) && iscorner(newvis) ) {

		if     ((*vis == BOT2RGHT2 && newvis == TOP2LEFT2) || (*vis == TOP2LEFT2 && newvis == BOT2RGHT2)) *vis = 22;
		else if((*vis == BOT2LEFT2 && newvis == TOP2RGHT2) || (*vis == TOP2RGHT2 && newvis == BOT2LEFT2)) *vis = 23;
		else if((*vis == TOP2LEFT2 && newvis == TOP2RGHT2) || (*vis == TOP2RGHT2 && newvis == TOP2LEFT2)) *vis = 24;
		else if((*vis == BOT2RGHT2 && newvis == BOT2LEFT2) || (*vis == BOT2LEFT2 && newvis == BOT2RGHT2)) *vis = 25;
		else if((*vis == BOT2LEFT2 && newvis == TOP2LEFT2) || (*vis == TOP2LEFT2 && newvis == TOP2LEFT2)) *vis = 26;
		else if((*vis == BOT2RGHT2 && newvis == TOP2RGHT2) || (*vis == TOP2RGHT2 && newvis == BOT2RGHT2)) *vis = 27;
		else *vis = CLEAR_VIS;

	}
	else if(iscorner(*vis) && newvis == 1) {

	}
	
	else if(*vis == 1 && iscorner(newvis)) {
		*vis = newvis;
	}
	
	else {					
		*vis = newvis;
	}
	
	if( is_wall_collision(world, newposition) == COLLISION_WALL ) { 
		*vis = TRANSP_VIS;
		return 0; 
	}
	return 1;
}



/**
 * Determines whether a tile is a wall tile
 *
 * Revisions:
 *     None.
 *loading
 * @param world 			The world struct.
 * @param newposition	Tile position.
 *
 * @return void.
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date March 29, 2014
 */
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



/**
 * Adds one to a value without going outside map bounds
 *
 * Sets the visibility of each fog tile at each level to be shown.
 *
 * Revisions:
 *     None.
 *loading
 * @param n				A value.
 *
 * @return void.
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date March 29, 2014
 */
int addOne(int n)
{
	if(n == 1519) return n;
	return ++n;
}



/**
 * Subtracts one from a value without going outside map bounds (below zero)
 *
 * Revisions:
 *     None.
 *loading
 * @param n   		A value.
 *
 * @return void.
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date March 29, 2014
 */
int subOne(int n)
{
	if (n == 0) return n;
	return --n;
}
