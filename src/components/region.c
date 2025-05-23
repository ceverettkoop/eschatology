#include "region.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../error.h"
#include "../regiontemplate.h"
#include "../vector.h"
#include "position.h"
#include "sc_map.h"
#include "sprite.h"
#include "tile.h"

#define MAX_CONSOLIDATIONS 200
#define MIN_ROOM_SZ 10
#define BACKGROUND_FLAG 0

static Region *init_region(EntityID id, GameState *gs, RegionTemplate template);
static void create_tiles(Region *p, GameState *gs, Sprite background);
static void gen_straight_tile_line(
    Position origin, bool is_x_axis, int line_len, Sprite sprite, Tile tile, GameState *gs);
static void generate_boundaries(Region *p, GameState *gs);
static void gen_rand_tile_line(Position origin, bool is_x_axis, int extent, int min_entity_count, int max_entity_count,
    Sprite sprite, Tile tile, GameState *gs);
static int *gen_rooms(Region *p, RegionTemplate template);
static void assign_tiles(Region *region_ptr, int *room_matrix, RegionTemplate template);
static void bsp_iterate(void *_matrix, int itr);
static void partition_space(void *_matrix, int room_id, int new_id);
static int consolidate_rooms(void *_matrix, int room_id_max);
static Vector id_adj_rooms(void *_matrix, int room_id);
static void combine_adj_rooms(void *_matrix, int room_id, Vector *adj_rooms, Vector *rooms_found);
static int room_sz(void *_matrix, int room_id);
static void add_background(int room_ct, void *_matrix);
static Vector tiles_bordering_room(int room, int *matrix);
static bool value_is_adj_to_pos(int value, int pos_index, int *matrix);
static int count_distinct_rooms(int *matrix);

ADD_COMPONENT_FUNC(Region);
FREE_COMPONENT_FUNC(Region);

