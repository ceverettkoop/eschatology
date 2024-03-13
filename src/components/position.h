#ifndef POSITION_H
#define POSITION_H

#include "sprite.h"
#include "entity.h"
#include "gamestate.h"
#include "../sc_map.h"


typedef struct Position_tag{
    int row;
    int column;
    Region *region_ptr;
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


void add_position(EntityID id, int row, int column, GameState *gs);
void free_position(EntityID id, GameState *gs);
SpriteID determine_sprite(Position pos, GameState *gs);
Position move(Position origin, Direction dir, int distance);


#endif /* POSITION_H */