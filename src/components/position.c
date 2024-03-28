#include "position.h"

#include "../error.h"
#include "region.h"

static Direction is_border(Position pos);
static void change_region(Direction dir, Position* pos);

SpriteID determine_sprite(Position pos, GameState* gs) {
    DrawPriority best = -1;
    Sprite* sprite_found = NULL;
    Sprite* sprite_to_draw = NULL;
    // values in loop
    EntityID key;
    Position* value;
    sc_map_foreach(&gs->Position_map, key, value) {
        if (value->column == pos.column && value->row == pos.row && value->region_ptr == pos.region_ptr) {
            // matching value = find sprite for given entity
            sprite_found = sc_map_get_64v(&gs->Sprite_map, key);
            if (sprite_found->draw_priority > best) {
                best = sprite_found->draw_priority;
                sprite_to_draw = sprite_found;
            }
        }
    }
    if (sprite_to_draw != NULL) {
        return sprite_to_draw->id;
    } else {
        err_no_sprite_to_draw(pos.row, pos.column);
        return 0;
    }
}

ADD_COMPONENT_FUNC(Position);
FREE_COMPONENT_FUNC(Position);

Position calc_destination(Position origin, Direction dir, int distance) {
    Position ret_val;
    ret_val.region_ptr = origin.region_ptr;

    // TODO ACCOUNT FOR BLOCKING ETC
    switch (dir) {
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

    if (ret_val.row >= ROWS) {
        ret_val.row = ROWS - 1;
    }
    if (ret_val.row < 0) {
        ret_val.row = 0;
    }
    if (ret_val.column >= COLUMNS) {
        ret_val.column = COLUMNS - 1;
    }
    if (ret_val.column < 0) {
        ret_val.column = 0;
    }

    return ret_val;
}

void change_position(Position* pos, Position dest) {
    // if it's a border, change our region
    Direction border_dir = is_border(dest);
    if (border_dir != DIR_NONE) {
        change_region(border_dir, pos);
        return;  // done
    } else {
        pos->column = dest.column;
        pos->row = dest.row;
        // region only changed by change region
        return;
    }
}

static Direction is_border(Position pos) {
    if (pos.row == ROWS - 1) {
        return DIR_S;
    }
    if (pos.row == 0) {
        return DIR_N;
    }
    if (pos.column == COLUMNS - 1) {
        return DIR_E;
    }
    if (pos.column == 0) {
        return DIR_W;
    }
    return DIR_NONE;
}

// HAVE TO SHIFT US TO A VALID TILE IN THE NEW SPOT
// TODO ADJUST REGION GEN TO ALLOW THIS
static void change_region(Direction dir, Position* pos) {}

bool is_move_valid(EntityID entity, Position* new_pos) { 

    return false; 
}
