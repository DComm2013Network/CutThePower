/** @ingroup Gameplay */
/** @{ */
/** @file components.h */
/** @} */

#ifndef GAMEPLAY_COMPONENTS_H
#define GAMEPLAY_COMPONENTS_H

#include "../components.h"

#define MAX_WIDTH  100 /**< The maximum width of a level (in tiles). */
#define MAX_HEIGHT 100 /**< The maximum height of a level (in tiles). */

/**
 * An enumeration of all gameplay components.
 * 
 * Each gameplay system will check an entity's mask to see if it has the 
 * desired components for the system to work on them.
 *
 * @enum Component
 */
typedef enum {
	COMPONENT_EMPTY = 0,
	COMPONENT_POSITION = 1 << 0,
	COMPONENT_RENDER = 1 << 1,
	COMPONENT_INPUT = 1 << 2,
	COMPONENT_CONTROLLABLE = 1 << 3,
	COMPONENT_COLLISION = 1 << 4,
	COMPONENT_LEVEL = 1 << 5,
	COMPONENT_MOVEMENT = 1 << 6
} Component;

/**
 * Describes an entity's position, dimensions and level.
 *
 * @struct PositionComponent
 */
>>>>>>> 7c92fd17bb8fbf03ee7ca048b7772f1d9f6cfc52
typedef struct {
	float x;    /**< The entity's x-coordinate in tile units (may be fractional). */
	float y;    /**< The entity's y-coordinate in tile units (may be fractional). */
	int width;  /**< The entity's width in pixels. */
	int height; /**< The entity's height in pixels. */
	int s;      /**< Unused. */
	int level;  /**< The floor on which the entity is situated. */
} PositionComponent;

/**
 * Describes an entity's rendering information.
 *
 * @struct RenderComponent
 */
typedef struct {
	int colour; /**< The entity's colour. */
	int width;  /**< The entity's width in pixels. */
	int height; /**< The entity's height in pixels. */
} RenderComponent;

/**
 * Describes the inputs applied to this entity.
 *
 * @struct InputComponent
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
	int type; /**< The type of collision the entity experienced. */
} CollisionComponent;

/**
 * Describes a floor's properties.
 *
 * @struct LevelComponent
 */
typedef struct {
	int levelID;                              /**< The floor number. */
	unsigned char map[MAX_WIDTH][MAX_HEIGHT]; /**< The set of tiles on this floor. */
	int width;                                /**< The floor's width in tiles. */
	int height;                               /**< The entity's x-coordinate in tile units (may be fractional). */
	int tileSize;                             /**< The entity's x-coordinate in tile units (may be fractional). */
} LevelComponent;

/**
 * Describes an entity's position, dimensions and level.
 *
 * @struct PositionComponent
 */
typedef struct {
	int id;             /**< The entity's x-coordinate in tile units (may be fractional). */
	int lastDirection;  /**< The entity's x-coordinate in tile units (may be fractional). *///0001=up 0010=right 0100=down 1000=left
	float maxSpeed;     /**< The entity's x-coordinate in tile units (may be fractional). */
	float movX;         /**< The entity's x-coordinate in tile units (may be fractional). */
	float movY;         /**< The entity's x-coordinate in tile units (may be fractional). */
	float acceleration; /**< The entity's x-coordinate in tile units (may be fractional). */
} MovementComponent;

#endif
