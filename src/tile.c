#include <stdlib.h>
#include "tile.h"

Tile get_grass_tile(){
    static TileProperties prop;
    Tile ret;

    prop.is_passable = true;
    prop.damage_per_turn = 0;
    prop.background = SPRITE_GRASS;   

    ret.properties_ptr = &prop;
    ret.entity_count = 0;
    ret.entities = NULL;
    return ret;
}

Tile get_stonefloor_tile(){
    static TileProperties prop;
    Tile ret;

    prop.is_passable = true;
    prop.damage_per_turn = 0;
    prop.background = SPRITE_STONEFLOOR;   

    ret.properties_ptr = &prop;
    ret.entity_count = 0;
    ret.entities = NULL;
    return ret;
}

Tile get_wall_tile(){
    static TileProperties prop;
    Tile ret;

    prop.is_passable = false;
    prop.damage_per_turn = 0;
    prop.background = SPRITE_WALL;   

    ret.properties_ptr = &prop;
    ret.entity_count = 0;
    ret.entities = NULL;
    return ret;
}

Tile get_mountain_tile(){
    static TileProperties prop;
    Tile ret;

    prop.is_passable = false;
    prop.damage_per_turn = 0;
    prop.background = SPRITE_MOUNTAIN;   

    ret.properties_ptr = &prop;
    ret.entity_count = 0;
    ret.entities = NULL;
    return ret;
}