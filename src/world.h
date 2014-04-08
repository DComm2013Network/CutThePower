#ifndef WORLD_H
#define WORLD_H

#include <SDL2/SDL.h>
#include "Network/Packets.h"
#include "components.h"
#include "Gameplay/poweruptypes.h"

#define WIDTH 1280
#define HEIGHT 768

#define GAME_SPEED 30

//0 is off, 1 is on. Remember to make clean to get it to work.
#define DISPLAY_CUTSCENES 1

//max FPS
#define FPS_MAX 120

//Maximum entities that will be used.
#define MAX_ENTITIES 256

//Maximum string lengths
#define MAX_STRING 			15
#define MAX_KEYMAP_STRING 	6

#define IN_THIS_COMPONENT(mask, x) (((mask) & (x)) == (x))

//This contains all of the entities' components and their respective component masks.
typedef struct {
	unsigned int 			mask[MAX_ENTITIES];
	PositionComponent		position[MAX_ENTITIES];
	CommandComponent		command[MAX_ENTITIES];
	MovementComponent		movement[MAX_ENTITIES];
	CollisionComponent		collision[MAX_ENTITIES];
	ControllableComponent	controllable[MAX_ENTITIES];
	LevelComponent			level[MAX_ENTITIES];
	MouseComponent			mouse[MAX_ENTITIES];
	TextFieldComponent		text[MAX_ENTITIES];
	ButtonComponent			button[MAX_ENTITIES];
	RenderPlayerComponent	renderPlayer[MAX_ENTITIES];
	PlayerComponent			player[MAX_ENTITIES];
	TagComponent			tag[MAX_ENTITIES];
	AnimationComponent		animation[MAX_ENTITIES];
	WormholeComponent		wormhole[MAX_ENTITIES];
	ObjectiveComponent		objective[MAX_ENTITIES];
	TileComponent			tile[MAX_ENTITIES];
	CutsceneComponent		cutscene[MAX_ENTITIES];
	PowerUpComponent		powerup[MAX_ENTITIES];
} World;

class FPS {
private:
	float max_frame_ticks;
	Uint32 last_ticks;
	int fps;
	int numFrames;
	Uint32 startTime;

	Uint32 current_ticks;
	Uint32 target_ticks;

public:
	void init() {
		startTime = SDL_GetTicks();
		max_frame_ticks = (1000.0/(float)FPS_MAX) + 0.00001;
		fps = 0;
		last_ticks = SDL_GetTicks();
		numFrames = 0; 
	}

	void limit() {
		fps++;
		target_ticks = last_ticks + Uint32(fps * max_frame_ticks);
		current_ticks = SDL_GetTicks();

		if (current_ticks < target_ticks) {
			SDL_Delay(target_ticks - current_ticks);
			current_ticks = SDL_GetTicks();
		}

		if (current_ticks - last_ticks >= 1000) {
			fps = 0;
			last_ticks = SDL_GetTicks();
		}
	}

	float update() {
		numFrames++;
		float display_fps = ( numFrames/(float)(SDL_GetTicks() - startTime) )*1000;
		if (numFrames >= (100.0 / ((double)60 / GAME_SPEED))) {
			startTime = SDL_GetTicks();
			numFrames = 0;
		}
		return display_fps;
	}

	float getFPS() {
		int newtime = SDL_GetTicks();
		if (newtime - startTime != 0) {
			float display_fps = ( numFrames/(float)(newtime - startTime) )*1000;
			if (numFrames >= ((100.0 / ((double)60 / GAME_SPEED)) + 1)) {
				startTime = SDL_GetTicks();
				numFrames = 0;
			}
			if (display_fps > 0) {
				return display_fps;
			} else {
				return FPS_MAX;
			}
		} else {
			return FPS_MAX;
		}
	}
};

void init_world(World* world);
unsigned int create_entity(World* world, unsigned int attributes);
unsigned int create_player(World* world, int x, int y, bool controllable, int collisiontype, int playerNo, PKT_GAME_STATUS *status_update);
unsigned int create_level(World* world, int** map, int width, int height, int tileSize, int floor);
unsigned int create_stair(World* world, int targetLevel, int targetX, int targetY, int x, int y, int width, int height, int level);
unsigned int create_objective(World* world, float x, float y, int w, int h, int id, int level);
unsigned int create_block(World* world, int x, int y, int width, int height, int level);
unsigned int create_powerup(World * world, float x, float y, int width, int height, int type, int level);
void destroy_entity(World* world, const unsigned int entity);
void destroy_world(World *world);
void destroy_world_not_player(World *world);

void disable_component(World *world, unsigned int entity, unsigned int component);
void enable_component(World *world, unsigned int entity, unsigned int component);

#endif
