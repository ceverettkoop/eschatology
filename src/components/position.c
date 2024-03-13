#include "position.h"
#include "../region.h"
#include "error.h"

SpriteID determine_sprite(Position pos, GameState *gs) { 
    EntityID key;
    Position *value;
    DrawPriority best = -1;
    Sprite* sprite_found = NULL;
    Sprite* sprite_to_draw = NULL;
    sc_map_foreach(gs->position_map, key, value){
        if( value->column == pos.column && value->row == pos.row && value->region_ptr == pos.region_ptr){
            //matching value = find sprite for given entity
            sprite_found = sc_map_get_64v(gs->sprite_map, key);
            if (sprite_found->draw_priority > best){
                best = sprite_found->draw_priority;
                sprite_to_draw = sprite_found;
            }
        }
    }
    if(sprite_to_draw != NULL){
        return sprite_to_draw->id;
    }else{
        err_no_sprite_to_draw(pos.row, pos.column);
    }
}

void add_position(EntityID id, int row, int column, GameState* gs) {
    void *pos_ptr = malloc(sizeof(Position));
    check_malloc(pos_ptr);

    void *old_val = sc_map_put_64v(gs->position_map, id, pos_ptr);
    if(sc_map_found(gs->position_map)){
        free(old_val);
        err_component_exists("Position");
    }
    ((Position*)pos_ptr)->row = row;
    ((Position*)pos_ptr)->column = column;
    ((Position*)pos_ptr)->region_ptr = gs->cur_region_ptr;
}

void free_position(EntityID id, GameState *gs) {
    void *component_to_free = sc_map_get_64v(gs->position_map, id);
    if(sc_map_found(gs->position_map)){
        free(component_to_free);
        sc_map_del_64v(gs->position_map, id);
    }else{
        err_free_missing_component("Position");
    }
}

Position move(Position origin, Direction dir, int distance) { 
    
    Position ret_val;
    ret_val.region_ptr = origin.region_ptr;

    //TODO ACCOUNT FOR BLOCKING ETC

    switch (dir){
    case DIR_N:
        ret_val.column = origin.column;
        ret_val.row = origin.row - distance;
        break;
    case DIR_NE:
        ret_val.column = origin.column + distance;
        ret_val.row = origin.row - 1;
        break;
    case DIR_E:
        ret_val.column = origin.column + distance;
        ret_val.row = origin.row;
        break;
    case DIR_SE:
        ret_val.column = origin.column + distance;
        ret_val.row = origin.row + distance;
        break;
    case DIR_S:
        ret_val.column = origin.column;
        ret_val.row = origin.row + distance;
        break;
    case DIR_SW:
        ret_val.column = origin.column - distance;
        ret_val.row = origin.row + distance;
        break;
    case DIR_W:
        ret_val.column = origin.column - distance;
        ret_val.row = origin.row;
        break;
    case DIR_NW:
        ret_val.column = origin.column - distance;
        ret_val.row = origin.row - distance;
        break;
    default:
        break;
    }

    //overflow - this needs to be altered to CHANGE REGION if character is allowed to get there
    if(ret_val.row >= ROWS ){
        ret_val.row = ROWS - 1;
    }
    if(ret_val.row < 0 ){
        ret_val.row = 0;
    }
    if(ret_val.column >= COLUMNS ){
        ret_val.column = COLUMNS - 1;
    }
    if(ret_val.column < 0 ){
        ret_val.column = 0;
    }
    
}