// region not handled as an entity per se
static Region *init_region(EntityID id, GameState *gs, RegionTemplate template) {
    Region *region_ptr = sc_map_get_64v(&gs->Region_map, id);
    int *room_matrix;
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

    room_matrix = gen_rooms(region_ptr, template);
    debug_print_room_matrix(room_matrix);
    assign_tiles(region_ptr, room_matrix, template);

    generate_boundaries(region_ptr, gs);  // includes exits
    
    free(room_matrix);

    //initial update of sprites
    update_all_sprites(region_ptr);
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

void update_sprites(Vector positions) {
    for (size_t i = 0; i < positions.size; i++){
        Position pos = VEC_GET(positions, Position, i);
        SpriteID id = determine_sprite(pos, pos.region_ptr->gs);
        SpriteID *id_ptr = &((SpriteID*)pos.region_ptr->displayed_sprite)[i];
        *id_ptr = id;
    }
}

void update_all_sprites(Region *reg_ptr) {
    for (size_t i = 0; i < REGION_AREA; i++){
        Position pos = index_to_pos(i, reg_ptr);
        SpriteID id = determine_sprite(pos, reg_ptr->gs);
        SpriteID *id_ptr = &((SpriteID*)reg_ptr->displayed_sprite)[i];
        *id_ptr = id;
    }
}

// creates tiles and sets them to defaulg background
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

int *gen_rooms(Region *p, RegionTemplate template) {
    const int min_room_ct = rand() / (RAND_MAX / (template.room_ct_range[R_MAX] - template.room_ct_range[R_MIN])) +
                            template.room_ct_range[R_MIN];
    bool success = false;
    // generate dummy grid to seperate into regions and init to 1 (single room)
    int *space_matrix = calloc(REGION_AREA, sizeof(int));
    check_malloc(space_matrix);
    int consolidations = 0;
    int room_ct;

    while (!success) {
        // initialize matrix to single room (ID 1)
        for (size_t i = 0; i < REGION_AREA; i++) {
            space_matrix[i] = 1;
        }
        // divide rooms per iterations
        for (int i = 0; i < template.bsp_iterations; i++) {
            bsp_iterate(space_matrix, i);
        }
        // count distinct rooms without consolidation
        room_ct = count_distinct_rooms(space_matrix);
        success = (min_room_ct <= room_ct);
        consolidations++;
        // take what we can get
        if (consolidations > MAX_CONSOLIDATIONS) {
            fprintf(stderr, "Warning: room generation failed within max allowable iterations, using result\n");
            success = true;
        }
    }
    // designate one room as background and place hallways between others
    add_background(room_ct, space_matrix);
    return space_matrix;
}

void assign_tiles(Region *region_ptr, int *room_matrix, RegionTemplate template) {
    for (size_t i = 0; i < REGION_AREA; i++) {
        // background
        if (room_matrix[i] == BACKGROUND_FLAG) {
            EntityID id = ((EntityID *)(region_ptr->tile_ids))[i];
            Sprite *sprite = sc_map_get_64v(&(region_ptr->gs->Sprite_map), id);
            *sprite = template.default_background;
        } else {  // room tiles  
            EntityID id = ((EntityID *)(region_ptr->tile_ids))[i];
            Sprite *sprite = sc_map_get_64v(&(region_ptr->gs->Sprite_map), id);
            *sprite = template.room_floor;
        }
    }
}

static void bsp_iterate(void *_matrix, int itr) {
    // find all existing rooms and split them
    int max_room_id = count_distinct_rooms(_matrix);
    int next_room_id = max_room_id + 1;
    
    // split each existing room
    for (int room_id = 1; room_id <= max_room_id; room_id++) {
        if (room_sz(_matrix, room_id) > 16) {  // only split rooms with at least 16 tiles (4x4)
            partition_space(_matrix, room_id, next_room_id);
            next_room_id++;
        }
    }
}

static void partition_space(void *_matrix, int room_id, int new_id) {
    int(*matrix)[COLUMNS] = _matrix;
    int min_row = ROWS, min_col = COLUMNS, max_row = -1, max_col = -1;
    
    // find bounding box of the room
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLUMNS; col++) {
            if (matrix[row][col] == room_id) {
                if (row < min_row) min_row = row;
                if (row > max_row) max_row = row;
                if (col < min_col) min_col = col;
                if (col > max_col) max_col = col;
            }
        }
    }
    
    // check if room is large enough to split
    int width = max_col - min_col + 1;
    int height = max_row - min_row + 1;
    
    if (width < 4 || height < 4) {
        return; // too small to split
    }
    
    // decide split direction - prefer splitting longer dimension
    bool split_horizontally = (height > width) || (height == width && rand() % 2);
    
    if (split_horizontally) {
        // split horizontally (create top and bottom rooms)
        int split_row = min_row + 2 + (rand() % (height - 3)); // ensure at least 2 tiles on each side
        
        for (int row = min_row; row <= max_row; row++) {
            for (int col = min_col; col <= max_col; col++) {
                if (matrix[row][col] == room_id && row >= split_row) {
                    matrix[row][col] = new_id;
                }
            }
        }
    } else {
        // split vertically (create left and right rooms)
        int split_col = min_col + 2 + (rand() % (width - 3)); // ensure at least 2 tiles on each side
        
        for (int row = min_row; row <= max_row; row++) {
            for (int col = min_col; col <= max_col; col++) {
                if (matrix[row][col] == room_id && col >= split_col) {
                    matrix[row][col] = new_id;
                }
            }
        }
    }
}

// consolidates rooms in provided matrix and returns count of rooms in result
int consolidate_rooms(void *_matrix, int room_id_max) {
    int *cur = _matrix;
    int itr_count = 0;
    int found;
    Vector rooms_found = new_vector(sizeof(int));
    for (size_t i = 0; i < room_id_max; i++) {
        Vector adj_rooms = id_adj_rooms(_matrix, i);
        // append adj rooms to current room_id, ignoring rooms found already
        combine_adj_rooms(_matrix, i, &adj_rooms, &rooms_found);
        if (room_sz(_matrix, i) > MIN_ROOM_SZ) vec_push_back(&rooms_found, &i, 1);
        free_vec(&adj_rooms);
    }
    found = rooms_found.size;
    free_vec(&rooms_found);
    return found;
}

