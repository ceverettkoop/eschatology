#include <stdlib.h>
#include "error.h"
#include "entity.h"
#include "avatar.h"
#include "sprites.h"
#include "tile.h"

Entity* init_player_avatar(Region *initial_region){
    Entity *ret_ptr = malloc(sizeof(Entity));
    check_malloc(ret_ptr);
    PlayerProperties *prop_ptr = malloc(sizeof(MobileProperties));
    check_malloc(prop_ptr);

    ret_ptr->type = PLAYER;
    ret_ptr->sprite = SPRITE_PLAYER;
    ret_ptr->prop_ptr = prop_ptr;
    ret_ptr->region_ptr = initial_region;
    ret_ptr->pos = (TilePos){.row = (ROWS / 2), .column = (COLUMNS / 2)};

    return ret_ptr;
}
