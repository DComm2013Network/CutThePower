/**
 * [File description]
 *
 * @file world.cpp
 */

#include "world.h"
#include "Gameplay/powerups.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

#include <stdlib.h>
#include <stdio.h>

extern unsigned int background;	//this is here because we need to keep the background loaded while in the menu.
									//this needs to be cleaned up when the destroy_world function is called.
/**
 * This function initializes every mask to be 0, so that there are no components.
 * 
 * @param world The world struct containing the entity masks to be zeroed.
 *
 * @designer
 * @author 
 */
void init_world(World* world) {
	int i;
	for(i = 0; i < MAX_ENTITIES; ++i) {
		world->mask[i] = COMPONENT_EMPTY;
	}
}

/**
 * This function loops through the masks of all the entities and finds the first one
 * that is not used.
 *
 * @param world 		The world struct containing all entities.
 * @param attributes 	The component mask to apply to the entity.
 *
 * @return 	The entity number if there was an entity available, or MAX_ENTITIES if all
 *			of the entities are in use.
 *
 * @designer
 * @author
 */
unsigned int create_entity(World* world, unsigned int attributes) {
	unsigned int entity;
	for(entity = 0; entity < MAX_ENTITIES; entity++) {
		if (world->mask[entity] == COMPONENT_EMPTY) {
			world->mask[entity] = attributes;
			return entity;
		}
	}
	return MAX_ENTITIES;
}

/**
 * [Brief description (one sentence)]
 *
 * [Detailed description (if necessary)]
 *
 * @param world
 * @param map
 * @param width
 * @param height
 * @param tileSize
 *
 * @return
 *
 * @designer
 * @author
 */
unsigned int create_level(World* world, int** map, int width, int height, int tileSize, int floor) {
	
	unsigned int entity = 0;
	int i = 0;
	int n = 0;
	
	entity = create_entity(world, COMPONENT_LEVEL);
	
	world->level[entity].map = (int**)malloc(sizeof(int*) * width);
	for (i = 0; i < width; i++) {
		world->level[entity].map[i] = (int*)malloc(sizeof(int) * height);
		for (n = 0; n < height; n++) {
			
			world->level[entity].map[i][n] = map[i][n];
			
		}
	}
	world->level[entity].levelID = floor;
	world->level[entity].width = width;
	world->level[entity].height = height;
	world->level[entity].tileSize = tileSize;
	
	return entity;
}

/**
 * [Brief description (one sentence)]
 *
 * [Detailed description (if necessary)]
 *
 * @param world
 * @param x
 * @param y
 * @param controllable
 *
 * @return
 *
 * @designer
 * @author
 */
unsigned int create_player(World* world, int x, int y, bool controllable, int collisiontype, int playerNo, PKT_GAME_STATUS *status_update) {
	unsigned int entity;
	PositionComponent pos;
	RenderPlayerComponent render;
	MovementComponent movement;
	ControllableComponent control;
	CommandComponent command;
	CollisionComponent collision;
	PlayerComponent player;
	PowerUpComponent powerup;


	render.width = 40;
	render.height = 40;
	
	pos.x = x;
	pos.y = y;

	pos.width = 30;
	pos.height = 30;
	pos.level = 0;
	
	movement.id = 0;
	movement.lastDirection = 0;
	movement.acceleration = 1.50;
	movement.defMaxSpeed = 8;
	movement.maxSpeed = 8;
	movement.movX = 0;
	movement.movY = 0;
	movement.friction = 0.30;
	
	command.commands[C_UP] = false;
	command.commands[C_DOWN] = false;
	command.commands[C_LEFT] = false;
	command.commands[C_RIGHT] = false;
	
	control.active = true;

	collision.id = 0;
	collision.type = collisiontype;
	collision.timer = 0;
	collision.timerMax = 0;
	collision.active = true;
	collision.radius = 0;

	player.playerNo = playerNo;
	player.teamNo = status_update->otherPlayers_teams[playerNo];
	player.character = status_update->characters[playerNo];
	player.readyStatus = status_update->readystatus[playerNo];
	memcpy(player.name, status_update->otherPlayers_name[playerNo], MAX_NAME);
	player.tilez = 0;
	player.onTile = 0;
	
	powerup.captureTime = 0;
	powerup.duration = 0;
	powerup.type = PU_NONE;

	for(entity = 0; entity < MAX_ENTITIES; ++entity) {
		if (world->mask[entity] == COMPONENT_EMPTY) {
			if (controllable) {
				world->mask[entity] =	COMPONENT_POSITION | 
										COMPONENT_RENDER_PLAYER | 
										COMPONENT_COMMAND | 
										COMPONENT_MOVEMENT | 
										COMPONENT_COLLISION |
										COMPONENT_CONTROLLABLE |
										COMPONENT_PLAYER |
										COMPONENT_ANIMATION |
										COMPONENT_POWERUP;
			} else {
				world->mask[entity] =	COMPONENT_POSITION | 
										COMPONENT_RENDER_PLAYER | 
										COMPONENT_ANIMATION |
										COMPONENT_COLLISION | 
										COMPONENT_MOVEMENT |
										COMPONENT_PLAYER |
										COMPONENT_POWERUP;
			}
			world->position[entity] = pos;
			world->renderPlayer[entity] = render;
			world->command[entity] = command;
			world->movement[entity] = movement;
			world->collision[entity] = collision;
			world->player[entity] = player;
			world->powerup[entity] = powerup;

			if (controllable) {
				world->controllable[entity] = control;
			}
			return entity;
		}
	}
	return MAX_ENTITIES;
}

