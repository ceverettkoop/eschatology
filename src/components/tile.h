#ifndef TILE_H
#define TILE_H

#include <stdbool.h>
#include "../gamestate.h"
#include "../error.h"
#include "component.h"

typedef struct Tile_tag{
    bool passable;
} Tile;

ADD_COMPONENT_DECL(Tile);
FREE_COMPONENT_DECL(Tile);

#endif /* TILE_H */
