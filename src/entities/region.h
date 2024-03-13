#ifndef REGION_H
#define REGION_H

#include "gamestate.h"
#include "user_input.h"
#include "tile.h"

#define REGION_WIDTH 105
#define REGION_HEIGHT 20
#define ROWS (REGION_HEIGHT)
#define COLUMNS (REGION_WIDTH)

typedef struct Region_tag Region;

struct Region_tag {
    GameState *gs;
    Region *north;
    Region *east;
    Region *south;
    Region *west;
};

Region *generate_region(GameState *gs);
void generate_neighbors(Region *region_ptr);
void update_region(Region *region_ptr, UserInput input);

#endif /* REGION_H */
