#ifndef POSITION_H
#define POSITION_H

#include "sprite.h"
#include "sc_map.h"
#include "../gamestate.h"
#include "../error.h"
#include "component.h"

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
    DIR_NONE
} Direction;


ADD_COMPONENT_DECL(Position);
FREE_COMPONENT_DECL(Position);
SpriteID determine_sprite(Position pos, GameState *gs);
Position calc_destination(Position origin, Direction dir, int distance);
void move_position(Position *pos, Position dest);
Direction is_border(Position pos);
void change_region(Direction dir, Position *pos);

#endif /* POSITION_H */