//creates a target for the hackers
unsigned int create_objective(World* world, float x, float y, int w, int h, int id, int level) {
	unsigned int entity;
	
	entity = create_entity(world, COMPONENT_POSITION | COMPONENT_COLLISION | COMPONENT_OBJECTIVE);
	
	if (entity == MAX_ENTITIES) {
		return MAX_ENTITIES;
	}
	
	world->position[entity].x = x;
	world->position[entity].y = y;
	world->position[entity].width = w;
	world->position[entity].height = h;
	world->position[entity].level = level;
	
	world->collision[entity].id = 0;
	world->collision[entity].type = COLLISION_TARGET;
	world->collision[entity].timer = 0;
	world->collision[entity].timerMax = 0;
	world->collision[entity].active = true;
	world->collision[entity].radius = 0;
	
	world->objective[entity].objectiveID = id;
	world->objective[entity].status = 1;
	
	return entity;
}

/**
 * Creates a stair entity 
 *
 * Creates and sets the parameters necessary for the creation of a stair.
 *
 * @param world
 * @param targetLevel - level the stair will go too.
 * @param targetX - x location on the target level the player will be placed.
 * @param targetY - y location on the target level the player will be placed.
 * @param x - x position of the stair.
 * @param y - y position of the stair.
 * @param width - width of the stair.
 * @param height - height of the stair.
 * @param level - level the stair is on.
 *
 * @return the entity id of the newly created stair
 *
 * @designer JOSH & IAN
 * @author JOSH & IAN
 */
unsigned int create_stair(World* world, int targetLevel, int targetX, int targetY, int x, int y, int width, int height, int level) {
	unsigned int entity;
	
	entity = create_entity(world, COMPONENT_POSITION | COMPONENT_COLLISION | COMPONENT_WORMHOLE);
	
	world->position[entity].x = x;
	world->position[entity].y = y;
	world->position[entity].width = width;
	world->position[entity].height = height;
	world->position[entity].level = level;
	
	world->wormhole[entity].targetLevel = targetLevel;
	world->wormhole[entity].targetX = targetX;
	world->wormhole[entity].targetY = targetY;
	
	world->collision[entity].id = 0;
	world->collision[entity].type = COLLISION_STAIR;
	world->collision[entity].timer = 0;
	world->collision[entity].timerMax = 0;
	world->collision[entity].active = true;
	world->collision[entity].radius = 0;
	
	return entity;
}

