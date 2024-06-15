#include "region.h"

#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "../error.h"
#include "../regiontemplate.h"
#include "../vector.h"
#include "position.h"
#include "sc_map.h"
#include "sprite.h"
#include "tile.h"

static Region *init_region(EntityID id, GameState *gs, RegionTemplate template);
static void create_tiles(Region *p, GameState *gs, Sprite background);
static void gen_straight_tile_line(
    Position origin, bool is_x_axis, int line_len, Sprite sprite, Tile tile, GameState *gs);
static void generate_boundaries(Region *p, GameState *gs);
static void gen_rand_tile_line(Position origin, bool is_x_axis, int extent, int min_entity_count, int max_entity_count,
    Sprite sprite, Tile tile, GameState *gs);
static void gen_rooms(Region *p, RegionTemplate template);
static void bsp_iterate(void *_matrix, int itr);
static void partition_space(void *_matrix, int room_id, int new_id);
static bool consolidate_rooms(void *_matrix, int min_room_count, int min_room_sz);
static Vector id_adj_rooms(void *_matrix, int room_id);
static void combine_adj_rooms(void *_matrix, int room_id, Vector *adj_rooms, Vector *rooms_found);

static const int MAX_ITERATIONS = 100;

ADD_COMPONENT_FUNC(Region);
FREE_COMPONENT_FUNC(Region);

// region not handled as an entity per se
static Region *init_region(EntityID id, GameState *gs, RegionTemplate template) {
    Region *region_ptr = sc_map_get_64v(&gs->Region_map, id);
    if (!sc_map_found(&gs->Region_map)) err_entity_not_found();

    region_ptr->gs = gs;
    // ID 0 = player_id, here a flag for not initialized
    region_ptr->north = 0;
    region_ptr->south = 0;
    region_ptr->west = 0;
    region_ptr->east = 0;

    // new seed
    srand((unsigned int)time(NULL));

    // region creation = creation of tile entities with position component that points to this region
    create_tiles(region_ptr, gs, template.default_background);
    gen_rooms(region_ptr, template);
    generate_boundaries(region_ptr, gs);  // includes exits
    return region_ptr;
}

Region *generate_region(GameState *gs, EntityID *_id, RegionTemplate template) {
    EntityID id = new_entity(gs);
    *_id = id;
    Region *reg_ptr = malloc(sizeof(Region));
    check_malloc(reg_ptr);
    sc_map_put_64v(&gs->Region_map, id, reg_ptr);
    return init_region(id, gs, template);
}

void generate_neighbors(EntityID id, GameState *gs, RegionTemplate template) {
    Region *region_ptr = sc_map_get_64v(&gs->Region_map, id);
    Region *new_ptr = NULL;
    if (!sc_map_found(&gs->Region_map)) err_entity_not_found();
    if (region_ptr->north == 0) {
        generate_region(gs, &region_ptr->north, template);
        new_ptr = sc_map_get_64v(&gs->Region_map, region_ptr->north);
        if (!sc_map_found(&gs->Region_map)) err_entity_not_found();
        new_ptr->south = id;
    }
    if (region_ptr->south == 0) {
        generate_region(gs, &region_ptr->south, template);
        new_ptr = sc_map_get_64v(&gs->Region_map, region_ptr->south);
        if (!sc_map_found(&gs->Region_map)) err_entity_not_found();
        new_ptr->north = id;
    }
    if (region_ptr->west == 0) {
        generate_region(gs, &region_ptr->west, template);
        new_ptr = sc_map_get_64v(&gs->Region_map, region_ptr->west);
        if (!sc_map_found(&gs->Region_map)) err_entity_not_found();
        new_ptr->east = id;
    }
    if (region_ptr->east == 0) {
        generate_region(gs, &region_ptr->east, template);
        new_ptr = sc_map_get_64v(&gs->Region_map, region_ptr->east);
        if (!sc_map_found(&gs->Region_map)) err_entity_not_found();
        new_ptr->west = id;
    }
}

// creates tiles and sets them to defauly background
static void create_tiles(Region *p, GameState *gs, Sprite background) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLUMNS; col++) {
            EntityID tile_entity_id = new_entity(gs);
            Position pos = {.row = row, .column = col, .region_ptr = p};
            Tile tile = {.passable = true};
            add_Position(tile_entity_id, pos, gs);
            add_Tile(tile_entity_id, tile, gs);
            add_Sprite(tile_entity_id, background, gs);
            p->tile_ids[row][col] = tile_entity_id;
        }
    }
}

