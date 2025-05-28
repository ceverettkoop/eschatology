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

static const int INNER_ROWS = ROWS - 2;
static const int INNER_COLS = COLUMNS - 2;
static const int INNER_AREA = (ROWS - 2) * (COLUMNS - 2);

static Region *init_region(EntityID id, GameState *gs, RegionTemplate template, bool *north_align, bool *south_align, bool *west_align, bool *east_align);
static void create_tiles(Region *p, GameState *gs, Sprite background);
static void gen_straight_tile_line(
    Position origin, bool is_x_axis, int line_len, Sprite sprite, Tile tile, GameState *gs);
static void gen_rand_tile_line(Position origin, bool is_x_axis, int extent, int min_entity_count, int max_entity_count,
    Sprite sprite, Tile tile, GameState *gs);
static int *gen_rooms(Region *p, RegionTemplate template);
static void assign_tiles(Region *region_ptr, int *room_matrix, RegionTemplate template);
static void bsp_iterate(void *_matrix, int itr);
static void partition_space(void *_matrix, int room_id, int new_id);
static int room_sz(void *_matrix, int room_id);
static void add_background(int room_ct, void *_matrix);
static void add_corridors_to_full_height_rooms(void *_matrix);
static int count_distinct_rooms(int *matrix);
static bool* extract_border_passable_tiles(Region *region, Direction border);
static void generate_boundaries_with_alignment(Region *p, GameState *gs, bool *north_align, bool *south_align, bool *west_align, bool *east_align);

ADD_COMPONENT_FUNC(Region);
FREE_COMPONENT_FUNC(Region);

// region not handled as an entity per se
static Region *init_region(EntityID id, GameState *gs, RegionTemplate template, bool *north_align, bool *south_align, bool *west_align, bool *east_align) {
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
    generate_boundaries_with_alignment(region_ptr, gs, north_align, south_align, west_align, east_align);  // includes exits

    room_matrix = gen_rooms(region_ptr, template);
    //debug_print_room_matrix(room_matrix);
    assign_tiles(region_ptr, room_matrix, template);
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
    return init_region(id, gs, template, NULL, NULL, NULL, NULL);
}

static Region *generate_region_with_alignment(GameState *gs, EntityID *_id, RegionTemplate template, bool *north_align, bool *south_align, bool *west_align, bool *east_align) {
    EntityID id = new_entity(gs);
    *_id = id;
    Region *reg_ptr = malloc(sizeof(Region));
    check_malloc(reg_ptr);
    sc_map_put_64v(&gs->Region_map, id, reg_ptr);
    return init_region(id, gs, template, north_align, south_align, west_align, east_align);
}

