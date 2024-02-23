#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "region.h"

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

TilePos move(TilePos origin, Direction dir, int distance);


#endif /* MOVEMENT_H */
