#include "region.h"

#include <stdlib.h>

#include "components/position.h"
#include "components/sprite.h"
#include "components/tile.h"
#include "error.h"
#include "time.h"

static void init_region(EntityID id, GameState *gs);
static void create_tiles(Region *p, GameState *gs);
static void gen_straight_tile_line(
    Position origin, bool is_x_axis, int line_len, Sprite sprite, Tile tile, GameState *gs);
static void generate_boundaries(Region *p, GameState *gs);
static void gen_rand_tile_line(Position origin, bool is_x_axis, int extent, int min_entity_count, int max_entity_count,
    Sprite sprite, Tile tile, GameState *gs);

ADD_COMPONENT_FUNC(Region);
FREE_COMPONENT_FUNC(Region);

// region not handled as an entity per se
void init_region(EntityID id, GameState *gs) {
    Region *region_ptr = sc_map_get_64v(gs->Region_map, id);
    if (!sc_map_found(gs->Region_map)) err_entity_not_found();

    region_ptr->gs = gs;
    // ID 0 = player_id, here a flag for not initialized
    region_ptr->north = 0;
    region_ptr->south = 0;
    region_ptr->west = 0;
    region_ptr->east = 0;

    // new seed
    srand(time(NULL));

    // region creation = creation of tile entities with position component that points to this region
    create_tiles(region_ptr, gs);
    generate_boundaries(region_ptr, gs);  // includes exits
}

Region *generate_region(GameState *gs) { 

    return nullptr; 
}

void generate_neighbors(EntityID id, GameState *gs) {
    Region *region_ptr = sc_map_get_64v(gs->Region_map, id);
    Region *new_ptr = NULL;
    if (!sc_map_found(gs->Region_map)) err_entity_not_found();
    if (region_ptr->north == 0) {
        region_ptr->north = new_entity(gs);
        new_ptr = sc_map_get_64v(gs->Region_map, region_ptr->north);
        if (!sc_map_found(gs->Region_map)) err_entity_not_found();
        new_ptr->south = id;
    }
    if (region_ptr->south == 0) {
        region_ptr->south = new_entity(gs);
        new_ptr = sc_map_get_64v(gs->Region_map, region_ptr->south);
        if (!sc_map_found(gs->Region_map)) err_entity_not_found();
        new_ptr->north = id;
    }
    if (region_ptr->west == 0) {
        region_ptr->west = new_entity(gs);
        new_ptr = sc_map_get_64v(gs->Region_map, region_ptr->west);
        if (!sc_map_found(gs->Region_map)) err_entity_not_found();
        new_ptr->east = id;
    }
    if (region_ptr->east == 0) {
        region_ptr->east = new_entity(gs);
        new_ptr = sc_map_get_64v(gs->Region_map, region_ptr->east);
        if (!sc_map_found(gs->Region_map)) err_entity_not_found();
        new_ptr->west = id;
    }
}

// creates tiles and sets them to grass
static void create_tiles(Region *p, GameState *gs) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLUMNS; col++) {
            EntityID tile_entity_id = new_entity(gs);
            Position pos = {.row = row, .column = col, .region_ptr = p};
            Tile tile = {.passable = true};
            add_Position(tile_entity_id, pos, gs);
            add_Tile(tile_entity_id, tile, gs);
            add_Sprite(tile_entity_id, SPRITE_GRASS, gs);
            p->tile_ids[row][col] = tile_entity_id;
        }
    }
}

