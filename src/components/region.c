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

typedef struct {
    int x, y, width, height;
    int room_id;
} RoomRect;

static Region *init_region(EntityID id, GameState *gs, RegionTemplate template, bool *north_align, bool *south_align, bool *west_align, bool *east_align);
static void create_tiles(Region *p, GameState *gs, Sprite background);
static void gen_straight_tile_line(
    Position origin, bool is_x_axis, int line_len, Sprite sprite, Tile tile, GameState *gs);
static void gen_rand_tile_line(Position origin, bool is_x_axis, int extent, int min_entity_count, int max_entity_count,
    Sprite sprite, Tile tile, GameState *gs);
static int *gen_rooms(Region *p, RegionTemplate template);
static void assign_tiles(Region *region_ptr, int *room_matrix, RegionTemplate template);
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
    const int min_room_ct = rand() % (template.room_ct_range[R_MAX] - template.room_ct_range[R_MIN] + 1) + 
                            template.room_ct_range[R_MIN];
    
    // Initialize matrix - 0 = background/corridor, positive = room ID
    int *space_matrix = calloc(INNER_AREA, sizeof(int));
    check_malloc(space_matrix);
    
    // Try to place rectangular rooms
    RoomRect *rooms = malloc(min_room_ct * sizeof(RoomRect));
    check_malloc(rooms);
    int placed_rooms = 0;
    
    // Place rooms with overlap checking
    for (int attempts = 0; attempts < min_room_ct * 10 && placed_rooms < min_room_ct; attempts++) {
        int width = template.min_room_width + (rand() % (template.max_room_width - template.min_room_width + 1));
        int height = template.min_room_height + (rand() % (template.max_room_height - template.min_room_height + 1));
        int x = rand() % (INNER_COLS - width);
        int y = rand() % (INNER_ROWS - height);
        
        // Check for overlap with existing rooms (leave 1-tile gap)
        bool can_place = true;
        for (int i = 0; i < placed_rooms; i++) {
            if (!(x >= rooms[i].x + rooms[i].width + 1 || 
                  x + width + 1 <= rooms[i].x ||
                  y >= rooms[i].y + rooms[i].height + 1 || 
                  y + height + 1 <= rooms[i].y)) {
                can_place = false;
                break;
            }
        }
        
        if (can_place) {
            rooms[placed_rooms] = (RoomRect){x, y, width, height, placed_rooms + 1};
            
            // Fill matrix with room ID
            for (int ry = y; ry < y + height; ry++) {
                for (int rx = x; rx < x + width; rx++) {
                    space_matrix[ry * INNER_COLS + rx] = placed_rooms + 1;
                }
            }
            placed_rooms++;
        }
    }
    
    // Connect rooms with L-shaped corridors
    for (int i = 1; i < placed_rooms; i++) {
        RoomRect *room1 = &rooms[i-1];
        RoomRect *room2 = &rooms[i];
        
        // Get center points
        int cx1 = room1->x + room1->width / 2;
        int cy1 = room1->y + room1->height / 2;
        int cx2 = room2->x + room2->width / 2;
        int cy2 = room2->y + room2->height / 2;
        
        // Create L-shaped corridor
        // Horizontal segment
        int start_x = (cx1 < cx2) ? cx1 : cx2;
        int end_x = (cx1 < cx2) ? cx2 : cx1;
        for (int x = start_x; x <= end_x; x++) {
            if (space_matrix[cy1 * INNER_COLS + x] == 0) {
                space_matrix[cy1 * INNER_COLS + x] = BACKGROUND_FLAG;
            }
        }
        
        // Vertical segment
        int start_y = (cy1 < cy2) ? cy1 : cy2;
        int end_y = (cy1 < cy2) ? cy2 : cy1;
        for (int y = start_y; y <= end_y; y++) {
            if (space_matrix[y * INNER_COLS + cx2] == 0) {
                space_matrix[y * INNER_COLS + cx2] = BACKGROUND_FLAG;
            }
        }
    }
    
    free(rooms);
    return space_matrix;
}

void assign_tiles(Region *region_ptr, int *room_matrix, RegionTemplate template) {
    // Assign tiles for the inner area
    for (int inner_row = 0; inner_row < INNER_ROWS; inner_row++) {
        for (int inner_col = 0; inner_col < INNER_COLS; inner_col++) {
            int region_row = inner_row + 1;
            int region_col = inner_col + 1;
            int matrix_index = inner_row * INNER_COLS + inner_col;
            
            EntityID id = region_ptr->tile_ids[region_row][region_col];
            Sprite *sprite = sc_map_get_64v(&(region_ptr->gs->Sprite_map), id);
            
            if (room_matrix[matrix_index] == 0) {
                // Unassigned space - use default background
                *sprite = template.default_background;
            } else if (room_matrix[matrix_index] == BACKGROUND_FLAG) {
                // Corridor - use background but ensure passable
                *sprite = template.default_background;
                Tile *tile = sc_map_get_64v(&(region_ptr->gs->Tile_map), id);
                if (sc_map_found(&(region_ptr->gs->Tile_map))) {
                    tile->passable = true;
                }
            } else {
                // Room - use room floor
                *sprite = template.room_floor;
                Tile *tile = sc_map_get_64v(&(region_ptr->gs->Tile_map), id);
                if (sc_map_found(&(region_ptr->gs->Tile_map))) {
                    tile->passable = true;
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
