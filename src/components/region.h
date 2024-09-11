#ifndef REGION_H
#define REGION_H

#include "component.h"
#include "../gamestate.h"
#include "../user_input.h"
#include "../regiontemplate.h"

#define REGION_WIDTH 105
#define REGION_HEIGHT 20
#define REGION_AREA (REGION_HEIGHT) * (REGION_WIDTH)
#define ROWS (REGION_HEIGHT)
#define COLUMNS (REGION_WIDTH)

typedef struct Region_tag Region;

struct Region_tag {
    GameState *gs;
    EntityID north;
    EntityID east;
    EntityID south;
    EntityID west;
    EntityID tile_ids[ROWS][COLUMNS];
    SpriteID displayed_sprite[ROWS][COLUMNS];
};

ADD_COMPONENT_DECL(Region);
FREE_COMPONENT_DECL(Region);
Region *generate_region(GameState *gs, EntityID *_id, RegionTemplate template);
//TODO free regions I guess although they just get freed at program exit
void generate_neighbors(EntityID id, GameState *gs, RegionTemplate template);
void update_region(Region *region_ptr, UserInput input);

#endif /* REGION_H */
