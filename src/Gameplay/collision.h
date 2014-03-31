#ifndef COLLISION_H
#define COLLISION_H

void collision_system(World *world, unsigned int entity, PositionComponent* temp, unsigned int* entity_number, unsigned int* tile_number, unsigned int* hit_entity);
void wall_collision(World *world, PositionComponent temp, unsigned int* tile_number);
void entity_collision(World *world, unsigned int entity, PositionComponent temp, unsigned int* entity_number, unsigned int* hit_entity);

#endif