void generate_neighbors(EntityID id, GameState *gs, RegionTemplate template) {
    Region *region_ptr = sc_map_get_64v(&gs->Region_map, id);
    Region *new_ptr = NULL;
    if (!sc_map_found(&gs->Region_map)) err_entity_not_found();
    
    if (region_ptr->north == 0) {
        bool *south_align = extract_border_passable_tiles(region_ptr, DIR_N);
        generate_region_with_alignment(gs, &region_ptr->north, template, NULL, south_align, NULL, NULL);
        new_ptr = sc_map_get_64v(&gs->Region_map, region_ptr->north);
        if (!sc_map_found(&gs->Region_map)) err_entity_not_found();
        new_ptr->south = id;
        free(south_align);
    }
    if (region_ptr->south == 0) {
        bool *north_align = extract_border_passable_tiles(region_ptr, DIR_S);
        generate_region_with_alignment(gs, &region_ptr->south, template, north_align, NULL, NULL, NULL);
        new_ptr = sc_map_get_64v(&gs->Region_map, region_ptr->south);
        if (!sc_map_found(&gs->Region_map)) err_entity_not_found();
        new_ptr->north = id;
        free(north_align);
    }
    if (region_ptr->west == 0) {
        bool *east_align = extract_border_passable_tiles(region_ptr, DIR_W);
        generate_region_with_alignment(gs, &region_ptr->west, template, NULL, NULL, NULL, east_align);
        new_ptr = sc_map_get_64v(&gs->Region_map, region_ptr->west);
        if (!sc_map_found(&gs->Region_map)) err_entity_not_found();
        new_ptr->east = id;
        free(east_align);
    }
    if (region_ptr->east == 0) {
        bool *west_align = extract_border_passable_tiles(region_ptr, DIR_E);
        generate_region_with_alignment(gs, &region_ptr->east, template, NULL, NULL, west_align, NULL);
        new_ptr = sc_map_get_64v(&gs->Region_map, region_ptr->east);
        if (!sc_map_found(&gs->Region_map)) err_entity_not_found();
        new_ptr->west = id;
        free(west_align);
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

// creates tiles and sets them to default background
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

static void gen_straight_tile_line(
    Position origin, bool is_x_axis, int line_len, Sprite sprite, Tile tile, GameState *gs) {
    EntityID to_change_id;
    Sprite *sprite_to_change;
    Tile *tile_to_change;

    int row = origin.row;
    int col = origin.column;
    for (int i = 0; i < line_len; i++) {
        to_change_id = origin.region_ptr->tile_ids[row][col];
        sprite_to_change = sc_map_get_64v(&gs->Sprite_map, to_change_id);
        if (!sc_map_found(&gs->Sprite_map)) err_entity_not_found();
        tile_to_change = sc_map_get_64v(&gs->Tile_map, to_change_id);
        if (!sc_map_found(&gs->Tile_map)) err_entity_not_found();
        *sprite_to_change = sprite;
        *tile_to_change = tile;
        
        if (is_x_axis) {
            col++;
            if (col >= COLUMNS) break;
        } else {
            row++;
            if (row >= ROWS) break;
        }
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
    // generate matrix for inner region (excluding outermost ring)
    int *space_matrix = calloc(INNER_AREA, sizeof(int));
    check_malloc(space_matrix);
    int consolidations = 0;
    int room_ct;

    while (!success) {
        // initialize matrix to single room (ID 1)
        for (size_t i = 0; i < INNER_AREA; i++) {
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
    // add corridors through full-height rooms before adding background
    add_corridors_to_full_height_rooms(space_matrix);
    // designate one room as background and place hallways between others
    add_background(room_ct, space_matrix);
    return space_matrix;
}

void assign_tiles(Region *region_ptr, int *room_matrix, RegionTemplate template) {
    // Only assign tiles for the inner area (excluding outermost ring)
    for (int inner_row = 0; inner_row < INNER_ROWS; inner_row++) {
        for (int inner_col = 0; inner_col < INNER_COLS; inner_col++) {
            // Map inner matrix coordinates to actual region coordinates
            int region_row = inner_row + 1;  // offset by 1 to skip top border
            int region_col = inner_col + 1;  // offset by 1 to skip left border
            
            // Calculate index in the inner matrix
            int matrix_index = inner_row * INNER_COLS + inner_col;
            
            EntityID id = region_ptr->tile_ids[region_row][region_col];
            Sprite *sprite = sc_map_get_64v(&(region_ptr->gs->Sprite_map), id);
            
            // background
            if (room_matrix[matrix_index] == BACKGROUND_FLAG) {
                *sprite = template.default_background;
            } else {  // room tiles  
                *sprite = template.room_floor;
            }
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
    int *matrix = _matrix;
    int min_row = INNER_ROWS, min_col = INNER_COLS, max_row = -1, max_col = -1;
    
    // find bounding box of the room in the inner matrix
    for (int row = 0; row < INNER_ROWS; row++) {
        for (int col = 0; col < INNER_COLS; col++) {
            int index = row * INNER_COLS + col;
            if (matrix[index] == room_id) {
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
                int index = row * INNER_COLS + col;
                if (matrix[index] == room_id && row >= split_row) {
                    matrix[index] = new_id;
                }
            }
        }
    } else {
        // split vertically (create left and right rooms)
        int split_col = min_col + 2 + (rand() % (width - 3)); // ensure at least 2 tiles on each side
        
        for (int row = min_row; row <= max_row; row++) {
            for (int col = min_col; col <= max_col; col++) {
                int index = row * INNER_COLS + col;
                if (matrix[index] == room_id && col >= split_col) {
                    matrix[index] = new_id;
                }
            }
        }
    }
}



int room_sz(void *_matrix, int room_id) {
    int sz = 0;
    for (size_t i = 0; i < INNER_AREA; i++) {
        if (((int *)_matrix)[i] == room_id) sz++;
    }
    return sz;
}



void add_background(int room_ct, void *_matrix) {
    int *matrix = _matrix;
    if (room_ct == 0) return;
    
    // Pick one room randomly to be background
    int background_room = (rand() % room_ct) + 1;
    
    // Convert the selected room to background
    for (size_t i = 0; i < INNER_AREA; i++) {
        if (matrix[i] == background_room) {
            matrix[i] = BACKGROUND_FLAG;
        }
    }
    
    // Add 1-tile background border around remaining rooms
    int *temp_matrix = malloc(INNER_AREA * sizeof(int));
    check_malloc(temp_matrix);
    
    // Copy current matrix to temp
    for (int i = 0; i < INNER_AREA; i++) {
        temp_matrix[i] = matrix[i];
    }
    
    // For each room tile, check if it's on the edge and add background border
    for (int row = 0; row < INNER_ROWS; row++) {
        for (int col = 0; col < INNER_COLS; col++) {
            int index = row * INNER_COLS + col;
            if (temp_matrix[index] > 0) { // if it's a room (not background)
                // Check all 8 directions for edge detection
                for (int dr = -1; dr <= 1; dr++) {
                    for (int dc = -1; dc <= 1; dc++) {
                        if (dr == 0 && dc == 0) continue; // skip center
                        int nr = row + dr;
                        int nc = col + dc;
                        
                        // If adjacent cell is out of bounds or different room, make current cell background
                        if (nr < 0 || nr >= INNER_ROWS || nc < 0 || nc >= INNER_COLS) {
                            matrix[index] = BACKGROUND_FLAG;
                            goto next_cell; // break out of both loops
                        }
                        
                        int neighbor_index = nr * INNER_COLS + nc;
                        if (temp_matrix[neighbor_index] != temp_matrix[index] && temp_matrix[neighbor_index] != BACKGROUND_FLAG) {
                            matrix[index] = BACKGROUND_FLAG;
                            goto next_cell; // break out of both loops
                        }
                    }
                }
                next_cell:;
            }
        }
    }
    
    free(temp_matrix);
}


void debug_print_room_matrix(int *room_matrix) {
    printf("\n=== Room Matrix Debug ===\n");
    printf("Inner region size: %dx%d (INNER_ROWS x INNER_COLS)\n", INNER_ROWS, INNER_COLS);
    printf("Matrix contents:\n");
    
    for (int row = 0; row < INNER_ROWS; row++) {
        for (int col = 0; col < INNER_COLS; col++) {
            int index = row * INNER_COLS + col;
            printf("%2d ", room_matrix[index]);
        }
        printf("\n");
    }
    printf("========================\n\n");
}

static int count_distinct_rooms(int *matrix) {
    int max_room_id = 0;
    for (size_t i = 0; i < INNER_AREA; i++) {
        if (matrix[i] > max_room_id) {
            max_room_id = matrix[i];
        }
    }
    return max_room_id;
}

static void add_corridors_to_full_height_rooms(void *_matrix) {
    int *matrix = _matrix;
    
    // Check each room to see if it's full height within the inner matrix
    int max_room_id = count_distinct_rooms(_matrix);
    for (int room_id = 1; room_id <= max_room_id; room_id++) {
        int min_col = INNER_COLS, max_col = -1;
        
        // Check if this room spans from top boundary (row 0) to bottom boundary (row INNER_ROWS-1)
        bool has_top_row = false, has_bottom_row = false;
        
        // Check top row of inner matrix
        for (int col = 0; col < INNER_COLS; col++) {
            int top_index = 0 * INNER_COLS + col;
            if (matrix[top_index] == room_id) {
                has_top_row = true;
                if (col < min_col) min_col = col;
                if (col > max_col) max_col = col;
            }
        }
        
        // Check bottom row of inner matrix
        for (int col = 0; col < INNER_COLS; col++) {
            int bottom_index = (INNER_ROWS - 1) * INNER_COLS + col;
            if (matrix[bottom_index] == room_id) {
                has_bottom_row = true;
                if (col < min_col) min_col = col;
                if (col > max_col) max_col = col;
            }
        }
        
        // Check if room spans the full height
        if (has_top_row && has_bottom_row) {
            bool spans_full_height = true;
            for (int row = 0; row < INNER_ROWS; row++) {
                bool has_room_in_row = false;
                for (int col = min_col; col <= max_col; col++) {
                    int index = row * INNER_COLS + col;
                    if (matrix[index] == room_id) {
                        has_room_in_row = true;
                        break;
                    }
                }
                if (!has_room_in_row) {
                    spans_full_height = false;
                    break;
                }
            }
            
            if (spans_full_height && (max_col - min_col + 1) >= 4) {
                // Create horizontal corridor at random height
                int corridor_row = 1 + (rand() % (INNER_ROWS - 2)); // between rows 1 and INNER_ROWS-2
                
                // Convert corridor tiles to background (passable)
                for (int col = min_col; col <= max_col; col++) {
                    int index = corridor_row * INNER_COLS + col;
                    if (matrix[index] == room_id) {
                        matrix[index] = BACKGROUND_FLAG;
                    }
                }
            }
        }
    }
}

static bool* extract_border_passable_tiles(Region *region, Direction border) {
    bool *passable_tiles = malloc(sizeof(bool) * (REGION_WIDTH - 2));
    check_malloc(passable_tiles);
    
    switch(border) {
        case DIR_N:
            // Extract passable tiles from north border (excluding corners)
            for (int col = 1; col < COLUMNS - 1; col++) {
                EntityID tile_id = region->tile_ids[0][col];
                Tile *tile = sc_map_get_64v(&region->gs->Tile_map, tile_id);
                passable_tiles[col - 1] = sc_map_found(&region->gs->Tile_map) ? tile->passable : false;
            }
            break;
        case DIR_S:
            // Extract passable tiles from south border (excluding corners)
            for (int col = 1; col < COLUMNS - 1; col++) {
                EntityID tile_id = region->tile_ids[ROWS - 1][col];
                Tile *tile = sc_map_get_64v(&region->gs->Tile_map, tile_id);
                passable_tiles[col - 1] = sc_map_found(&region->gs->Tile_map) ? tile->passable : false;
            }
            break;
        case DIR_W:
            // Extract passable tiles from west border (excluding corners)
            for (int row = 1; row < ROWS - 1; row++) {
                EntityID tile_id = region->tile_ids[row][0];
                Tile *tile = sc_map_get_64v(&region->gs->Tile_map, tile_id);
                passable_tiles[row - 1] = sc_map_found(&region->gs->Tile_map) ? tile->passable : false;
            }
            break;
        case DIR_E:
            // Extract passable tiles from east border (excluding corners)
            for (int row = 1; row < ROWS - 1; row++) {
                EntityID tile_id = region->tile_ids[row][COLUMNS - 1];
                Tile *tile = sc_map_get_64v(&region->gs->Tile_map, tile_id);
                passable_tiles[row - 1] = sc_map_found(&region->gs->Tile_map) ? tile->passable : false;
            }
            break;
        default:
            // Initialize to false for invalid directions
            for (int i = 0; i < REGION_WIDTH - 2; i++) {
                passable_tiles[i] = false;
            }
            break;
    }
    
    return passable_tiles;
}

static void generate_boundaries_with_alignment(Region *p, GameState *gs, bool *north_align, bool *south_align, bool *west_align, bool *east_align) {
    // north
    gen_straight_tile_line((Position){.row = 0, .column = 0, .region_ptr = p}, true, REGION_WIDTH, SPRITE_MOUNTAIN,
        (Tile){.passable = false}, gs);
    if (north_align) {
        // Use aligned passable tiles
        for (int col = 1; col < COLUMNS - 1; col++) {
            if (north_align[col - 1]) {
                EntityID to_change_id = p->tile_ids[0][col];
                Sprite *sprite_to_change = sc_map_get_64v(&gs->Sprite_map, to_change_id);
                Tile *tile_to_change = sc_map_get_64v(&gs->Tile_map, to_change_id);
                if (sc_map_found(&gs->Sprite_map) && sc_map_found(&gs->Tile_map)) {
                    *sprite_to_change = SPRITE_GRASS;
                    *tile_to_change = (Tile){.passable = true};
                }
            }
        }
    } else {
        // Generate random passable tiles
        gen_rand_tile_line((Position){.row = 0, .column = 1, .region_ptr = p}, true, REGION_WIDTH - 2, 1, 6, SPRITE_GRASS,
            (Tile){.passable = true}, gs);
    }
    
    // west
    gen_straight_tile_line((Position){.row = 0, .column = 0, .region_ptr = p}, false, REGION_HEIGHT, SPRITE_MOUNTAIN,
        (Tile){.passable = false}, gs);
    if (west_align) {
        // Use aligned passable tiles
        for (int row = 1; row < ROWS - 1; row++) {
            if (west_align[row - 1]) {
                EntityID to_change_id = p->tile_ids[row][0];
                Sprite *sprite_to_change = sc_map_get_64v(&gs->Sprite_map, to_change_id);
                Tile *tile_to_change = sc_map_get_64v(&gs->Tile_map, to_change_id);
                if (sc_map_found(&gs->Sprite_map) && sc_map_found(&gs->Tile_map)) {
                    *sprite_to_change = SPRITE_GRASS;
                    *tile_to_change = (Tile){.passable = true};
                }
            }
        }
    } else {
        // Generate random passable tiles
        gen_rand_tile_line((Position){.row = 1, .column = 0, .region_ptr = p}, false, REGION_HEIGHT - 2, 1, 6, SPRITE_GRASS,
            (Tile){.passable = true}, gs);
    }
    
    // south
    gen_straight_tile_line((Position){.row = ROWS - 1, .column = 0, .region_ptr = p}, true, REGION_WIDTH,
        SPRITE_MOUNTAIN, (Tile){.passable = false}, gs);
    if (south_align) {
        // Use aligned passable tiles
        for (int col = 1; col < COLUMNS - 1; col++) {
            if (south_align[col - 1]) {
                EntityID to_change_id = p->tile_ids[ROWS - 1][col];
                Sprite *sprite_to_change = sc_map_get_64v(&gs->Sprite_map, to_change_id);
                Tile *tile_to_change = sc_map_get_64v(&gs->Tile_map, to_change_id);
                if (sc_map_found(&gs->Sprite_map) && sc_map_found(&gs->Tile_map)) {
                    *sprite_to_change = SPRITE_GRASS;
                    *tile_to_change = (Tile){.passable = true};
                }
            }
        }
    } else {
        // Generate random passable tiles
        gen_rand_tile_line((Position){.row = ROWS - 1, .column = 1, .region_ptr = p}, true, REGION_WIDTH - 2, 1, 6,
            SPRITE_GRASS, (Tile){.passable = true}, gs);
    }
    
    // east
    gen_straight_tile_line((Position){.row = 0, .column = COLUMNS - 1, .region_ptr = p}, false, REGION_HEIGHT,
        SPRITE_MOUNTAIN, (Tile){.passable = false}, gs);
    if (east_align) {
        // Use aligned passable tiles
        for (int row = 1; row < ROWS - 1; row++) {
            if (east_align[row - 1]) {
                EntityID to_change_id = p->tile_ids[row][COLUMNS - 1];
                Sprite *sprite_to_change = sc_map_get_64v(&gs->Sprite_map, to_change_id);
                Tile *tile_to_change = sc_map_get_64v(&gs->Tile_map, to_change_id);
                if (sc_map_found(&gs->Sprite_map) && sc_map_found(&gs->Tile_map)) {
                    *sprite_to_change = SPRITE_GRASS;
                    *tile_to_change = (Tile){.passable = true};
                }
            }
        }
    } else {
        // Generate random passable tiles
        gen_rand_tile_line((Position){.row = 1, .column = COLUMNS - 1, .region_ptr = p}, false, REGION_HEIGHT - 2, 1, 6,
            SPRITE_GRASS, (Tile){.passable = true}, gs);
    }
}