unsigned int create_block(World* world, int x, int y, int width, int height, int level) {
	unsigned int entity = create_entity(world, COMPONENT_POSITION | COMPONENT_COLLISION);
	
	world->position[entity].x = x;
	world->position[entity].y = y;
	world->position[entity].width = width;
	world->position[entity].height = height;
	world->position[entity].level = level;
	
	world->collision[entity].id = 0;
	world->collision[entity].type = COLLISION_SOLID;
	world->collision[entity].timer = 0;
	world->collision[entity].timerMax = 0;
	world->collision[entity].active = true;
	world->collision[entity].radius = 0;
	
	return entity;
}

unsigned int create_powerup(World * world, float x, float y, int width, int height, int type, int level) {
	unsigned int entity;
	
	entity = create_entity(world, COMPONENT_POSITION | COMPONENT_COLLISION);
	
	world->position[entity].x = x;
	world->position[entity].y = y;
	world->position[entity].width = width;
	world->position[entity].height = height;
	world->position[entity].level = level;
	
	world->collision[entity].id = 0;
	world->collision[entity].type = type;
	world->collision[entity].timer = 0;
	world->collision[entity].timerMax = 0;
	world->collision[entity].active = true;
	world->collision[entity].radius = 0;
	
	return entity;
}

/**
 * Clean up is easy.
 *
 * [Detailed description (if necessary)]
 * 
 * TODO: free all memory on heap. Memory leaks suck.
 *
 * @param world
 * @param entity
 *
 * @designer
 * @author
 */
void destroy_entity(World* world, const unsigned int entity) {

	int i, j;
	
	if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_ANIMATION)) {
		
		//printf("animation count: %d\n", world->animation[entity].animation_count);
		
		for(i = 0; i < world->animation[entity].animation_count; i++) {
			//printf("-- count %s: %d\n", world->animation[entity].animations[i].name, world->animation[entity].animations[i].surface_count);
			
			free(world->animation[entity].animations[i].name);
			
			
			for(j = 0; j < world->animation[entity].animations[i].surface_count; j++) {
				
				SDL_FreeSurface(world->animation[entity].animations[i].surfaces[j]);
				
				//printf("Free frame!\n");
				
			}
			
			free(world->animation[entity].animations[i].surfaces);
		}
		free(world->animation[entity].animations);
		
		//we have already free'd the surface so make sure it isn't free'd again.
		world->renderPlayer[entity].playerSurface = NULL;
	}
	if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_RENDER_PLAYER)) {
		
		//printf("POINTER: %p\n", world->renderPlayer[entity].playerSurface);
		
		if (world->renderPlayer[entity].playerSurface != NULL)
			SDL_FreeSurface(world->renderPlayer[entity].playerSurface);
		
	}
	if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_TEXTFIELD)) {
		
		free(world->text[entity].text);
		free(world->text[entity].name);
		
	}
	if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_BUTTON)) {
		free(world->button[entity].label);
	}
	if (IN_THIS_COMPONENT(world->mask[entity], COMPONENT_LEVEL)) {
		
		for (i = 0; i < world->level[entity].width; i++) {
			free(world->level[entity].map[i]);
		}
		
		free(world->level[entity].map);
	}
	
	world->mask[entity] = COMPONENT_EMPTY;
}

/**
 * Deletes all entities in the world.
 *
 * @param world The world struct
 *
 * @designer Jordan Marling
 *
 * @author Jordan Marling
 */
void destroy_world(World *world) {
	unsigned int entity;
	
	for(entity = 0; entity < MAX_ENTITIES; entity++) {
		destroy_entity(world, entity);
	}
	background = MAX_ENTITIES + 1;
}

void destroy_world_not_player(World *world) {
	unsigned int entity;
	
	for(entity = 0; entity < MAX_ENTITIES; entity++) {
		if(!IN_THIS_COMPONENT(world->mask[entity], COMPONENT_PLAYER) && !IN_THIS_COMPONENT(world->mask[entity], COMPONENT_STILE)) {
			destroy_entity(world, entity);
		}
	}
}




void disable_component(World *world, unsigned int entity, unsigned int component) {
	
	if (IN_THIS_COMPONENT(world->mask[entity], component)) {
		world->mask[entity] ^= component;
	}
	
}

void enable_component(World *world, unsigned int entity, unsigned int component) {
	
	if (!IN_THIS_COMPONENT(world->mask[entity], component)) {
		world->mask[entity] ^= component;
	}
	
}

