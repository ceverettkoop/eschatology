#ifndef POSITION_H
#define POSITION_H

#include "component.h"
#include "sprite.h"
#include "../entity.h"
#include "../gamestate.h"
#include "sc_map.h"

struct Region_tag;

typedef struct Position_tag{
    int row;
    int column;
    struct Region_tag *region_ptr;
}Position;

typedef enum{
    DIR_N,
    DIR_NE,
    DIR_E,
    DIR_SE,
    DIR_S,
    DIR_SW,
    DIR_W,
    DIR_NW,
} Direction;


ADD_COMPONENT_DECL(Position);
FREE_COMPONENT_DECL(Position);
SpriteID determine_sprite(Position pos, GameState *gs);
Position move(Position origin, Direction dir, int distance);


#endif /* POSITION_H */
