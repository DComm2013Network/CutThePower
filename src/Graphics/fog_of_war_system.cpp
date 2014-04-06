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


extern SDL_Rect map_rect;

int subOne(int n);
int addOne(int n);

int set_visibility_type(FowPlayerPosition *fowp, int yDel, int xDel, int newvis);
int is_wall_collision(World *world, PositionComponent newposition);
PositionComponent set_newposition(PositionComponent *pos, int yDel, int xDel);

int fogOfWarHeight = 64;
int fogOfWarWidth = 32;

extern int level;
extern teamNo_t player_team;




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
	
	for(int i = 0; i < NUMSPEECH; i++)
		Mix_FreeChunk(fow -> copSpeech.speech[i]);
}


/**
 * Resets all fog of war tiles to opaque
 * 
 * Loops through the tile map to reset the visibility level
 *
 * Revisions:
 *     None.
 *loading
 * @param fow   		A pointer to a fogOfWarStruct. Contains a reference to the tile's visibility, i.e. whether or not it should be blitted.
 *
 * @return void.
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date March 29, 2014
 */
void reset_fog_of_war(FowComponent *fow)
{

	int const TOTALTILESX = 2560;
	int const TOTALTILESY = 1520;

	for(int y = 0; y < TOTALTILESY; y++)
	{
		for(int x = 0; x < TOTALTILESX; x++)
		{
			for(int lev = 0; lev < NUMLEVELS; lev++)
			{
				fow -> tiles[y][x].visible[ lev ] = OPAQUE_VIS;			
			}
		}	
	}
}


/**
 * Checks whether a near tile is a wall
 * 
 * Loops through the tile map to blit all the tiles that need blitting.
 *
 * Revisions:
 *     None.
 *loading
 * @param fow   		A pointer to a fogOfWarStruct. Contains a reference to the tile's visibility, i.e. whether or not it should be blitted.
 * @param	yd				Delta y from current position
 * @param xd				Delta x from current position
 *
 * @return int. 0 on wall collision. 1 otherwise
 * 
 * @designer Sam Youssef
 * @author Sam Youssef
 *
 * @date March 29, 2014
 */
