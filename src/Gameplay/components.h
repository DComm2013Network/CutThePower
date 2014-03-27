/** @ingroup Gameplay */
/** @{ */
/** @file components.h */
/** @} */
#ifndef GAMEPLAY_COMPONENTS_H
#define GAMEPLAY_COMPONENTS_H

#include "../components.h"
#include "../Network/Packets.h"
#include "collisiontypes.h"

typedef struct {
	float x;
	float y;
	int width;
	int height;
	int level;
} PositionComponent;

typedef struct {
	bool active;
} ControllableComponent;

typedef struct {
	int id;
	int type;
	int timer;
	int timerMax;
	bool active;
	int radius;
} CollisionComponent;

typedef struct {
	int levelID;
	int** map;
	int width;
	int height;
	int tileSize;
} LevelComponent;

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
	bool status;
} ObjectiveComponent;

#endif