static void generate_boundaries(Region *p, GameState *gs) {
    // north
    gen_straight_tile_line((Position){.row = 0, .column = 0, .region_ptr = p}, true, REGION_WIDTH, SPRITE_MOUNTAIN,
        (Tile){.passable = false}, gs);
    gen_rand_tile_line((Position){.row = 0, .column = 0, .region_ptr = p}, true, REGION_WIDTH, 1, 6, SPRITE_GRASS,
        (Tile){.passable = true}, gs);
    // west
    gen_straight_tile_line((Position){.row = 0, .column = 0, .region_ptr = p}, false, REGION_WIDTH, SPRITE_MOUNTAIN,
        (Tile){.passable = false}, gs);
    gen_rand_tile_line((Position){.row = 0, .column = 0, .region_ptr = p}, false, REGION_HEIGHT, 1, 6, SPRITE_GRASS,
        (Tile){.passable = true}, gs);
    // south
    gen_straight_tile_line((Position){.row = ROWS - 1, .column = 0, .region_ptr = p}, true, REGION_WIDTH,
        SPRITE_MOUNTAIN, (Tile){.passable = false}, gs);
    gen_rand_tile_line((Position){.row = ROWS - 1, .column = 0, .region_ptr = p}, true, REGION_WIDTH, 1, 6,
        SPRITE_GRASS, (Tile){.passable = true}, gs);
    // east
    gen_straight_tile_line((Position){.row = 0, .column = COLUMNS - 1, .region_ptr = p}, false, REGION_WIDTH,
        SPRITE_MOUNTAIN, (Tile){.passable = false}, gs);
    gen_rand_tile_line((Position){.row = 0, .column = COLUMNS - 1, .region_ptr = p}, false, REGION_HEIGHT, 1, 6,
        SPRITE_GRASS, (Tile){.passable = true}, gs);
}

static void gen_straight_tile_line(
    Position origin, bool is_x_axis, int line_len, Sprite sprite, Tile tile, GameState *gs) {
    EntityID to_change_id;
    Sprite *sprite_to_change;
    Tile *tile_to_change;

    int row = origin.row;
    int col = origin.column;
    for (int i = 0; i < line_len; i++) {
        if (is_x_axis) {
            col++;
            if (col >= COLUMNS) break;
        } else {
            row++;
            if (row >= ROWS) break;
        }
        to_change_id = origin.region_ptr->tile_ids[row][col];
        sprite_to_change = sc_map_get_64v(&gs->Sprite_map, to_change_id);
        if (!sc_map_found(&gs->Sprite_map)) err_entity_not_found();
        tile_to_change = sc_map_get_64v(&gs->Tile_map, to_change_id);
        if (!sc_map_found(&gs->Tile_map)) err_entity_not_found();
        *sprite_to_change = sprite;
        *tile_to_change = tile;
    }
}

// populates between 0 and max_entity_count elements along line
static void gen_rand_tile_line(Position origin, bool is_x_axis, int extent, int min_entity_count, int max_entity_count,
    Sprite sprite, Tile tile, GameState *gs) {
    EntityID to_change_id;
    Sprite *sprite_to_change;
    Tile *tile_to_change;

    int row = origin.row;
    int col = origin.column;
    const int count = rand() / (RAND_MAX / (max_entity_count - min_entity_count)) + min_entity_count;

    for (int i = 0; i < count; i++) {
        int offset = rand() / (RAND_MAX / extent);
        if (is_x_axis) {
            col = origin.column + offset;
            if (col >= COLUMNS) continue;
        } else {
            row = origin.row + offset;
            if (row >= ROWS) continue;
        }
        to_change_id = origin.region_ptr->tile_ids[row][col];
        sprite_to_change = sc_map_get_64v(&gs->Sprite_map, to_change_id);
        if (!sc_map_found(&gs->Sprite_map)) err_entity_not_found();
        tile_to_change = sc_map_get_64v(&gs->Tile_map, to_change_id);
        if (!sc_map_found(&gs->Tile_map)) err_entity_not_found();
        *sprite_to_change = sprite;
        *tile_to_change = tile;
    }
}

void gen_rooms(Region *p, RegionTemplate template) {
    const int count = rand() / (RAND_MAX / (template.room_ct_range[R_MAX] - template.room_ct_range[R_MIN])) +
                      template.room_ct_range[R_MIN];

    // generate dummy grid to seperate into regions and init to 0
    int *space_matrix = calloc(REGION_AREA, sizeof(int));
    int *cur = space_matrix;
    for (size_t i = 0; i < REGION_AREA; i++) {
        *cur = 0;
        cur++;
    }
    for (int i = 0; i < template.bsp_iterations; i++) {
        bsp_iterate(space_matrix, i);
    }
    consolidate_rooms(space_matrix, count);

}

