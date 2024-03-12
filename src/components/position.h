#ifndef POSITION_H
#define POSITION_H

#include "sprites.h"
#include "entity.h"
#include "gamestate.h"
#include "../sc_map.h"

SpriteID determine_sprite(int row, int column, GameState *gs);
void add_position(EntityID id, int row, int column, GameState *gs);
void free_position(EntityID id, GameState *gs);

typedef struct Position_tag{
    int row;
    int column;
}Position;

#endif /* POSITION_H */
