#ifndef GAMEPLAY_COMPONENTS_H
#define GAMEPLAY_COMPONENTS_H

#include "../components.h"

#define MAX_WIDTH 100
#define MAX_HEIGHT 100

//Example components. These should be in their simplest form so no duplicate data.

typedef struct {
	float x;
	float y;
	int width;
	int height;
	int s;
	int level;
} PositionComponent;

typedef struct {
	int colour;
	int width;
	int height;
} RenderComponent;

typedef struct {
	bool active;
} ControllableComponent;

typedef struct {
	int type;
} CollisionComponent;

typedef struct {
	int levelID;
	unsigned char map[MAX_WIDTH][MAX_HEIGHT];
	int width;
	int height;
	int tileSize;
} LevelComponent;

typedef struct {
	int id;
	int lastDirection;//0001=up 0010=right 0100=down 1000=left
	float maxSpeed;
	float movX;
	float movY;
	float acceleration;
} MovementComponent;

#endif