static void bsp_iterate(void *_matrix, int itr) {
    // iterate over each previous iteration once, iteration 0 just blank space
    // sweep whole map but only increment matching values
    int new_id;
    int end_id;
    if (itr == 0){
        new_id = 1;
        end_id = 2;
    }else{
        new_id = itr * itr;
        end_id = ((itr + 1) * (itr + 1));
    }
    for (int room_id = 0; new_id < end_id; room_id++, new_id++){
        partition_space(_matrix, room_id, new_id);
    }
}

static void partition_space(void *_matrix, int room_id, int new_id) {
    //define extent of room defined by given id
    int *cur = _matrix;
    int (*matrix)[COLUMNS] = _matrix;
    int tl = -1;
    int br;
    int min_row;
    int min_col;
    int max_row;
    int max_col;
    int room_area;
    //define top left on first note of id, define bottom right on last
    for (size_t i = 0; i < REGION_AREA; i++, cur++){
        if(tl == -1){
            if(*cur == room_id){
                tl = i;
            }
        }else{
            if(*cur == room_id){
                br = i;
            }
        }
    }
    //convert abs coordinate to row/col
    min_col = tl % COLUMNS;
    max_col = br % COLUMNS;
    min_row = (int) tl / COLUMNS;
    max_row = (int) br / COLUMNS;
    room_area = (max_col - min_col + 1) * (max_row - min_row + 1);

    //now split the room
    bool split_rows = (rand() > rand());
    int split_point;
    if (split_rows){
        split_point = (rand() / (RAND_MAX / (max_row - min_row))) + min_row;
        for (int i = 0, row = min_row, col = min_col; i < room_area; i++){
            if(row > split_point){
                matrix[row][col] = new_id;
            }
            if(col == max_col){
                row++;
                col = min_col;
            }else{
                col++;
            }
        }
    }else{
        split_point = (rand() / (RAND_MAX / (max_col - min_col))) + min_col;
        for (int i = 0, row = min_row, col = min_col; i < room_area; i++){
            if(col > split_point){
                matrix[row][col] = new_id;
            }
            if(col == max_col){
                row++;
                col = min_col;
            }else{
                col++;
            }
        }
    }
}

bool consolidate_rooms(void *_matrix, int min_room_count, int min_room_sz) {
    Vector found_ids = new_vector(sizeof(int));
    Vector rooms_found = new_vector(sizeof(int) * REGION_AREA);
    int *cur = _matrix;
    int itr_count = 0;

    for (size_t i = 0; i < REGION_AREA; i++){
        if (!vec_contains(&found_ids, cur)){
            vec_push_back(&found_ids, cur, 1);
        }
        cur++;
    }

    while(rooms_found.size < min_room_count){
        for (size_t i = 0; i < found_ids.size; i++){
            int room_id = VEC_GET(found_ids, int, i);
            Vector adj_rooms = id_adj_rooms(_matrix, room_id);
            //append adj rooms to current room_id, ignorning rooms found already
            combine_adj_rooms(_matrix, room_id, &adj_rooms, &rooms_found);
            if (room_sz(_matrix, room_id) > )
            free_vec(&adj_rooms);
        }

        itr_count++;
        if(itr_count < MAX_ITERATIONS) return 
    }

    free_vec(&found_ids);
}

Vector id_adj_rooms(void *_matrix, int room_id) {
    Vector ret_vec = new_vector(sizeof(int));
    int *matrix = _matrix;
    int *cur = _matrix;
    int adj_value;
    Position cur_pos;

    //find all neighbors
    for (size_t i = 0; i < REGION_AREA; i++){
        if (*cur == room_id){
            cur_pos = index_to_pos(i, NULL);
            adj_value = *(matrix + pos_to_index(calc_destination(cur_pos, DIR_N)));
            if (adj_value == room_id) vec_push_back(&ret_vec, &adj_value, 1);
            adj_value = *(matrix + pos_to_index(calc_destination(cur_pos, DIR_W)));
            if (adj_value == room_id) vec_push_back(&ret_vec, &adj_value, 1);
            adj_value = *(matrix + pos_to_index(calc_destination(cur_pos, DIR_S)));
            if (adj_value == room_id) vec_push_back(&ret_vec, &adj_value, 1);
            adj_value = *(matrix + pos_to_index(calc_destination(cur_pos, DIR_E)));
            if (adj_value == room_id) vec_push_back(&ret_vec, &adj_value, 1);
        }
    }

    return ret_vec;
}

void combine_adj_rooms(void *_matrix, int room_id, Vector *adj_rooms, Vector *rooms_found) {
    int *cur = _matrix;
    for (size_t i = 0; i < REGION_AREA; i++){
        if ( vec_contains(adj_rooms, *cur) && !vec_contains(rooms_found, *cur)){
            *cur = room_id;
        }
        cur++;
    }
}
