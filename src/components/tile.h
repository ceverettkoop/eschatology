#ifndef TILE_H
#define TILE_H

#include "component.h"
#include <stdbool.h>

typedef struct Tile_tag{
    bool passable;
} Tile;

ADD_COMPONENT_DECL(Tile);
FREE_COMPONENT_DECL(Tile);

#endif /* TILE_H */