int get_visibility_of_near_wall(FowPlayerPosition *fowp, int yd, int xd)
{
		PositionComponent newposition;
		
		newposition = set_newposition(fowp -> pos, yd, xd);
		
		if(is_wall_collision(fowp -> world, newposition) == COLLISION_WALL) return false;
		return true;
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
	

	#define LFT -1
	#define RGT +1
	#define TOP -1
	#define BOT +1

	#define LFT3 LFT + LFT + LFT
	#define RGT3 RGT + RGT + RGT
	#define BOT3 BOT + BOT + BOT
	#define TOP3 TOP + TOP + TOP

	#define BOT2 BOT + BOT
	#define TOP2 TOP + TOP
	#define LFT2 LFT + LFT
	#define RGT2 RGT + RGT
	
	set_visibility_type(fowp, 0, 0, 0);


	int l;
	int r;
	int b;
	int t;
	


	if((t = set_visibility_type(fowp, TOP,  0, CLEAR_VIS))) {	
		if(set_visibility_type   (fowp, TOP2, 0, CLEAR_VIS))
		{
			set_visibility_type(fowp, TOP3, 0, CLEAR_VIS);
			
			if(get_visibility_of_near_wall(fowp, 0, LFT))
			{			
				set_visibility_type(fowp, TOP3, LFT, CLEAR_VIS);	
				set_visibility_type(fowp, TOP2, LFT, CLEAR_VIS);	
			}
			
			if(get_visibility_of_near_wall(fowp, 0, RGT))	
			{
				set_visibility_type(fowp, TOP3, RGT, CLEAR_VIS);		
				set_visibility_type(fowp, TOP2, RGT, CLEAR_VIS);
			}				
		}
	}
	
	
	if((b = set_visibility_type(fowp, BOT,  0, CLEAR_VIS))) {	
		if(set_visibility_type   (fowp, BOT2, 0, CLEAR_VIS))
		{
			set_visibility_type(fowp, BOT3, 0, CLEAR_VIS);
			
			if(get_visibility_of_near_wall(fowp, 0, LFT))
			{			
				set_visibility_type(fowp, BOT3, LFT, CLEAR_VIS);	
				set_visibility_type(fowp, BOT2, LFT, CLEAR_VIS);	
			}
			
			if(get_visibility_of_near_wall(fowp, 0, RGT))	
			{
				set_visibility_type(fowp, BOT3, RGT, CLEAR_VIS);		
				set_visibility_type(fowp, BOT2, RGT, CLEAR_VIS);
			}				
		}
	}


	if((l = set_visibility_type(fowp, 0, LFT,  CLEAR_VIS))) {
		if(set_visibility_type	 (fowp, 0, LFT2, CLEAR_VIS))
		{
			
			set_visibility_type (fowp, 0, LFT3, CLEAR_VIS);
				
			if(	get_visibility_of_near_wall(fowp, BOT, 0))
			{
				set_visibility_type (fowp, BOT, LFT2, CLEAR_VIS);	
				set_visibility_type (fowp, BOT, LFT3, CLEAR_VIS);
			}		
			if(	get_visibility_of_near_wall(fowp, TOP, 0))
			{
				set_visibility_type (fowp, TOP, LFT2, CLEAR_VIS);	
				set_visibility_type (fowp, TOP, LFT3, CLEAR_VIS);						
			}
		}
	}
	
	
	if((r = set_visibility_type(fowp, 0, RGT,  CLEAR_VIS))) {
		if(set_visibility_type	 (fowp, 0, RGT2, CLEAR_VIS))
		{
			set_visibility_type (fowp, 0, RGT3, CLEAR_VIS);
			
			if(	get_visibility_of_near_wall(fowp, TOP, 0))
			{
				set_visibility_type (fowp, TOP, RGT3, CLEAR_VIS);
				set_visibility_type (fowp, TOP, RGT2, CLEAR_VIS);
			}
			
			if(	get_visibility_of_near_wall(fowp, BOT, 0))
			{
				set_visibility_type (fowp, BOT, RGT3, CLEAR_VIS);				
				set_visibility_type (fowp, BOT, RGT2, CLEAR_VIS);						
			}
		}
	}
	
	
	if(t && l && set_visibility_type(fowp, TOP, LFT, CLEAR_VIS)) {
	
		if( l && t ) {
			if(get_visibility_of_near_wall(fowp, TOP2, LFT) &&	
				 get_visibility_of_near_wall(fowp, TOP2, 0  ) &&
				 get_visibility_of_near_wall(fowp, TOP,  0  ) &&
			   set_visibility_type(fowp, TOP2, LFT2, CLEAR_VIS)) {
			   
				if(get_visibility_of_near_wall(fowp, 0, LFT2)) {
					set_visibility_type(fowp, TOP, LFT3, CLEAR_VIS);
				}	
			}
		}
	}
						
			
	if(t && r && set_visibility_type(fowp, TOP, RGT, CLEAR_VIS)) {
	
		if( r && t )
		{
			if(get_visibility_of_near_wall(fowp, TOP2, RGT) &&
		 		 get_visibility_of_near_wall(fowp, TOP2, 0  ) &&
 		 		 get_visibility_of_near_wall(fowp, TOP,  0  ) &&
				 set_visibility_type(fowp, TOP2, RGT2, CLEAR_VIS)) {
				
				if(get_visibility_of_near_wall(fowp, 0, RGT2)) {
					set_visibility_type(fowp, TOP, RGT3, CLEAR_VIS);
				}
			}
		}
	}
	
	
	if(b && l && set_visibility_type(fowp, BOT, LFT, CLEAR_VIS)) {
	
		if( l && b ) 
		{
			if(get_visibility_of_near_wall(fowp, BOT2, LFT) &&	
				 get_visibility_of_near_wall(fowp, BOT2, 0  ) &&	
				 get_visibility_of_near_wall(fowp, BOT,  0  ) &&
			   set_visibility_type(fowp, BOT2, LFT2, CLEAR_VIS)) {
			   
				if(get_visibility_of_near_wall(fowp, 0, LFT2)) {
					set_visibility_type(fowp, BOT, LFT3, CLEAR_VIS);
				}				
			}
		}
	}



	if(b && r && set_visibility_type(fowp, BOT, RGT, CLEAR_VIS)) {
	
		if( r && b )
		{
			if( get_visibility_of_near_wall(fowp, BOT2, RGT) &&		
					get_visibility_of_near_wall(fowp, BOT2, 0  ) &&	
					get_visibility_of_near_wall(fowp, BOT , 0  ) &&	
					set_visibility_type(fowp, BOT2, RGT2, CLEAR_VIS)) {
			
				if(get_visibility_of_near_wall(fowp, 0, RGT2)) {
					set_visibility_type(fowp, BOT, RGT3, CLEAR_VIS);
				}
			}		
		}
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
 * Sets a PositionComponent struct with the desired position
 * 
 * Copies from current position and applies delta-x & delta-y to get new position.
 *
 * Revisions:
 *     None.
 *loading
 * @param pos 	PositionComponent struct with current position information
 * @param	yDel	Delta y from current position
 * @param xDel	Delta x from current position
 *
 * @return void.
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
	
	if( fowp -> isControllablePlayer ) { 
		int count = fow -> tilesVisibleToControllablePlayerCount;
		
		fow -> tilesVisibleToControllablePlayer[ count ][0] = y;
		fow -> tilesVisibleToControllablePlayer[ count ][1] = x;	
		fow -> tilesVisibleToControllablePlayerCount++;
	}
	
	return &fow -> tiles[y][x].visible[ pos->level ];
}


/**
 * Sets a tile's level of visibility.
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

	if( is_wall_collision(world, newposition) == COLLISION_WALL ) 
	{ 
		*vis = TRANSP_VIS;
		return 0; 
	}

	if(*vis == 0 || newvis == 0)
		*vis = CLEAR_VIS;
	
	else
		*vis = newvis;
	
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
