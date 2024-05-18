#ifndef POSITION_H
#define POSITION_H

#include <stdbool.h>

#include "../error.h"
#include "../gamestate.h"
#include "interaction.h"
#include "component.h"
#include "sc_map.h"
#include "sprite.h"

struct Region_tag;

typedef struct Position_tag {
    int row;
    int column;
    struct Region_tag *region_ptr;
} Position;

typedef enum {
    DIR_N,
    DIR_NE,
    DIR_E,
    DIR_SE,
    DIR_S,
    DIR_SW,
    DIR_W,
    DIR_NW,
    DIR_NONE
} Direction;

typedef enum {
    IMPASSABLE,
    ACTION,
    MOVED
} MoveResult;

ADD_COMPONENT_DECL(Position);
FREE_COMPONENT_DECL(Position);
SpriteID determine_sprite(Position pos, GameState *gs);
MoveResult attempt_move(GameState *gs, EntityID entity, Direction dir, Interaction *result);
Position calc_destination(Position origin, Direction dir);

#endif /* POSITION_H */
