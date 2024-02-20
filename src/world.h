#ifndef WORLD_H
#define WORLD_H

#include "sprites.h"
#include "entity.h"
#include "player_input.h"

static const int REGION_WIDTH = 106;
static const int REGION_HEIGHT = 20;

typedef struct Region_tag {
    Entity* entity_ptr_map[REGION_WIDTH][REGION_HEIGHT];
    SpriteID sprite_map[REGION_WIDTH][REGION_HEIGHT];
} Region;

//caller to free
Region* generate_region(unsigned seed);
void update_region(Region* region_ptr, PlayerInput input);

#endif /* WORLD_H */
