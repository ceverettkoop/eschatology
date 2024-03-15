#ifndef REGION_H
#define REGION_H

#include "component.h"
#include "../gamestate.h"
#include "../user_input.h"

#define REGION_WIDTH 105
#define REGION_HEIGHT 20
#define REGION_AREA (REGION_HEIGHT) * (REGION_WIDTH)
#define ROWS (REGION_HEIGHT)
#define COLUMNS (REGION_WIDTH)

typedef struct Region_tag Region;

struct Region_tag {
    GameState *gs;
    EntityID *north;
    EntityID *east;
    EntityID *south;
    EntityID *west;
    EntityID tile_ids[REGION_AREA];
};

ADD_COMPONENT_DECL(Region);
FREE_COMPONENT_DECL(Region);
void init_region(EntityID id, GameState *gs);
//TODO free regions I guess although they just get freed at program exit
void generate_neighbors(Region *region_ptr);
void update_region(Region *region_ptr, UserInput input);

#endif /* REGION_H */
