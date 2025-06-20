#include "position.h"

#include "../error.h"
#include "../vector.h"
#include "interaction.h"
#include "region.h"
#include "tile.h"

static Direction is_border(Position pos);
static void change_region(Direction dir, Position* pos, GameState* gs);
static void change_position(Position* pos, Position dest, GameState* gs);

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
            if (!sc_map_found(&gs->Sprite_map)) err_entity_not_found();
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

MoveResult attempt_move(GameState* gs, EntityID entity, Direction dir, Interaction* result) {
    EntityID key;
    Position* value;
    Vector entities_found = new_vector(sizeof(EntityID));
    Position* origin_ptr = sc_map_get_64v(&gs->Position_map, entity);
    if (!sc_map_found(&gs->Position_map)) err_entity_not_found();
    Position destination = calc_destination(*origin_ptr, dir);

    sc_map_foreach(&gs->Position_map, key, value) {
        if (cmp_pos(value, &destination)) {
            vec_push_back(&entities_found, &key, 1);
        }
    }

    for (size_t i = 0; i < entities_found.size; i++) {
        EntityID id = VEC_GET(entities_found, EntityID, i);
        // exit on impassable tile
        Tile* tile = sc_map_get_64v(&gs->Tile_map, id);
        if (tile)
            if (!tile->passable) {
                free_vec(&entities_found);
                return IMPASSABLE;
            }
        // check for interaction that takes place of move
        Interaction* intr = sc_map_get_64v(&gs->Interaction_map, id);
        if (intr) {
            free_vec(&entities_found);
            *result = *intr;
            return ACTION;
        }
    }
    // default case is we moved, move now
    free_vec(&entities_found);
    change_position(origin_ptr, destination, gs);
    return MOVED;
}

ADD_COMPONENT_FUNC(Position);
FREE_COMPONENT_FUNC(Position);

Position calc_destination(Position origin, Direction dir) {
    Position ret_val;
    ret_val.region_ptr = origin.region_ptr;

    switch (dir) {
        case DIR_N:
            ret_val.column = origin.column;
            ret_val.row = origin.row - 1;
            break;
        case DIR_NE:
            ret_val.column = origin.column + 1;
            ret_val.row = origin.row - 1;
            break;
        case DIR_E:
            ret_val.column = origin.column + 1;
            ret_val.row = origin.row;
            break;
        case DIR_SE:
            ret_val.column = origin.column + 1;
            ret_val.row = origin.row + 1;
            break;
        case DIR_S:
            ret_val.column = origin.column;
            ret_val.row = origin.row + 1;
            break;
        case DIR_SW:
            ret_val.column = origin.column - 1;
            ret_val.row = origin.row + 1;
            break;
        case DIR_W:
            ret_val.column = origin.column - 1;
            ret_val.row = origin.row;
            break;
        case DIR_NW:
            ret_val.column = origin.column - 1;
            ret_val.row = origin.row - 1;
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

bool pos_is_valid(Position pos) {
    if (pos.row < 0 || pos.row >= ROWS || pos.column < 0 || pos.column >= COLUMNS) {
        return false;
    }
    return true;
}

int pos_to_index(Position pos) {
    if (!pos_is_valid(pos)) return -1;
    return pos.column + (pos.row * COLUMNS);
}

Position index_to_pos(int index, struct Region_tag* region) {
    Position ret_val;
    ret_val.region_ptr = region;
    ret_val.column = (index + COLUMNS) % COLUMNS;
    ret_val.row = (int)(index / COLUMNS);
    return ret_val;
}

static void change_position(Position* pos, Position dest, GameState* gs) {
    // if it's a border, change our region
    Direction border_dir = is_border(dest);
    if (border_dir != DIR_NONE) {
        change_region(border_dir, pos, gs);
        return;  // done
    } else {
        pos->column = dest.column;
        pos->row = dest.row;
        // region only changed by change region
        return;
    }
}

bool cmp_pos(Position* a, Position* b) {
    if (a->region_ptr != b->region_ptr) return false;
    if (a->column != b->column) return false;
    if (a->row != b->row) return false;
    return true;
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
static void change_region(Direction dir, Position* pos, GameState* gs) {
    Region* current_region = pos->region_ptr;
    Region* new_region = NULL;
    EntityID new_region_id = 0;

    // Get the neighboring region based on exit direction
    switch (dir) {
        case DIR_N:
            new_region = sc_map_get_64v(&current_region->gs->Region_map, current_region->north);
            new_region_id = current_region->north;
            if (!sc_map_found(&current_region->gs->Region_map)) return;
            pos->region_ptr = new_region;
            pos->row = ROWS - 1;  // Enter from south
            break;
        case DIR_S:
            new_region = sc_map_get_64v(&current_region->gs->Region_map, current_region->south);
            new_region_id = current_region->south;
            if (!sc_map_found(&current_region->gs->Region_map)) return;
            pos->region_ptr = new_region;
            pos->row = 0;  // Enter from north
            break;
        case DIR_E:
            new_region = sc_map_get_64v(&current_region->gs->Region_map, current_region->east);
            new_region_id = current_region->east;
            if (!sc_map_found(&current_region->gs->Region_map)) return;
            pos->region_ptr = new_region;
            pos->column = 0;  // Enter from west
            break;
        case DIR_W:
            new_region = sc_map_get_64v(&current_region->gs->Region_map, current_region->west);
            new_region_id = current_region->west;
            if (!sc_map_found(&current_region->gs->Region_map)) return;
            pos->region_ptr = new_region;
            pos->column = COLUMNS - 1;  // Enter from east
            break;
        default:
            return;
    }

    // err on impassable tile at destination, region gen should not allow this
    EntityID tile_id = new_region->tile_ids[pos->row][pos->column];
    Tile* new_tile = sc_map_get_64v(&current_region->gs->Tile_map, tile_id);
    if (!new_tile->passable) err_placed_on_impassable_tile(pos);

    // all is well, pos is changed, update gamestate
    gs->cur_region_ptr = new_region;

    // if the new region does not have neighbors, create them now
    if (new_region->north == 0 || new_region->south == 0 || new_region->west == 0 || new_region->east == 0)
        generate_neighbors(new_region_id, gs, DEFAULT_REGION);
}