Vector id_adj_rooms(void *_matrix, int room_id) {
    Vector ret_vec = new_vector(sizeof(int));
    int *matrix = _matrix;
    int *cur = _matrix;
    int adj_value;
    Position cur_pos;

    // find all neighbors
    for (size_t i = 0; i < REGION_AREA; i++) {
        if (*cur == room_id) {
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

int room_sz(void *_matrix, int room_id) {
    int sz = 0;
    for (size_t i = 0; i < REGION_AREA; i++) {
        if (((int *)_matrix)[i] == room_id) sz++;
    }
    return sz;
}

bool value_is_adj_to_pos(int value, int pos_index, int *matrix) {
    Position pos = index_to_pos(pos_index, NULL);
    Position other;
    // loop through 4 cardinal directions
    for (size_t i = 0; i < 4; i++) {
        other = calc_destination(pos, i);
        if (!cmp_pos(&pos, &other)) {
            if (value == *(matrix + pos_to_index(other))) return true;
        }
    }
    return false;
}

//
Vector tiles_bordering_room(int room, int *matrix) {
    Vector ret = new_vector(sizeof(int));
    for (size_t i = 0; i < REGION_AREA; i++) {
        if (matrix[i] != room) {
            if (value_is_adj_to_pos(matrix[i], i, matrix)) {
                vec_push_back(&ret, &i, 1);
            }
        }
    }
    return ret;
}

void add_background(int room_ct, void *_matrix) {
    int *matrix = _matrix;
    if (room_ct == 0) return;
    
    // Pick one room randomly to be background
    int background_room = (rand() % room_ct) + 1;
    
    // Convert the selected room to background
    for (size_t i = 0; i < REGION_AREA; i++) {
        if (matrix[i] == background_room) {
            matrix[i] = BACKGROUND_FLAG;
        }
    }
    
    // Add 1-tile background border around remaining rooms
    int(*grid)[COLUMNS] = _matrix;
    int temp_matrix[ROWS][COLUMNS];
    
    // Copy current matrix to temp
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLUMNS; col++) {
            temp_matrix[row][col] = grid[row][col];
        }
    }
    
    // For each room tile, check if it's on the edge and add background border
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLUMNS; col++) {
            if (temp_matrix[row][col] > 0) { // if it's a room (not background)
                // Check all 8 directions for edge detection
                for (int dr = -1; dr <= 1; dr++) {
                    for (int dc = -1; dc <= 1; dc++) {
                        if (dr == 0 && dc == 0) continue; // skip center
                        int nr = row + dr;
                        int nc = col + dc;
                        
                        // If adjacent cell is out of bounds or different room, make current cell background
                        if (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLUMNS || 
                            (temp_matrix[nr][nc] != temp_matrix[row][col] && temp_matrix[nr][nc] != BACKGROUND_FLAG)) {
                            grid[row][col] = BACKGROUND_FLAG;
                            goto next_cell; // break out of both loops
                        }
                    }
                }
                next_cell:;
            }
        }
    }
}

void combine_adj_rooms(void *_matrix, int room_id, Vector *adj_rooms, Vector *rooms_found) {
    int *cur = _matrix;
    for (size_t i = 0; i < REGION_AREA; i++) {
        if (vec_contains(adj_rooms, cur) && !vec_contains(rooms_found, cur)) {
            *cur = room_id;
        }
        cur++;
    }
}

void debug_print_room_matrix(int *room_matrix) {
    printf("\n=== Room Matrix Debug ===\n");
    printf("Region size: %dx%d (ROWS x COLUMNS)\n", ROWS, COLUMNS);
    printf("Matrix contents:\n");
    
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLUMNS; col++) {
            int index = row * COLUMNS + col;
            printf("%2d ", room_matrix[index]);
        }
        printf("\n");
    }
    printf("========================\n\n");
}

static int count_distinct_rooms(int *matrix) {
    int max_room_id = 0;
    for (size_t i = 0; i < REGION_AREA; i++) {
        if (matrix[i] > max_room_id) {
            max_room_id = matrix[i];
        }
    }
    return max_room_id;
}