static void generate_boundaries(Region *p, GameState *gs) {
    // north
    gen_straight_tile_line((Position){.row = 0, .column = 0, .region_ptr = p}, true, REGION_WIDTH, SPRITE_MOUNTAIN,
        (Tile){.passable = false}, gs);
    gen_rand_tile_line(
        (Position){.row = 0, .column = 0}, true, REGION_WIDTH, 1, 6, SPRITE_GRASS, (Tile){.passable = true}, gs);
    // west
    gen_straight_tile_line((Position){.row = 0, .column = 0, .region_ptr = p}, false, REGION_WIDTH, SPRITE_MOUNTAIN,
        (Tile){.passable = false}, gs);
    gen_rand_tile_line(
        (Position){.row = 0, .column = 0}, false, REGION_WIDTH, 1, 6, SPRITE_GRASS, (Tile){.passable = true}, gs);
    // south
    gen_straight_tile_line((Position){.row = ROWS - 1, .column = 0, .region_ptr = p}, true, REGION_WIDTH,
        SPRITE_MOUNTAIN, (Tile){.passable = false}, gs);
    gen_rand_tile_line(
        (Position){.row = ROWS - 1, .column = 0}, true, REGION_WIDTH, 1, 6, SPRITE_GRASS, (Tile){.passable = true}, gs);
    // east
    gen_straight_tile_line((Position){.row = 0, .column = COLUMNS - 1, .region_ptr = p}, false, REGION_WIDTH,
        SPRITE_MOUNTAIN, (Tile){.passable = false}, gs);
    gen_rand_tile_line((Position){.row = 0, .column = COLUMNS - 1}, false, REGION_WIDTH, 1, 6, SPRITE_GRASS,
        (Tile){.passable = true}, gs);
}

static void gen_straight_tile_line(
    Position origin, bool is_x_axis, int line_len, Sprite sprite, Tile tile, GameState *gs) {
    EntityID to_change_id;
    Position *to_change_ptr;
    Sprite *sprite_to_change;
    Tile *tile_to_change;
    EntityID origin_id = origin.region_ptr->tile_ids[origin.row][origin.column];
    Position *origin_pos_ptr = sc_map_get_64v(gs->Position_map, origin_id);
    if (!sc_map_found(gs->Position_map)) err_entity_not_found();

    int row = origin.row;
    int col = origin.column;
    for (int i = 0; i < line_len; i++) {
        if (is_x_axis) {
            if (row == ROWS) break;
            row++;
        } else {
            if (col == COLUMNS) break;
            col++;
        }
        to_change_id = origin.region_ptr->tile_ids[row][col];
        sprite_to_change = sc_map_64v(gs->Sprite_map, to_change_id);
        if (!sc_map_found(gs->Sprite_map)) err_entity_not_found();
        tile_to_change = sc_map_64v(gs->Tile_map, to_change_id);
        if (!sc_map_found(gs->Tile_map)) err_entity_not_found();
        *sprite_to_change = sprite;
        *tile_to_change = tile;
    }
}

// populates between 0 and max_entity_count elements along line
static void gen_rand_tile_line(Position origin, bool is_x_axis, int extent, int min_entity_count, int max_entity_count,
    Sprite sprite, Tile tile, GameState *gs) {
    EntityID to_change_id;
    Position *to_change_ptr;
    Sprite *sprite_to_change;
    Tile *tile_to_change;
    EntityID origin_id = origin.region_ptr->tile_ids[origin.row][origin.column];
    Position *origin_pos_ptr = sc_map_get_64v(gs->Position_map, origin_id);
    if (!sc_map_found(gs->Position_map)) err_entity_not_found();

    int row = origin.row;
    int col = origin.column;
    const int count = rand() / (RAND_MAX / (max_entity_count - min_entity_count)) + min_entity_count;

    for (int i = 0; i < count; i++) {
        int offset = rand() / (RAND_MAX / extent);
        if (is_x_axis) {
            row = origin.row + offset;
            if (row >= ROWS) continue;
        } else {
            col = origin.column + offset;
            if (col >= COLUMNS) continue;
        }
        to_change_id = origin.region_ptr->tile_ids[row][col];
        sprite_to_change = sc_map_64v(gs->Sprite_map, to_change_id);
        if (!sc_map_found(gs->Sprite_map)) err_entity_not_found();
        tile_to_change = sc_map_64v(gs->Tile_map, to_change_id);
        if (!sc_map_found(gs->Tile_map)) err_entity_not_found();
        *sprite_to_change = sprite;
        *tile_to_change = tile;
    }
}