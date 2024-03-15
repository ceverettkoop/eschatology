#include <stdlib.h>
#include "region.h"
#include "error.h"
#include "time.h"
#include "components/position.h"
#include "components/tile.h"
#include "components/sprite.h"

static void blank_with_grass(Region *p, GameState *gs);
static void generate_boundaries(Region *p, GameState *gs);
static void gen_straight_tile_line(Position origin, Region* region_ptr, bool is_x_axis, int line_len, Tile template, GameState *gs);
static void gen_rand_tile_line(
    EntityID origin, bool is_x_axis, int extent, int min_entity_count, int max_entity_count, EntityID template );



ADD_COMPONENT_FUNC(Region);
FREE_COMPONENT_FUNC(Region);



//region not handled as an entity per se
void init_region(EntityID id, GameState *gs){
    Region *region_ptr =  sc_map_get_64v(gs->Region_map, id);
    if (!sc_map_found(gs->Region_map)) err_entity_not_found();

    region_ptr->gs = gs;
    //ID 0 = player_id, here a flag for not initialized
    region_ptr->north = 0;
    region_ptr->south = 0;
    region_ptr->west = 0;
    region_ptr->east = 0;

    // new seed
    srand(time(NULL));

    //region creation = creation of tile entities with position component that points to this region
    blank_with_grass(region_ptr, gs);
    generate_boundaries(region_ptr, gs);  // includes exits


}

void generate_neighbors(Region *region_ptr) {
    if (region_ptr->north == NULL) {
        region_ptr->north = generate_region(region_ptr->gs);
        region_ptr->north->south = region_ptr;
    }
    if (region_ptr->south == NULL) {
        region_ptr->south = generate_region(region_ptr->gs);
        region_ptr->south->north = region_ptr;
    }
    if (region_ptr->west == NULL) {
        region_ptr->west = generate_region(region_ptr->gs);
        region_ptr->west->east = region_ptr;
    }
    if (region_ptr->east == NULL) {
        region_ptr->east = generate_region(region_ptr->gs);
        region_ptr->east->west = region_ptr;
    }
}

static void blank_with_grass(Region *p, GameState *gs) {
    int index = 0;
    for (int row = 0; row < ROWS; row++){
        for (int col = 0; col < COLUMNS; col++){
            EntityID tile_entity_id = new_entity(gs);\
            Position pos = {
                .row = row,
                .column = col,
                .region_ptr = p
            };
            Tile tile = {
                .passable = true
            };
            add_Position(tile_entity_id, pos, gs);
            add_Tile(tile_entity_id, tile, gs);
            add_Sprite(tile_entity_id, SPRITE_GRASS, gs);
            p->tile_ids[index] = tile_entity_id;
            index++;
        }
    }
}

static void generate_boundaries(Region *p) {
    // north
    gen_straight_tile_line(&(p->tile_matrix[0][0]), true, REGION_WIDTH, get_mountain_tile());
    gen_rand_tile_line(&(p->tile_matrix[0][0]), true, REGION_WIDTH, 1, 6, get_grass_tile());
    // west
    gen_straight_tile_line(&(p->tile_matrix[0][0]), false, REGION_HEIGHT, get_mountain_tile());
    gen_rand_tile_line(&(p->tile_matrix[0][0]), false, REGION_HEIGHT, 1, 6, get_grass_tile());
    // south
    gen_straight_tile_line(&(p->tile_matrix[ROWS - 1][0]), true, REGION_WIDTH, get_mountain_tile());
    gen_rand_tile_line(&(p->tile_matrix[ROWS - 1][0]), true, REGION_WIDTH, 1, 6, get_grass_tile());
    // east
    gen_straight_tile_line(&(p->tile_matrix[0][COLUMNS - 1]), false, REGION_HEIGHT, get_mountain_tile());
    gen_rand_tile_line(&(p->tile_matrix[0][COLUMNS - 1]), false, REGION_HEIGHT, 1, 6, get_grass_tile());
}

static void gen_straight_tile_line(Position origin, Region* region_ptr, bool is_x_axis, int line_len, Tile template, GameState *gs) {
    EntityID origin_id;
    
    EntityID key;
    void *value;
    sc_map_foreach(gs->Tile_map, origin_id, value){
        //if key is in Position map, check region
        //if region and pos map break with this origin_id
    }

}

// populates between 0 and max_entity_count elements along line
static void gen_rand_tile_line(
    Tile *origin, bool is_x_axis, int extent, int min_entity_count, int max_entity_count, Tile tile) {
    const int count = rand() / (RAND_MAX / (max_entity_count - min_entity_count)) + min_entity_count;
    for (int i = 0; i < count; i++) {
        int offset = rand() / (RAND_MAX / extent);
        if (is_x_axis) {
            *(origin + offset) = tile;
        } else {
            *(origin + offset * COLUMNS) = tile;
        }
    }
}