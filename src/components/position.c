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
