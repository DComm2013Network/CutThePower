#ifndef COLLISION_H
#define COLLISION_H

#include "../world.h"
#include "components.h"

void collision_system(World *world, unsigned int entity, PositionComponent* temp, unsigned int* entity_number, unsigned int* tile_number, unsigned int* hit_entity);
void wall_collision(World *world, PositionComponent temp, unsigned int* tile_number);
void entity_collision(World *world, unsigned int entity, PositionComponent temp, unsigned int* entity_number, unsigned int* hit_entity);

void rebuild_floor(World * world, int targl);
int check_tag_collision(World* world, unsigned int currentEntityID);
void anti_stuck_system(World *world, unsigned int curEntityID, int otherEntityID);

bool tag_player(World* world, unsigned int entity);
bool capture_objective(World* world, unsigned int entity);
unsigned int create_stile(World * world, int type, int xPos, int yPos, int level);

#endif

