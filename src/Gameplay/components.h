/** @ingroup Gameplay */
/** @{ */
/** @file components.h */
/** @} */
#ifndef GAMEPLAY_COMPONENTS_H
#define GAMEPLAY_COMPONENTS_H

#include "../components.h"
#include "../Network/Packets.h"
#include "collisiontypes.h"

/**
 * Describes an entity's position, dimensions and level.
 *
 * @struct PositionComponent
 */
typedef struct {
	float x;
	float y;
	int width;
	int height;
	int level;
} PositionComponent;

/**
 * Determines whether an entity may be controlled.
 *
 * @struct ControllableComponent
 */
typedef struct {
	bool active;
} ControllableComponent;

/**
 * Holds the type of collision an entity had.
 *
 * @struct CollisionComponent
 */
typedef struct {
	int id;
	int type;
	int timer;
	int timerMax;
	bool active;
	int radius;
} CollisionComponent;

/**
 * Describes a floor's properties.
 *
 * @struct LevelComponent
 */
typedef struct {
	int levelID;
	int** map;
	int width;
	int height;
	int tileSize;
} LevelComponent;

/**
 * Describes an entity's position, dimensions and level.
 *
 * @struct PositionComponent
 */
typedef struct {
	int id;
	int lastDirection;
	float defMaxSpeed;
	float maxSpeed;
	float movX;
	float movY;
	float acceleration;
	float friction;
} MovementComponent;

typedef struct {
	teamNo_t teamNo;
	playerNo_t playerNo;
	status_t readyStatus;
	char name[MAX_NAME];
	character_t character;
	unsigned int tilez;
	int onTile;
} PlayerComponent;

typedef struct {
	playerNo_t	tagger_id; /* the person who tagged */
	playerNo_t  taggee_id; /* the person who got tagged */
} TagComponent;

typedef struct {
    int targetLevel;
    int targetX;
    int targetY;    
} WormholeComponent;

typedef struct {
	unsigned int objectiveID;
	unsigned int status;
} ObjectiveComponent;

typedef struct {
	unsigned int type;
	unsigned int start_time;
} TileComponent;

typedef struct {
	unsigned int captureTime;
	unsigned int duration; //in milliseconds
	unsigned int type;
} PowerUpComponent;

#endif
