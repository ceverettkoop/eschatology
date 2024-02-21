#ifndef TILE_H
#define TILE_H

#include <stdbool.h>
#include <stdint.h>
#include "sprites.h"
#include "entity.h"

//terrain that can be passed
typedef struct TileProperties{
    bool is_passable;
    int damage_per_turn;
    SpriteID background;
}TileProperties;

typedef struct Tile_tag{
    TileProperties *properties_ptr;
    u_int8_t entity_count;
    Entity *entities;
}Tile;

Tile get_grass_tile();
Tile get_stonefloor_tile();
Tile get_wall_tile();
Tile get_mountain_tile();

#endif /* TILE_H */
