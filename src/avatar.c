#include "avatar.h"
#include "error.h"
#include "sprites.h"
#include "tile.h"
#include "entity.h"
#include <stdlib.h>

Entity* init_player_avatar(Region *initial_region){
    Entity *ret_ptr = malloc(sizeof(Entity));
    check_malloc(ret_ptr);
    MobileProperties *prop_ptr = malloc(sizeof(MobileProperties));
    check_malloc(prop_ptr);

    ret_ptr->is_mobile = true;
    ret_ptr->sprite = SPRITE_PLAYER;
    ret_ptr->properties_ptr = prop_ptr;
    ret_ptr->region_ptr = initial_region;
    ret_ptr->pos = (TilePos){.row = (ROWS / 2), .column = (COLUMNS / 2)};

    prop_ptr->is_player = true;
    prop_ptr->is_passable = false;

    return ret_ptr;
}
