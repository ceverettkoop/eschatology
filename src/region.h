#ifndef REGION_H
#define REGION_H

#include "sprites.h"
#include "entity.h"
#include "user_input.h"

static const int REGION_WIDTH = 106;
static const int REGION_HEIGHT = 20;

typedef struct Region_tag {
    Entity *entity_ptr_matrix[REGION_WIDTH][REGION_HEIGHT];
    SpriteID sprite_map[REGION_WIDTH][REGION_HEIGHT];
    Region *north;
    Region *east;
    Region *south;
    Region *west;
} Region;


Region *generate_region();
void generate_neighbors(Region *region_ptr);
void update_region(Region *region_ptr, UserInput input);

#endif /* REGION_H */
