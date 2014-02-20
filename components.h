#ifndef COMPONENTS_H
#define COMPONENTS_H

#define MAX_WIDTH 50
#define MAX_HEIGHT 50

//This is an enumeration of all the components. This is so that each system can check an entities
//mask to see if it has the desired components for the system to work on them.
typedef enum 
{
	COMPONENT_EMPTY = 0,
	COMPONENT_POSITION = 1 << 0,
	COMPONENT_RENDER = 1 << 1,
	COMPONENT_INPUT = 1 << 2,
	COMPONENT_CONTROLLABLE = 1 << 3,
	COMPONENT_COLLISION = 1 << 4,
	COMPONENT_LEVEL = 1 << 5,
	COMPONENT_MOVEMENT = 1 << 6
} Component;

//Example components. These should be in their simplest form so no duplicate data.

typedef struct
{
	
	float x;
	float y;
	int width;
	int height;
	int s;
	int level;
	
} PositionComponent;

typedef struct
{
	
	int colour;
	int width;
	int height;
	
} RenderComponent;

typedef struct
{
	
	bool up;
	bool left;
	bool right;
	bool down;
	
} InputComponent;

typedef struct {
	bool active;
} ControllableComponent;

typedef struct {
	int type;
} CollisionComponent;

typedef struct {
	int levelID;
	int map[MAX_WIDTH][MAX_HEIGHT];
	int width;
	int height;
	int tileSize;
} LevelComponent;

typedef struct {
	int id;
	int lastDirection;
	float velocity;
	float accelleration;
} MovementComponent;

#endif
